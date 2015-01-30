include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp RateMatcher_ntp.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

SRC_OMP = $(subst RateMatcher_ntp, RateMatcher_omp, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)

#CXXFLAGS += -DDEBUG_INTL

TARGETS = RM.out RM_omp.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))
OBJS_OMP = $(patsubst %.cpp, %.o, $(SRC_OMP))

ifeq ($(TX),1)
	CXXFLAGS += -DTxRateM
endif

all: $(TARGETS)

RM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

RM_omp.out: $(OBJS_OMP)
	$(CXX) $^ $(LINKFLAGS) -fopenmp -o $@

RateMatcher_omp.o: RateMatcher_omp.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) -fopenmp

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS) $(OBJS_OMP)

