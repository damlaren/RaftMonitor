/*
 * This is the packet interpreter for the RPC calls of LogCabin,
 * Ongaro's RAFT implementation.
 */
#include "crafter.h"
#include "ongaro.h"
#include "Common.h"
    
//int ongaroRAFT::packetType(std::vector<uint8_t> pkt) {
int ongaroRAFT::packetType(std::vector<unsigned char> pkt) {
    int version = (int)pkt[12]; //RAFT communication version, only appears in requests
    if (version == 1) { //this is a request, not a reply
        // 1 = client service, 2 = RAFT service, as defined in Common.h
        int service = (int)pkt[14];
        int opCode = (int)pkt[17];
        if (service == 1) {
            switch (opCode) { //opcodes are defined in Client.proto and Raft.proto
 		case 0:
                    return GET_RPC_VERSIONS;
                case 1:
                    return OPEN_SESSION;
                case 2:
                    return GET_CONFIG;
                case 3:
                    return SET_CONFIG;
                case 4:
                    return READ_ONLY_TREE;
                case 5:
                    return READ_WRITE_TREE;
                default:
                    return INVALID;
            }
        }
        else if (service == 2) {
            switch (opCode) {
 		case 0:
    	            return GET_RPC_VERSIONS;
                case 1:
                    return REQUEST_VOTE;
                case 2:
                    return APPEND_HEARTBEAT;
                case 3:
                    return APPEND_SNAPSHOT;
                default:
                    return INVALID;
            }
        }
        else {
            return INVALID; //invalid service number
        }
    }
    else {
         return REPLY;
    }

}

