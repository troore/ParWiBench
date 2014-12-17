include ../../../common/makefile.config

SRC = ../OFDMMain.cpp $(LIB_DIR)/fft/mic/fft.cpp OFDM.cpp

SRC += $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I../ -I$(SRC_DIR) -I$(LIB_DIR)/fft/mic -I$(LIB_DIR)/fft -I$(LIB_DIR)

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = OFDM.out

#LINKFLAGS += -lfftw3f

ifeq ($(TX),1)
	CXXFLAGS += -DMOD
	ICCFLAGS += -DMOD
endif

all: $(TARGETS)

OFDM.out: $(OBJS)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

