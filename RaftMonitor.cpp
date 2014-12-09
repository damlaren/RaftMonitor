/* 
 * An attempt to monitor RAFT communications via raw packet analysis.
 */

#include <iostream>
#include <tins/tins.h>
#include "packetutils/Common.h"
#include "packetutils/ongaro.h"
 
using namespace Tins;
using namespace std;


PacketSender sender;
std::string p[3] = {"192.168.2.4","192.168.2.5","192.168.2.6"};
std::vector<std::string> incs (&p[0], &p[0]+2);
std::string st[3] = {"192.168.2.1","192.168.2.2","192.168.2.3"};
std::vector<std::string> ips (&st[0], &st[0]+2);

int out_port = 61023;

void sendPacket(const RawPDU &data, const IP &ip, const TCP &tcp) {
    //get the appropriate destination IP based on incoming port
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator it2;
    it=find(incs.begin(),incs.end(), ip.dst_addr().to_string());
    it2=find(ips.begin(), ips.end(), ip.src_addr().to_string());
    std::string nDstIp = ip.dst_addr().to_string();
    std::string nSrcIp = ip.src_addr().to_string();
    if (it != incs.end()) {
      int pos = std::distance(incs.begin(), it);
      nDstIp = ips[pos];
    }
    if (it2 != ips.end()) {
      int pos = std::distance(ips.begin(), it2);
      nSrcIp = incs[pos];
    }
    //construct the packet
    TCP tsend = TCP(out_port, tcp.sport());
    tsend.seq(tcp.seq());
    //tsend.ack_seq(tcp.ack_seq());
    tsend.flags(tcp.flags());
    EthernetII pkt = EthernetII() /  IP(nDstIp, nSrcIp) / tsend / data;
    //send the packet
    sender.send(pkt, "lo");
}
 
bool callback(const PDU &pdu) {
    const RawPDU &data = pdu.rfind_pdu<RawPDU>();
    const IP &ip = pdu.rfind_pdu<IP>();
    const TCP &tcp = pdu.rfind_pdu<TCP>();
    
     if (ip.dst_addr().to_string().compare("192.168.2.4") == 0 ||
	 ip.dst_addr().to_string().compare("192.168.2.5") == 0 || 
	 ip.dst_addr().to_string().compare("192.168.2.6") == 0 ||
	 ip.src_addr().to_string().compare("192.168.2.1") == 0 ||
	 ip.src_addr().to_string().compare("192.168.2.2") == 0 ||
	 ip.src_addr().to_string().compare("192.168.2.3")) {
        sendPacket(data, ip, tcp);
    }
    
    const int size = sizeof(data.payload());

    int version = (int)data.payload()[12]; //RAFT communication version, only appears in requests

    std::string action = "Reply";
    std::vector<uint8_t> pkt;
    
    int type = ongaroRAFT::packetType(data.payload());

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
        

    std::cout << ip.src_addr() << ":" << tcp.sport() << ": "<< action << " -> "
              << ip.dst_addr() << ":" << tcp.dport()  << std::endl;

    return true;
}
 
int main() {
    Sniffer sniffer("lo", 2000); //lo interface
    //sniffer.set_filter("ip dst 127.0.0.1");
    sniffer.sniff_loop(callback);
}
