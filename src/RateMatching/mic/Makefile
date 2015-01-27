include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp
SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC_C = $(SRC) RateMatcher_C.cpp
SRC_SIMD = $(SRC) RateMatcher_SIMD.cpp

INC_FLAGS += -I../ -I. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

#CXXFLAGS += -DDEBUG_INTL

TARGETS = RM_C.out RM_SIMD.out

OBJS_C := $(patsubst %.cpp, %.o, $(SRC_C))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))


ifeq ($(TX),1)
	CXXFLAGS += -DTxRateM
	ICCFLAGS += -DTxRateM
endif

all: $(TARGETS)

RM_C.out: $(OBJS_C)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

RM_SIMD.out: $(OBJS_SIMD)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@


%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS_C) $(OBJS_SIMD)

