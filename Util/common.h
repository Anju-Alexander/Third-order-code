typedef unsigned char byte;

/*********This file has global constants and common functions********/

#include <time.h>

#define BITSLICE 3
#define ORIGINAL 5 //These values are specific to our customized third order scheme
#define PRG 6
#define LRV 7
#define LRV_PRG 8
#define PRESENT_THIRD 9
#define PRESENT_THIRD_PRG 10

#define TRNG -1 // Set value to 1 to use device-specific TRNG. Set to 0 to use AES-based PRG.

#define NPRG 1 // Normal variant
#define IPRG 0 // Increasing shares variant

#define RPRG 2 // Robust PRG
#define MPRG 3 // Multiple PRG

#define subset_k 16 // 160*256 approx 8=16 bits for subset sum generator
#define subset_k_p 13 
/***********Input parameters to define********/

#define shares_N 4 // #shares.
#define VARIANT IPRG //// Either Normal (NPRG)/ Increasing (IPRG) variants

/***********end of input parameters**********/

#define UNIT 1000000 //Time unit (Milli seconds)

extern int cipher; // Cipher can be either AES/ PRESENT/ BITSLICE
extern int type_PRG; // Either Robust/ multiple PRG

void rand_in(void);
void rand_dein(void);
void gen_rand(byte *a,int n);
unsigned int gen_rand32(void);

void init_randcount(void);
unsigned int get_randcount(void);
void set_randcount(unsigned int randc);

double cal_time(clock_t stop, clock_t start);
void reset_systick(void);

int compare_output(byte *out1,byte *out2,byte size);
