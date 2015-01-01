include ../../../common/makefile.config

SRC = ../TurboMain.cpp Turbo.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/opencl/clutil.cpp $(LIB_DIR)/meas.cpp $(LIB_DIR)/check/check.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR)/ -I$(LIB_DIR)/check -I$(LIB_DIR)/opencl

LINKFLAGS += -L$(OPENCL_SDK_HOME)/lib -lOpenCL

TARGETS = Turbo.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

Turbo.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
