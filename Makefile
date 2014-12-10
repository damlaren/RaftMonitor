SUBDIRS = packetutils RaftMonitor clients testdriver

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard packetutils/*.cpp))
OBJECTS += $(patsubst %.cpp, %.o, $(wildcard *.cpp))

RaftMonitor: $(OBJECTS)
	g++ -o RaftMonitor $^ -ltins
	#g++ -o RaftMonitor $^ -lcrafter

clean:
	rm -f $(OBJECTS)
	rm -f RaftMonitor
