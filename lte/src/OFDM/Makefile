include ../../../common/makefile.config

SRC = OFDMMain.cpp OFDM.cpp

SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/GeneralFunc.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp

SRC += $(LTE_DIR)/src/LTEUplink/BSPara.cpp $(LTE_DIR)/src/LTEUplink/UserPara.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = OFDM.out

LINKFLAGS += -lfftw3f

all: $(TARGETS)

OFDM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

