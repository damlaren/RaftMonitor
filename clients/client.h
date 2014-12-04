#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

/*
 * RAFT client base class. Our test framework calls
 * various client implementations through a common
 * interface. Each client class must provide those
 * operations for a specific RAFT implementation.
 * Each client is spun off in its own thread.
 */

#include <string>

class RaftClient
{
 public:
  // Never forget.
  virtual ~RaftClient(){};
  
  // Set up a client associated with this object.
  virtual bool createClient() = 0;

  // Shut it down.
  virtual void destroyClient() = 0;
  
  // Connect to a RAFT cluster and indicate success.
  // - hosts: Addresses of hosts in cluster; for
  //          example, "logcabin:61023".
  virtual bool connectToCluster(const std::string& hosts) = 0;
  
  // At a bare minimum, a test client should be able
  // to write some data to the cluster, and indicate
  // whether that write failed or succeeded.
  // - path: Path to file to write.
  //         For success, the directory must exist.
  // - contents: Value to write to file.
  virtual bool writeFile(const std::string& path,
			 const std::string& contents) = 0;

  // Should be set by an implementing client to the port number used
  // for sending RAFT messages by the cluster's nodes
  int clusterPort;

  // Should contain an implementation that launches a RAFT cluster for the
  // client to use, placing nodes on loopback interfaces 192.168.2.{1,2,3...}
  // for however many nodes are requested.
  virtual void launchCluster(int numNodes) = 0;


  // Should contain an implementation that shuts down a RAFT cluster of the
  // argument size currently running.
  virtual void stopCluster(int numNodes) = 0;

  // Read file contents, return as string.
  virtual std::string readFile(const std::string& path) = 0;
};

#endif
