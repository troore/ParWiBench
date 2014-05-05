

//Variable: datarate choose from them.
#define  BPSK_1_2   1
#define  BPSK_3_4   2
#define  QPSK_1_2   3
#define  QPSK_3_4   4
#define  QAM16_1_2  5
#define  QAM16_3_4  6
#define  QAM64_2_3  7
#define  QAM64_3_4  8

#define  BPSK   1
#define  QPSK   2
#define  QAM16  3
#define  QAM64  4

#define  R_1_2  1
#define  R_2_3  2
#define  R_3_4  3


//Include files.
#include <math.h>
#include "complex.h"
#include "gettime.h"

#include "inout_tools.h"
#include "PHY_RX_init_para.cc"
#include "PHY_RX_time_synchro.cc"
#include "PHY_RX_freq_synchro.cc"
#include "PHY_RX_remove_gi.cc"
#include "PHY_RX_fft.cc"
#include "PHY_RX_channel_estimation.cc"
#include "PHY_RX_constellation_demap.cc"
#include "PHY_RX_de_interleaver.cc"
#include "PHY_RX_viterbi.cc"
#include "PHY_RX_scrambler.cc"





