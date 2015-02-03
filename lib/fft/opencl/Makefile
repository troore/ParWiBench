
CXXFLAGS = -g -Wall -I../../ -I. -I../
LINKFLAGS = -lOpenCL

ifeq ($(NV),1)
	CXXFLAGS += -I/usr/local/cuda/include
	CXXFLAGS += -DNV
else
	CXXFLAGS += -I/opt/AMDAPP/include
endif

fft: fft.cpp ../../opencl/clutil.cpp test.cpp ../util.cpp ../../check/check.cpp ../../timer/meas.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LINKFLAGS)

clean:
	rm -rf fft *.o
