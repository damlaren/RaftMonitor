#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

#include <sys/types.h>
#include <vector>
#include <string>
#include <map>

/*
 * Raft cluster configuration base class. Provides
 * for setting up or tearing down clusters and
 * Raft nodes.
 */
class RaftClusterConfig
{
public:

  // Create a subprocess and return its pid (or -1 on failure).
  // - path: file path to executable
  // - args: program arguments; MUST END with a nullptr
  pid_t createProcess(const char* path, char* const args[]);

  // Kill subprocess, return true on success.
  bool stopProcess(pid_t pid);

  // Should contain an implementation that launches a RAFT cluster for the
  // client to use, placing nodes on loopback interfaces 192.168.2.{1,2,3...}
  // for however many nodes are requested.
  virtual void launchCluster(int numNodes) = 0;

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
  // Never forget.
  virtual ~RaftClient(){};

  // --- Abstract functions ---
  virtual RaftClusterConfig* getClusterConfig() = 0;
  
  // Set up a client associated with this object.
  virtual bool createClient() = 0;

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

  // Should be set by an implementing client to the
  // port number used for sending RAFT messages by the
  // cluster's nodes
  int clusterPort;
};

#endif
