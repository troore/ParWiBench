include ../../../../common/makefile.config

SRC = OFDM.cpp ../OFDMMain.cpp $(LIB_DIR)/fft/c/fft.cpp

SRC += $(LIB_DIR)/FIFO.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/matrix.cpp

SRC += $(LTE_DIR)/src/LTEUplink/lte_phy.cpp

INC_FLAGS += -I. -I$(LTE_DIR)/include -I$(LIB_DIR)/fft/include

OBJS := $(patsubst %.cpp, %.o, $(SRC))

TARGETS = OFDM.out

#LINKFLAGS += -lfftw3f

all: $(TARGETS)

OFDM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

