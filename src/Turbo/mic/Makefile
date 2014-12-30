include ../../../common/makefile.config
SRC = $(LIB_DIR)/check/check.cpp
SRC += ../TurboMain.cpp Turbo.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/meas.cpp

ifeq ($(PARD),1)
	SRC += Turbo_pardec.cpp
	CXXFLAGS += -DPARD
endif

SRC += $(SRC_DIR)/lte_phy.cpp


INC_FLAGS += -I$(LIB_DIR)/check -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)

CXXFLAGS += 
LINKFLAGS += 

TARGETS = Turbo.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DTurboEnc
endif

Turbo.out: $(OBJS)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
