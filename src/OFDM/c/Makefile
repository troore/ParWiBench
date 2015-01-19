include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp $(LIB_DIR)/fft/c/fft.cpp OFDM.cpp test.cpp

SRC += $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I../ -I$(SRC_DIR) -I$(LIB_DIR)/fft/c -I$(LIB_DIR)

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = OFDM.out

#LINKFLAGS += -lfftw3f

ifeq ($(TX),1)
	CXXFLAGS += -DMOD
endif

all: $(TARGETS)

OFDM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

