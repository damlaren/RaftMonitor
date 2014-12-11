#ifndef LG_CLIENT_H
#define LG_CLIENT_H

#include <map>
#include "../client.h"

class LogCabinRaftClusterConfig : public RaftClusterConfig
{
public:
  // Map node ID (1, 2, ...) to process pid.
  std::map<int, pid_t> id2pid;

  // Location of configuration file.
  std::string confFile;

  // Write configuration file (logcabin.conf) for nodes.
  bool writeConfFile(const std::string& storageDir);

  virtual void launchCluster(int numNodes, int port);

  virtual void stopCluster();

  virtual std::string getHost(int nodeId);

  virtual std::string getHostPort(int nodeId);

  virtual bool launchNode(int id);

  virtual bool killNode(int id);
};

namespace LogCabin {
  namespace Client {
    class Cluster;
  }
}

/*
 * The LogCabin RAFT client is the first and probably
 * the easiest one to implement. It is adapted from
 * the "HelloWorld" example in LogCabin. The entire
 * client can be implemented using C++.
 */
class LogCabinRaftClient : public RaftClient
{
 public:
  LogCabin::Client::Cluster *cluster; // Raft Cluster. This is an object from ongaro's LogCabin.

  LogCabinRaftClient(int id);

  virtual bool createClient(RaftClusterConfig *config);

  virtual void destroyClient();

  virtual bool connectToCluster(const std::string& hosts);

  virtual bool writeFile(const std::string& path,
			 const std::string& contents);

  virtual std::string readFile(const std::string& path);
};

#endif
