include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp  RateMatcher_ntp.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/opencl/clutil.cpp $(LIB_DIR)/check/check.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)

LINKFLAGS += -L$(OPENCL_SDK_HOME)/lib64 -lOpenCL


TARGETS = RM.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DTxRateM
endif

all: $(TARGETS)

RM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

