include ../../../common/makefile.config

SRC = ../ModMain.cpp Modulation.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp $(LIB_DIR)/GeneralFunc.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/timer/meas.cpp

INC_FLAGS += -I. -I$(SRC_DIR) -I$(LIB_DIR)/check -I$(LIB_DIR)/power/micpower -I$(LIB_DIR)/timer -I..

TARGETS = ModDemod.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DMod
	ICCFLAGS += -DMod
endif

ModDemod.out: $(OBJS)
	$(ICC) -openmp -g $^ $(LINKFLAGS) $(MMIC) -o $@

%.o: %.cpp
	$(ICC) -openmp -g $(MMIC) -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
