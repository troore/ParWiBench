
#ifndef __SUBBLOCKINTERLEAVER_LTE_H_
#define __SUBBLOCKINTERLEAVER_LTE_H_

#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <omp.h>
#include "lte_phy.h"
void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset);
/*
template <class U, class T>
class SubblockInterleaver_lte
{
private:
	int D;
        int K_pi;
        int Rate;
	int C_sb;
	int R_sb;
        int NumDummy;
        T DummyValue;

	int InterColumnPattern[32];

public:
	SubblockInterleaver_lte(void);
	void SubblockInterleaving(U SeqLen,T **pInpMtr,T **pOutMtr);
	void SubblockDeInterleaving(U SeqLen,T **pInpMtr, T **pOutMtr);
	~SubblockInterleaver_lte(void);
};
*/

 
/*
template<class U, class T>
SubblockInterleaver_lte<U,T>::SubblockInterleaver_lte(void)
{
	Rate=3;
	C_sb=32;
	DummyValue = (T)1000000;
	int arr[32]={0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};
	for(int i=0;i<32;i++){InterColumnPattern[i]=arr[i];}
}
*/

static int InterColumnPattern[32] = {0,16,8,24,4,20,12,28,
									2,18,10,26,6,22,14,30,
									1,17,9,25,5,21,13,29,3,
									19,11,27,7,23,15,31};

template <typename T>
#ifdef DEBUG_INTL
inline void SubblockInterleaving(int SeqLen, T **pInpMtr, T **pOutMtr)
#else
inline void SubblockInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
#endif
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
//	int InterColumnPattern[32];
		
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
#ifdef DEBUG_INTL
	R_sb = (int)(ceil((((float)D) / ((float)C_sb))));
#else
	R_sb = (D + (C_sb - 1)) / C_sb;
#endif
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;

#ifdef DEBUG_INTL
	T **pInterMatrix=new T*[R_sb];
	for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}
#else
	T pInterMatrix[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif

#ifdef DEBUG_INTL
	T *VpInpSeq;
	T *VpOutSeq;
#endif
	int r,StrIdx;	
	omp_set_num_threads(32);
//#pragma omp parallel for private(r,StrIdx)
//	for (int c = 0; c < 32; c++)
//		                        {
	for (StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
#ifdef DEBUG_INTL
		VpInpSeq=*(pInpMtr+StrIdx);
		VpOutSeq=*(pOutMtr+StrIdx);
#endif
		//////////////// write into matrix //////////////////
		for (r = 0; r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
					pInterMatrix[r * C_sb + c] = DummyValue;
				}
				else
				{
					pInterMatrix[r * C_sb + c] = pInpMtr[StrIdx * D + k - NumDummy];
				}
			}
		}
		
		OutIdx=0;
		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
				
				T v = pInterMatrix[r * C_sb + col];
				
				if (v == DummyValue)
				{}
				else
				{
					pOutMtr[StrIdx * D + OutIdx] = v;
					OutIdx++;
				}  
			}
		}  
	}
	
//////////////////// Interleaving for i=2 ///////////////////////
#ifdef DEBUG_INTL
	int *Pi=new int[K_pi];
	T *pInterSeq=new T[K_pi];
#else
	int Pi[((BLOCK_SIZE + 4 + 31) / 32) * 32];
	T pInterSeq[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif

#ifdef DEBUG_INTL
	VpInpSeq=*(pInpMtr+(Rate-1));
	VpOutSeq=*(pOutMtr+(Rate-1));
#endif
	
	for (int k = 0;k < NumDummy; k++)
	{
#ifdef DEBUG_INTL
		*(pInterSeq+k)=DummyValue;
#else
		pInterSeq[k] = DummyValue;
#endif
	}
	for (int k = NumDummy; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		*(pInterSeq+k)=*(VpInpSeq+(k-NumDummy));
#else
		pInterSeq[k] = pInpMtr[(Rate - 1) * D + (k - NumDummy)];
#endif
	}

	for (int k = 0; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
#else
		int idxP = k / R_sb;
#endif
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;

#ifdef DEBUG_INTL
		*(Pi+k)=idx;
#else
		Pi[k] = idx;
#endif
	}

	OutIdx=0;
	for (int k = 0; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		T v = *(pInterSeq+(*(Pi+k)));
#else
		T v = pInterSeq[Pi[k]];
#endif
		if (v == DummyValue)
		{}
		else
		{
#ifdef DEBUG_INTL
			*(VpOutSeq+OutIdx) = v;
#else
			pOutMtr[(Rate - 1) * D + OutIdx] = v;
#endif
			OutIdx++;
		}
	}

#ifdef DEBUG_INTL
	delete[] pInterSeq;
	delete[] Pi;
#endif
}


//void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset);
/*{
    int D;
    int K_pi;
    int Rate;
    int R_sb, C_sb;
    int NumDummy;
    int DummyValue;
    //	int InterColumnPattern[32];
    
    int OutIdx;
    
    D = SeqLen;
    Rate = 3;
    C_sb = 32;
    

    R_sb = (D + (C_sb - 1)) / C_sb;

    
    K_pi = R_sb * C_sb;
    NumDummy = K_pi - D;
    DummyValue = (int)1000000;

    int pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];

    

    
    for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
    {

        //////////////// write into matrix //////////////////
        for (int r = 0; r < R_sb; r++)
        {
            for (int c = 0; c < C_sb; c++)
            {
                int k = r * C_sb + c;
                
                if (k < NumDummy)
                {
                    pInterMatrix[r * C_sb + c] = DummyValue;
                }
                else
                {
                    pInterMatrix[r * C_sb + c] = InSeq[offset + StrIdx + (k - NumDummy) * 3];
                }
            }
        }
        
        OutIdx=0;
        for (int c = 0; c < C_sb; c++)
        {
            int col = InterColumnPattern[c];
            for (int r = 0; r < R_sb; r++)
            {
                int k = col * R_sb + r;
                
                int v = pInterMatrix[r * C_sb + col];
                
                if (v == DummyValue)
                {}
                else
                {
                    OutSeq[offset + StrIdx + OutIdx * 3] = v;
                    OutIdx++;
                }
            }
        }
    }
    
    //////////////////// Interleaving for i=2 ///////////////////////

    int Pi[((BLOCK_SIZE + 31) / 32) * 32];
    int pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];

    

    
    for (int k = 0;k < NumDummy; k++)
    {

        pInterSeq[k] = DummyValue;

    }
    for (int k = NumDummy; k < K_pi; k++)
    {

        pInterSeq[k] = InSeq[(Rate - 1)  + 3*(k - NumDummy)];

    }
    
    for (int k = 0; k < K_pi; k++)
    {

        int idxP = k / R_sb;

        int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;
        

        Pi[k] = idx;

    }
    
    OutIdx=0;
    for (int k = 0; k < K_pi; k++)
    {

        int v = pInterSeq[Pi[k]];

        if (v == DummyValue)
        {}
        else
        {

            OutSeq[(Rate - 1)  + OutIdx*3] = v;

            OutIdx++;
        }
    }
    

}*/

template<typename T>
#ifdef DEBUG_INTL
inline void SubblockDeInterleaving(int SeqLen, T **pInpMtr, T **pOutMtr)
#else
inline void SubblockDeInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
#endif
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
	
	int InIdx;
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
#ifdef DEBUG_INTL
	R_sb = (int)(ceil((((float)D) / ((float)C_sb))));
#else
	R_sb = (D + (C_sb - 1)) / C_sb;
#endif
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;
	
//////////////////// DeInterleaving for i=0,1 ///////////////////////
#ifdef DEBUG_INTL
	T **pInterMatrix=new T*[R_sb];
	for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}
#else
	T pInterMatrix[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif

#ifdef DEBUG_INTL
	T *VpInpSeq;
	T *VpOutSeq;
#endif
	
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
#ifdef DEBUG_INTL
		VpInpSeq=*(pInpMtr+StrIdx);
		VpOutSeq=*(pOutMtr+StrIdx);
#endif

		InIdx=0;
		for (int r = 0;r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+c)=DummyValue;
#else
					pInterMatrix[r * C_sb + c] = DummyValue;
#endif
				}
				else
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+c)=(T)0;
#else
					pInterMatrix[r * C_sb + c] = (T)0;
#endif
				}
			}
		}

		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
#ifdef DEBUG_INTL
				T v = *(*(pInterMatrix+r)+col);
#else
				T v = pInterMatrix[r * C_sb + col];
#endif
				if (v == DummyValue)
				{}
				else
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+col)=*(VpInpSeq+InIdx);
#else
					pInterMatrix[r * C_sb + col] = pInpMtr[StrIdx * D + InIdx];
#endif
					InIdx++;
				}  
			}
		}  

		OutIdx=0;
		for (int r = 0; r < R_sb; r++)
		{
			for(int c = 0; c < C_sb; c++)
			{
#ifdef DEBUG_INTL
				T v = *(*(pInterMatrix+r)+c);
#else
				T v = pInterMatrix[r * C_sb + c];
#endif
				if (v == DummyValue)
				{}
				else
				{
					#ifdef DEBUG_INTL
					*(VpOutSeq+OutIdx)=*(*(pInterMatrix+r)+c);
					#else
					pOutMtr[StrIdx * D + OutIdx] = pInterMatrix[r * C_sb + c];
					#endif
					OutIdx++;
				}
			}
		}
	}
#ifdef DEBUG_INTL
	for(int r=0;r<R_sb;r++){delete[] *(pInterMatrix+r);}
	delete[] pInterMatrix;
#endif

//////////////////// DeInterleaving for i=2 ///////////////////////
#ifdef DEBUG_INTL	
	int *Pi=new int[K_pi];
	T *pInterSeq=new T[K_pi];
	VpInpSeq=*(pInpMtr+(Rate-1));
	VpOutSeq=*(pOutMtr+(Rate-1));
#else
	int Pi[((BLOCK_SIZE + 4 + 31) / 32) * 32];
	T pInterSeq[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif
	
	for (int k = 0; k < NumDummy; k++)
		pInterSeq[k] = DummyValue;
//////////////// Pi & DePi//////////////////
	for(int k=0;k<K_pi;k++)
	{
#ifdef DEBUG_INTL
		int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
#else
		int idxP = k / R_sb;
#endif
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;
		Pi[k]=idx;
	}
/////////////// DeInterleaving ////////////////////
	InIdx=0;
	for(int k=0;k<K_pi;k++)
	{
		T v = pInterSeq[Pi[k]];
		if (v == DummyValue)
		{}
		else
		{
#ifdef DEBUG_INTL	
			*(pInterSeq+(*(Pi+k)))=*(VpInpSeq+InIdx);
#else
			pInterSeq[Pi[k]] = pInpMtr[(Rate - 1) * D + InIdx];
#endif
			InIdx++;
		}
	}
	OutIdx=0;
	for (int k = NumDummy; k < K_pi; k++)
	{
		#ifdef DEBUG_INTL
		*(VpOutSeq+OutIdx)=*(pInterSeq+k);
		#else
		pOutMtr[(Rate - 1) * D + OutIdx] = pInterSeq[k];
		#endif
		OutIdx++;
	}

#ifdef DEBUG_INTL
	delete[] pInterSeq;
	delete[] Pi;
#endif
}

#endif

