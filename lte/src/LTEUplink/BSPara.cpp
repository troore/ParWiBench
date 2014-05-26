#include "BSPara.h"
/////////// Single User Base Station Parameters/////////////////

BSPara::BSPara()
{}

void BSPara::initBSPara(void)
{
	NumULSymbSF=14;
	NIFFT=128;
	CPLen=9;
	RSU=0;
	RSV=0;

	DMRSSymbPos[0]=3;
	DMRSSymbPos[1]=10;

	SCLocPattern=1;
	MDFTPerUser=75;
	MQAMPerUser=16;
	NumLayerPerUser=2;
	
	BlkSize = 2368;
	DataLengthPerUser = 2368;

	EstimationMethod=0;
	NumRxAntenna=2;

	ProcessingStatusFlag=false;
	BSParameterPrintFlag=false;
	BufferSizeFlag=false;

	/* Turbo parameters */
	Rate = 3;
	n_gens = 2;
	gens[0] = 013, gens[1] = 015;
	constraint_length = 4;
	n_iterations = 2;
	/* Turbo parameters over... */
	
	if(BSParameterPrintFlag)
	{
		cout<<"single user Uplink LTE parameter:"<<endl;
		cout<<"Number of symbols per subframe="<<NumULSymbSF<<endl;
		cout<<"SCFDMA IFFT length="<<NIFFT<<"   Cyclic Prefix length="<<CPLen<<endl;
		cout<<"DFT length="<<MDFTPerUser<<endl;
		switch(MQAMPerUser)
		{
			case 4:cout<<"Modulation scheme is QPSK"<<endl;break;
			case 16:cout<<"Modulation scheme is 16QAM"<<endl;break;
			case 64:cout<<"Modulation scheme is 64QAM"<<endl;break;
			default:cout<<"Invalid modulation scheme"<<endl;
		}

		cout<<"Number of Transmitter antennas="<<NumLayerPerUser<<endl;
		cout<<"Number of Receiver antennas="<<NumRxAntenna<<endl;
	}

}

BSPara::~BSPara()
{
}
