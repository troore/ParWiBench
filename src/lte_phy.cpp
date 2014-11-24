#include "lte_phy.h"

void lte_phy_init(LTE_PHY_PARAMS *lte_phy_params, int enum_fs, int mod_type, int n_tx_ant, int n_rx_ant)
{
	switch(enum_fs)
	{
	case LTE_PHY_FS_TEST:
		lte_phy_params->fs = 0;
		lte_phy_params->N_samps_per_symb  = 0;
		lte_phy_params->N_dft_sz = 0;
		lte_phy_params->N_fft_sz = 0;
		lte_phy_params->N_samps_cp_l_0    = LTE_PHY_N_SAMPS_CP_L_ELSE_30_72MHZ;
		lte_phy_params->N_samps_cp_l_else = LTE_PHY_N_SAMPS_CP_L_ELSE_30_72MHZ;
		lte_phy_params->data_len_per_subfr = 6144;
		break;
	case LTE_PHY_FS_30_72MHZ:
		lte_phy_params->fs = 30720000;
		lte_phy_params->N_samps_per_symb  = LTE_PHY_N_SAMPS_PER_SYMB_30_72MHZ;
		lte_phy_params->N_dft_sz = LTE_PHY_DFT_SIZE_30_72MHZ;
		lte_phy_params->N_fft_sz = LTE_PHY_FFT_SIZE_30_72MHZ;
		lte_phy_params->N_samps_cp_l_0    = LTE_PHY_N_SAMPS_CP_L_0_30_72MHZ;
		lte_phy_params->N_samps_cp_l_else = LTE_PHY_N_SAMPS_CP_L_ELSE_30_72MHZ;
		lte_phy_params->data_len_per_subfr = 18432;
		break;
	case LTE_PHY_FS_1_92MHZ:
		lte_phy_params->fs = 1920000;
		lte_phy_params->N_samps_per_symb  = LTE_PHY_N_SAMPS_PER_SYMB_1_92MHZ;
		lte_phy_params->N_dft_sz = LTE_PHY_DFT_SIZE_1_92MHZ;
		lte_phy_params->N_fft_sz = LTE_PHY_FFT_SIZE_1_92MHZ;
		lte_phy_params->N_samps_cp_l_0    = LTE_PHY_N_SAMPS_CP_L_0_1_92MHZ;
		lte_phy_params->N_samps_cp_l_else = LTE_PHY_N_SAMPS_CP_L_ELSE_1_92MHZ;
		lte_phy_params->data_len_per_subfr = 2368;
		break;
	default:
		break;
	}

	lte_phy_params->mod_type = mod_type;
	
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

	lte_phy_params->dmrs_symb_pos[0] = 3;
	lte_phy_params->dmrs_symb_pos[1] = 10;

	lte_phy_params->sc_loc = 1;

	// Compute data length for each module
	// Turbo
	lte_phy_params->te_in_buf_sz = lte_phy_params->data_len_per_subfr;
	lte_phy_params->te_out_buf_sz = RATE * (lte_phy_params->te_in_buf_sz + 4 * ((lte_phy_params->te_in_buf_sz + BLOCK_SIZE - 1) / BLOCK_SIZE));
	lte_phy_params->td_in_buf_sz = lte_phy_params->te_out_buf_sz;
	lte_phy_params->td_out_buf_sz = lte_phy_params->te_in_buf_sz;

	// Rate matching
	lte_phy_params->rm_in_buf_sz = lte_phy_params->te_out_buf_sz;
	lte_phy_params->rm_out_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (lte_phy_params->N_symb_per_subfr - 2) * lte_phy_params->N_bits_per_samp;
	lte_phy_params->rdm_in_buf_sz = lte_phy_params->rm_out_buf_sz;
	lte_phy_params->rdm_out_buf_sz = lte_phy_params->rm_in_buf_sz;

	// Scrambling
	lte_phy_params->scramb_in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (lte_phy_params->N_symb_per_subfr - 2) * lte_phy_params->N_bits_per_samp;
	lte_phy_params->scramb_out_buf_sz = lte_phy_params->scramb_in_buf_sz;
	lte_phy_params->descramb_in_buf_sz = lte_phy_params->scramb_in_buf_sz;
	lte_phy_params->descramb_out_buf_sz = lte_phy_params->scramb_in_buf_sz;

	// Modulation
	lte_phy_params->mod_in_buf_sz = lte_phy_params->descramb_out_buf_sz;
	lte_phy_params->mod_out_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (lte_phy_params->N_symb_per_subfr - 2);
	lte_phy_params->demod_in_buf_sz = lte_phy_params->mod_out_buf_sz;
	lte_phy_params->demod_out_buf_sz = lte_phy_params->mod_in_buf_sz;

	// Transform precoding
	lte_phy_params->trans_encoder_in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (LTE_PHY_N_SYMB_PER_SUBFR - 2);
	lte_phy_params->trans_encoder_out_buf_sz = lte_phy_params->trans_encoder_in_buf_sz;
	lte_phy_params->trans_decoder_in_buf_sz = lte_phy_params->trans_encoder_in_buf_sz;
	lte_phy_params->trans_decoder_out_buf_sz = lte_phy_params->trans_encoder_in_buf_sz;

	// Resource mapping
	lte_phy_params->resm_in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (lte_phy_params->N_symb_per_subfr - 2);
	lte_phy_params->resm_out_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_fft_sz * lte_phy_params->N_symb_per_subfr;
	lte_phy_params->resdm_in_buf_sz = lte_phy_params->resm_out_buf_sz;
	lte_phy_params->resdm_out_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * lte_phy_params->N_symb_per_subfr;

	// Equalizing
	lte_phy_params->eq_in_buf_sz = lte_phy_params->N_rx_ant * LTE_PHY_N_SYMB_PER_SUBFR * lte_phy_params->N_dft_sz;
	lte_phy_params->eq_out_buf_sz = lte_phy_params->N_rx_ant * (lte_phy_params->N_symb_per_subfr - 2) * lte_phy_params->N_dft_sz;

	// OFDM
	lte_phy_params->ofmod_in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_fft_sz * lte_phy_params->N_symb_per_subfr;
	lte_phy_params->ofmod_out_buf_sz = lte_phy_params->N_tx_ant * (lte_phy_params->N_fft_sz + lte_phy_params->N_samps_cp_l_0) * lte_phy_params->N_symb_per_subfr;
	lte_phy_params->ofdemod_in_buf_sz = lte_phy_params->ofmod_out_buf_sz;
	lte_phy_params->ofdemod_out_buf_sz = lte_phy_params->ofmod_in_buf_sz;
}
