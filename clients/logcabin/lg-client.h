#ifndef LG_CLIENT_H
#define LG_CLIENT_H

#include "../client.h"
#include "Client/Client.h" // LogCabin RAFT

using LogCabin::Client::Cluster;
using LogCabin::Client::Tree;

/*
 * The LogCabin RAFT client is the first and probably
 * the easiest one to implement. It is adapted from
 * the "HelloWorld" example in LogCabin. The entire
 * client can be implemented using C++.
 */
class LogCabinRaftClient : public RaftClient
{
 private:
  Cluster cluster;
  Tree tree;

 public:
  virtual bool createClient();

  virtual void destroyClient();

  virtual bool connectToCluster(const std::string& hosts);

  virtual bool writeFile(const std::string& path,
			 const std::string& contents);
};

#endif
