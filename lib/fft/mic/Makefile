include ../../../common/makefile.config
all: fft fft_check

INC_DIRS = ./ ../

#CXXFLAGS += -DDEBUG

fft: fft.cpp
	$(ICC) $(MMIC) -o $@ -g $^ $(INC_DIRS:%=-I%) $(ICCFLAGS)

fft_check: fft_check.c
	$(ICC) $(MMIC) -o $@ -g $^ -lm $(INC_DIRS:%=-I%)

clean:
	rm -rf *.o fft fft_check
