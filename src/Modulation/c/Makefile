include ../../../common/makefile.config

SRC = Modulation.cpp $(SRC_DIR)/main_test.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

SRC_OMP = $(subst Modulation, Modulation_omp, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)

TARGETS = ModDemod.out ModDemod_omp.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))
OBJS_OMP = $(patsubst %.cpp, %.o, $(SRC_OMP))

ifeq ($(TX),1)
	CXXFLAGS += -DMod
endif

all: $(TARGETS)

ModDemod.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

ModDemod_omp.out: $(OBJS_OMP)
	$(CXX) $^ $(LINKFLAGS) -fopenmp -o $@

Modulation_omp.o: Modulation_omp.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) -fopenmp

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_OMP)
