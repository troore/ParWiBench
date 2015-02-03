include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ResMapper_simd.cpp test.cpp ../util.cpp
#SRC = $(SRC_DIR)/main_test.cpp ../c/ResMapper_maptable.cpp test.cpp ../util.cpp
SRC += $(SRC_DIR)/lte_phy.cpp $(SRC_DIR)/OFDM/c/OFDM.cpp $(SRC_DIR)/Equalizing/c/Equalizer.cpp
SRC += $(LIB_DIR)/gauss.cpp  $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp
SRC += $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/refs/dmrs.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower
INC_FLAGS += -I$(SRC_DIR)/OFDM -I$(SRC_DIR)/Equalizing

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

ifeq ($(VEC),1)
	ICPCFLAGS += -vec -vec-report2
else
	ICPCFLAGS += -no-vec
endif

#LINKFLAGS += -liomp5
LINKFLAGS += -lpthread
LINKFLAGS += -L$(LIB_DIR)/fft/mic -lfft

#$(warning $(SRC))
#$(warning $(UTIL_DIR))
OBJS := $(patsubst %.cpp,%.o, $(SRC))

TARGETS = ResMapper.out

ifeq ($(TX),1)
	ICPCFLAGS += -DSCMapper
endif

all: $(TARGETS)

ResMapper.out: $(OBJS)
#	cd $(LIB_DIR)/fft/mic && $(MAKE) libfft.a
	$(MAKE) -C $(LIB_DIR)/fft/mic libfft.a
	$(ICPC) $^ $(LINKFLAGS) -o $@


%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

