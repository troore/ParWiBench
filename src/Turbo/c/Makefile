include ../../../common/makefile.config

SRC = ../TurboMain.cpp Turbo.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp 

ifeq ($(PARD),1)
	SRC += Turbo_pardec.cpp
	CXXFLAGS += -DPARD
endif

SRC += $(SRC_DIR)/lte_phy.cpp


INC_FLAGS += -I. -I.. -I$(SRC_DIR)

CXXFLAGS += 
LINKFLAGS += 

TARGETS = Turbo.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DTurboEnc
endif

Turbo.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
