
CXXFLAGS := -g -I./

LINKFLAGS := -lpthread

TARGETS := hostinfo echoclient echoserveri

all: $(TARGETS)

hostinfo: hostinfo.cpp csapp.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LINKFLAGS)


echoclient: echoclient.cpp csapp.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LINKFLAGS)

echoserveri: echoserveri.cpp csapp.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LINKFLAGS)

clean:
	rm -rf $(TARGETS)
