#ifndef __MATRIX_H_
#define __MATRIX_H_

void MatrixProd(int d1, int d2, int d3, std::complex<float> *M1, std::complex<float> *M2, std::complex<float> *oM);

void MatrixInv(int sz, std::complex<float> *pM, std::complex<float> *pInvM);
#endif
