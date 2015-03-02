include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ../c/Scrambler.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp

SRC_SIMD = $(subst ../c/Scrambler.cpp, Scrambler_simd.cpp, $(SRC))

INC_FLAGS += -I./ -I../ -I$(LIB_DIR) -I$(SRC_DIR) -I$(LIB_DIR)/power/micpower

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

LINKFLAGS += -lpthread

TARGETS = ScrDescr.out ScrDescr_simd.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))
$(warning $(OBJS))
OBJS_SIMD := $(patsubst %.cpp, %.o, $(SRC_SIMD))
$(warning $(OBJS_SIMD))

ifeq ($(TX),1)
	ICPCFLAGS += -DScramb
endif

ifeq ($(VEC),1)
	ICPCFLAGS += -vec -vec-report2
else
	ICPCFLAGS += -no-vec
endif

all: $(TARGETS)

ScrDescr.out: $(OBJS)
	$(ICPC) $^ $(LINKFLAGS) -o $@

ScrDescr_simd.out: $(OBJS_SIMD)
	$(ICPC) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_SIMD)

