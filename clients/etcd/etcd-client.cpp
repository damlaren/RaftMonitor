#include "etcd-client.h"
#include <cstdio>

EtcdClusterConfig::EtcdClusterConfig(std::string path) {
  etcdPath = path;
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

std::string EtcdClusterConfig::getPeerString(int numNodes, int thisNode, int port) {
  std::string peerStr = std::string("");
  bool first = true;
  for (int i = 1; i <= numNodes) {
    if (i != thisNode) {
      if (!first) {
	peerStr += ";";
      } else {
	first = false;
      }
      peerStr += ((getAddress() + ":") + getPeerPort(i, port));
    }
  }
  return peerStr;
}

void EtcdClusterConfig::launchNode(int numNodes, int nodeNum, std::string dataPathRoot) {

  char* progCopy = copyStr(etcdPath.c_str());
  char* peerAddr = copyStr((getAddress() + getPeerPort(nodeNum, clusterPort)).c_str());
  char* pubAddr = copyStr((getAddress() + getPublicPort(nodeNum, clusterPort)).c_str());
  char* dataDir = copyStr((dataPathRoot + std::string("/machine") + std::to_string(nodeNum)).c_str());
  char* peerCopy = copyStr(getPeerString(numNodes, nodeNum, clusterPort).c_str());
  
  char* const args[] = { progCopy, "-peer-addr", peerAddr, "-addr", pubAddr, "-peers", peerCopy, "data-dir", dataDir };
  pid_t pid = createProcess(etcdPath.c_str(), args);

  delete[] progCopy;
  delete[] peerAddr;
  delete[] pubAddr;
  delete[] dataDir;
  delete[] peerCopy;

  if (pid == -1) {
    std::cerr << "Failed to launch subprocess" << std::endl;
    return false;
  }

  id2pid[nodeNum] = pid;

  return true;
}

void EtcdClusterConfig::launchCluster(int numNodes, int port) {
  id2pid.clear();

  clusterPort = port; 

  std::string dataPathRoot = RaftEnv::rootDir + std::string("/teststorage");

  char* progCopy = copyStr(etcdPath.c_str());
  char* peerAddr = copyStr((getAddress() + getPeerPort(1, port)).c_str());
  char* pubAddr = copyStr((getAddress() + getPublicPort(1, port)).c_str());
  char* dataDir = copyStr((dataPathRoot + std::string("/machine1")).c_str());

  char* const args[] = { progCopy, "-peer-addr", peerAddr, "-addr", pubAddr, "-data-dir", dataDir };
  // Start node 1
  pid_t pid = createProcess(etcdPath.c_str(), args);

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
    if (!launchNode(i, dataPathRoot)) {
      std::cerr << "Error: failed to launch cluster; exiting" << std::endl;
      exit(1);
    }
  }
}

void EtcdClusterConfig::killNode(int nodeNum) {
  if (id2pid.find(nodeNum) == id2pid.end()) {
    return false;
  }
  pid_t pid = id2pid[nodeNum];
  id2pid.erase(nodeNum);
  return stopProcess(pid);
}

void EtcdClusterConfig::stopCluster(int numNodes) {
  for (int i = 1; i <= numNodes; i++) {
    if (!killNode(i)) {
      std::cerr << "Failed to kill node: " << i << " with pid: " << id2pid[i] << std::endl;
    }
  }
}

EtcdRaftClient::EtcdRaftClient(EtcdClusterConfig* configPtr, int id) : RaftClient(id) {
  clusterConfig = configPtr;
}

std::string EtcdRaftCLient::getRequestURL(std::string path, std::string leaderUrl) {
  return std::string(leaderUrl + "/v2" + path);
}

/* Extract the first 'value' shown in the given output. */
std::string extractFirstValue(const std::string& output) {
  size_t loc = output.find("value");
  size_t cloc = output.find(":", loc);
  size_t q1loc = output.find("\"", cloc);
  size_t q2loc = output.find("\"", q1loc+1);
  char valbuf[512];
  output.copy(valbuf, q2loc - q1loc, q1loc);
  std::string val = std::string(valbuf);
  return val;
}

bool EtcdRaftClient::writeFile(const std::string& path, const std::string& contents) {
  auto pipe = popen((std::string("curl -L ") + getRequestURL(path, cur_leader) + std::string(" -XPUT -d value=") + contents).c_str(), "r");
  char line[512];
  std::string res;
  atypeGood = false;
  while (fgets(line, 512, pipe)) {
    std::string ln = std::string(line);
    res += ln;
    if (!atypeGood) {
      if (ln.find("action") != std::string::npos) { // This tells us we're looking at possibly valid output.
	size_t loc = ln.find(":");
	char atype[32];
	std::string ats;
	ln.copy(atype, 32, loc+2);
	ats = std::string(atype);
	if (ats == std::string("\"set\"")) {
	  atypeGood = true;
	}
      }
    }    
  }
  pclose(pipe);
  if (atypeGood) {
    std::string val = extractFirstValue(res);
    if (val == contents) {
      return true;
    }
  }
  return false;
}

std::string EtcdRaftClient::readFile(const std::string& path) {
  auto pipe = popen((std::string("curl -L ") + getRequestURL(path, cur_leader)).c_str(), "r");
   char line[512];
  std::string res;
  atypeGood = false;
  while (fgets(line, 512, pipe)) {
    std::string ln = std::string(line);
    res += ln;
    if (!atypeGood) {
      if (ln.find("action") != std::string::npos) { // This tells us we're looking at possibly valid output.
	size_t loc = ln.find(":");
	char atype[32];
	std::string ats;
	ln.copy(atype, 32, loc+2);
	ats = std::string(atype);
	if (ats == std::string("\"get\"")) {
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

EtcdRaftClient::connectToCluster(const std::string& hosts) {
  /* Do nothing. */
}




