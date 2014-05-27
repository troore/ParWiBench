include ../../../common/makefile.config

SRC = TransformPreMain.cpp TransformPrecoder.cpp 
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp 

SRC += $(LTE_DIR)/src/LTEUplink/BSPara.cpp $(LTE_DIR)/src/LTEUplink/UserPara.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include 

TARGETS = TPTD.out

LINKFLAGS += -lfftw3f

OBJS := $(patsubst %.cpp, %.o, $(SRC))

all: $(TARGETS)

TPTD.out: $(OBJS) 
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
