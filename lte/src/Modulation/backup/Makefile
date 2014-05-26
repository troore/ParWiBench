
CXXFLAGS := -g -fPIC -I../include
LDFLAGS := -lm

SHLDFLAGS := $(LDFLAGS) -shared -fPIC

OBJS := $(patsubst %.cpp,%.o, $(filter-out main_wrapper.cpp, $(wildcard *.cpp)))
$(warning $(OBJS))
SHOBJS := $(patsubst %.cpp,%.o, $(filter-out main.cpp, $(wildcard *.cpp)))
#$(warning $(SHOBJS))

#all: turbo libturbo

mod: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

#libturbo: libturbo.so
#	@echo "Generate libturbo successfully"

#libturbo.so: $(SHOBJS)
#	$(CXX) -o $(@) $^ $(SHLDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJS) $(SHOBJS)
