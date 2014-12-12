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
#include "packetutils/etcd.h"
#include "RaftMonitor.h"

using namespace std;
using namespace Crafter;

std::vector<int> ports;

ofstream out_file;

int out_port = 61023;

RaftMonitor RaftMonitor::rm;

RaftMonitor* RaftMonitor::getRaftMonitor()
{
  return &rm;
}

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
        /*for (int j = 0; j < numhosts; j++) {
            counts.push_back(0);
            counts.push_back(0);
        }**/
    }
    currentLeader = 1;
}

RaftMonitor::RaftMonitor() {
  currentLeader = 1;
}


RaftMonitor::~RaftMonitor() {
    
}

/*
 * Given a string with an IP address,
 * get the number of the last field
 */
int getLastIP(string ip) {
    char * str = strdup(ip.c_str());
    char * pch;
    pch = strtok(str,".");
    char* res;
    while (pch != NULL) {
        res = pch;
        pch = strtok (NULL, ".");
    }
    return atoi(res);
}

int RaftMonitor::getPortNum(int port) {
    int pos = -1;
    for (int i = 0; i < rm.ports.size(); i++) {
        if (rm.ports[i] == port) {
            pos = i;
        }
    }
    if (pos == -1) {
        rm.ports.push_back(port);
        pos = rm.ports.size()-1;
    }
    
    return pos;
}

int RaftMonitor::getCurrentLeader()
{
  return currentLeader;
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
    if (rm.implementation.compare("logcabin") == 0) {
        if (bytes.size() > 18) {
            type = ongaroRAFT::packetType(bytes);
        }
        else { //this is not a RAFT RPC packet
                type = -1;
        }
    }
    else if (rm.implementation.compare("etcd") == 0) {
        type = etcdRAFT::packetType(bytes);
    }

    switch (type) { 
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
                rm.num_elections++;
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

    // Reads come from the leader (or are supposed to).
    // This is a quick and dirty, but not perfect,
    // way of identifying a leader.
    if (type == READ_ONLY_TREE ||
	type == READ_WRITE_TREE)
    {
      std::string ipStr(ip->GetSourceIP());
      int nodeId = getLastIP(ipStr);
      if (nodeId != rm.currentLeader)
      {
	rm.currentLeader = nodeId;
	std::cout << "RM: new leader = "
		  << rm.currentLeader << std::endl;
      }
    }
 
    //update counts
    //for if IPs ever worked as intended...
    //int src_num = getLastIP(ip->GetSourceIP());
    //int dest_num = getLastIP(ip->GetDestinationIP());
    //int src_num = rm.getPortNum(tcp->GetSrcPort());
    //int dest_num = rm.getPortNum(tcp->GetDstPort());
    //int index = (rm.num_hosts*(src_num))+(dest_num);
    //if(!(index >= 0 && index < rm.counts.size())) {
      //std::cout << "index = " << index << std::endl
      //	<< "src_num=" << src_num << std::endl
      //	<< "dest_num=" << dest_num << std::endl
      //	<< "|rm|=" << rm.counts.size() << std::endl;
    //}
    //else
    //{
      //rm.counts[index]++;
    //}
    rm.counts[std::make_pair(tcp->GetSrcPort(),tcp->GetDstPort())]+=1;
    
    if (action.compare("Append/Heartbeat") != 0 && action.compare("Reply") != 0
            && action.compare("Session Reply") != 0) {
 
    std::cout << ip->GetSourceIP() << ":" << tcp->GetSrcPort() << ": "<< action << " -> "
              << ip->GetDestinationIP() << ":" << tcp->GetDstPort()  << std::endl;
    
    if (rm.run_test) {
        out_file << ip->GetSourceIP() << ":" << tcp->GetSrcPort() << ": "<< action << " -> "
              << ip->GetDestinationIP() << ":" << tcp->GetDstPort()  << std::endl;
    }
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
        num_elections = 0;
        if (partition != 0) {
                start_block(source, fraction);
        }
        //get starting time
        string str = getTime();
        out_file << "+----------------------------------------------+\n";
        out_file << "Beginning test " + testname + " at time " + str + "\n";
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
    /*for (int i = 0; i < num_hosts; i++) {
        for (int j = 0; j < num_hosts; j++) {
          //out_file << "Packets " + ips[i] + "->" + ips[j] + ":";
            out_file << "Packets " + to_string(ports[i]) + "->" + to_string(ports[j]) + ":";
            out_file << to_string(counts[(num_hosts*i+j)]);
            out_file << endl;
        }
    }*/
    for(map<pair<int,int>, int>::const_iterator it = counts.begin();
    it != counts.end(); ++it) {
        out_file << "Packets " + to_string(it->first.first) + "->" + to_string(it->first.second) + ":";
        out_file << to_string(it->second);
        out_file << endl;
    }

    out_file << "Elections: " << to_string(num_elections) << endl;
    //get ending time, write, and close
    string str = getTime();
    out_file << "Ending test " + testname + " at time " + str + "\n";
    out_file << "+----------------------------------------------+\n";
    out_file.close();
    //reset the packet counts
    //fill(counts.begin(),counts.end(),0); 
    counts.clear();
    return 0;
}


void RaftMonitor::start_block(const string& src_ip, float fraction) {
    system(string("iptables -A OUTPUT -m statistic --mode random --source " + src_ip +
            " --probability " + to_string(fraction) + " -j DROP").c_str());
}

void RaftMonitor::clear_block(const string& src_ip, float fraction) {
    system(string("iptables -D OUTPUT -m statistic --mode random --source " + src_ip +
            " --probability " + to_string(fraction) + " -j DROP").c_str());
}

int startRaft(string impl, int numhosts, string iface) {
        RaftMonitor* prm = RaftMonitor::getRaftMonitor();
	*prm = RaftMonitor(impl, numhosts, iface);
	prm->sniff = new Sniffer("tcp", prm->interface,
				 prm->callback);

	// sniff until external thread cancels it.
	prm->alive = true;
        prm->sniff->Capture(-1);

        return 0;
}
