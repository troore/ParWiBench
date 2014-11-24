
#include <complex>
#include "lte_phy.h"

void MatrixProd(int d1, int d2, int d3, std::complex<float> *M1, std::complex<float> *M2, std::complex<float> *oM)
{
	int r, c, i;

	for (r = 0; r < d1; r++)
	{
		for (c = 0; c < d3; c++)
		{
			std::complex<float> tmp;

			tmp = std::complex<float>(0.0, 0.0);
			for (i = 0; i < d2; i++)
				tmp += M1[r * d2 + i] * M2[i * d3 + c];
		
			oM[r * d3 + c] = tmp;
		}
	}
}


void MatrixInv(int sz, std::complex<float> *pM, std::complex<float> *pInvM)
{
//	T** pX=new T*[sz];
//	for(int r=0;r<sz;r++){*(pX+r)=new T[2*sz];}
	std::complex<float> pX[LTE_PHY_N_ANT_MAX * 2 * LTE_PHY_N_ANT_MAX];

	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < sz; c++)
			pX[r * (2 * sz) + c] = pM[r * sz + c];
		for (int c = sz; c < 2 * sz; c++)
		{
			if (c == (r + sz))
			{
				pX[r * (2 * sz) + c] = (std::complex<float>)1.0;
			}
			else
			{
				pX[r * (2 * sz) + c] = (std::complex<float>)0.0;
			}
		}
	}

//	T* pCurRow=new T[2*sz];
	std::complex<float> pCurRow[2  * LTE_PHY_N_ANT_MAX];

	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < (2 * sz); c++)
		{
			pCurRow[c] = pX[r * (2 * sz) + c] / pX[r * (2 * sz) + r];
		}

		for (int c = 0; c < (2 * sz); c++)
		{
			pX[r * (2 * sz) + c] = pCurRow[c];
		}

		for (int er = r + 1; er < sz; er++)
		{
			std::complex<float> curC = pX[er * (2 * sz) + r];

			for (int c = 0; c < (2 * sz); c++)
			{
				pX[er * (2 * sz) + c] -= curC * pCurRow[c];
			}  
		}
	}

	for (int r = sz - 1; r >= 0; r--)
	{
		for (int c = 0; c < (2 * sz); c++)
		{
			pCurRow[c] = pX[r * (2 * sz) + c];
		}

		for (int er = r - 1; er >= 0; er--)
		{
			std::complex<float> curC = pX[er * (2 * sz) + r];

			for (int c = 0; c < (2 * sz); c++)
			{
				pX[er * (2 * sz) + c] -= curC * pCurRow[c];
			}
		}
	}

	for (int r = 0; r < sz; r++)
	{

		for (int c = 0; c < sz; c++)
		{
			int col = c + sz;

			pInvM[r * sz + c] = pX[r * (2 * sz) + col];
		}

	}
}

