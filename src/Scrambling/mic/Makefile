include ../../../common/makefile.config

SRC = ../ScrambMain.cpp  Scrambler.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I$(LIB_DIR) -I$(SRC_DIR) -I../ -I./ -I$(LIB_DIR)/timer -I$(LIB_DIR)/power/micpower

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

