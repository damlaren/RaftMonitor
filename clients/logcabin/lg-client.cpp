/*
 * Implementation of LogCabinRaftClient.
 */

#include "../env.h"
#include "lg-client.h"
#include <iostream>

std::string LogCabinRaftClusterConfig::getHost(int nodeNumber)
{
  return std::string("192.168.2.") + std::to_string(nodeNumber);
}

void LogCabinRaftClusterConfig::launchCluster(int numNodes)
{
  id2pid.clear();

  // TODO: set up loopback interfaces

  // TODO: set up redirects

  // TODO: logcabin.conf must already exist. Initialize it?

  // start individual Raft nodes
  for (int id = 1; id <= numNodes; id++)
  {
    if (!launchNode((RaftEnv::rootDir + std::string("/logcabin/logcabin.conf")).c_str(), id))
    {
      std::cerr << "Failed to launch cluster; exiting" << std::endl;
      exit(1);
    }
  }

  // TODO reconfigure once all nodes have joined
}

void LogCabinRaftClusterConfig::stopCluster(int numNodes)
{
  for (int id = 1; id <= numNodes; id++)
  {
    if (!killNode(id))
    {
      std::cerr << "Failed to kill node: " << id << std::endl;
    }
  }

  // TODO: shut down redirects
  
  // TODO: shut down loopback interfaces
}

bool LogCabinRaftClusterConfig::launchNode(const char* confFile, int id)
{
  const std::string program = RaftEnv::rootDir + "/logcabin/build/LogCabin";
  const std::string idStr = std::to_string(id);

  // sigh... need char*, not const char*.
  char* idCopy = copyStr(idStr.c_str());
  char* confCopy = copyStr(confFile);
  char* progCopy = copyStr(program.c_str());

  char* const args[] = {progCopy, "--id", idCopy, "--config", confCopy, nullptr};
  pid_t pid = createProcess(program.c_str(), args);

  delete[] idCopy;
  delete[] confCopy;
  delete[] progCopy;
  if (pid == -1)
  {
    std::cerr << "error: failed to launch subprocess" << std::endl;
    return false;
  }

  id2pid[id] = pid;

  return true;
}

bool LogCabinRaftClusterConfig::killNode(int id)
{
  if (id2pid.find(id) == id2pid.end())
  {
    return false;
  }
  pid_t pid = id2pid[id];
  id2pid.erase(id);
  return stopProcess(pid);
}

bool LogCabinRaftClient::createClient(RaftClusterConfig *config)
{
  cluster = nullptr;
  clusterConfig = config;
  return true;
}

void LogCabinRaftClient::destroyClient()
{
  delete cluster;
}

bool LogCabinRaftClient::connectToCluster(const std::string& hosts)
{
  cluster = new Cluster(hosts);

  return true;
}

bool LogCabinRaftClient::writeFile(const std::string& path,
				   const std::string& contents)
{
  LogCabin::Client::Result result;

  Tree tree = cluster->getTree();
  result = tree.write(path, contents);

  return (result.status == LogCabin::Client::Status::OK);
}

std::string LogCabinRaftClient::readFile(const std::string& path)
{
  std::string content;
  Tree tree = cluster->getTree();
  tree.read(path, content);
  return content;
}
