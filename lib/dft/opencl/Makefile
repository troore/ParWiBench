dft: dft.cpp ../../opencl/clutil.cpp ../../meas.cpp
	$(CXX) -o $@ $^ -g -Wall -I/opt/AMDAPP/include -I../../ -L/opt/AMDAPP/lib -lOpenCL

clean:
	rm -rf dft *.o
