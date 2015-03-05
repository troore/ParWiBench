include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp Equalizer_inline.cpp test.cpp ../util.cpp
SRC += $(SRC_DIR)/OFDM/c/OFDM.cpp $(SRC_DIR)/ResMapping/c/ResMapper.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp
SRC += $(SRC_DIR)/lte_phy.cpp
SRC += $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC_SIMD = $(subst Equalizer_inline.cpp, Equalizer_simd.cpp, $(SRC))
$(warning $(SRC_SIMD))
SRC_OMP = $(subst Equalizer_inline.cpp, Equalizer_inline_omp.cpp, $(SRC))
$(warning $(SRC_OMP))

OBJS := $(patsubst %.cpp,%.o, $(SRC))
OBJS_SIMD := $(patsubst %.cpp,%.o, $(SRC_SIMD))
OBJS_OMP := $(patsubst %.cpp,%.o, $(SRC_OMP))

INC_FLAGS += -I. -I.. -I$(LIB_DIR) -I$(SRC_DIR) -I$(LIB_DIR)/refs -I$(LIB_DIR)/power/micpower
INC_FLAGS += -I$(SRC_DIR)/OFDM -I$(SRC_DIR)/ResMapping

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

ifeq ($(VEC),1)
	ICPCFLAGS += -vec -vec-report2
else
	ICPCFLAGS += -no-vec
endif

#CXXFLAGS += -DDEBUG
LINKFLAGS += -lpthread
LINKFLAGS += -L$(LIB_DIR)/fft/mic -lfft

TARGETS = CEEQ.out CEEQ_simd.out CEEQ_omp.out

all:$(TARGETS)

CEEQ.out: $(OBJS)
	$(MAKE) -C $(LIB_DIR)/fft/mic libfft.a
	$(ICPC) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_SIMD) $(OBJS_OMP)

