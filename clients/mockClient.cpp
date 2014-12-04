/* A mock client that doesn't actually run any RAFT implementation at all
   but is just intended to test the runner and general setup code. */

#include "client.h"
#include "../RaftTestRunner.h"

class MockClient :  public RaftClient {
public:
  
  MockClient() {
    clusterPort = 12000;
  }
  
  bool createClient() {
    /* Do nothing */
  }

  void destroyClient() {
    /* Do nothing */
  }

  bool connectToCluster(const std::string& hosts) {
    return true;
  }

  bool writeFile(const std::string& path,
		 const std::string& contents) {
    return false;
  }

  void launchCluster(int numNodes) {
    /* Do nothing */
  }

  void stopCluster(int numNodes) {
    /* Do Nothing */
  }

  std::string readFile(const std::string& path) {
    return std::string("");
  }

};

int main(int argc, char** argv) {
  MockClient* mc = new MockClient();
  runTests(mc, 5);
  delete mc;
}
