#ifndef __bs_h__
#define __bs_h__
#include "../Util/common.h"
int run_aes_share_bitslice(byte in[16],byte out[16],byte key[16],byte n,int nt,double time[2]);
#endif