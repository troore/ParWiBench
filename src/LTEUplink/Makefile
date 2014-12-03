include ../../common/makefile.config

TARGETS = LTEUplink.out LTEUplinkTx.out LTEUplinkRx.out

FILE_LIST = Turbo/Turbo.cpp RateMatching/RateMatcher.cpp RateMatching/SubblockInterleaver_lte.cpp Scrambling/Scrambler.cpp Modulation/Modulation.cpp TransformPrecoding/TransformPrecoder.cpp ResMapping/ResMapper.cpp Equalizing/Equalizer.cpp OFDM/OFDM.cpp

COM_SRC = $(SRC_DIR)/lte_phy.cpp
#SRC +=$(CHANNEL_DIR)/Channel.cpp
COM_SRC += $(LIB_DIR)/FIFO.cpp $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/matrix.cpp 
COM_SRC += $(foreach file, $(FILE_LIST), $(SRC_DIR)/$(file))

SIM_SRC = LTEUplink.cpp $(COM_SRC)
TX_SRC = LTEUplinkTx.cpp $(COM_SRC)
RX_SRC = LTEUplinkRx.cpp $(COM_SRC)

INC_FLAGS += -I. -I$(LTE_DIR)/include

LINKFLAGS += -lfftw3f

COM_OBJS := $(patsubst %.cpp, %.o, $(COM_SRC))

# option -pg for gprof profiling
#CXXFLAGS += -pg
#LINKFLAGS += -pg

all: $(TARGETS)

LTEUplink.out: $(COM_OBJS) LTEUplink.o
	$(CXX) $^ $(LINKFLAGS) -o $@

LTEUplinkTx.out: $(COM_OBJS) LTEUplinkTx.o
	$(CXX) $^ $(LINKFLAGS) -o $@

LTEUplinkRx.out: $(COM_OBJS) LTEUplinkRx.o
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(COM_OBJS) ./*.o
