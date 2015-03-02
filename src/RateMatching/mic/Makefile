include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ../c/RateMatcher_ntp.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp
SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC_SIMD = $(subst ../c/RateMatcher_ntp, RateMatcher_simd, $(SRC))
#$(warning $(SRC))
#$(warning $(SRC_SIMD))
SRC_OMP = $(subst ../c/RateMatcher_ntp, ../c/RateMatcher_omp, $(SRC))
#$(warning $(SRC_OMP))

INC_FLAGS += -I../ -I. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

#CXXFLAGS += -DDEBUG_INTL

OBJS := $(patsubst %.cpp,%.o, $(SRC))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))
OBJS_OMP := $(patsubst %.cpp, %.o, $(SRC_OMP))


ifeq ($(TX),1)
	CXXFLAGS += -DTxRateM
	ICCFLAGS += -DTxRateM
endif

TARGETS = RM.out RM_simd.out RM_omp.out

all: $(TARGETS)

RM.out: $(OBJS)
	$(ICPC) $^ $(LINKFLAGS) -o $@

RM_simd.out: $(OBJS_SIMD)
	$(ICPC) $^ $(LINKFLAGS) -o $@

RM_omp.out: $(OBJS_OMP)
	$(ICPC) $^ $(LINKFLAGS) -openmp -o $@

../c/RateMatcher_omp.o: ../c/RateMatcher_omp.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS) -openmp

%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_SIMD) $(OBJS_OMP)
