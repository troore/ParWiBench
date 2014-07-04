
#ifndef __LTEUPLINK_H_
#define __LTEUPLINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <complex>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>

#include "gauss.h"
#include "FIFO.h"
#include "BSPara.h"
#include "UserPara.h"
#include "matrix.h"
#include "GeneralFunc.h"

#include "Turbo.h"
#include "RateMatcher.h"
#include "Scrambler.h"
#include "Modulation.h"
#include "TransformPrecoder.h"
#include "ResMapper.h"
#include "Equalizer.h"
#include "OFDM.h"
#include "Sync.h"


//#define NSTD 1
#define MAX_SFRAMES 1
#define MAX_BLOCK_SIZE 6144

#endif
