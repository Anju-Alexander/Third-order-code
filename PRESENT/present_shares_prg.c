#include <stdio.h>

#if TRNG == 1
#include "MK64F12.h"
#else
#include <time.h>
#endif

#include "../Util/share.h"
#include "../Util/prg3.h"
#include "../Util/driver_functions.h"

#include "present.h"
#include "present_htable_PRG.h"

void addroundkey_present_share(byte state[8][shares_N],byte key[10][shares_N],int n)
{
    int i,j;
   for(i=0;i<n;i++)
    for(j=0;j<8;j++)
        state[j][i] = state[j][i] ^ key[j+2][i];
}


void keyschedule_present_share(byte *key,int round)
{
    byte save1,save2;
    int i;

   		save1  = key[0];
		save2  = key[1];

		for(i=0;i<8;i++)
            key[i] = key[i+2];

		key[8] = save1;
		key[9] = save2;
		save1 = key[0] & 7;								//61-bit left shift

        for(i=0;i<9;i++)
			key[i] = key[i] >> 3 | key[i+1] << 5;

		key[9] = key[9] >> 3 | save1 << 5;
		key[9] = sbox_p[key[9]>>4]<<4 | (key[9] & 0xF);	//S-Box application

		if((round+1) % 2 == 1)							//round counter addition
			key[1] ^= 128;
		key[2] = ((((round+1)>>1) ^ (key[2] & 15)) | (key[2] & 240));

}

void permute_present_share(byte state[8][shares_N],int n)
{
    int i,j;
    byte temp[8];

    for(j=0;j<n;j++)
    {

    for(i=0;i<8;i++)
    {
			temp[i] = 0;
	}

	for(i=0;i<64;i++)
	{
		byte position = (16*i) % 63;						//Artithmetic calculation of the pLayer
		if(i == 63)									//exception for bit 63
			position = 63;
		byte element_source		= i / 8;
		byte bit_source 			= i % 8;
		byte element_destination	= position / 8;
		byte bit_destination 	= position % 8;
		temp[element_destination] |= ((state[element_source][j]>>bit_source) & 0x1) << bit_destination;
    }

	for(i=0;i<=7;i++)
			state[i][j] = temp[i];

    }
}



/**********************third order**********/

void run_present_shares_third(byte *in,byte*out,byte *key,int n,double *time,int nt, int type)
{

    int ni=n*(n-1)/2;
    byte stateshare[8][shares_N],keyshare[10][shares_N],state[8];
    byte i=0, key_in[10];
    int round;
    unsigned int begin1, end1;
    long sec,nsec;
    double temp=0.0;
   /******************copying the key***************/
   for(i=0;i<10;i++)
   key_in[i]=key[i];

    //printf("initialisation of multiple PRGs\n");
    //init_mprg2(n,ni);
   for(i=0;i<8;i++)
    state[i]=in[i];

    //printf("pre-processing of tables\n");
    printf("Pre-computation of 160 tables for AES-128...\n");

    

    gen_t_forall_present_third(n,type); //Pre-processing table T1 for all rounds
 

    printf("Done with pre-processing of tables\n");   

    printf("Online phase\n");
      
      for (int j=0;j<nt;j++)
      {
        for(i=0;i<10;i++)
        key[i]=key_in[i];
        
        for(i=0;i<8;i++)
       {
        //stateshare[i]=(byte*) malloc(n*sizeof(byte));
        share_rnga(state[i],stateshare[i],n);
        
        }
     for(i=0;i<10;i++)
        {
        //keyshare[i]=(byte*) malloc(n*sizeof(byte));
        share_rnga(key[i],keyshare[i],n);
        
        }
      for(round=0;round<31;round++)
       {
        addroundkey_present_share(stateshare,keyshare,n);

        subbytestate_share_prg_present(stateshare,n,subbyte_htable_present_third,round,type);
        permute_present_share(stateshare,n);

        keyschedule_present(key,round);

        for(i=0;i<10;i++)
           {
            share_rnga(key[i],keyshare[i],n); //Key share

           }

        }
        addroundkey_present_share(stateshare,keyshare,n);


    for(i=0;i<8;i++)
        out[i]=decode(stateshare[i],n);

      }


}
