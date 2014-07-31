include ../../../common/makefile.config

SRC = ResMapMain.cpp ResMapper.cpp 
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/GeneralFunc.cpp
SRC += $(LTE_DIR)/src/LTEUplink/lte_phy.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include

$(warning $(SRC))
$(warning $(UTIL_DIR))
OBJS := $(patsubst %.cpp,%.o, $(SRC))
$(warning $(OBJS))

TARGETS = ResMapper.out

all: $(TARGETS)

ResMapper.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

