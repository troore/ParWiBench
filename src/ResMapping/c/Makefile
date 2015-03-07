include ../../../common/makefile.config

#SRC = $(SRC_DIR)/main_test.cpp ResMapper.cpp $(LIB_DIR)/refs/dmrs.cpp test.cpp ../util.cpp
SRC = $(SRC_DIR)/main_test.cpp ResMapper_maptable.cpp $(LIB_DIR)/refs/dmrs.cpp test.cpp ../util.cpp
# check
SRC += $(SRC_DIR)/OFDM/c/OFDM.cpp $(SRC_DIR)/Equalizing/c/Equalizer.cpp
SRC += $(LIB_DIR)/gauss.cpp  $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)
# check
INC_FLAGS += -I$(SRC_DIR)/OFDM -I$(SRC_DIR)/Equalizing

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
	$(MAKE) -C $(LIB_DIR)/fft/c clean 
	rm -rf $(TARGETS) $(OBJS)

