include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp

SRC_C = $(SRC) Scrambler_C.cpp

SRC_SIMD = $(SRC) Scrambler_SIMD.cpp

INC_FLAGS += -I./  -I../ -I$(LIB_DIR) -I$(SRC_DIR) -I$(LIB_DIR)/power/micpower

LINKFLAGS += -lpthread

TARGETS = ScrDescr_C.out ScrDescr_SIMD.out

OBJS_C := $(patsubst %.cpp, %.o, $(SRC_C))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))

ifeq ($(TX),1)
	ICCFLAGS += -DScramb
endif

ifeq ($(VEC),0)
	ICCFLAGS += -no-vec
endif

all: $(TARGETS)

ScrDescr_C.out: $(OBJS_C)
	$(ICC) $^ $(LINKFLAGS) $(MMIC) -o $@ -openmp

ScrDescr_SIMD.out: $(OBJS_SIMD)
	$(ICC) $^ $(LINKFLAGS) $(MMIC) -o $@ -openmp


%.o: %.cpp
	$(ICC) -c $< -o $@ $(ICCFLAGS) $(MMIC) -openmp

clean:
	rm -rf $(TARGETS) $(OBJS_C) $(OBJS_SIMD)

