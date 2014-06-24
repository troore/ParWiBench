
#include "TurboMain.h"

int RANDOMSEED;

//#define TurboEnc

using namespace std;

//fsm FSM("rsc.txt");
//Interleaver_lte<int,int> EInter;
//Interleaver_lte<int,float> DInter;

int main()
{
#ifdef TurboEnc
	
	cout<<"Turbo Encoder starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Turbo TbE(&User);
//FIFO<int> TbEIn(1,TbE.InBufSz);
//	FIFO<int> TbEOut(1,TbE.OutBufSz);
	int *piSeq = new int[TbE.InBufSz];
	int *pcSeq = new int[TbE.OutBufSz];
	
//	ReadInputFromFiles(TbE.pInpBuf,(TbE.InBufSz),"TurboEncoderInput");
//	ReadInputFromFiles(piSeq, (TbE.InBufSz), "TurboEncoderInput");
	ReadInputFromFiles(piSeq, (TbE.InBufSz), "TurboEncoderRandomInput");
//	GeneRandomInput(piSeq, TbE.InBufSz, "TurboEncoderRandomInput");
//GeneRandomInput(TbE.pInpBuf,TbE.InBufSz);
//	TbE.TurboEncoding(&TbEOut);
	TbE.TurboEncoding(piSeq, pcSeq);
	
//	WriteOutputToFiles(&TbEOut,(TbE.OutBufSz),"testTurboEncoderOutput");
	WriteOutputToFiles(pcSeq, (TbE.OutBufSz), "testTurboEncoderOutput");
//	WriteOutputToFiles(pcSeq, (TbE.OutBufSz), "testTurboEncoderOutput.bak");
//ReadOutput(&TbEOut,(TbE.OutBufSz));
	
	cout<<"Turbo Encoder ends"<<endl;
	
	#else
	
	cout<<"Turbo Decoder starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Turbo TbD(&BS);
//FIFO<float> TbDIn(1,TbD.InBufSz);
//	FIFO<int> TbDOut(1,TbD.OutBufSz);
	float *pInpData = new float[TbD.InBufSz];
	int *pOutBits = new int[TbD.OutBufSz];
	
//	ReadInputFromFiles(TbD.pInpBuf,TbD.InBufSz,"TurboDecoderInput");
//	ReadInputFromFiles(pInpData, TbD.InBufSz, "TurboDecoderInput");
	
	ReadInputFromFiles(pInpData, TbD.InBufSz, "testTurboEncoderOutput");
	for (int i = 0; i < TbD.InBufSz; i++)
		pInpData[i] = (1 - 2 * pInpData[i]);
	
//GeneRandomInput(TbD.pInpBuf,TbD.InBufSz,"TurboDecoderRandomInput");
//GeneRandomInput(TbD.pInpBuf,TbD.InBufSz);
//	TbD.TurboDecoding(&TbDOut);
	TbD.TurboDecoding(pInpData, pOutBits);

	
	for (int i = 0; i < TbD.OutBufSz; i++)
		pOutBits[i] = (1 - pOutBits[i]) / 2;
	
		
//	WriteOutputToFiles(&TbDOut,TbD.OutBufSz,"testTurboDecoderOutput");
	WriteOutputToFiles(pOutBits, TbD.OutBufSz, "testTurboDecoderOutput");
//ReadOutput(&TbDOut,TbD.OutBufSz);
	
	cout<<"Turbo Decoder ends"<<endl;

	#endif
	
	return 0;
}




