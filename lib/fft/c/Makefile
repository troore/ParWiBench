
all: fft fft_check

INC_DIRS=../include ../../

fft: fft.cpp
	g++ -o $@ -g $^ $(INC_DIRS:%=-I%)

fft_check: fft_check.c
	gcc -o $@ -g $^ -lm $(INC_DIRS:%=-I%)

clean:
	rm -rf *.o fft fft_check
