include ../../../common/makefile.config

SRC = fft.cpp test.cpp ../util.cpp
#SRC = test.cpp
SRC += $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRC))

all: fft

INC_DIRS = ./ ../ $(LIB_DIR)

ICPCFLAGS += $(INC_DIRS:%=-I%)

LINKFLAGS += -L$(LIB_DIR)/fft/mic -lfft

fft: $(OBJS) libfft.a
	$(ICPC) $(MMIC) -o $@ $(OBJS) $(LINKFLAGS)

libfft.a: fft.o ../util.o
	ar rcs $@ $^

%.o: %.cpp
	$(ICPC) $(MMIC) -o $@ -c $< $(ICPCFLAGS)


clean:
	rm -rf *.o $(OBJS) fft *.a *.so
