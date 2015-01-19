include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp Modulation.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/timer/meas.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/power/micpower

TARGETS = ModDemod.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	ICCFLAGS += -DMod
endif

ModDemod.out: $(OBJS)
	$(ICC) -openmp -g $^ $(LINKFLAGS) $(MMIC) -o $@

%.o: %.cpp
	$(ICC) -openmp -g $(MMIC) -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
