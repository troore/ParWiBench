
#include "ResMapMain.h"


//#define SCMapper
	
int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;

void test_SCMapper(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Resource mapping starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "SubCarrierMapInputReal", "SubCarrierMapInputImag");
	
	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in, lte_phy_params->resm_out);
	
	WriteOutputToFiles(lte_phy_params->resm_out, lte_phy_params->resm_out_buf_sz, "testSubCarrierMapOutputReal", "testSubCarrierMapOutputImag");
	
	std::cout << "Resource mapping ends" << std::endl;
}

void test_SCDemapper(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Resource demapping starts" << std::endl;
	
	int in_buf_sz, out_buf_sz;

	in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_fft_sz * lte_phy_params->N_symb_per_subfr;
	
	ReadInputFromFiles(lte_phy_params->resdm_in, lte_phy_params->resdm_in_buf_sz, "SubCarrierDemapInputReal", "SubCarrierDemapInputImag");
	
	SubCarrierDemapping(lte_phy_params, lte_phy_params->resdm_in, lte_phy_params->resdm_out);

	WriteOutputToFiles(lte_phy_params->resdm_out, lte_phy_params->resdm_out_buf_sz, "testSubCarrierDemapOutputReal", "testSubCarrierDemapOutputImag");

	std::cout << "Resource demapping ends" << std::endl;

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
	
#ifdef SCMapper
	
	test_SCMapper(&lte_phy_params);

	#else

	test_SCDemapper(&lte_phy_params);

	#endif
	
	return 0;
}




