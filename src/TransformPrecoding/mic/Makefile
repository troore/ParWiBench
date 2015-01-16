include ../../../common/makefile.config

SRC = ../TransformPreMain.cpp TransformPrecoder.cpp 
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/dft/mic/dft.cpp $(LIB_DIR)/timer/meas.cpp

SRC += $(SRC_DIR)/lte_phy.cpp $(LIB_DIR)/power/micpower/micpower.cpp $(LIB_DIR)/check/check.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/dft -I$(LIB_DIR)/dft/mic -I$(LIB_DIR)/power/micpower -I$(LIB_DIR)/check -I$(LIB_DIR)/timer

TARGETS = TPTD.out

#CXXFLAGS += -DDEBUG_FFT
#LINKFLAGS += -lfftw3f

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	ICCFLAGS += -DTransformPre
endif

all: $(TARGETS)

TPTD.out: $(OBJS) 
	$(ICC) $^ $(LINKFLAGS) $(MMIC) -openmp -o $@

%.o: %.cpp
	$(ICC) -c $< -o $@ $(ICCFLAGS) $(MMIC) -openmp

clean:
	rm -rf $(TARGETS) $(OBJS)
