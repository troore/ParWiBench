include ../../common/makefile.config

SRC = TransformPreMain.cpp TransformPrecoder.cpp 
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/dft/dft.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/dft

TARGETS = TPTD.out

#CXXFLAGS += -DDEBUG_FFT
#LINKFLAGS += -lfftw3f

OBJS := $(patsubst %.cpp, %.o, $(SRC))

all: $(TARGETS)

TPTD.out: $(OBJS) 
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
