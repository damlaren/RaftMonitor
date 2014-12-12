RaftMonitor
====================

--- Build Instructions ---

Built on Ubuntu 14.04.

1. Build RAFT reference implementations
   a. LogCabin: Follow instructions on: https://github.com/logcabin/logcabin
   b. Etcd: Binaries are available at: https://github.com/coreos/etcd/releases/

2. Build libcrafter (https://github.com/pellegre/libcrafter)
   * sudo apt-get install autoconf libtool
   * git clone https://github.com/pellegre/libcrafter
   * cd libcrafter/libcrafter
   * ./autogen.sh 
   * make
   * sudo make install
   * sudo ldconfig
   * Modify /usr/local/include/crafter/Utils/CrafterUtils.h - put in #include "TCPConnection.h" at the end

3. Build the system
   * make

--- Run Instructions ---

1. Setup loopback interface: sudo ./setup_lo.sh
2. Run the testdriver. Some examples:

   a. To run a basic test of logcabin with 3 nodes and 3 clients,
      with 1 test iteration where each test phase lasts 1 second:

      sudo ./testdriver/testdriver logcabin 3 3 basic 1 1
   
   b. 2 iterations of a test to kill leader node:

      sudo ./testdriver/testdriver logcabin 3 3 kill 2 1 leader

   c. Block 100% of the packets from a random node for 1 second:

      sudo ./testdriver/testdriver logcabin 3 3 block 1 1 random 1.0
