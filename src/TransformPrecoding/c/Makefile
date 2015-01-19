include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp TransformPrecoder.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/dft/c/dft.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/dft

TARGETS = TPTD.out

#CXXFLAGS += -DDEBUG_FFT
#LINKFLAGS += -lfftw3f

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DTransformPre
endif

all: $(TARGETS)

TPTD.out: $(OBJS) 
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
