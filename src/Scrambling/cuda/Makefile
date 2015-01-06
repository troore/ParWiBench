include ../../../common/makefile.config

NVCC = /usr/local/cuda/bin/nvcc
LINKFLAGS += -L/usr/local/cuda/lib64 -lcudart

SRC = ../ScrambMain.cpp
CUDA_SRC = Scrambler.cu
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/meas.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(LIB_DIR) -I$(SRC_DIR)

TARGETS = ScrDescr.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DScramb
endif

ScrDescr.out: $(OBJS) Scrambler.cuo
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

%.cuo: %.cu
	$(NVCC) -c $< -o $@ $(CXXFLAGS) -arch=sm_35

clean:
	rm -rf $(TARGETS) $(OBJS)

