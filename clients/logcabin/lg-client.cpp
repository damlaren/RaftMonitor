/*
 * Implementation of LogCabinRaftClient.
 */

#include "lg-client.h"

bool LogCabinRaftClient::createClient()
{
  cluster = nullptr;
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
