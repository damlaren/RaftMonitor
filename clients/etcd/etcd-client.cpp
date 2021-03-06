#include "../env.h"
#include "etcd-client.h"
#include <assert.h>
#include <cstdio>
#include <iostream>

std::string getRequestURL(std::string path, std::string leaderUrl) {
  return std::string(leaderUrl + "/v2/keys" + path);
}

EtcdClusterConfig::EtcdClusterConfig() {
}

std::string EtcdClusterConfig::getPeerPort(int nodeNumber, int port) {
  return std::to_string(port + nodeNumber);
}

std::string EtcdClusterConfig::getPublicPort(int nodeNumber, int port) {
  return std::to_string(port + nodeNumber - 1000);
}

std::string EtcdClusterConfig::getAddress() {
  return "192.168.2.1";
}

std::string EtcdClusterConfig::getHost(int nodeId)
{
  return getAddress();
}

std::string EtcdClusterConfig::getHostPort(int nodeId)
{
  return getHost(nodeId) + std::string(":") +
    std::to_string(clusterPort);
}

std::string EtcdClusterConfig::getPeerString(int numNodes, int thisNode, int port) {
  std::string peerStr = std::string("");
  bool first = true;
  for (int i = 1; i <= numNodes; i++) {
    if (i != thisNode) {
      if (!first) {
	peerStr += ",";
      } else {
	first = false;
      }
      peerStr += ((getAddress() + ":") + getPeerPort(i, port));
    }
  }
  return peerStr;
}

bool EtcdClusterConfig::launchNode(int nodeNum) {
  const std::string program = RaftEnv::rootDir + "/etcd-v0.4.6-linux-amd64/etcd";
  const std::string machineName = std::string("machine") + std::to_string(nodeNum);

  char* progCopy = copyStr(program.c_str());
  char* peerAddr = copyStr((getAddress() + std::string(":") + getPeerPort(nodeNum, clusterPort)).c_str());
  char* pubAddr = copyStr((getAddress() + std::string(":") + getPublicPort(nodeNum, clusterPort)).c_str());
  char* dataDir = copyStr((dataPathRoot + std::string("/") + machineName).c_str());
  char* peerCopy = copyStr(getPeerString(numNodes, nodeNum, clusterPort).c_str());
  char* machCopy = copyStr(machineName.c_str());
  
  char* const args[] = { progCopy, "-peer-addr", peerAddr, "-addr", pubAddr, "-peers", peerCopy, "data-dir", dataDir, "-name", machCopy, nullptr };
  pid_t pid = createProcess(program.c_str(), args);

  delete[] progCopy;
  delete[] peerAddr;
  delete[] pubAddr;
  delete[] dataDir;
  delete[] peerCopy;
  delete[] machCopy;

  if (pid == -1) {
    std::cerr << "Failed to launch subprocess" << std::endl;
    return false;
  }

  id2pid[nodeNum] = pid;

  return true;
}

void EtcdClusterConfig::launchCluster(int nNodes, int port) {
  const std::string program = RaftEnv::rootDir + "/etcd-v0.4.6-linux-amd64/etcd";
  id2pid.clear();

  clusterPort = port; 
  numNodes = nNodes;

  dataPathRoot = RaftEnv::rootDir + std::string("/teststorage");
  executeCommand((std::string("rm -rf ") + dataPathRoot + "/machine*").c_str());

  char* progCopy = copyStr(program.c_str());
  char* peerAddr = copyStr((getAddress() + std::string(":") + getPeerPort(1, port)).c_str());
  char* pubAddr = copyStr((getAddress() + std::string(":") + getPublicPort(1, port)).c_str());
  char* dataDir = copyStr((dataPathRoot + std::string("/machine1")).c_str());

  char* const args[] = { progCopy, "-peer-addr", peerAddr, "-addr", pubAddr, "-data-dir", dataDir,
			 "-name", "machine1", nullptr };

  std::cout << "launching etcd node:";
  char* arg = args[0];
  int i = 0;
  while (arg)
  {
    std::cout << " " << arg;
    arg = args[++i];
  }
  std::cout << std::endl;

  // Start node 1
  pid_t pid = createProcess(program.c_str(), args);

  delete[] progCopy;
  delete[] peerAddr;
  delete[] pubAddr;
  delete[] dataDir;

  if (pid == -1) {
    std::cerr << "Error: failed to launch cluster; exiting" << std::endl;
    exit(1);
  }

  id2pid[1] = pid;

  for (int i = 2; i <= numNodes; i++) {
    if (!launchNode(i)) {
      std::cerr << "Error: failed to launch cluster; exiting" << std::endl;
      exit(1);
    }
  }
}

bool EtcdClusterConfig::killNode(int nodeNum) {
  if (id2pid.find(nodeNum) == id2pid.end()) {
    return false;
  }
  pid_t pid = id2pid[nodeNum];
  id2pid.erase(nodeNum);
  return stopProcess(pid);
}

void EtcdClusterConfig::stopCluster() {
  for (int i = 1; i <= numNodes; i++) {
    if (!killNode(i)) {
      std::cerr << "Failed to kill node: " << i << " with pid: " << id2pid[i] << std::endl;
    }
  }
}

EtcdRaftClient::EtcdRaftClient(int id) :
  RaftClient(id)
{
}

/* Extract the first 'value' shown in the given output. */
std::string extractFirstValue(const std::string& output) {
  size_t loc = output.find("value");
  size_t cloc = output.find(":", loc);
  size_t q1loc = output.find("\"", cloc);
  size_t q2loc = output.find("\"", q1loc+1);
  char valbuf[512] = {'0'};
  output.copy(valbuf, (q2loc - q1loc)-1, q1loc+1);
  std::string val = std::string(valbuf);
  return val;
}

bool EtcdRaftClient::writeFile(const std::string& path, const std::string& contents) {
  std::string cmd = std::string("curl -L http://") + getRequestURL(path, cur_leader) + std::string(" -XPUT -d value=") + contents;
  std::cout << "writing: " << cmd << std::endl;
  auto pipe = popen(cmd.c_str(), "r");
  char line[512];
  std::string res;
  bool atypeGood = false;
  while (fgets(line, 512, pipe)) {
    std::string ln = std::string(line);
    res += ln;
    if (!atypeGood) {
      if (ln.find("action") != std::string::npos) { // This tells us we're looking at possibly valid output.
	size_t loc = ln.find(":");
	char atype[32];
	std::string ats;
        ln.copy(atype, 5, loc+1);
	ats = std::string(atype);
	if (ats.find(std::string("\"set\"")) != std::string::npos) {
	  atypeGood = true;
	}
      }
    }    
  }
  pclose(pipe);
  std::cout << "agood: " << atypeGood << std::endl;
  if (atypeGood) {
    std::string val = extractFirstValue(res);
    std::cout << "Val: " << val << std::endl;
    if (val == contents) {
      return true;
    }
  }
  return false;
}

std::string EtcdRaftClient::readFile(const std::string& path) {
  std::string cmd = std::string("curl -L http://") + getRequestURL(path, cur_leader);
  std::cout << "reading: " << cmd << std::endl;
  auto pipe = popen(cmd.c_str(), "r");
  char line[512];
  std::string res;
  bool atypeGood = false;
  while (fgets(line, 512, pipe)) {
    std::string ln = std::string(line);
    res += ln;
    if (!atypeGood) {
      if (ln.find("action") != std::string::npos) { // This tells us we're looking at possibly valid output.
	size_t loc = ln.find(":");
	char atype[32];
	std::string ats;
	ln.copy(atype, 32, loc+1);
	ats = std::string(atype);
	if (ats.find(std::string("\"get\"")) != std::string::npos) {
	  atypeGood = true;
	}
      }
    }    
  }
  pclose(pipe);
  if (atypeGood) {
    std::string val = extractFirstValue(res);
    return val;
  }
  return std::string("");
}

bool EtcdRaftClient::connectToCluster(const std::string& hosts) {
  /* Do nothing. */
  return true; // Except that.
}

bool EtcdRaftClient::createClient(RaftClusterConfig *config)
{
  clusterConfig = static_cast<EtcdClusterConfig*>(config);
  assert(clusterConfig);

  cur_leader = clusterConfig->getAddress() +
    std::string(":") +
    clusterConfig->getPublicPort(1, clusterConfig->clusterPort);
  return true;
}

void EtcdRaftClient::destroyClient()
{
}
