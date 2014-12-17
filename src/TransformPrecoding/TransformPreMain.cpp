
#include "TransformPreMain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "meas.h"

//#define TransformPre

int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;

void test_encoder(LTE_PHY_PARAMS *lte_phy_params)
{

//	std::cout << "Transform Precoder starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->trans_encoder_in, lte_phy_params->trans_encoder_in_buf_sz, "/home/xblee/ParWiBench/src/TransformPrecoding/TransformPrecoderInputReal", "/home/xblee/ParWiBench/src/TransformPrecoding/TransformPrecoderInputImag");

	TransformPrecoding(lte_phy_params, lte_phy_params->trans_encoder_in, lte_phy_params->trans_encoder_out);

	WriteOutputToFiles(lte_phy_params->trans_encoder_out, lte_phy_params->trans_encoder_out_buf_sz, "testTransformPrecoderOutputReal", "TransformPrecoding/testTransformPrecoderOutputImag");

//	std::cout << "Transform Precoder ends" << std::endl;
	
}

void test_decoder(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Transform Decoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->trans_decoder_in, lte_phy_params->trans_decoder_in_buf_sz, "../TransformDecoderInputReal", "../TransformDecoderInputImag");

	double tbegin, ttime;
	tbegin = dtime();

	TransformDecoding(lte_phy_params, lte_phy_params->trans_decoder_in, lte_phy_params->trans_decoder_out);

	ttime = dtime() - tbegin;
	printf("whole time is %fms\n", ttime * 1000.0);
	
	WriteOutputToFiles(lte_phy_params->trans_decoder_out, lte_phy_params->trans_decoder_out_buf_sz, "../testTransformDecoderOutputReal", "../testTransformDecoderOutputImag");
	
	std::cout << "Transform Decoder ends" << std::endl;

}

int main(int argc, char *argv[])
{
	int enum_fs;
    int n_tx_ant, n_rx_ant;
	int mod_type;
	

	if (argc != 5)
	{
		printf("Usage: ./a.out enum_fs mod_type n_tx_ant n_rx_ant\n");
		
		return 1;
	}
	
	enum_fs = atoi(argv[1]);
	mod_type = atoi(argv[2]);
	n_tx_ant = atoi(argv[3]);
	n_rx_ant = atoi(argv[4]);
	

	lte_phy_init(&lte_phy_params, enum_fs, mod_type, n_tx_ant, n_rx_ant);
	
	#ifdef TransformPre

	test_encoder(&lte_phy_params);
	
	#else

	test_decoder(&lte_phy_params);

	#endif


	return 0;
}

