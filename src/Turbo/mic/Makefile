include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp Turbo.cpp test.cpp 
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

ifeq ($(PARD),1)
	SRC += Turbo_pardec.cpp
	CXXFLAGS += -DPARD
endif

SRC += $(SRC_DIR)/lte_phy.cpp


INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

CXXFLAGS += 
LINKFLAGS += 

TARGETS = Turbo.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	ICCFLAGS += -DTurboEnc
endif

Turbo.out: $(OBJS)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
