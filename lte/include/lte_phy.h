#ifndef __LTE_PHY_H_
#define __LTE_PHY_H_

/******************************************
DEFINES
*******************************************/

// FFT sizes
#define LTE_PHY_FFT_SIZE_1_92MHZ  128
#define LTE_PHY_FFT_SIZE_3_84MHZ  256
#define LTE_PHY_FFT_SIZE_7_68MHZ  512
#define LTE_PHY_FFT_SIZE_15_36MHZ 1024
#define LTE_PHY_FFT_SIZE_23_04MHZ 1536
#define LTE_PHY_FFT_SIZE_30_72MHZ 2048

// DFT sizes
#define LTE_PHY_DFT_SIZE_1_92MHZ  75
#define LTE_PHY_DFT_SIZE_3_84MHZ  150
#define LTE_PHY_DFT_SIZE_7_68MHZ  300
#define LTE_PHY_DFT_SIZE_15_36MHZ 600
#define LTE_PHY_DFT_SIZE_23_04MHZ 900
#define LTE_PHY_DFT_SIZE_30_72MHZ 1200

// N_rb_ul
#define LTE_PHY_N_RB_UL_1_4MHZ 6
#define LTE_PHY_N_RB_UL_3MHZ   15
#define LTE_PHY_N_RB_UL_5MHZ   25
#define LTE_PHY_N_RB_UL_10MHZ  50
#define LTE_PHY_N_RB_UL_15MHZ  75
#define LTE_PHY_N_RB_UL_20MHZ  100
#define LTE_PHY_N_RB_UL_MAX    110

// N_sc_rb
#define LTE_PHY_N_SC_RB_UL 12

// N_ant
#define LTE_PHY_N_ANT_MAX 1

// Symbol, CP, Slot, Subframe, and Frame timing
// Generic
#define LTE_PHY_SFN_MAX           1023
#define LTE_PHY_N_SLOTS_PER_SUBFR 2
#define LTE_PHY_N_SUBFR_PER_FRAME 10
#define LTE_PHY_N_SYMB_PER_SLOT 7
#define LTE_PHY_N_SYMB_PER_SUBFR (LTE_PHY_N_SLOTS_PER_SUBFR * LTE_PHY_N_SYMB_PER_SLOT)

// 1.25MHz bandwidths
#define LTE_PHY_N_SAMPS_PER_SYMB_1_92MHZ  128
#define LTE_PHY_N_SAMPS_CP_L_0_1_92MHZ    9
#define LTE_PHY_N_SAMPS_CP_L_ELSE_1_92MHZ 9

// 20MHz bandwidths
#define LTE_PHY_N_SAMPS_PER_SYMB_30_72MHZ  2048
#define LTE_PHY_N_SAMPS_CP_L_0_30_72MHZ    9
#define LTE_PHY_N_SAMPS_CP_L_ELSE_30_72MHZ 9
//#define LTE_PHY_N_SAMPS_PER_SLOT_30_72MHZ  15360
//#define LTE_PHY_N_SAMPS_PER_SUBFR_30_72MHZ (LTE_PHY_N_SAMPS_PER_SLOT_30_72MHZ * LTE_PHY_N_SLOTS_PER_SUBFR)
//#define LTE_PHY_N_SAMPS_PER_FRAME_30_72MHZ (LTE_PHY_N_SAMPS_PER_SUBFR_30_72MHZ * LTE_PHY_N_SUBFR_PER_FRAME)


typedef enum{
    LTE_PHY_FS_1_92MHZ = 0, // 1.25MHz bandwidth
    LTE_PHY_FS_3_84MHZ,     // 2.5MHz bandwidth
    LTE_PHY_FS_7_68MHZ,     // 5MHz bandwidth
    LTE_PHY_FS_15_36MHZ,    // 10MHz bandwidth
	LTE_PHY_FS_23_04MHZ,		// 15MHz bandwidth
    LTE_PHY_FS_30_72MHZ,    // 20MHz bandwidths
    LTE_PHY_FS_N_ITEMS,
}LTE_PHY_FS_ENUM;

enum modem_type_t {
	LTE_BPSK,
	LTE_QPSK,
	LTE_QAM16,
	LTE_QAM64
};

// Modulation type and number of bits mapping
#define BPSK_BITS_PER_SAMP 1
#define QPSK_BITS_PER_SAMP 2
#define QAM16_BITS_PER_SAMP 4
#define QAM64_BITS_PER_SAMP 6


typedef struct
{
	int fs; /* sampling rate */
	int N_samps_per_symb; /* FFT size */
	int N_dft_sz; /* DFT size for uplink SC-FDMA */
	int N_samps_cp_l_0; /* cyclic prefix length for the 1st OFDM symbol per slot */
 	int N_samps_cp_l_else; /* normal cyclic prefix length */
	int N_samps_per_slot;
	int N_samps_per_subfr;
	int N_samps_per_frame;
	int N_bits_per_samp;

	int N_symb_per_subfr;

	int N_tx_ant;
	int N_rx_ant;

}LTE_PHY_PARAMS;

// Function declarations
void lte_phy_init(LTE_PHY_PARAMS *lte_phy_params, int mod_type, int enum_fs, int n_tx_ant, int n_rx_ant);

#endif
