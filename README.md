cs244b_final_project
====================

--- Build Instructions ---

Built on Ubuntu 14.04.

1. Build RAFT reference implementations
   a. LogCabin: Follow instructions on https://github.com/logcabin/logcabin

2. Build libtins
   * Adapted from instructions at http://libtins.github.io/download/#using
   ** Make sure dependencies are installed
   * Update CMakeCache.txt with location of libtins on system
   * cd to libtins/build
   * cmake ../
   ** TODO: include C++11 features? -DLIBTINS_ENABLE_CXX11=1
   * make
   * sudo make install

3. Build RaftMonitor
   * 'make' in this directory

--- Run Instructions ---

1. Setup loopback interface as on LogCabin
2. 'source setup_monitor.sh' to set up path for running RaftMonitor
