/* 
 * An attempt to monitor RAFT communications
 */

#include <iostream>
#include <tins/tins.h>
 
using namespace Tins;
using namespace std;
 
bool callback(const PDU &pdu) {
    const RawPDU &data = pdu.rfind_pdu<RawPDU>();
    const IP &ip = pdu.rfind_pdu<IP>();
    
    const int size = sizeof(data.payload());

    int version = (int)data.payload()[12]; //RAFT communication version, only appears in requests

    std::string action = "Reply";

    if (version == 1) { //this is a request, not a reply
        // 1 = client service, 2 = RAFT service, as defined in Common.h
        int service = (int)data.payload()[14];
        int opCode = (int)data.payload()[17];
        if (service == 1) {
            switch (opCode) { //opcodes are defined in Client.proto and Raft.proto
 		case 0:
    	            action = "Get Supported RPC Versions From";
                    break;
                case 1:
                    action = "Open Session With";
                    break;
                case 2:
                    action = "Get Configuration From";
                    break;
                case 3:
                    action = "Set Configuration Of";
                    break;
                case 4:
                    action = "Get Read-Only Tree From";
                    break;
                case 5:
                    action = "Get Read-Write Tree From";
                    break;
                default:
                    action = "Invalid opcode";
            }
        }
        else if (service == 2) {
            switch (opCode) {
 		case 0:
    	            action = "Get Supported RPC Versions From";
                    break;
                case 1:
                    action = "Request Vote From";
                    break;
                case 2:
                    action = "Append Entries/Heartbeat";
                    break;
                case 3:
                    action = "Append Snapshot Chunk";
                    break;
                default:
                    action = "Invalid opcode";
            }
        }
        else {
            action = "Invalid service number";
        }

    }

    std::cout << ip.src_addr() << ": " << action << " -> "
              << ip.dst_addr() << std::endl;
    return true;
}
 
int main() {
    Sniffer sniffer("lo", 2000); //lo interface
    sniffer.sniff_loop(callback);
}
