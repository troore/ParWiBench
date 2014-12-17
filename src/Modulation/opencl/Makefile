include ../../../common/makefile.config

AMD_SDK_HOME := /opt/AMDAPP

SRC = ../ModMain.cpp Modulation.cpp api.cpp

SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp $(LIB_DIR)/GeneralFunc.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I$(SRC_DIR) -I$(AMD_SDK_HOME)/include -I../

LINKFLAGS += -L$(AMD_SDK_HOME)/lib -IOpenCL -lOpenCL
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
