include ../../../common/makefile.config

SRC = ../RateMatcherMain.cpp  RateMatcher.cpp SubblockInterleaver_lte.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp 
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I../ -I. -I$(SRC_DIR)

#CXXFLAGS += -DDEBUG_INTL

TARGETS = RM.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DTxRateM
	ICCFLAGS += -DTxRateM
endif

all: $(TARGETS)

RM.out: $(OBJS)
	$(ICC) $(MMIC) -openmp $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(ICC) $(MMIC) -openmp -c $< -o $@ $(ICCFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)

