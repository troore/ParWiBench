include ../../../../common/makefile.config

AMD_SDK_HOME := /opt/AMDAPP

SRC = ../ScrambMain.cpp  Scrambler.cpp
SRC += $(LIB_DIR)/FIFO.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/opencl/clutil.cpp

SRC += $(LTE_DIR)/src/LTEUplink/lte_phy.cpp

INC_FLAGS += -I$(LTE_DIR)/include -I$(LIB_DIR) -I$(AMD_SDK_HOME)/include 

LINKFLAGS += -L$(AMD_SDK_HOME)/lib -lOpenCL

TARGETS = ScrDescr.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ScrDescr.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

