# Dependencies:
# - pthread
# - protobuf
# - logcabin
# - everything under the clients directory

testdriver: testdriver.cpp ../clients/*.o ../clients/logcabin/lg-client.o ../RaftMonitor.o ../packetutils/ongaro.o
	g++ -o testdriver -std=c++11 ../clients/logcabin/lg-client.o ../clients/etcd/etcd-client.o ../clients/*.o ../packetutils/ongaro.o  ../packetutils/etcd.o ../RaftMonitor.o ../logcabin/build/liblogcabin.a -lprotobuf -lpthread -lcrafter testdriver.cpp

clean:
	rm -f *.o testdriver
