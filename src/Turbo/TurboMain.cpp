
#include "Turbo.h"
#include "Turbo_pardec.h"
#include "GeneralFunc.h"
#include "gauss.h"
#include "meas.h"
#include "lte_phy.h"
#include "check.h"
//#define TurboEnc

LTE_PHY_PARAMS lte_phy_params;

void test_turbo_encoding(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Turbo Encoder starts" << std::endl;
	
//	ReadInputFromFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../TurboEncoderInput");
	GeneRandomInput(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../TurboEncoderInput");
//	WriteOutputToFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "TurboEncoderInput");

	/*
	for (int i = 0; i < lte_phy_params->te_in_buf_sz; i++)
		lte_phy_params->te_in[i] = 1;
	*/

	turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out);

	WriteOutputToFiles(lte_phy_params->te_out, lte_phy_params->te_out_buf_sz, "../testTurboEncoderOutput");

	std::cout << "Turbo Encoder ends" << std::endl;
}

void test_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, int n_iters)
{
	std::cout << "Turbo Decoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "../testTurboEncoderOutput");
//	GeneRandomInput(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "../testsuite/testTurboEncoderOutput");
	
	for (int i = 0; i < lte_phy_params->td_in_buf_sz; i++)
	{
		lte_phy_params->td_in[i] = (1 - 2 * lte_phy_params->td_in[i]);
	}

//for(int i=0;i<1000;i++)
#ifndef PARD
//	printf("tuobo\n");
	double tbegin,ttime;
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
	tbegin = dtime();
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
	ttime = dtime();
	printf("real time is %f\n",ttime - tbegin);
#else
//	printf("par\n");
	par_turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, 1, n_iters);
#endif
	
	for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
	{
		lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;
		//	printf("%d", lte_phy_params->td_out[i]);
	}
//	printf("\n");
	
	WriteOutputToFiles(lte_phy_params->td_out, lte_phy_params->td_out_buf_sz, "../testTurboDecoderOutput");
	
	std::cout << "Turbo Decoder ends" << std::endl;
}

int main(int argc, char *argv[])
{
	int enum_fs;
	int n_tx_ant, n_rx_ant;
	int mod_type;

	int num_buffered_subframes = 1;

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
	double ttime,tbegin;
	tbegin = dtime();
#ifdef TurboEnc

	test_turbo_encoding(&lte_phy_params);
	
#else

	int n_iters = 1;
	
	test_turbo_decoding(&lte_phy_params, n_iters);

#endif
	ttime = dtime();
	printf("whole time is %f\n",ttime - tbegin);
	char s1[100] = "../TurboEncoderInput";
	char s2[100] = "../testTurboDecoderOutput";
	printf("%d\n",check_float(s1,s2));
	return 0;
}




