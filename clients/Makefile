SUBDIRS = logcabin

.PHONY: subdirs $(SUBDIRS)

default: client env subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

client: client.o
	g++ -std=c++11 -c client.cpp

env: env.o
	g++ -std=c++11 -c env.cpp
