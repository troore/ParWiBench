include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/timer/meas.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

SRC_C = $(SRC) test_C.cpp Modulation_C.cpp
SRC_SIMD = $(SRC) test.cpp Modulation_SIMD.cpp

TARGETS = ModDemod_C.out ModDemod_SIMD.out

OBJS_C := $(patsubst %.cpp, %.o, $(SRC_C))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))

ifeq ($(TX),1)
	ICCFLAGS += -DMod
endif

all:$(TARGETS)

ModDemod_C.out: $(OBJS_C)
	$(ICC) -openmp -g $^ $(LINKFLAGS) $(MMIC) -o $@

ModDemod_SIMD.out: $(OBJS_SIMD)
	$(ICC) -openmp -g $^ $(LINKFLAGS) $(MMIC) -o $@

%.o: %.cpp
	$(ICC) -openmp -g $(MMIC) -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS_C) $(OBJS_SIMD)
