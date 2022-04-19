#include "../AES/aes.h"

void run_aes_shares_prg(byte *in,byte *out,byte *key,int n,int choice,int type,double *time,int nt);

double run_aes_share_bitslice8(byte in[16],byte out[16],byte key[16],byte n,int nt);


/************specific to third order************ */
 void run_aes_shares_third(byte *in, byte *out, byte *key, int n, int type, int nt, double time[11]);

 /****************present third order********************/
 void run_present_shares_third(byte *in,byte*out,byte *key,int n,double *time,int nt, int type);
