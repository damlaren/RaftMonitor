/* 
 * An attempt to monitor RAFT communications via raw packet analysis.
 */

#include <iostream>
#include <tins/tins.h>
#include "packetutils/Common.h"
#include "packetutils/ongaro.h"
 
using namespace Tins;
using namespace std;
 
bool callback(const PDU &pdu) {
    const RawPDU &data = pdu.rfind_pdu<RawPDU>();
    const IP &ip = pdu.rfind_pdu<IP>();
    
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
                action = "Invalid opcode";
    }
        

    std::cout << ip.src_addr() << ": " << action << " -> "
              << ip.dst_addr() << std::endl;
    return true;
}
 
int main() {
    Sniffer sniffer("lo", 2000); //lo interface
    sniffer.sniff_loop(callback);
}
