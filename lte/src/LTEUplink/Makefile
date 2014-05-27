include ../../../common/makefile.config

TARGETS = LTEUplink.out

FILE_LIST = Turbo/Turbo.cpp RateMatching/RateMatcher.cpp RateMatching/SubblockInterleaver_lte.cpp Scrambling/Scrambler.cpp Modulation/Modulation.cpp TransformPrecoding/TransformPrecoder.cpp ResMapping/ResMapper.cpp Equalizing/Equalizer.cpp OFDM/OFDM.cpp

SRC = LTEUplink.cpp
SRC += $(LTE_DIR)/src/LTEUplink/BSPara.cpp $(LTE_DIR)/src/LTEUplink/UserPara.cpp
#SRC +=$(CHANNEL_DIR)/Channel.cpp
SRC += $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/GeneralFunc.cpp $(UTIL_DIR)/matrix.cpp 
SRC += $(foreach file, $(FILE_LIST), $(LTE_DIR)/src/$(file))

INC_FLAGS += -I. -I$(LTE_DIR)/include

LINKFLAGS += -lfftw3f

OBJS := $(patsubst %.cpp, %.o, $(SRC))

LTEUplink.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
