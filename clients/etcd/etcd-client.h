#ifndef RM_ETCD_CLIENT_H
#define RM_ETCD_CLIENT_H

#include <map>
#include "../client.h"

class EtcdClusterConfig : public RaftClusterConfig {
 public:
  std::map<int, pid_t> id2pid;

  std::string etcdPath;

  std::string dataPathRoot;

  // --- functions ---

  virtual void launchCluster(int numNodes, int port);

  virtual void stopCluster();
  
  std::string getAddress();
  
  EtcdClusterConfig();

  std::string getPeerPort(int nodeNumber, int port);
  std::string getPublicPort(int nodeNumber, int port);
  std::string getPeerString(int numNodes, int thisNode,
			    int port);

  virtual std::string getHost(int nodeId);

  virtual std::string getHostPort(int nodeId);

  virtual bool launchNode(int id);

  virtual bool killNode(int id);
};

class EtcdRaftClient : public RaftClient {
 public:
  EtcdRaftClient(int id);
  bool createClient(RaftClusterConfig *config);
  void destroyClient();
  bool connectToCluster(const std::string& hosts);
  bool writeFile(const std::string& path,
		 const std::string& contents);
  std::string readFile(const std::string& path);
  EtcdClusterConfig* clusterConfig;
  std::string cur_leader;
};

#endif
