include ../../../common/makefile.config

SRC = ofdm.cpp main.cpp 

SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/GeneralFunc.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp

SRC += $(LTE_DIR)/LTEUplink/BSPara.cpp $(LTE_DIR)/LTEUplink/UserPara.cpp

INC_FLAGS += -I$(LTE_DIR)/LTEUplink

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = ofdm.out

LINKFLAGS += -lfftw3f

all: $(TARGETS)

ofdm.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

