SUBDIRS = packetutils RaftMonitor

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
	
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard */*.cpp))
OBJECTS += $(patsubst %.cpp, %.o, $(wildcard *.cpp))

RaftMonitor: $(OBJECTS)
	g++ -o RaftMonitor $^ -ltins

clean:
	rm -f RaftMonitor.o
	rm -f RaftMonitor
