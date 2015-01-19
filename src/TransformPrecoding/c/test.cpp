
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <complex>
#include <iostream>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"

#include "TransformPrecoder.h"

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

void check()
{
	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;

	strcpy(tx_in_fname, "../testsuite/TransformPrecoderInputReal");
	strcpy(rx_out_fname, "../testsuite/testTransformDecoderOutputReal");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
	
	strcpy(tx_in_fname, "../testsuite/TransformPrecoderInputImag");
	strcpy(rx_out_fname, "../testsuite/testTransformDecoderOutputImag");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef TransformPre

	test_encoder(lte_phy_params);

//#else
	test_decoder(lte_phy_params);
	check();

//#endif

}
