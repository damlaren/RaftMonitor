/*
 * Simple test program to demonstrate use of the
 * LogCabin client.
 */

#include <assert.h>
#include <iostream>
#include "lg-client.h"

int main(const int argc, const char *argv[])
{
  // Create cluster configuration.
  RaftClusterConfig *clusterConfig = new LogCabinRaftClusterConfig();
  clusterConfig->launchCluster(3);

  std::cout << "started cluster" << std::endl;

  // Create at least one client.
  RaftClient *client = new LogCabinRaftClient();
  bool yay = client->createClient(clusterConfig);
  assert(yay);

  std::cout << "created client" << std::endl;

  // TODO: how to spec multiple hosts?
  client->connectToCluster("192.168.2.1:61023");

  std::cout << "connected" << std::endl;
  
  if (!client->writeFile("/testfile", "testvalue"))
  {
    std::cerr << "Write failed" << std::endl;
  }
  if (client->readFile("/testfile") != "testvalue") {
    std::cerr << "Read failed" << std::endl;
  }

  // Destroy client
  client->destroyClient();
  delete client;

  // Stop the clustera
  clusterConfig->stopCluster(3);
  delete clusterConfig;

  return 0;
}
