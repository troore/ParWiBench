include ../../../../common/makefile.config

SRC = ../TurboMain.cpp ../c/Turbo.cpp ParTurbo.cpp
SRC += $(LIB_DIR)/FIFO.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/opencl/clutil.cpp

SRC += $(LTE_DIR)/src/LTEUplink/lte_phy.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include -I$(LIB_DIR) -I$(LIB_DIR)/opencl -I$(AMD_SDK_HOME)/include

#CXXFLAGS +=
CXXFLAGS += -DPARTURBO
CXXFLAGS += -DGOLD
#CXXFLAGS += -DDEBUG
LINKFLAGS += -L$(AMD_SDK_HOME)/lib -lOpenCL

TARGETS = ParTurbo.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ParTurbo.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
