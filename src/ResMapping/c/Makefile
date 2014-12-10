include ../../common/makefile.config

SRC = ResMapMain.cpp ResMapper.cpp 
SRC += $(LIB_DIR)/gauss.cpp  $(LIB_DIR)/GeneralFunc.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I$(SRC_DIR)

#$(warning $(SRC))
#$(warning $(UTIL_DIR))
OBJS := $(patsubst %.cpp,%.o, $(SRC))

TARGETS = ResMapper.out

ifeq ($(TX),1)
	CXXFLAGS += -DSCMapper
endif

all: $(TARGETS)

ResMapper.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

