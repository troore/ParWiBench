include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp  Scrambler.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/opencl/clutil.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I.. -I$(SRC_DIR) -I$(LIB_DIR)

LINKFLAGS += -lOpenCL

TARGETS = ScrDescr.out

ifeq ($(TX),1)
	CXXFLAGS += -DScramb
endif

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ScrDescr.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
