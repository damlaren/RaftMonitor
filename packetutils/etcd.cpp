/*
 * This is the packet interpreter for the HTTP requests of etcd
 */

#include "crafter.h"
#include "etcd.h"
#include "Common.h"

using namespace Crafter;

int etcdRAFT::packetType(std::vector<unsigned char> pkt) {
    //get the payload string of the http
    std::string data(pkt.begin(),pkt.end());
    if (data.find("POST /log/append HTTP/1.1") != std::string::npos) {
        return APPEND_HEARTBEAT;
    }
    else if (data.find("200 OK") != std::string::npos) {
        return REPLY;
    }
    else if (data.find("GET /version HTTP/1.1") != std::string::npos) {
        return GET_RPC_VERSIONS;
    } 
    else if (data.find("GET /v2/admin") != std::string::npos) {
        return GET_CONFIG;
    } 
    //when a node joins the cluster
    else if (data.find("PUT /join") != std::string::npos) {
        return SET_CONFIG;
    } 
    else if (data.find("POST /vote") != std::string::npos) {
        return REQUEST_VOTE;
    }
    else if (data.find("PUT /v2/keys") != std::string::npos) {
        return READ_WRITE_TREE; //actually adding a key/value pair
    }
    else if (data.find("GET /v2/keys") != std::string::npos) {
        return READ_ONLY_TREE; //reading a value
    } 
    return REPLY;
}