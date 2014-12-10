/*
 * Test driver program. Right now it only demonstrates
 * use of the Client API.
 */

#include "../clients/logcabin/lg-client.h"
#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace std;

// Flag for current Raft implementation that's in use.
enum class RaftImplementation
{
  UNKNOWN,
  LOGCABIN
};
RaftImplementation raftImpl = RaftImplementation::UNKNOWN;

// Configuration for Raft cluster.
RaftClusterConfig *clusterConfig = nullptr;

// ... The clients.
vector<RaftClient*> clients;

// --- Functions ---

// Print the help and go away.
void help()
{
  cout << "From the project root directory: " << endl
       << "sudo ./testdriver/testdriver <impl> <nClients>" << endl
       << "\timpl: Raft implementation to use. Values={logcabin}." << endl
       << "\tnClients: Number of clients to start." << endl
       << "TODO: later we'll select a test to run." << endl;
  exit(0);
}

// Select an implementation from command-line argument.
void selectImplementation(const std::string& implStr)
{
  if (implStr == "logcabin")
  {
    raftImpl = RaftImplementation::LOGCABIN;
  }

  if (raftImpl == RaftImplementation::UNKNOWN)
  {
    cerr << "Unrecognized implementation: " << implStr << endl;
    help();
  }
}

// Allocate a new RaftClusterConfig adapted for this
// implementation.
RaftClusterConfig* createClusterConfig()
{
  switch (raftImpl)
  {
  case RaftImplementation::LOGCABIN:
    return new LogCabinRaftClusterConfig();
  default:
    help();
  }
}

// Create requested number of clients.
void createClients(int nClients)
{
  for (int i = 0; i < nClients; i++)
  {
    RaftClient *newClient = nullptr;
    switch (raftImpl)
    {
    case RaftImplementation::LOGCABIN:
      newClient = new LogCabinRaftClient();
      break;
    default:
      help();
    }
    bool yay = newClient->createClient(clusterConfig);
    assert(yay);
    clients.push_back(newClient);
  }
}

int main(const int argc, const char *argv[])
{
  if (argc < 3 || std::string(argv[0]) == "--help")
  {
    help();
  }

  selectImplementation(argv[1]);
  int nClients = std::stoi(argv[2]);

  // Create cluster configuration.
  clusterConfig = createClusterConfig();
  assert(clusterConfig != nullptr);

  // Launch the cluster.
  clusterConfig->launchCluster(3, 61023);
  cout << "testdriver: started cluster" << endl;
  sleep(1);

  // Configure packet loss on a Raft node.
  // It'll automatically unconfigure itself on
  // destruction.
  // Dropping packets from 2 nodes causes the program
  // to hang with the logcabin implementation, which
  // is good!
  PacketDropConfig packetDropper("192.168.2.1", 1.0);
  //packetDropper.startDropping();
  //PacketDropConfig packetDropper2("192.168.2.3", 1.0);
  //packetDropper2.startDropping();

  // Create at least one client.
  createClients(nClients);
  cout << "created clients" << endl;

  // Connect clients to cluster.
  // TODO: cluster hostname is hardcoded
  for (RaftClient* client : clients)
  {
    std::string host = std::string("logcabin:") +
      std::to_string(clusterConfig->clusterPort);
    client->connectToCluster(host);
  }
  std::cout << "connected" << std::endl;
  sleep(1);
  
  // TODO: for now just pick one client and write
  // something.
  RaftClient *client = clients[0];
  if (!client->writeFile("/testfile", "testvalue"))
  {
    cerr << "Write failed" << endl;
  }
  cout << "write succeeded!" << endl;
  if (client->readFile("/testfile") != "testvalue") {
    cerr << "Read failed" << endl;
  }
  cout << "read succeeded!" << endl;

  //sleep(10); // Here to check if packetDropper works

  // Destroy clients
  for (RaftClient* client : clients)
  {
    client->destroyClient();
    delete client;
  }

  // Stop the cluster
  clusterConfig->stopCluster(3);
  delete clusterConfig;

  return 0;
}
