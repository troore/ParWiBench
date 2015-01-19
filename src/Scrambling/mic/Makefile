include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp  Scrambler.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/check/check.cpp

INC_FLAGS += -I./  -I../ -I$(LIB_DIR) -I$(SRC_DIR) -I$(LIB_DIR)/power/micpower

LINKFLAGS += -lpthread

TARGETS = ScrDescr.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	ICCFLAGS += -DScramb
endif

ScrDescr.out: $(OBJS)
	$(ICC) $^ $(LINKFLAGS) $(MMIC) -o $@ -g -openmp

%.o: %.cpp
	$(ICC) -c $< -o $@ $(ICCFLAGS) $(MMIC) -g -openmp

clean:
	rm -rf $(TARGETS) $(OBJS)

