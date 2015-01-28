include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp
SRC += $(SRC_DIR)/lte_phy.cpp
SRC += $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp
OBJS := $(patsubst %.cpp,%.o, $(SRC))

SRC_C = $(SRC) Equalizer_C.cpp
SRC_SIMD = $(SRC) Equalizer_SIMD.cpp
SRC_OMP = $(SRC) Equalizer_OMP.cpp
SRC_all = $(SRC) Equalizer_all.cpp

OBJS_C := $(patsubst %.cpp,%.o, $(SRC_C))
OBJS_SIMD := $(patsubst %.cpp,%.o, $(SRC_SIMD))
OBJS_OMP := $(patsubst %.cpp,%.o, $(SRC_OMP))
OBJS_all := $(patsubst %.cpp,%.o, $(SRC_all))

INC_FLAGS += -I. -I.. -I$(LIB_DIR) -I$(SRC_DIR) -I$(LIB_DIR)/refs -I$(LIB_DIR)/power/micpower

#CXXFLAGS += -DDEBUG

TARGETS = CEEQ_C.out CEEQ_SIMD.out CEEQ_OMP.out CEEQ_all.out

all:$(TARGETS)

CEEQ_C.out: $(OBJS_C)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

CEEQ_SIMD.out: $(OBJS_SIMD)
	        $(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

CEEQ_OMP.out: $(OBJS_OMP)
	        $(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

CEEQ_all.out: $(OBJS_all)
	        $(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS_C) $(OBJS_SIMD) $(OBJS_OMP) $(OBJS_all)

