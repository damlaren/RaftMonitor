/* 
 * An attempt to monitor RAFT communications via raw packet analysis.
 */

#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <sys/socket.h>
#include "packetutils/Common.h"
#include "packetutils/ongaro.h"
#include "RaftMonitor.h"

using namespace std;
using namespace Crafter;

int p[3] = {61024,61025,61026};
std::vector<int> ports (&p[0], &p[0]+2);
std::string st[3] = {"192.168.2.1","192.168.2.2","192.168.2.3"};

ofstream out_file;

int out_port = 61023;

template <typename T>
std::string to_string(T value)
{
	std::ostringstream os ;
	os << value ;
	return os.str() ;
}

RaftMonitor::RaftMonitor(string impl, int numhosts, string iface) {
    implementation = impl;
    num_hosts = numhosts;
    interface = iface;
    //initialize test counts
    num_elections = 0;
    run_test = false;
    for (int i = 0; i < numhosts; i++) {
        ips.push_back("192.168.2." + to_string(i+1));
        for (int j = 0; j < numhosts; j++) {
            counts.push_back(0);
        }
    }
}

RaftMonitor::RaftMonitor() {
    
}


RaftMonitor::~RaftMonitor() {
    
}

int RaftMonitor::getIndex(string ip) {
    int pos = -1;
    for (int i = 0; i < num_hosts; i++) {
        if (st[i].compare(ip) == 0) {
            pos = i;
        }
    }
    return pos;
}

void RaftMonitor::sendPacket(RawLayer* data, IP* ip, TCP* tcp) {
    //get the appropriate destination IP based on incoming port
    std::vector<int>::iterator it;
    it=find(ports.begin(),ports.end(), (int)tcp->GetDstPort());
    int pos = std::distance(ports.begin(), it);
    //construct the packet
    tcp->SetDstPort(out_port);
    ip->SetDestinationIP(ips[pos]);
    //send the packet
    try {
        Packet packet(Ethernet() / *ip / *tcp / *data);
        packet.Send("lo");
    } catch(exception& e) {
        cout << e.what() <<  std::endl;
    }
}
 
void RaftMonitor::callback(Packet* sniff_packet, void* user) {
    RawLayer* data = sniff_packet->GetLayer<RawLayer>();
    std::vector<unsigned char> bytes (0);
    if (data != 0) {
        std::string by = data->GetStringPayload();
        bytes.assign(by.begin(), by.end());
    }

    IP* ip = sniff_packet->GetLayer<IP>();
    TCP* tcp = sniff_packet->GetLayer<TCP>();
    
    std::string action = "Reply";
    
    //get the packet type
    int type;
    if (bytes.size() > 18) {
        type = ongaroRAFT::packetType(bytes);
    }
    else { //this is not a RAFT RPC packet
        type = -1;
    }

    switch (type) { //opcodes are defined in Client.proto and Raft.proto
        case GET_RPC_VERSIONS:
                action = "Get Supported RPC Versions From";
                break;
        case OPEN_SESSION:
                action = "Open Session With";
                break;
        case GET_CONFIG:
                action = "Get Configuration From";
                break;
        case SET_CONFIG:
                action = "Set Configuration Of";
                break;
        case READ_ONLY_TREE:
                action = "Get Read-Only Tree From";
                break;
        case READ_WRITE_TREE:
                action = "Get Read-Write Tree From";
                break;
        case REQUEST_VOTE:
                action = "Request Vote From";
                break;
        case APPEND_HEARTBEAT:
                action = "Append/Heartbeat";
                break;
        case APPEND_SNAPSHOT:
                action = "Append Snapshot";
                break;
        case REPLY:
                action = "Reply";
                break;
        default:
                action = "Session Reply";
    }
 
    std::cout << ip->GetSourceIP() << ":" << tcp->GetSrcPort() << ": "<< action << " -> "
              << ip->GetDestinationIP() << ":" << tcp->GetDstPort()  << std::endl;
    
    if (rm.run_test) {
        out_file << ip->GetSourceIP() << ":" << tcp->GetSrcPort() << ": "<< action << " -> "
              << ip->GetDestinationIP() << ":" << tcp->GetDstPort()  << std::endl;
    }
    
}

string RaftMonitor::getTime() {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

         time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
        std::string str(buffer);
        return str;
}
/*
 * This function indicates the beginning of a Raft test. 
 * fraction is the amount of packets to drop from source
 * If parition is 0, it indicates that this is not a parition test
 * so no iptables rules will be added.
 * Returns -1 if anything goes wrong, 0 otherwise
 */
int RaftMonitor::startTest(string testname, int partition, float fraction, string source) {
    logfile = testname + ".log";
    out_file.open(logfile.c_str(),  ios::out | ios::app);
    if (out_file.is_open()) {     
        run_test = true;
        if (partition != 0) {
                start_block(source, fraction);
        }
        //get starting time
        string str = getTime();
        out_file << "+----------------------------------------------+\n";
        out_file << "Beginning test " + testname + " at time " + str;
        return 0;
    }
    else {
        return -1; //failed to open log file
    }
}

/*
 * Stops the current test that is running
 * Returns -1 if anything goes wrong, 0 otherwise
 */
int RaftMonitor::stopTest(string testname, int partition, float fraction, string source) {
    if (partition != 0) {
        clear_block(source, fraction);
    }
    run_test = false;
    //write out all counts
    //get ending time, write, and close
    string str = getTime();
    out_file << "Ending test " + testname + " at time " + str;
    out_file << "+----------------------------------------------+\n";
    out_file.close();
    return 0;
}


void RaftMonitor::start_block(const string& src_ip, float fraction) {
    system(string("iptables -A OUTPUT -m statistic --mode random --source" + src_ip +
            " --probability " + to_string(fraction) + " -j DROP").c_str());
}

void RaftMonitor::clear_block(const string& src_ip, float fraction) {
    system(string("iptables -D OUTPUT -m statistic --mode random --source" + src_ip +
            " --probability " + to_string(fraction) + " -j DROP").c_str());
}
 
int startRaft(string impl, int numhosts, string iface) {

        rm = RaftMonitor(impl, numhosts, iface);
        Sniffer sniff("tcp", rm.interface, rm.callback);
        //sniffer loop
        //sniff.Spawn(-1); //spawns sniffer in a new thread
        sniff.Capture(-1);
        
        while (true) {
            sleep(1);
        }

        sniff.Cancel(); //quit the sniffer

        return 0;

}

int main() {
    startRaft("ongaro", 3);
}

