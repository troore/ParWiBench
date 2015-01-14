
CXXFLAGS = -g -Wall -I../../ -I.
LINKFLAGS = -lOpenCL

ifeq ($(NV),1)
	CXXFLAGS += -I/usr/local/cuda/include
	CXXFLAGS += -DNV
else
	CXXFLAGS += -I/opt/AMDAPP/include
endif

dft: dft.cpp ../../opencl/clutil.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LINKFLAGS)

clean:
	rm -rf dft *.o
