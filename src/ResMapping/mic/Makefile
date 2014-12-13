include ../../../common/makefile.config

SRC = ../ResMapMain.cpp ResMapper.cpp 
SRC += $(LIB_DIR)/gauss.cpp  $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/meas.cpp $(LIB_DIR)/refs/dmrs.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/refs

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

