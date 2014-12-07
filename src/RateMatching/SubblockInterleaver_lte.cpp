
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
void SubblockInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	int DummyValue;
//	int InterColumnPattern[32];

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
//	R_sb = (int)(ceil((((float)D) / ((float)C_sb))));
	R_sb = (D + (C_sb - 1)) / C_sb;
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;
	
//	int OutIdx;
//	T **pInterMatrix=new T*[R_sb];
//	for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}
	T pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];
//	T *VpInpSeq;
//	T *VpOutSeq;
	int OutIdx;
	
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
		//	VpInpSeq=*(pInpMtr+StrIdx);
		//	VpOutSeq=*(pOutMtr+StrIdx);
		//////////////// write into matrix //////////////////
		for (int r = 0; r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
					//	*(*(pInterMatrix+r) + c) = DummyValue;
					pInterMatrix[r * C_sb + c] = DummyValue;
				}
				else
				{
					//	*(*(pInterMatrix+r)+c)=*(VpInpSeq+(k-NumDummy));
					pInterMatrix[r * C_sb + c] = pInpMtr[StrIdx * D + k - NumDummy];
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
				//	T v = *(*(pInterMatrix+r)+col);
				T v = pInterMatrix[r * C_sb + col];
				
				if (v == DummyValue)
				{}
				else
				{
					//	*(VpOutSeq+OutIdx)=v;
					pOutMtr[StrIdx * D + OutIdx] = v;
					OutIdx++;
				}  
			}
		}  
	}
	
//////////////////// Interleaving for i=2 ///////////////////////
//	int *Pi=new int[K_pi];
	int Pi[((BLOCK_SIZE + 31) / 32) * 32];
//	T *pInterSeq=new T[K_pi];
	T pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];
	
//	VpInpSeq=*(pInpMtr+(Rate-1));
//	VpOutSeq=*(pOutMtr+(Rate-1));
	for (int k = 0;k < NumDummy; k++)
	{
		//	*(pInterSeq+k)=DummyValue;
		pInterSeq[k] = DummyValue;
	}
	for (int k = NumDummy; k < K_pi; k++)
	{
		//	*(pInterSeq+k)=*(VpInpSeq+(k-NumDummy));
		pInterSeq[k] = pInpMtr[(Rate - 1) * D + (k - NumDummy)];
	}

	for (int k = 0; k < K_pi; k++)
	{
		//	int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
		int idxP = k / R_sb;
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;
		
		//	*(Pi+k)=idx;
		Pi[k] = idx;
	}

	OutIdx=0;
	for (int k = 0; k < K_pi; k++)
	{
		//	T v = *(pInterSeq+(*(Pi+k)));
		T v = pInterSeq[Pi[k]];
		if (v == DummyValue)
		{}
		else
		{
			//	*(VpOutSeq+OutIdx) = v;
			pOutMtr[(Rate - 1) * D + OutIdx] = v;
			OutIdx++;
		}
	}
////////////////END interleaving/////////////////
//	delete[] pInterSeq;
//	delete[] Pi;
////////////////////END Interleaving for i=2 ////////////////////
}

/*
template<class U, class T>
void SubblockInterleaver_lte<U,T>::SubblockDeInterleaving(U SeqLen,T **pInpMtr, T **pOutMtr)
{
	D=SeqLen;
	R_sb=(int)(ceil((((double)D)/((double)C_sb))));
	K_pi=R_sb*C_sb;
	NumDummy=K_pi-D;
	int InIdx;
	int OutIdx;
//////////////////// DeInterleaving for i=0,1 ///////////////////////
	T **pInterMatrix=new T*[R_sb];
	for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}

	T *VpInpSeq;
	T *VpOutSeq;
	for(int StrIdx=0;StrIdx<(Rate-1);StrIdx++)
	{
		VpInpSeq=*(pInpMtr+StrIdx);
		VpOutSeq=*(pOutMtr+StrIdx);
//////////////////// Write DeInter Matrix//////////////////////
		InIdx=0;
		//////////////// write into matrix //////////////////
		for(int r=0;r<R_sb;r++)
		{
			for(int c=0;c<C_sb;c++)
			{
				int k=r*C_sb+c;
				if(k<NumDummy){*(*(pInterMatrix+r)+c)=DummyValue;}
				else{*(*(pInterMatrix+r)+c)=(T)0;}
			}
		}
		////////////// END write into matrix ////////////////
		////////////// Read out matrix ///////////////////////
		for(int c=0;c<C_sb;c++)
		{
			int col = InterColumnPattern[c];
			for(int r=0;r<R_sb;r++)
			{
				int k=col*R_sb+r;
				T v = *(*(pInterMatrix+r)+col);
				if(v==DummyValue){}
				else
				{
					*(*(pInterMatrix+r)+col)=*(VpInpSeq+InIdx);      
					InIdx++;
				}  
			}
		}  
		////////////// END read out matrix////////////////////
////////////////////END Write DeInter Matrix//////////////////////
//////////////////// Read Out DeInter Matrix//////////////////////
		OutIdx=0;
		for(int r=0;r<R_sb;r++)
		{
			for(int c=0;c<C_sb;c++)
			{
				T v = *(*(pInterMatrix+r)+c);
				if(v==DummyValue){}
				else
				{
					*(VpOutSeq+OutIdx)=*(*(pInterMatrix+r)+c);
					OutIdx++;
				}
			}
		}
///////////////////END Read Out DeInter Matrix////////////////////
	}
	for(int r=0;r<R_sb;r++){delete[] *(pInterMatrix+r);}
	delete[] pInterMatrix;

////////////////////END DeInterleaving for i=0,1 ////////////////////
//////////////////// DeInterleaving for i=2 ///////////////////////
	int *Pi=new int[K_pi];

	T *pInterSeq=new T[K_pi];
	VpInpSeq=*(pInpMtr+(Rate-1));
	VpOutSeq=*(pOutMtr+(Rate-1));
	for(int k=0;k<NumDummy;k++)
	{*(pInterSeq+k)=DummyValue;}
//////////////// Pi & DePi//////////////////
	for(int k=0;k<K_pi;k++)
	{
		int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
		int idx = ((InterColumnPattern[idxP])+(C_sb*(k%R_sb))+1)%K_pi;
		*(Pi+k)=idx;
	}
///////////////END Pi & DePi/////////////////
/////////////// DeInterleaving ////////////////////
	InIdx=0;
	for(int k=0;k<K_pi;k++)
	{
		T v = *(pInterSeq+(*(Pi+k)));
		if(v==DummyValue){}
		else{*(pInterSeq+(*(Pi+k)))=*(VpInpSeq+InIdx);InIdx++;}
	}
	OutIdx=0;
	for(int k=NumDummy;k<K_pi;k++)
	{*(VpOutSeq+OutIdx)=*(pInterSeq+k);OutIdx++;}
////////////////END DeInterleaving/////////////////
	delete[] pInterSeq;
	delete[] Pi;

////////////////////END DeInterleaving for i=2 ////////////////////
}
*/

