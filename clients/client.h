#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

#include <pthread.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <map>

// Returns a char* copy of argument.
// Allocates new memory which must be delete[]d.
char* copyStr(const char* src);

/*
 * Configuration for packet loss corresponding to an
 * entry in iptables. On creation, a rule to drop
 * a fraction <frac> of outbound packets from <srcAddr>
 * is added to iptables. The rule is automatically
 * removed when this object is destroyed.
 *
 * It is important to destroy these objects to remove
 * extra entries from iptables after a test is done.
 */
class PacketDropConfig
{
 private:
  bool dropping;
  std::string srcAddr;
  float frac;

 public:
  PacketDropConfig(const std::string& s, float f);
  ~PacketDropConfig();

  // Start or stop dropping packets.
  void startDropping();
  void stopDropping();
};

/*
 * Raft cluster configuration base class. Provides
 * functions that aren't specific to any one Raft
 * node or client, including:
 * - Setting up and tearing down clusters
 * - Helper functions for creating or stopping processes
 */
class RaftClusterConfig
{
public:
  // Should be set by an implementing client to the
  // port number used for sending RAFT messages by the
  // cluster's nodes
  int clusterPort;

  // Oops. I actually did forget this time.
  virtual ~RaftClusterConfig(){};

  // Create a subprocess and return its pid
  // (or -1 on failure).
  // - path: file path to executable
  // - args: program arguments. Must follow conventions:
  //         1. first arg = executable path
  //         2. last arg = nullptr
  pid_t createProcess(const char* path,
		      char* const args[]);

  // Kill subprocess, return true on success.
  bool stopProcess(pid_t pid);

  // Launch a Raft cluster for the client to use,
  // placing nodes on loopback interfaces
  // 192.168.2.{1,2,3...} for however many nodes
  // are requested, listening on passed port.
  virtual void launchCluster(int numNodes, int port) = 0;

  // Should contain an implementation that shuts down a RAFT cluster of the
  // argument size currently running.
  virtual void stopCluster(int numNodes) = 0;
};

/*
 * Raft client base class. Our test framework calls
 * various client implementations through a common
 * interface. Each client class must provide those
 * operations for a specific Raft implementation.
 * Each client is spun off in its own thread.
 */
class RaftClient
{
 public:
  // Unique ID assigned to this client.
  // Keep it >= 1.
  int clientId;

  // Thread on which this client runs operations.
  pthread_t thread;

  // Assign ID to this client.
  RaftClient(int id);

  // Never forget.
  virtual ~RaftClient(){};

  // Run test operations: just do some writes in a
  // KV store.
  void runTestOperations(int nIterations, int nClients);

  // --- Abstract functions ---
  
  // Set up a client associated with this object.
  virtual bool createClient(RaftClusterConfig *config) = 0;

  // Shut it down.
  virtual void destroyClient() = 0;
  
  // Connect to a RAFT cluster using this client's
  // hosts vector and indicate success.
  // TODO: remove hosts string, get from cluster config
  virtual bool connectToCluster(const std::string& hosts) = 0;

  // At a bare minimum, a test client should be able
  // to write some data to the cluster, and indicate
  // whether that write failed or succeeded.
  // - path: Path to file to write.
  //         For success, the directory must exist.
  // - contents: Value to write to file.
  virtual bool writeFile(const std::string& path,
			 const std::string& contents) = 0;

  // Read file contents, return as string.
  virtual std::string readFile(const std::string& path) = 0;

  // --- Variables ---

  // Raft cluster configuration must be passed in during creation.
  RaftClusterConfig* clusterConfig;
};

/*
 * Arguments for spawning a separate client thread
 * to do operations on the Raft implementation.
 */
typedef struct ClientOperations
{
  RaftClient *client; // Which client to run ops on
  int nIterations; // # of times to do the operation
  int nClients; // total # of clients running
};

// Function to start an independent client.
// <arg> refers to a ClientOperations.
void* runClient(void *arg);

#endif
