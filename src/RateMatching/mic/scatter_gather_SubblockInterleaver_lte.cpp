
#include"SubblockInterleaver_lte.h"
#include<immintrin.h>
//#define index 0x000005a000000540000004e00000048000000420000003c00000036000000300000002a000000240000001e00000018000000120000000c00000006000000000;
//			15	14	13	12	11	10	9	8	7	6	5	4	3	2	1       0	

#define LEN16 16

#ifdef __MIC__
typedef __attribute__((aligned(64))) union zmmf {
	        __m512i reg;
	        int elems[LEN16];
} zmmi_t;
#endif

void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset)
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

	int pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];
	zmmi_t pInterMatrix_simd[((BLOCK_SIZE + 31) / 32) * 2];
	
	int num_threads=32,StrIdx,r;
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
*/		}
	//	printf("right\n");
		OutIdx=0;
		
/*		for(int c=0;c<C_sb;c++)
		{
			int col = InterColumnPattern[c];
			
		}

		for(int k = NumDummy; k < C_sb * R_sb; k+=C_sb)
		{
			
		}
*/
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


}

