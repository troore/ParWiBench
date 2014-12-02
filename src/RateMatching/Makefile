include ../../common/makefile.config

SRC = RateMatcherMain.cpp  RateMatcher.cpp 
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp 
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I$(SRC_DIR)

#CXXFLAGS += -DDEBUG_INTL

TARGETS = RM.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

all: $(TARGETS)

RM.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

