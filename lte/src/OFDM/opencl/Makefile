include ../../../../common/makefile.config

AMD_SDK_HOME := /opt/AMDAPP

SRC = OFDM.cpp ../OFDMMain.cpp $(LIB_DIR)/fft/opencl/fft.cpp $(LIB_DIR)/opencl/clutil.cpp

SRC += $(LIB_DIR)/FIFO.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp

SRC += $(LTE_DIR)/src/LTEUplink/lte_phy.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include -I$(LIB_DIR)/opencl -I$(LIB_DIR)/fft/include

INC_FLAGS += -I$(AMD_SDK_HOME)/include

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = OFDM.out

#LINKFLAGS += -lfftw3f
LINKFLAGS += -L$(AMD_SDK_HOME)/lib -lOpenCL

all: $(TARGETS)

OFDM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

