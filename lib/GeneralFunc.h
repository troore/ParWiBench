
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
void ReadInputFromFiles(float *pIn, int Sz, const char *nameReal, const char *nameImag);
void ReadInputFromFiles(float (*pIn)[2], int Sz, const char *nameReal, const char *nameImag);



void GeneRandomInput(FIFO<int> *pIn,int Sz[],const char *name);
void GeneRandomInput(FIFO<float> *pIn,int Sz[],const char *name);
void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[],const char *nameReal, const char *nameImag);
void GeneRandomInput(complex<float> *pIn, int Sz, const char *nameReal, const char *nameImag);
void GeneRandomInput(float (*pIn)[2], int Sz, const char *nameReal, const char *nameImag);

void GeneRandomInput(FIFO<int> *pIn,int Sz[]);
void GeneRandomInput(FIFO<float> *pIn,int Sz[]);
void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[]);

void GeneRandomInput(int *pIn,int Sz,const char *name);
void GeneRandomInput(float *pIn,int Sz,const char *name);

void WriteOutputToFiles(FIFO<int> *pOut,int Sz[],const char *name);
void WriteOutputToFiles(int *pOut, int Sz, const char *name);

void WriteOutputToFiles(FIFO<float> *pOut,int Sz[],const char *name);
void WriteOutputToFiles(float *pOut, int Sz, const char *name);

void WriteOutputToFiles(FIFO<complex<float> > *pOut,int Sz[],const char *nameReal, const char *nameImag);
void WriteOutputToFiles(complex<float> *pOut, int Sz, const char *nameReal, const char *nameImag);
void WriteOutputToFiles(float *pOut, int Sz, const char *nameReal, const char *nameImag);
void WriteOutputToFiles(float (*pOut)[2], int Sz, const char *nameReal, const char *nameImag);

void ReadOutput(FIFO<int> *pOut,int Sz[]);
void ReadOutput(FIFO<float> *pOut,int Sz[]);
void ReadOutput(FIFO<complex<float> > *pOut,int Sz[]);


//void GenerateLTEChainInput(FIFO<int> *pDataSource, int DataK, int *pTxDS);
void GenerateLTEChainInput(FIFO<int> *pDataSource, int DataK, int *pTxDS, int RANDOMSEED);
void GenerateLTEChainInput(int *pDataSource, int DataK, int *pTxDS, int RANDOMSEED);
void ReadLTEChainOutput(FIFO<int> *pFileSink, int *pRxFS);
void ReadLTEChainOutput(int *pFileSink, int *pRxFS, int DataK);

#endif



