

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
#include "PHY_TX_init_para.cc"
#include "PHY_TX_scrambler.cc"
#include "PHY_TX_bcc.cc"
#include "PHY_TX_interleaver.cc"
#include "PHY_TX_constellation_map.cc"
#include "PHY_TX_insert_pilot.cc"
#include "PHY_TX_ifft.cc"
#include "PHY_TX_insert_gi.cc" 
#include "PHY_TX_add_preamble.cc"




