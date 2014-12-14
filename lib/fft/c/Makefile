
all: fft fft_check

INC_DIRS = ./ ../

#CXXFLAGS += -DDEBUG

fft: fft.cpp
	g++ -o $@ -g $^ $(INC_DIRS:%=-I%) $(CXXFLAGS)

fft_check: fft_check.c
	gcc -o $@ -g $^ -lm $(INC_DIRS:%=-I%)

clean:
	rm -rf *.o fft fft_check
