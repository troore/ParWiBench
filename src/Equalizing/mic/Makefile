include ../../../common/makefile.config

SRC = ../EqualizerMain.cpp Equalizer.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp
SRC += $(SRC_DIR)/lte_phy.cpp
SRC += $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp
OBJS := $(patsubst %.cpp,%.o, $(SRC))

INC_FLAGS += -I. -I.. -I$(LIB_DIR) -I$(SRC_DIR) -I$(LIB_DIR)/refs -I$(LIB_DIR)/power/micpower -I$(LIB_DIR)/timer -I$(LIB_DIR)/check

#CXXFLAGS += -DDEBUG

TARGETS = CEEQ.out

CEEQ.out: $(OBJS)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

