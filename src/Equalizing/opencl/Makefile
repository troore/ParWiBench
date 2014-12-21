include ../../../common/makefile.config

SRC = ../EqualizerMain.cpp Equalizer.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp $(LIB_DIR)/opencl/clutil.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

OBJS := $(patsubst %.cpp,%.o, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/refs -I$(LIB_DIR)

LINKFLAGS += -lOpenCL

TARGETS = CEEQ.out

CEEQ.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

