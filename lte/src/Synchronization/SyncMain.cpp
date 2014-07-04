#include <fstream>

#include "SyncMain.h"

//#define MOD

int main(int argc, char *argv[])
{
	#ifdef MOD

	std::cout << "Starts to add preamble..." << std::endl;

	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Sync sync(&User);

	complex<float> *pInpData = new complex<float>[sync.InBufSz];
	complex<float> *pOutData = new complex<float>[sync.OutBufSz];

#ifdef TESTBUFSZ
	std::cout << "TESTBUFSZ=" << TESTBUFSZ << std::endl;
	for (int i = 0; i < sync.InBufSz; i++)
	{
		pInpData[i] = complex<float>(1.0, 1.0);
	}
#else
	GeneRandomInput(pInpData, sync.InBufSz, "BeforePreambleInputReal", "BeforePreambleInputImag");
#endif


	sync.AddPreamble(pInpData, pOutData);

//	WriteOutputToFiles(pOutData, sync.OutBufSz, "AfterPreambleOutputReal", "AfterPreambleOutputImag");
	WriteOutputToFiles(pOutData, sync.OutBufSz, "AfterPreambleOutputReal", "AfterPreambleOutputImag");


	delete[] pInpData;
	delete[] pOutData;

	std::cout << "Preamble added!" << std::endl;
	
	#else

	std::cout << "Synchronization starts..." << std::endl;

	BSPara BS;
	BS.initBSPara();
	Sync sync(&BS);

	std::cout << sync.InBufSz << std::endl;
	std::cout << sync.OutBufSz << std::endl;
	
	ifstream fin_real, fin_imag;

	float real, imag;
	complex<float> data;
	int rx_len;

	complex<float> *pOutData = new complex<float>[sync.OutBufSz];

	fin_real.open("AfterPreambleOutputReal");
	fin_imag.open("AfterPreambleOutputImag");

	int cnt = 0;
	
	if (fin_real.is_open() && fin_imag.is_open())
	{
		while (!fin_real.eof() && !fin_imag.eof()/* && !rx_len*/)
		{
			std::cout << cnt++ << std::endl;
			fin_real >> real;
			fin_imag >> imag;
			data = complex<float>(real, imag);

			rx_len = sync.TryDetectPreamble(&data, 1, pOutData);

			std::cout << rx_len << std::endl;
		}
	}
	else
	{
		std::cout << "Fail to open file to read. Exiting..." << std::endl;

		return 1;
	} 

		/*
		 * Segmentation fault when close these two files, what the fuck???
		 * Because there is overflow (or underflow, maybe) for the member
		 * 'output_time_synchro', not being allocated or not fully being allocated.
		 */
	fin_imag.close();
	fin_real.close();


	WriteOutputToFiles(pOutData, sync.OutBufSz, "SynchronizationOutputReal", "SynchronizationOutputImag");

	delete[] pOutData;

	std::cout << "Sychronization ends." << std::endl;

	#endif
	
	return 0;
}
