include ../../../common/makefile.config

SRC = RateMatcherMain.cpp  RateMatcher.cpp  SubblockInterleaver_lte.cpp 
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp 
SRC += $(LTE_DIR)/src/LTEUplink/BSPara.cpp $(LTE_DIR)/src/LTEUplink/UserPara.cpp 

INC_FLAGS += -I. -I/$(LTE_DIR)/include

TARGETS = RM.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

all: $(TARGETS)

RM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

