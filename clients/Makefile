SUBDIRS = logcabin etcd

.PHONY: subdirs $(SUBDIRS)

default: client env subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

client: client.cpp
	g++ -std=c++11 -c client.cpp

env: env.cpp
	g++ -std=c++11 -c env.cpp

clean:
	rm -f *.o
