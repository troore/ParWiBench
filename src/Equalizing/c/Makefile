include ../../../common/makefile.config

SRC = ../EqualizerMain.cpp Equalizer.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

OBJS := $(patsubst %.cpp,%.o, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/refs

#CXXFLAGS += -DDEBUG

TARGETS = CEEQ.out

CEEQ.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

