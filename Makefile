
RaftMonitor: RaftMonitor.cpp
	g++ -o RaftMonitor RaftMonitor.cpp -ltins

clean:
	rm -f RaftMonitor.o
	rm -f RaftMonitor
