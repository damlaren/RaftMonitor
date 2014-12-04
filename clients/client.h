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

  // Read file contents, return as string.
  virtual std::string readFile(const std::string& path) = 0;
};

#endif
