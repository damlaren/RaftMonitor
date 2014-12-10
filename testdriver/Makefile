# Dependencies: A lot of things need to be built first.
# - logcabin
# - everything under the clients directory

testdriver: testdriver.cpp
	g++ -o testdriver -std=c++11 ../clients/logcabin/lg-client.o ../clients/*.o ../logcabin/build/liblogcabin.a -lprotobuf testdriver.cpp

clean:
	rm -f *.o testdriver
