#include <stdlib.h>
#include <sys/time.h>
#include "RateMatcher.h"
//#include "SubblockInterleaver_lte.h"
//SubblockInterleaver_lte<int,int> SbInterleaver;
//SubblockInterleaver_lte<int,float> SbDeInterleaver;

static int InterColumnPattern[32] = {0,16,8,24,4,20,12,28,
									 2,18,10,26,6,22,14,30,
									 1,17,9,25,5,21,13,29,3,
									 19,11,27,7,23,15,31};

inline void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset);

#include<immintrin.h>

#define LEN16 16

#ifdef __MIC__
typedef __attribute__((aligned(64))) union zmmf {
    __m512i reg;
    int elems[LEN16];
} zmmi_t;
#endif

inline void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset)
{
    int D;
    int K_pi;
    int Rate;
    int R_sb, C_sb;
    int NumDummy;
    int DummyValue;
    //	int InterColumnPattern[32];
    //printf("a");
    int OutIdx;
    
    D = SeqLen;
    Rate = 3;
    C_sb = 32;
    R_sb = (D + (C_sb - 1)) / C_sb;
    
    
    K_pi = R_sb * C_sb;
    NumDummy = K_pi - D;
    DummyValue = (int)1000000;
    
    int pInterMatrix[((BLOCK_SIZE + 4 + 31) / 32) * 32];
    zmmi_t pInterMatrix_simd[((BLOCK_SIZE + 4 + 31) / 32) * 2];
    zmmi_t p_index[2],p_index3[2];
    int col = InterColumnPattern[0];
    //p_index[col/16].elems[col%16] = 0;
    if(col<NumDummy)
        p_index3[col/16].elems[col%16] = 0;
    else
        p_index3[col/16].elems[col%16] = 12;
    for(int c=1;c<32;c++)
    {
        int lastcol = col;
        col = InterColumnPattern[c];
        if(col<NumDummy)
        {
            //	int lastcol = col;
            //	col = InterColumnPattern[c];
            //		p_index[col/16].elems[col%16] = p_index[lastcol/16].elems[lastcol%16] + R_sb - 1;
            p_index3[col/16].elems[col%16] = p_index3[lastcol/16].elems[lastcol%16] + (R_sb - 1) * 12;
        }
        else
        {
            //	int lastcol = col;
            //	col = InterColumnPattern[c];
            //		p_index[col/16].elems[col%16] = p_index[lastcol/16].elems[lastcol%16] + R_sb;
            p_index3[col/16].elems[col%16] = p_index3[lastcol/16].elems[lastcol%16] + R_sb*12;
        }
        //		printf("%d ",p_index[col/16].elems[col%16]);
    }
    /*	printf("\n");
     for(int c=0;c<32;c++)
     printf("%d ",p_index[c/16].elems[c%16]);
     printf("\n");
     */	int num_threads=32,StrIdx,r;
    //    omp_set_num_threads(num_threads);
    //#pragma omp parallel for private(StrIdx,r)
    //	for (int c = 0; c < num_threads; c++)
    //	                {
    //    	printf("hello from thread %d\n",c);
    zmmi_t index;
    //int kkk[32];
    for(int c = 0;c<16;c++)
    {
        index.elems[c] =  4*3*c;
        //	kkk[c]=c;
    }
    /*	ab.reg = _mm512_i32gather_epi32(index.reg,kkk,1);
     for(int c= 0;c<16;c++)
     printf("%d ",ab.elems[c]);
     return;
     */	for (StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
     {
         r=0;//first cycle, no SIMD
	 for (int c = 0; c < C_sb; c++)
         {
             int k = r * C_sb + c;
             
             if (k < NumDummy)
             {
                 //				pInterMatrix[r * C_sb + c] = DummyValue;
                 pInterMatrix_simd[r * 2 + c / 16].elems[c % 16] = DummyValue;
             }
             else
             {
                 //				pInterMatrix[r * C_sb + c] = InSeq[offset + StrIdx + (k - NumDummy) * 3];
                 pInterMatrix_simd[r * 2 + c / 16].elems[c % 16] = InSeq[offset + StrIdx + (k - NumDummy) * 3];
             }
         }
         //////////////// write into matrix //////////////////
         
         for (r = 1; r < R_sb; r++)
         {
             //			printf("\n");
             int* p;
             //			for(int c=0;c<16;c++) printf("%d ",InSeq[offset + StrIdx + (r*C_sb+c - NumDummy) * 3]);
             //			printf("\n");
             p = &InSeq[(offset + StrIdx + (r * C_sb - NumDummy) * 3)];
             pInterMatrix_simd[r * 2 + 0].reg = _mm512_i32gather_epi32(index.reg, p, 1);
             //			for(int c=0;c<16;c++) printf("%d ",pInterMatrix_simd[r*2+0].elems[c]);
             //			printf("\n");
             p = &InSeq[(offset + StrIdx + (r * C_sb + 16 - NumDummy) * 3)];
             pInterMatrix_simd[r * 2 + 1].reg = _mm512_i32gather_epi32(index.reg, p, 1);
             /*			for (int c = 0; c < C_sb; c++)
              {
              int k = r * C_sb + c;
              //pInterMatrix_simd[r * 2 + c / 16].elems[c % 16] = InSeq[offset + StrIdx + (k - NumDummy) * 3];
              pInterMatrix[r * C_sb + c] = InSeq[offset + StrIdx + (k - NumDummy) * 3];
              //	if(pInterMatrix[r * C_sb + c]!=pInterMatrix_simd[r * 2 + c / 16].elems[c % 16]) printf("%d %d %d wrong\n", StrIdx,r,c);
              //	else printf("%d %d %d right\n", StrIdx,r,c);
              }
              */		
	 }
         //	printf("right\n");
         OutIdx=0;
         for(int c=0;c<C_sb;c++)
         {
             //int col = InterColumnPattern[c];
             int v = pInterMatrix_simd[c / 16].elems[c % 16];
             if(v!=DummyValue)
	     {
                 OutSeq[offset+StrIdx+(p_index3[c/16].elems[c%16])/4 - 3] = pInterMatrix_simd[c/16].elems[c%16];
	     }
		 //			else
             //				OutSeq[offset+StrIdx+(p_index3[c/16].elems[c%16]-1)/4]= 100000;
         }
         /*
          for(int k = NumDummy; k < C_sb * R_sb; k+=C_sb)
          {
          
          }
          */
	 
         for(r =1;r<R_sb;r++)
         {
             int* p;
             p = &OutSeq[offset + StrIdx+(r-1)*3];
             _mm512_i32scatter_epi32(p,p_index3[0].reg,pInterMatrix_simd[r*2+0].reg,1);
             _mm512_i32scatter_epi32(p,p_index3[1].reg,pInterMatrix_simd[r*2+1].reg,1);
         }
	 
         /*
       	 for (int c = 0; c < C_sb; c++)
          {
          int col = InterColumnPattern[c];
          for (r = 0; r < R_sb; r++)
          {
          int k = col * R_sb + r;
          //	printf("%d %d %d\n",StrIdx,c,r);
          //	int v = pInterMatrix[r * C_sb + col];
          //	if (v != pInterMatrix_simd[r * 2 + col / 16].elems[col % 16]) printf("wrong\n");
          int v = pInterMatrix_simd[r * 2 + col / 16].elems[col % 16];
          if (v == DummyValue)
          {}
          else
          {
          if(OutSeq[offset + StrIdx + OutIdx * 3] != v) printf("%d %d %d %d %d %d wrong\n",OutIdx,c,r,c+r*C_sb,col,OutSeq[offset + StrIdx + OutIdx * 3]);
          //	else printf("%d %d %d %d %d %d right\n",OutIdx,c,r,c+r*C_sb,col,OutSeq[offset + StrIdx + OutIdx * 3]); 
          OutSeq[offset + StrIdx + OutIdx * 3] = v;
          OutIdx++;
          }
          }
	  }
	  */
     }
    
    //////////////////// Interleaving for i=2 ///////////////////////
    
    int Pi[((BLOCK_SIZE + 4 + 31) / 32) * 32];
    int pInterSeq[((BLOCK_SIZE + 4 + 31) / 32) * 32];
    
    
    
    
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
}

template<typename T>
#ifdef DEBUG_INTL
static void SubblockDeInterleaving(int SeqLen, T **pInpMtr, T **pOutMtr)
#else
static void SubblockDeInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
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


double ddtime();

void TxRateMatching(LTE_PHY_PARAMS *lte_phy_params, int *piSeq, int *pcSeq)
{
	int in_buf_sz;
	int out_buf_sz;
	int n_blocks;
	int rm_blk_sz;
	int rm_data_length;
	int rm_last_blk_len;
	int out_block_offset;
	int n_extra_bits;
	int cur_blk_len;

	#ifdef DEBUG_INTL
	int **pInMatrix = new int*[RATE];
	int **pOutMatrix = new int*[RATE];
	#else
	int pInMatrix[RATE * (BLOCK_SIZE + 4)];
	int pOutMatrix[RATE * (BLOCK_SIZE + 4)];
	#endif

	int i, j, r;

	in_buf_sz = lte_phy_params->rm_in_buf_sz;
	out_buf_sz = lte_phy_params->rm_out_buf_sz;
	rm_blk_sz = BLOCK_SIZE + 4;
	rm_data_length = (in_buf_sz / RATE);

	n_blocks = (rm_data_length + (rm_blk_sz - 1)) / rm_blk_sz;
	if (rm_data_length % rm_blk_sz)
	{
		rm_last_blk_len = (rm_data_length % rm_blk_sz);
	}
	else
	{
		rm_last_blk_len = rm_blk_sz;
	}
	
#ifdef DEBUG_INTL
	for (r = 0; r < RATE; r++)
	{
		*(pInMatrix + r) = new int[BLOCK_SIZE + 4];
		*(pOutMatrix + r) = new int[BLOCK_SIZE + 4];
	}
#endif
//	printf("%d %d %d %d\n",rm_blk_sz,rm_last_blk_len,rm_data_length,RATE);
	out_block_offset = 0;
//	printf("0");
	for (i = 0; i < n_blocks; i++)
	{
//		printf("a");
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;
//		printf("b");
		// Transposition
		_SubblockInterleaving(cur_blk_len, piSeq + out_block_offset, pcSeq + out_block_offset, 0);
//		_SubblockInterleaving(cur_blk_len, piSeq, pcSeq, out_block_offset);
/*		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pInMatrix[r * cur_blk_len + j] = piSeq[out_block_offset + RATE * j + r];
				//	cout << pInMatrix[r][i];
			}
		}
		//cout << endl;
//		_SubblockInterleaving(cur_blk_len, piSeq, pcSeq, out_block_offset);
//		double ttime,tbegin;
//		tbegin=ddtime();
//		SubblockInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
//		ttime = ddtime();
//		printf("Sub time is %f\n",ttime - tbegin);
		// Transposition again
		for (j = 0; j < cur_blk_len; j++)
		{
			//printf("old\n");
			for (r = 0; r < RATE; r++)
			{
				pcSeq[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
			}
		}
*/
		out_block_offset += RATE * cur_blk_len;
	}

	n_extra_bits = out_buf_sz - in_buf_sz;
	for (i = 0; i < n_extra_bits; i++)
	{
		pcSeq[in_buf_sz + i] = 0;
	}

#ifdef DEBUG_INTL
	for (r = 0; r < RATE; r++)
	{
		delete[] pInMatrix[r];
		delete[] pOutMatrix[r];
	}
	delete[] pInMatrix;
	delete[] pOutMatrix;
#endif
}


double ddtime()
{
	double tseconds = 0.0;
	struct timeval mytime;
	gettimeofday(&mytime,(struct timezone*)0);
	tseconds = (double)(mytime.tv_sec +
			mytime.tv_usec*1.0e-6);
	return( tseconds );
}

void RxRateMatching(LTE_PHY_PARAMS *lte_phy_params, float *pLLRin, float *pLLRout, int *pHD)
{
	int n_blocks;
	int rm_blk_sz;
	int rm_data_length;
	int rm_last_blk_len;
	int out_block_offset;
	int cur_blk_len;
	int out_buf_sz;
	
#ifdef DEBUG_INTL
	float **pInMatrix = new float*[RATE];
	float **pOutMatrix = new float*[RATE];
#else
	float pInMatrix[RATE * (BLOCK_SIZE + 4)];
	float pOutMatrix[RATE * (BLOCK_SIZE + 4)];
#endif

	int i, j, r;

	out_buf_sz = lte_phy_params->rdm_out_buf_sz;
	rm_blk_sz = BLOCK_SIZE + 4;
	rm_data_length = (out_buf_sz / RATE);

	n_blocks = (rm_data_length + (rm_blk_sz - 1)) / rm_blk_sz;
	if (rm_data_length % rm_blk_sz)
	{
		rm_last_blk_len = (rm_data_length % rm_blk_sz);
	}
	else
	{
		rm_last_blk_len = rm_blk_sz;
	}

#ifdef DEBUG_INTL
	for(r = 0; r < RATE; r++)
	{
		*(pInMatrix + r) = new float[BLOCK_SIZE + 4];
		*(pOutMatrix + r) = new float[BLOCK_SIZE + 4];
	}
#endif

	out_block_offset = 0;
	//printf("out_buf_sz = %d, n_blocks = %d, cur_blk_len = %d, RATE = %d\n",out_buf_sz, n_blocks,cur_blk_len,RATE);
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;
		//printf("n_blocks = %d, cur_blk_len = %d, RATE = %d\n",n_blocks,cur_blk_len,RATE);
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{

				pInMatrix[r * cur_blk_len + j] = pLLRin[out_block_offset + RATE * j + r];

			}
		}  
//		double ttime,tbegin;
//		tbegin = ddtime();
		SubblockDeInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
//		ttime = ddtime() - tbegin;
		//printf("Sub time is %f \n", ttime);
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				
				pLLRin[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
				
			}
		}

		out_block_offset += RATE * cur_blk_len;
	}

	for (i = 0; i < out_buf_sz; i++)
	{
		if (pLLRin[i] < 0)
		{
			pHD[i] = 0;
		}
		else
		{
			pHD[i] = 1;
		}
	}

	for (i = 0; i < out_buf_sz; i++)
	{
		pLLRout[i] = pLLRin[i];
	}

#ifdef DEBUG_INTL
	for (r = 0; r < RATE; r++)
	{
		delete[] *(pInMatrix + r);
		delete[] *(pOutMatrix + r);
	}
	delete[] pInMatrix;
	delete[] pOutMatrix;
#endif
}

