#include <crafter.h>
#include <iostream>
#include <fstream>

using namespace Crafter;

class RaftMonitor {
    private:
        static RaftMonitor rm;

    public:
	static RaftMonitor* getRaftMonitor();

        void start_block(const std::string&, float);
        void clear_block(const std::string&, float);
        int startTest(std::string, int, float, std::string);
        int stopTest(std::string, int, float, std::string);
        void sendPacket(RawLayer*, IP*, TCP*);
        static void callback(Packet*, void*);
        int getPortNum(int);
	
	int getCurrentLeader();

        RaftMonitor(std::string, int, std::string iface="lo");
        RaftMonitor();
	virtual ~RaftMonitor();
        //static void loadConfig(string);
        
        std::string interface;
        std::string implementation;
        std::string logfile;
        int num_hosts;
        std::map<std::pair<int,int>, int> counts;
        std::vector<int> ports;
        std::vector<std::string> ips; //vector of node ips
        int num_elections;
        bool run_test; //are we currently running a test?
	bool alive; //time to shut down?
	Sniffer* sniff; // crafter Sniffer

	int currentLeader; // id of last known leader node
        
    private:  
        int getIndex(std::string);
        std::string getTime();
        
};

int startRaft(std::string, int, std::string iface = "lo");
