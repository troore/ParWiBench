include ../../../common/makefile.config

SRC = main.cpp resmapper.cpp 
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp
SRC += $(LTE_DIR)/LTEUplink/BSPara.cpp $(LTE_DIR)/LTEUplink/UserPara.cpp

INC_FLAGS += -I$(LTE_DIR)/LTEUplink

$(warning $(SRC))
$(warning $(UTIL_DIR))
OBJS := $(patsubst %.cpp,%.o, $(SRC))
$(warning $(OBJS))

TARGETS = resmapper.out

all: $(TARGETS)

resmapper.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

