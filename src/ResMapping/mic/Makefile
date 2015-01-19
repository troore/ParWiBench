include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp ResMapper_simd.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp  $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/refs/dmrs.cpp
SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/check/check.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/refs -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

ICPCFLAGS += $(MMIC)
LINKFLAGS += $(MMIC)

LINKFLAGS += -liomp5

#$(warning $(SRC))
#$(warning $(UTIL_DIR))
OBJS := $(patsubst %.cpp,%.o, $(SRC))

TARGETS = ResMapper.out

ifeq ($(TX),1)
	ICPCFLAGS += -DSCMapper
endif

all: $(TARGETS)

ResMapper.out: $(OBJS)
	$(ICPC) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICPC) -c $< -o $@ $(ICPCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

