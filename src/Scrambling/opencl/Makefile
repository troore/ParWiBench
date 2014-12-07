include ../../../common/makefile.config

#AMD_SDK_HOME := /opt/AMDAPP
CUDA_HOME := /usr/local/cuda

SRC = ../ScrambMain.cpp  Scrambler.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/opencl/clutil.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I.. -I$(SRC_DIR) -I$(CUDA_HOME)/include -I$(LIB_DIR)

#LINKFLAGS += -L$(AMD_SDK_HOME)/lib -lOpenCL
LINKFLAGS += -L$(CUDA_HOME)/lib64 -lOpenCL

TARGETS = ScrDescr.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ScrDescr.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
