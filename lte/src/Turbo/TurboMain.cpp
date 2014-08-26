
#include "TurboMain.h"

int RANDOMSEED;

//#define TurboEnc

LTE_PHY_PARAMS lte_phy_params;

void test_turbo_encoding(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Turbo Encoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "TurboEncoderInput");

	turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out);

	WriteOutputToFiles(lte_phy_params->te_out, lte_phy_params->te_out_buf_sz, "testTurboEncoderOutput");

	std::cout << "Turbo Encoder ends" << std::endl;
}

void test_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Turbo Decoder starts" << std::endl;
	
//	ReadInputFromFiles(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "testTurboEncoderOutput");
	GeneRandomInput(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "testTurboEncoderOutput");
	
	for (int i = 0; i < lte_phy_params->td_in_buf_sz; i++)
		lte_phy_params->td_in[i] = (1 - 2 * lte_phy_params->td_in[i]);
   
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out);

	
	for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
		lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;
	
	WriteOutputToFiles(lte_phy_params->td_out, lte_phy_params->td_out_buf_sz, "testTurboDecoderOutput");
	
	std::cout << "Turbo Decoder ends" << std::endl;
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
	
//	turbo_init();

#ifdef TurboEnc

	test_turbo_encoding(&lte_phy_params);
	
	#else

//	for (int i = 0; i < 100; i++)
		test_turbo_decoding(&lte_phy_params);

	#endif

	return 0;
}




