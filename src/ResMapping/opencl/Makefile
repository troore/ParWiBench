include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ResMapper.cpp test.cpp ../util.cpp
SRC += $(SRC_DIR)/OFDM/c/OFDM.cpp $(SRC_DIR)/Equalizing/c/Equalizer.cpp
SRC += $(LIB_DIR)/gauss.cpp  $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/refs/dmrs.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/opencl/clutil.cpp $(LIB_DIR)/check/check.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)
INC_FLAGS += -I$(SRC_DIR)/OFDM -I$(SRC_DIR)/Equalizing

LINKFLAGS += -L$(OPENCL_SDK_HOME)/lib64 -lOpenCL

LINKFLAGS += -L$(LIB_DIR)/fft/c -lfft


#$(warning $(SRC))
#$(warning $(UTIL_DIR))
OBJS := $(patsubst %.cpp,%.o, $(SRC))

TARGETS = ResMapper.out

ifeq ($(TX),1)
	CXXFLAGS += -DSCMapper
endif

all: $(TARGETS)

ResMapper.out: $(OBJS)
	$(MAKE) -C $(LIB_DIR)/fft/c libfft.a
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

