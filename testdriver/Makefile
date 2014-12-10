# Dependencies:
# - pthread
# - protobuf
# - logcabin
# - everything under the clients directory

testdriver: testdriver.cpp ../clients/*.o ../clients/logcabin/lg-client.o
	g++ -o testdriver -std=c++11 ../clients/logcabin/lg-client.o ../clients/*.o ../logcabin/build/liblogcabin.a -lprotobuf -lpthread testdriver.cpp

clean:
	rm -f *.o testdriver
