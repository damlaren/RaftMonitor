#TODO: LogCabin must be built first

lg-client: lg-client.cpp
	g++ -c -std=c++11 -I ../../logcabin lg-client.cpp
	g++ -o test -std=c++11 -I ../../logcabin lg-client.o ../env.o ../client.o ../../logcabin/build/liblogcabin.a -lprotobuf test.cpp
	@exit 0

clean:
	rm -f lg-client.o test.o test
