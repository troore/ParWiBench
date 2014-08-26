PROJ=fft

CXX=g++

CXXFLAGS= -Wall -DUNIX -g -DDEBUG

# Check for 32-bit vs 64-bit
PROC_TYPE = $(strip $(shell uname -m | grep 64))
 
INC_DIRS=../include ../../opencl ../../
LIB_DIRS=

LIBS=-lOpenCL -lm
ifeq ($(PROC_TYPE),)
	CXXFLAGS+=-m32
else
	CXXFLAGS+=-m64
endif

AMDAPPSDKROOT=/opt/AMDAPP
#### Check for Linux-AMD ####
#ifdef AMDAPPSDKROOT
#   INC_DIRS=. $(AMDAPPSDKROOT)/include
#	ifeq ($(PROC_TYPE),)
#		LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86
#	else
#		LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86_64
#	endif
#else

#### Check for Linux-Nvidia ####
#ifdef NVSDKCOMPUTE_ROOT
#   INC_DIRS=. $(NVSDKCOMPUTE_ROOT)/OpenCL/common/inc
#endif

INC_DIRS+=$(AMDAPPSDKROOT)/include
#LIB_DIRS+=$(AMDAPPSDKROOT)/lib/

$(PROJ): $(PROJ).cpp ../../opencl/clutil.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

.PHONY: clean

clean:
	rm -rf *.o $(PROJ)
