
CXXFLAGS := -g -fPIC -I../../../../include
LDFLAGS := -lm

SHLDFLAGS := $(LDFLAGS) -shared -fPIC

OBJS := $(patsubst %.cpp,%.o, $(filter-out pccc_wrapper.cpp, $(wildcard *.cpp)))
$(warning $(OBJS))
SHOBJS := $(patsubst %.cpp,%.o, $(filter-out pccc.cpp, $(wildcard *.cpp)))
$(warning $(SHOBJS))

all: turbo libturbo

turbo: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

libturbo: libturbo.so
	@echo "Generate libturbo successfully"

libturbo.so: $(SHOBJS)
	$(CXX) -o $(@) $^ $(SHLDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJS) $(SHOBJS)
