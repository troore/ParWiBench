include ../../../common/makefile.config

SRC = main.cpp  RateMatcher.cpp  SubblockInterleaver_lte.cpp 
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp 
SRC += $(LTE_DIR)/LTEUplink/BSPara.cpp $(LTE_DIR)/LTEUplink/UserPara.cpp 

INC_FLAGS += -I/$(LTE_DIR)/LTEUplink

TARGETS = RM.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

all: $(TARGETS)

RM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

