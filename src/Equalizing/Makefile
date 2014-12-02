include ../../common/makefile.config

DEBUG = 1

SRC = EqualizerMain.cpp Equalizer.cpp
ifeq ($(DEBUG), 0)
SRC += $(LIB_DIR)/matrix.cpp
else
SRC += Matrix.cpp
endif
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp 
SRC += $(SRC_DIR)/lte_phy.cpp $(SRC_DIR)/ResMapping/ResMapper.cpp

INC_FLAGS += -I. -I$(SRC_DIR) -I$(SRC_DIR)/ResMapping
OBJS := $(patsubst %.cpp,%.o, $(SRC))

TARGETS = CEEq.out

ifeq ($(DEBUG), 0)
CXXFLAGS += -DDEBUG_EQ
endif
#LINKFLAGS += -lfftw3f

all: $(TARGETS)

CEEq.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

