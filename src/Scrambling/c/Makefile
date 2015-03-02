include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp  Scrambler.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

#$(warning $(SRC))

SRC_OMP = $(subst Scrambler, Scrambler_omp, $(SRC))

INC_FLAGS += -I. -I.. -I$(LIB_DIR) -I$(SRC_DIR)

TARGETS = ScrDescr.out ScrDescr_omp.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))
OBJS_OMP := $(patsubst %.cpp, %.o, $(SRC_OMP))

ifeq ($(TX),1)
	CXXFLAGS += -DScramb
endif

all: $(TARGETS)

ScrDescr.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

ScrDescr_omp.out: $(OBJS_OMP)
	$(CXX) $^ $(LINKFLAGS) -fopenmp -o $@

Scrambler_omp.o: Scrambler_omp.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) -fopenmp

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_OMP)

