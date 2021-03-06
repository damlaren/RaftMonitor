/*
 * Test driver program. Right now it only demonstrates
 * use of the Client API.
 */

#include "../clients/logcabin/lg-client.h"
#include "../clients/etcd/etcd-client.h"
#include "../RaftMonitor.h"
#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace std;

// Flag for current Raft implementation that's in use.
enum class RaftImplementation
{
  UNKNOWN,
  LOGCABIN,
  ETCD  
};
RaftImplementation raftImpl = RaftImplementation::UNKNOWN;

// Which of the test types is in use.
// Each has its own set of arguments.
enum class TestType
{
    BASIC, // No funny business.
    BLOCK, // Block packets from a random node
    KILL   // Kill a random node, restart it later
};

enum class TestMode
{
    RANDOM, // Pick node to tamper with at random.
    LEADER  // Shoot the leader.
};

// Arguments specific to the basic test.
typedef struct
{
} BasicArgs;

// Arguments specific to the block test.
typedef struct
{
  TestMode mode;
  float frac;    // what fraction of packets to drop.
} BlockArgs;

// Arguments specific to the kill test.
typedef struct
{
  TestMode mode;
} KillArgs;

// Wrapped-up test arguments.
// Every test is repeated for some number of iterations,
// and each iteration is some number of seconds long.
// During that whole time, clients will try to blast
// packets at the cluster.
typedef struct
{
  TestType type;
  int iterations; // # of times to repeat the test.
  int time; // in seconds-- an argument for sleep().
  union {
    BasicArgs basic;
    BlockArgs block;
    KillArgs kill;
  };
} TestArgs;
TestArgs testArgs;

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

// Configuration for Raft cluster.
RaftClusterConfig *clusterConfig = nullptr;

// ... The clients.
vector<RaftClient*> clients;

// --- Functions ---

// Print the help and go away.
void help()
{
  cout << "From the project root directory: " << endl
       << "sudo ./testdriver/testdriver <impl> <nNodes> <nClients> <testname> <iterations> <time> [test args] ..." << endl
       << "\timpl: Raft implementation to use. Values={logcabin,etcd}." << endl
       << "\tnNodes: Number of Raft nodes." << endl
       << "\tnClients: Number of clients to start." << endl
       << "\ttestname: Name of test to run. Values={basic,block,kill}." << endl
       << "\titerations: Number of iterations of test to run." << endl
       << "\ttime: How long to run each phase of a test (seconds)." << endl
       << "\ttest args: Any test-specific arguments, as follows:" << endl
       << "\t\tblock: <mode> <frac>" << endl
       << "\t\t\tmode: What node to block. Values={random,leader}." << endl
       << "\t\t\tfrac: What fraction of packets to drop from nodes." << endl
       << "\t\tkill: <mode>" << endl
       << "\t\t\tmode: What node to kill. Values={random,leader}." << endl;
  exit(0);
}

// Select an implementation from command-line argument.
void selectImplementation(const std::string& implStr)
{
  if (implStr == "logcabin")
  {
    raftImpl = RaftImplementation::LOGCABIN;
  }
  else if (implStr == "etcd")
  {
    raftImpl = RaftImplementation::ETCD;
  }

  if (raftImpl == RaftImplementation::UNKNOWN)
  {
    cout << "error: Unrecognized implementation: " << implStr << endl;
    help();
  }
}

TestMode eatTestMode(const std::string& modeStr)
{
  if (modeStr == "random")
  {
    return TestMode::RANDOM;
  }
  else if (modeStr == "leader")
  {
    return TestMode::LEADER;
  }
  else
  {
    cout << "error: Unrecognized test mode: " << modeStr << endl;
    help();
  }
}

// Consume required arguments for current test.
void eatTestArguments(const int argc, const char* argv[],
		      int& argi, string& testName)
{
  assert(argi < argc);
  testName = argv[argi++];

  TestType &testType = testArgs.type;
  if (testName == "basic")
  {
    testType = TestType::BASIC;
  }
  else if (testName == "block")
  {
    testType = TestType::BLOCK;
  }
  else if (testName == "kill")
  {
    testType = TestType::KILL;
  }
  else
  {
    cout << "error: Unrecognized test name " << testName << endl;
    exit(1);
  }

  assert(argi < argc);
  testArgs.iterations = stoi(argv[argi++]);
  assert(argi < argc);
  testArgs.time = stoi(argv[argi++]);

  switch (testType)
  {
  case TestType::BLOCK:
    assert(argi < argc);
    testArgs.block.mode = eatTestMode(argv[argi++]);
    assert(argi < argc);
    testArgs.block.frac = stof(argv[argi++]);
    break;
  case TestType::KILL:
    assert(argi < argc);
    testArgs.kill.mode = eatTestMode(argv[argi++]);
    break;
  default: // Fall through.
  case TestType::BASIC:
    break;
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
  case RaftImplementation::ETCD:
    return new EtcdClusterConfig();
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
    case RaftImplementation::ETCD:
      newClient = new EtcdRaftClient(id);
      break;
    default:
      help();
    }
    bool yay = newClient->createClient(clusterConfig);
    assert(yay);
    clients.push_back(newClient);
  }
}

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
  if (argc < 7 || string(argv[0]) == "--help")
  {
    help();
  }

  RaftMonitor *prm = RaftMonitor::getRaftMonitor();
  string impl = argv[1];
  selectImplementation(impl);
  int nNodes = stoi(argv[2]);
  int nClients = stoi(argv[3]);

  // Start the RaftMonitor.
  rmArgs.impl = impl;
  rmArgs.numhosts = nNodes;
  rmArgs.iface = "lo";
  cout << "testdriver: spawning RM" << endl;
  if (pthread_create(&rmThread, nullptr, spawnRaft,
		     &rmArgs) != 0)
  {
    cout << "error: couldn't create RM thread" << endl;
    exit(1);
  }

  // Wait for RM to come online.
  while (!prm->alive){ sleep(1); }
  cout << "testdriver: RM is online" << endl;

  // Identify and run all tests.
  // Reinitialize the cluster AND the file system
  // between each and every test.
  int argi = 4;
  while (argi < argc)
  {
    // Determine what test is being run
    string testName;
    eatTestArguments(argc, argv, argi, testName);
    cout << "testdriver: running test " << testName << endl;

    // Create cluster configuration.
    clusterConfig = createClusterConfig();
    assert(clusterConfig != nullptr);

    // Launch the cluster.
    clusterConfig->launchCluster(nNodes, 61023);
    cout << "testdriver: started cluster" << endl;
    sleep(1);

    // Create at least one client.
    createClients(nClients);
    cout << "testdriver: created clients" << endl;

    // Connect clients to cluster.
    for (RaftClient* client : clients)
    {
      string host = string("logcabin:") +
	to_string(clusterConfig->clusterPort);
      client->connectToCluster(host);
    }
    cout << "testdriver: connected" << endl;
    sleep(1);

    // --- Test core ---
    // Repeat the test for some number of iterations.
    for (int iter = 0; iter < testArgs.iterations; iter++)
    {
      // Set parameters depending on the test.
      // For BLOCK or KILL, pick a 'marked' node to
      // tamper with.
      int partition = 0;
      float frac = 0;
      int markedNode = clusterConfig->getRandomNodeId();
      string blockSource = "";
      if (testArgs.type == TestType::BLOCK)
      {
	partition = 1;
	frac = testArgs.block.frac;
	if (testArgs.block.mode == TestMode::LEADER)
	{
	  markedNode = prm->getCurrentLeader();
	  cout << "testdriver: going to block leader node " << markedNode << endl;
	}
	blockSource = clusterConfig->getHost(markedNode);
      }

      if (prm->startTest(testName, partition, frac,
			 blockSource) == -1)
      {
	cout << "error: rm->startTest failed" << endl;
      }

      // Start the test by having clients run commands.
      for (RaftClient* client : clients)
      {
	ClientOperations *opsInfo = new ClientOperations;
	opsInfo->client = client;
	opsInfo->nIterations = iter;
	opsInfo->nClients = clients.size();

	// Start a new thread for this client.
	if (pthread_create(&client->thread, nullptr,
			   runClient, opsInfo) != 0)
	{
	  cout << "error: couldn't create client thread" << endl;
	  exit(1);
	}
      }

      // Clients blast packets the whole time.
      sleep(testArgs.time);

      // Kill test! Let the clients spawn first.
      if (testArgs.type == TestType::KILL)
      {
	if (testArgs.kill.mode == TestMode::LEADER)
	{
	  markedNode = prm->getCurrentLeader();
	}

	// Now kill a node, wait, and bring it back.
	cout << "testdriver: killing node " << markedNode << endl;
	clusterConfig->killNode(markedNode);
	sleep(testArgs.time);

	cout << "testdriver: relaunching node " << markedNode << endl;
	clusterConfig->launchNode(markedNode);
	sleep(testArgs.time); // allow some time to do election, etc
      }

      // The test is done; stop the RM.
      if (prm->stopTest(testName, partition, frac,
			blockSource) == -1)
      {
	cout << "error: rm->startTest failed" << endl;
      }

      // Stop the clients.
      for (RaftClient* client : clients)
      {
	client->alive = false;
      }

      // Wait until all clients are finished.
      for (RaftClient* client : clients)
      {
	pthread_join(client->thread, nullptr);
      }
      sleep(1);

      cout << "testdriver: iteration " << iter << " complete" << endl;
    }
    // ---

    // Destroy clients
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
  }

  cout << "testdriver: test complete!" << endl;

  // Shut down the RaftMonitor.
  cout << "testdriver: shutting down RM" << endl;
  prm->sniff->Cancel();
  pthread_join(rmThread, nullptr);

  delete prm->sniff;

  cout << "testdriver: exiting" << endl;

  return 0;
}
