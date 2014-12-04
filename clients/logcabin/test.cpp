/*
 * Simple test program to demonstrate use of the
 * LogCabin client.
 */

#include <assert.h>
#include <iostream>
#include "lg-client.h"

int main(const int argc, const char *argv[])
{
  RaftClient *client = new LogCabinRaftClient();
  bool yay = client->createClient();
  assert(yay);

  // For this test it is assumed that the cluster
  // is already running.
  // TODO: how to spec multiple hosts?
  client->connectToCluster("192.168.2.1:61023");
  if (!client->writeFile("/testfile", "testvalue"))
  {
    std::cerr << "Write failed" << std::endl;
  }
  if (client->readFile("/testfile") != "testvalue") {
    std::cerr << "Read failed" << std::endl;
  }
  client->destroyClient();

  return 0;
}
