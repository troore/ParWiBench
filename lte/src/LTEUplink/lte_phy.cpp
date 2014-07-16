#include "lte_phy.h"

void lte_phy_init(LTE_PHY_PARAMS *lte_phy_params, int enum_fs, int mod_type, int n_tx_ant, int n_rx_ant)
{
	switch(enum_fs)
	{
	case LTE_PHY_FS_30_72MHZ:
		lte_phy_params->fs = 30720000;
		lte_phy_params->N_samps_per_symb  = LTE_PHY_N_SAMPS_PER_SYMB_30_72MHZ;
		lte_phy_params->N_dft_sz = LTE_PHY_DFT_SIZE_30_72MHZ;
		lte_phy_params->N_samps_cp_l_0    = LTE_PHY_N_SAMPS_CP_L_0_30_72MHZ;
		lte_phy_params->N_samps_cp_l_else = LTE_PHY_N_SAMPS_CP_L_ELSE_30_72MHZ;
		break;
	case LTE_PHY_FS_1_92MHZ:
		lte_phy_params->fs = 1920000;
		lte_phy_params->N_samps_per_symb  = LTE_PHY_N_SAMPS_PER_SYMB_1_92MHZ;
		lte_phy_params->N_dft_sz = LTE_PHY_DFT_SIZE_1_92MHZ;
		lte_phy_params->N_samps_cp_l_0    = LTE_PHY_N_SAMPS_CP_L_0_1_92MHZ;
		lte_phy_params->N_samps_cp_l_else = LTE_PHY_N_SAMPS_CP_L_ELSE_1_92MHZ;
		break;
	default:
		break;
	}
	
	switch(mod_type)
	{
	case LTE_BPSK:
		lte_phy_params->N_bits_per_samp = BPSK_BITS_PER_SAMP;
		break;
	case LTE_QPSK:
		lte_phy_params->N_bits_per_samp = QPSK_BITS_PER_SAMP;
		break;
	case LTE_QAM16:
		lte_phy_params->N_bits_per_samp = QAM16_BITS_PER_SAMP;
		break;
	case LTE_QAM64:
		lte_phy_params->N_bits_per_samp = QAM64_BITS_PER_SAMP;
		break;
	default:
		break;
	}

	lte_phy_params->N_symb_per_subfr = LTE_PHY_N_SYMB_PER_SUBFR;
	
	// Currently, number of TX antennas and RX antennas is same
	lte_phy_params->N_tx_ant = n_tx_ant;
	lte_phy_params->N_rx_ant = n_rx_ant;
}
