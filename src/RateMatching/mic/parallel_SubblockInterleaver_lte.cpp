
#include"SubblockInterleaver_lte.h"

void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset)
{
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

    
    int num_threads=32,StrIdx,r;
    omp_set_num_threads(num_threads);    
#pragma omp parallel for private(StrIdx,r)
	for (int c = 0; c < num_threads; c++)
	                {
//    	printf("hello from thread %d\n",c);
	for (StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
    {

        //////////////// write into matrix //////////////////
        for (r = 0; r < R_sb; r++)
        {
//		for (int c = 0; c < C_sb; c++)
//	    {
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
//	for (int c = 0; c < C_sb; c++)
//	{
            int col = InterColumnPattern[c];
            for (r = 0; r < R_sb; r++)
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
    

}

