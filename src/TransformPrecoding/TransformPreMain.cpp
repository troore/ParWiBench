
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex>
#include <iostream>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"

#include "TransformPrecoder.h"

//#define TransformPre

LTE_PHY_PARAMS lte_phy_params;

void test_encoder(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Transform Precoder starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->trans_encoder_in, lte_phy_params->trans_encoder_in_buf_sz, "../testsuite/TransformPrecoderInputReal", "../testsuite/TransformPrecoderInputImag");
	GeneRandomInput(lte_phy_params->trans_encoder_in, lte_phy_params->trans_encoder_in_buf_sz, "../testsuite/TransformPrecoderInputReal", "../testsuite/TransformPrecoderInputImag");

	TransformPrecoding(lte_phy_params, lte_phy_params->trans_encoder_in, lte_phy_params->trans_encoder_out);

	WriteOutputToFiles(lte_phy_params->trans_encoder_out, lte_phy_params->trans_encoder_out_buf_sz, "../testsuite/testTransformPrecoderOutputReal", "../testsuite/testTransformPrecoderOutputImag");

	std::cout << "Transform Precoder ends" << std::endl;
}

void test_decoder(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Transform Decoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->trans_decoder_in, lte_phy_params->trans_decoder_in_buf_sz, "../testsuite/testTransformPrecoderOutputReal", "../testsuite/testTransformPrecoderOutputImag");

	TransformDecoding(lte_phy_params, lte_phy_params->trans_decoder_in, lte_phy_params->trans_decoder_out);

	WriteOutputToFiles(lte_phy_params->trans_decoder_out, lte_phy_params->trans_decoder_out_buf_sz, "../testsuite/testTransformDecoderOutputReal", "../testsuite/testTransformDecoderOutputImag");
	
	std::cout << "Transform Decoder ends" << std::endl;

}

int main(int argc, char *argv[])
{
	int fs_id;
	int n_tx_ant, n_rx_ant;
	int mod_type;

	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;
	
	if (argc != 5 && argc != 2)
	{
		lte_phy_usage_info(argv[0]);
		
		return 1;
	}

	if (argc == 5)
	{
		fs_id = atoi(argv[1]);
		mod_type = atoi(argv[2]);
		n_tx_ant = atoi(argv[3]);
		n_rx_ant = atoi(argv[4]);

		lte_phy_init(&lte_phy_params, fs_id, mod_type, n_tx_ant, n_rx_ant);
	}
	else
	{
		fs_id = atoi(argv[1]);

		lte_phy_init(&lte_phy_params, fs_id);
	}

#ifdef TransformPre

	test_encoder(&lte_phy_params);
	
#else

	test_decoder(&lte_phy_params);
	
	strcpy(tx_in_fname, "../testsuite/TransformPrecoderInputReal");
	strcpy(rx_out_fname, "../testsuite/testTransformDecoderOutputReal");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
	
	strcpy(tx_in_fname, "../testsuite/TransformPrecoderInputImag");
	strcpy(rx_out_fname, "../testsuite/testTransformDecoderOutputImag");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);

#endif


	return 0;
}

