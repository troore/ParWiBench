
#include "TransformPreMain.h"

int RANDOMSEED;

//#define TransformPre

int main()
{
	#ifdef TransformPre
	
	cout<<"Transform Precoder starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	TransformPrecoder TP(&User);
//FIFO<complex<float> > TPIn(1,TP.InBufSz);
//	FIFO<complex<float> > TPOut(1,TP.OutBufSz);
	complex<float> *pQAMSeq = new complex<float>[TP.InBufSz];
	complex<float> *pDataMatrix = new complex<float>[TP.OutBufSz];
	
//	ReadInputFromFiles(TP.pInpBuf,(TP.InBufSz),"TransformPrecoderInputReal","TransformPrecoderInputImag");
	ReadInputFromFiles(pQAMSeq, (TP.InBufSz),"TransformPrecoderInputReal","TransformPrecoderInputImag");
//GeneRandomInput(TP.pInpBuf,TP.InBufSz,"TransformPrecoderRandomInputReal","TransformPrecoderRandomInputImag");
//GeneRandomInput(TP.pInpBuf,TP.InBufSz);
//	TP.TransformPrecoding(&TPOut);
	TP.TransformPrecoding(pQAMSeq, pDataMatrix);
	
//	WriteOutputToFiles(&TPOut,(TP.OutBufSz),"testTransformPrecoderOutputReal","testTransformPrecoderOutputImag");
	WriteOutputToFiles(pDataMatrix,(TP.OutBufSz),"testTransformPrecoderOutputReal","testTransformPrecoderOutputImag");
//WriteOutputToFiles(&TPOut,(TP.OutBufSz),"TransformPrecoderRandomOutputReal","TransformPrecoderRandomOutputImag");
//ReadOutput(&TPOut,(TP.OutBufSz));

	delete[] pQAMSeq;
	delete[] pDataMatrix;

	cout<<"Transform Precoder ends"<<endl;

	#else

	cout<<"Transform Decoder starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	TransformPrecoder TD(&BS);
//FIFO<complex<float> > TDIn(1,TD.InBufSz);
//	FIFO<complex<float> > TDOut(1,TD.OutBufSz);
	complex<float> *pDataMatrix = new complex<float>[TD.InBufSz];
	complex<float> *pDecQAMSeq = new complex<float>[TD.OutBufSz];
	
//	ReadInputFromFiles(TD.pInpBuf,(TD.InBufSz),"TransformDecoderInputReal","TransformDecoderInputImag");
	ReadInputFromFiles(pDataMatrix, (TD.InBufSz), "TransformDecoderInputReal","TransformDecoderInputImag");
//GeneRandomInput(TD.pInpBuf,TD.InBufSz,"TransformDecoderRandomInputReal","TransformDecoderRandomInputImag");
//GeneRandomInput(TD.pInpBuf,TD.InBufSz);
//	TD.TransformDecoding(&TDOut);
	TD.TransformDecoding(pDataMatrix, pDecQAMSeq);
	
//	WriteOutputToFiles(&TDOut,(TD.OutBufSz),"testTransformDecoderOutputReal","testTransformDecoderOutputImag");
	WriteOutputToFiles(pDecQAMSeq, (TD.OutBufSz), "testTransformDecoderOutputReal", "testTransformDecoderOutputImag");
//WriteOutputToFiles(&TDOut,(TD.OutBufSz),"TransformDecoderRandomOutputReal","TransformDecoderRandomOutputImag");
//ReadOutput(&TDOut,(TD.OutBufSz));

	delete[] pDataMatrix;
	delete[] pDecQAMSeq;
	
	cout<<"Transform Decoder ends"<<endl;

	#endif

	return 0;
}

