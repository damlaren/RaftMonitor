#ifndef RM_ETCD_CLIENT_H
#define RM_ETCD_CLIENT_H

#include <map>
#include "../client.h"

class EtcdClusterConfig : public RaftClusterConfig {
 public:
  std::map<int, pid_t> id2pid;

  std::string etcdPath;

  void launchCluster(int numNodes, int port);

  void stopCluster(int numNodes);
  
  std::string getAddress();
  
  EtcdClusterConfig(std::string etcdpath);
  
  std::string getPeerPort(int nodeNumber, int port);
  std::string getPublicPort(int nodeNumber, int port);
};

class EtcdRaftClient : public RaftClient {
 public:
  bool createClient();
  bool destroyClient();
  bool connectToCluster();
  bool writeFile();
  std::string readFile();
  EtcdRaftClient(EtcdClusterConfig* conf, int id);
  EtcdClusterConfig* clusterConfig;
  std::string cur_leader;
};

#endif
