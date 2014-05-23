
#ifndef _GENERALFUNC_H_
#define _GENERALFUNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <complex>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "gauss.h"
#include "FIFO.h"

using namespace std;

void ReadInputFromFiles(FIFO<int> *pIn,int Sz[],const char *name);
void ReadInputFromFiles(int *pIn,int Sz,const char *name);

void ReadInputFromFiles(FIFO<float> *pIn,int Sz[],const char *name);
void ReadInputFromFiles(float *pIn, int Sz, const char *name);

void ReadInputFromFiles(FIFO<complex<float> > *pIn,int Sz[],const char *nameReal, const char *nameImag);

void ReadInputFromFiles(complex<float> *pIn, int Sz, const char *nameReal, const char *nameImag);



void GeneRandomInput(FIFO<int> *pIn,int Sz[],const char *name);
void GeneRandomInput(FIFO<float> *pIn,int Sz[],const char *name);
void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[],const char *nameReal, const char *nameImag);

void GeneRandomInput(FIFO<int> *pIn,int Sz[]);
void GeneRandomInput(FIFO<float> *pIn,int Sz[]);
void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[]);


void WriteOutputToFiles(FIFO<int> *pOut,int Sz[],const char *name);
void WriteOutputToFiles(int *pOut, int Sz, const char *name);

void WriteOutputToFiles(FIFO<float> *pOut,int Sz[],const char *name);
void WriteOutputToFiles(float *pOut, int Sz, const char *name);

void WriteOutputToFiles(FIFO<complex<float> > *pOut,int Sz[],const char *nameReal, const char *nameImag);
void WriteOutputToFiles(complex<float> *pOut, int Sz, const char *nameReal, const char *nameImag);

void ReadOutput(FIFO<int> *pOut,int Sz[]);
void ReadOutput(FIFO<float> *pOut,int Sz[]);
void ReadOutput(FIFO<complex<float> > *pOut,int Sz[]);


void GenerateLTEChainInput(FIFO<int> *pDataSource, int DataK, int *pTxDS);
void ReadLTEChainOutput(FIFO<int> *pFileSink, int *pRxFS);

#endif



