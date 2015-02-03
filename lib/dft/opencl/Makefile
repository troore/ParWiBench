
include ../../../common/makefile.config

SRC = dft.cpp test.cpp ../util.cpp
SRC += ../../opencl/clutil.cpp
SRC += $(LIB_DIR)/check/check.cpp $(LIB_DIR)/timer/meas.cpp

OBJS = $(patsubst %.cpp, %.o, $(SRC))

INC_DIRS = . ../ $(LIB_DIR)

CXXFLAGS += $(INC_DIRS:%=-I%)

LINKFLAGS = -lOpenCL

TARGETS = dft

ifeq ($(NV),1)
	CXXFLAGS += -I/usr/local/cuda/include
	CXXFLAGS += -DNV
else
	CXXFLAGS += -I/opt/AMDAPP/include
endif

dft: $(OBJS)
	$(CXX) -o $@ $^ $(LINKFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
