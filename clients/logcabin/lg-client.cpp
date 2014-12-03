/*
 * Implementation of LogCabinRaftClient.
 */

#include "lg-client.h"

bool LogCabinRaftClient::createClient()
{
  return true;
}

void LogCabinRaftClient::destroyClient()
{
}

bool LogCabinRaftClient::connectToCluster(const std::string& hosts)
{
  cluster = Cluster(hosts);
  tree = cluster.getTree();
}

bool LogCabinRaftClient::writeFile(const std::string& path,
				   const std::string& contents)
{
  LogCabin::Client::Result result;

  result = tree.write(path, contents);

  return (result.status == LogCabin::Client::Status::OK);
}
