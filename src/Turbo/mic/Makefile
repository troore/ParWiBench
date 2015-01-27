include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp test.cpp 
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

ifeq ($(PARD),1)
	SRC += Turbo_pardec.cpp
	CXXFLAGS += -DPARD
endif

SRC += $(SRC_DIR)/lte_phy.cpp

SRC_C = $(SRC) Turbo_C.cpp
SRC_SIMD = $(SRC) Turbo_SIMD.cpp
SRC_OMP = $(SRC) Turbo_OMP.cpp
SRC_all = $(SRC) Turbo_all.cpp


INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

CXXFLAGS += 
LINKFLAGS += 

TARGETS = Turbo_C.out Turbo_SIMD.out Turbo_OMP.out Turbo_all.out

OBJS_C := $(patsubst %.cpp, %.o, $(SRC_C))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))
OBJS_OMP := $(patsubst %.cpp, %.o, $(SRC_OMP))
OBJS_all := $(patsubst %.cpp, %.o, $(SRC_all))

ifeq ($(TX),1)
	ICCFLAGS += -DTurboEnc
endif

all: $(TARGETS)

Turbo_C.out: $(OBJS_C)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

Turbo_SIMD.out: $(OBJS_SIMD)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

Turbo_OMP.out: $(OBJS_OMP)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

Turbo_all.out: $(OBJS_all)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS_C) $(OBJS_SIMD) $(OBJS_OMP) $(OBJS_all)
