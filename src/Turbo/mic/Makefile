include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ../c/Turbo_cache.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

ifeq ($(PARD),1)
	SRC += Turbo_pardec.cpp
	CXXFLAGS += -DPARD
endif

SRC += $(SRC_DIR)/lte_phy.cpp

SRC_SIMD = $(subst ../c/Turbo_cache, Turbo_simd, $(SRC))
#$(warning $(SRC))
#$(warning $(SRC_SIMD))
SRC_OMP = $(subst ../c/Turbo_cache, ../c/Turbo_omp, $(SRC))
#$(warning $(SRC_OMP))

OBJS := $(patsubst %.cpp,%.o, $(SRC))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))
OBJS_OMP := $(patsubst %.cpp, %.o, $(SRC_OMP))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

ifeq ($(VEC),1)
	ICPCFLAGS += -vec -vec-report2
else
	ICPCFLAGS += -no-vec
endif

LINKFLAGS += -lpthread

TARGETS = Turbo.out Turbo_simd.out Turbo_omp.out

ifeq ($(TX),1)
	ICCFLAGS += -DTurboEnc
endif

all: $(TARGETS)


Turbo.out: $(OBJS)
	$(ICPC) $^ $(LINKFLAGS) -o $@

Turbo_simd.out: $(OBJS_SIMD)
	$(ICPC) $^ $(LINKFLAGS) -o $@

Turbo_omp.out: $(OBJS_OMP)
	$(ICPC) $^ $(LINKFLAGS) -openmp -o $@

../c/Turbo_omp.o: ../c/Turbo_omp.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS) -openmp

%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_SIMD) $(OBJS_OMP)
