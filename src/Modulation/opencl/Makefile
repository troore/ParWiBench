include ../../../common/makefile.config

SRC = ../ModMain.cpp Modulation.cpp

SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/opencl/clutil.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I.. -I$(SRC_DIR) -I$(LIB_DIR)

LINKFLAGS += -lOpenCL

TARGETS = ModDemod.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DMod
endif

ModDemod.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
