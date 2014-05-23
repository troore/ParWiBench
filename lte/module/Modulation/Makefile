include ../../../common/makefile.config

SRC = Modulation.cpp main.cpp
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp

SRC += $(LTE_DIR)/LTEUplink/BSPara.cpp $(LTE_DIR)/LTEUplink/UserPara.cpp

INC_FLAGS += -I$(LTE_DIR)/LTEUplink

TARGETS = ModDemod.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ModDemod.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
