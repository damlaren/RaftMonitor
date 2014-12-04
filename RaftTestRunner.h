#ifndef RM_RAFTTESTRUNNER_H
#define RM_RAFTTESTRUNNER_H

#include <cstdio>
#include "clients/client.h"
#include <string>

void setupLo(int numNodes) {
  std::string loSetupCmd = "./setup_lo.sh ";
  loSetupCmd += std::to_string(numNodes);
  system(loSetupCmd.c_str());
}

void teardownLo(int numNodes) {
  std::string loStopCmd = "./shutdown_lo.sh ";
  loStopCmd += std::to_String(numNodes);
  system(loStopCmd.c_str());
}

void setupRedirs(int numNodes, int clusterPort) {
  std::string reSetupCmd = "./setup_redirects.sh ";
  reSetupCmd += std::to_string(numNodes);
  reSetupCmd += " ";
  reSetupCmd += std::to_string(clusterPort);
  system(reSetupCmd.c_str());
}

void teardownRedirs(int numNodes, int clusterPort) {
  std::string reStopCmd = "./teardown_redirects.sh ";
  reStopCmd += std::to_string(numNodes):
  reStopCmd += " ";
  reStopCmd += std::to_string(clusterPort);
  system(reStopCmd.c_str());
}

void setupSimulatedCluster(RaftClient* clientPtr, int numNodes) {
  setupLo(numNodes);
  setupRedirs(numNodes, clientPtr->clusterPort);
  clientPtr->launchCluster(numNodes);
}

void stopSimulatedCluster(RaftClient* clientPtr, int numNodes) {
  clientPtr->stopCluster(numNodes);
  teardownRedirs(numNodes, clientPtr->clusterPort);
  teardownLo(numNodes);
}

void runTestCore(RaftClient* clientPtr, int numNodes) {
  // TODO: Place actual tests or call them from here.
}

void runTests(RaftClient* clientPtr) {
  setupSimulatedCluster(clientPtr, 3);
  runTestCore(clientPtr, 3);
  stopSimulatedCluster(clientPtr, 3);
}

void runTests(RaftClient* clientPtr, int numNodes) {
  setupSimulatedCluster(clientPtr, numNodes);
  runTestCore(clientPtr, numNodes);
  stopSimulatedCluster(clientPtr, numNodes);
}


#endif
