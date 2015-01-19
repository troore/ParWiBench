include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp Equalizer.cpp test.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp  $(LIB_DIR)/refs/dmrs.cpp $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

OBJS := $(patsubst %.cpp,%.o, $(SRC))

INC_FLAGS += -I. -I.. -I$(SRC_DIR) -I$(LIB_DIR)

#CXXFLAGS += -DDEBUG

TARGETS = CEEQ.out

CEEQ.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

