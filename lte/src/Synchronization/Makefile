include ../../../common/makefile.config

SRC = SyncMain.cpp Sync.cpp

SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/GeneralFunc.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp

SRC += $(LTE_DIR)/src/LTEUplink/BSPara.cpp $(LTE_DIR)/src/LTEUplink/UserPara.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = Sync.out

#LINKFLAGS += -lfftw3f

#CXXFLAGS += -DDBUFSZ

all: $(TARGETS)

Sync.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

