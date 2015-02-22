
include ../../../common/makefile.config

SRC = fft.cpp test.cpp ../util.cpp
#SRC = test.cpp
SRC += $(LIB_DIR)/check/check.cpp $(LIB_DIR)/timer/meas.cpp

SRC_OMP = $(subst fft, fft_omp, $(SRC))

OBJS = $(patsubst %.cpp, %.o, $(SRC))
OBJS_OMP = $(patsubst %.cpp, %.o, $(SRC_OMP))

INC_DIRS = ./ ../ $(LIB_DIR)

CXXFLAGS += $(INC_DIRS:%=-I%)

#LINKFLAGS += -L$(LIB_DIR)/fft/c -lfft

TARGETS = fft fft_omp

all: $(TARGETS) 

#fft: $(OBJS) libfft.a
fft: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LINKFLAGS)

libfft.a: fft.o ../util.o
	ar rcs $@ $^

fft_omp: $(OBJS_OMP)
	$(CXX) -o $@ $^ $(LINKFLAGS) -fopenmp

fft_omp.o: fft_omp.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS) -fopenmp

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -rf *.o $(OBJS) $(OBJS_OMP) $(TARGETS) *.a *.so
