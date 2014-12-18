include ../../../common/makefile.config

SRC = ../TransformPreMain.cpp TransformPrecoder.cpp 
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/dft/mic/dft.cpp $(LIB_DIR)/meas.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)/dft -I$(LIB_DIR)/dft/mic

TARGETS = TPTD.out

#CXXFLAGS += -DDEBUG_FFT
#LINKFLAGS += -lfftw3f

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DTransformPre
endif

all: $(TARGETS)

TPTD.out: $(OBJS) 
	$(ICC) $^ $(LINKFLAGS) $(MMIC) -openmp -o $@

%.o: %.cpp
	$(ICC) -c $< -o $@ $(ICCFLAGS) $(MMIC) -openmp

clean:
	rm -rf $(TARGETS) $(OBJS)
