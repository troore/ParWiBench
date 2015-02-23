include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp Equalizer.cpp test.cpp ../util.cpp
# check
SRC += $(SRC_DIR)/OFDM/c/OFDM.cpp $(SRC_DIR)/ResMapping/c/ResMapper.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

OBJS := $(patsubst %.cpp,%.o, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)
# check
INC_FLAGS += -I$(SRC_DIR)/OFDM -I$(SRC_DIR)/ResMapping

LINKFLAGS += -L$(LIB_DIR)/fft/c -lfft

#CXXFLAGS += -DDEBUG

TARGETS = CEEQ.out

CEEQ.out: $(OBJS)
	$(MAKE) -C $(LIB_DIR)/fft/c libfft.a
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

