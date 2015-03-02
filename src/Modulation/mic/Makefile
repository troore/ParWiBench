include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ../c/Modulation.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/timer/meas.cpp

SRC_SIMD = $(subst ../c/Modulation.cpp, Modulation_simd.cpp, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

LINKFLAGS += -lpthread

TARGETS = ModDemod.out ModDemod_simd.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))

ifeq ($(TX),1)
	ICPCFLAGS += -DMod
endif

ifeq ($(VEC),1)
	ICPCFLAGS += -vec -vec-report2
else
	ICPCFLAGS += -no-vec
endif

all:$(TARGETS)

ModDemod.out: $(OBJS)
	$(ICPC) $^ $(LINKFLAGS) -o $@

ModDemod_simd.out: $(OBJS_SIMD)
	$(ICPC) -g $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_SIMD)
