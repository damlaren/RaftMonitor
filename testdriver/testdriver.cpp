/*
 * Test driver program. Right now it only demonstrates
 * use of the Client API.
 */

#include "../clients/logcabin/lg-client.h"
#include "../RaftMonitor.h"
#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace std;

const int NUM_HOSTS = 3;

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
       << "sudo ./testdriver/testdriver <impl> <nClients> <testname-1> [testname-2] ..." << endl
       << "\timpl: Raft implementation to use. Values={logcabin}." << endl
       << "\tnClients: Number of clients to start." << endl
       << "\ttestnames: Names of tests to run. Values={TODO: not used yet}." << endl;
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
    cout << "error: Unrecognized implementation: " << implStr << endl;
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
    int id = i + 1;
    RaftClient *newClient = nullptr;
    switch (raftImpl)
    {
    case RaftImplementation::LOGCABIN:
      newClient = new LogCabinRaftClient(id);
      break;
    default:
      help();
    }
    bool yay = newClient->createClient(clusterConfig);
    assert(yay);
    clients.push_back(newClient);
  }
}

// Args for spinning off RM.
typedef struct
{
  string impl;
  int numhosts;
  string iface;
} RMArgs;
RMArgs rmArgs;

// Thread in which to run RM.
pthread_t rmThread;

// Start Raft in its own thread.
void* spawnRaft(void *arg)
{
  RMArgs* prmArgs = static_cast<RMArgs*>(arg);
  assert(prmArgs);
  startRaft(prmArgs->impl, prmArgs->numhosts,
	    prmArgs->iface);

  pthread_exit(nullptr);
}

int main(const int argc, const char *argv[])
{
  if (argc < 4 || string(argv[0]) == "--help")
  {
    help();
  }

  RaftMonitor *prm = RaftMonitor::getRaftMonitor();
  string impl = argv[1];
  selectImplementation(impl);
  int nClients = stoi(argv[2]);

  // Start the RaftMonitor.
  // TODO: this makes assumptions about host IP addrs.
  // And to assume makes something of you and me.
  rmArgs.impl = impl;
  rmArgs.numhosts = 3;
  rmArgs.iface = "lo";
  if (pthread_create(&rmThread, nullptr, spawnRaft,
		     &rmArgs) != 0)
  {
    cout << "error: couldn't create RM thread" << endl;
    exit(1);
  }

  // Wait for RM to come online.
  // TODO: This isn't perfect-- sniffer may not start
  // capturing yet. Not sure if that's a problem.
  while (!prm->alive){ sleep(1); }

  // Identify and run all tests.
  // Reinitialize the cluster AND the file system
  // between each and every test.
  // TODO: tests could 'build' on each other,
  // but that can come in a later system.
  for (int i = 3; i < argc; i++)
  {
    // Determine what test is being run
    string testName = argv[i];
    cout << "testdriver: running test " << testName << endl;

    // Create cluster configuration.
    clusterConfig = createClusterConfig();
    assert(clusterConfig != nullptr);

    // Launch the cluster.
    clusterConfig->launchCluster(NUM_HOSTS, 61023);
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
    cout << "testdriver: created clients" << endl;

    // Connect clients to cluster.
    // TODO: cluster hostname is hardcoded
    for (RaftClient* client : clients)
    {
      string host = string("logcabin:") +
	to_string(clusterConfig->clusterPort);
      client->connectToCluster(host);
    }
    cout << "testdriver: connected" << endl;
    sleep(1);

    // Start the test by having clients run commands.
    for (RaftClient* client : clients)
    {
      ClientOperations *opsInfo = new ClientOperations;
      opsInfo->client = client;
      opsInfo->nIterations = 5;
      opsInfo->nClients = clients.size();

      // Start a new thread for this client.
      if (pthread_create(&client->thread, nullptr,
			 runClient, opsInfo) != 0)
      {
	cout << "error: couldn't create client thread" << endl;
	exit(1);
      }
    }

    // wait until all clients are finished
    for (RaftClient* client : clients)
    {
      pthread_join(client->thread, nullptr);
    }

    // Destroy clients
    sleep(1);
    cout << "testdriver: destroying clients" << endl;
    for (RaftClient* client : clients)
    {
      client->destroyClient();
      delete client;
    }

    // Stop the cluster
    cout << "testdriver: stopping cluster" << endl;
    clusterConfig->stopCluster();
    delete clusterConfig;

    cout << "testdriver: test complete!" << endl;
  }

  // Shut down the RaftMonitor.
  cout << "testdriver: shutting down RM" << endl;
  RaftMonitor::getRaftMonitor()->sniff->Cancel();
  pthread_join(rmThread, nullptr);

  return 0;
}
