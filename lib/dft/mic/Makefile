include ../../../common/makefile.config

SRC = ../c/dft.cpp test.cpp ../util.cpp
SRC += $(LIB_DIR)/timer/meas.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/power/micpower/micpower.cpp

SRC_SIMD = $(subst ../c/dft, dft_simd, $(SRC))
SRC_OMP = $(subst ../c/dft, ../c/dft_omp, $(SRC))

OBJS = $(patsubst %.cpp, %.o, $(SRC))
OBJS_SIMD = $(patsubst %.cpp, %.o, $(SRC_SIMD))
OBJS_OMP = $(patsubst %.cpp, %.o, $(SRC_OMP))

TARGETS := dft dft_simd dft_omp

all: $(TARGETS)

INC_DIRS = ./ ../ $(LIB_DIR) $(LIB_DIR)/power/micpower

ICPCFLAGS += $(INC_DIRS:%=-I%)

#LINKFLAGS += -L$(LIB_DIR)/fft/mic -lfft
LINKFLAGS += -lpthread

ifeq ($(VEC),1)
	ICPCFLAGS += -vec -vec-report2
else
	ICPCFLAGS += -no-vec
endif

dft: $(OBJS)
	$(ICPC) $(MMIC) -o $@ $(OBJS) $(LINKFLAGS)

dft_simd: $(OBJS_SIMD)
	$(ICPC) $(MMIC) -o $@ $(OBJS_SIMD) $(LINKFLAGS)

libdft.a: dft.o ../util.o
	ar rcs $@ $^

libdftsimd.a: dft_simd.o ../util.o
	ar rcs $@ $^

%.o: %.cpp
	$(ICPC) $(MMIC) -o $@ -c $< $(ICPCFLAGS)


clean:
	rm -rf *.o $(OBJS) $(OBJS_SIMD) $(OBJS_OMP) $(TARGETS)  *.a *.so
