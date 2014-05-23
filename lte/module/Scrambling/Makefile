include ../../../common/makefile.config

SRC = main.cpp  Scrambler.cpp 
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp

SRC += $(LTE_DIR)/LTEUplink/BSPara.cpp $(LTE_DIR)/LTEUplink/UserPara.cpp 

INC_FLAGS += -I$(LTE_DIR)/LTEUplink 

TARGETS = ScrDescr.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ScrDescr.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

