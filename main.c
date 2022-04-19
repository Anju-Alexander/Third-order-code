#include <stdio.h>

#include "AES/aes.h"
#include "AES/aes_htable_PRG.h"
#include "Util/driver_functions.h"
#include "Util/prg3.h"
#include "PRESENT/present.h"
#include "Util/prg3.h"

#define AES 1
#define PRESENT 2
#define CRV_present 4




//*******************main*****************************************

int main()
{
    /**********Input parameters for Higher-order LUT-based block cipher implementation********/

    int nt = 10; //Number of times to repeat experiments
    int shares = shares_N; // #Input shares. Set the parameter in common.h.
    int cipher = BITSLICE; //Cipher can be AES or PRESENT or BITSLICE or CRV_present
    int scheme = VARIANT; //Set the parameter in common.h file. Type of LUT construction. normal--> NPRG  Increasing shares--> IPRG
    int type_PRG = MPRG; //Type of PRG to generate randoms, either robust-->RPRG or multiple-->MPRG
   
    double time[11]={0,0,0,0,0,0,0,0,0,0,0};// To hold offline and online execution clock cycle count

    int i,k;
/*
    printf("**********************************************\n");
    printf("Input choices\n");
    printf("Cipher: %d (1:AES 2:PRESENT 3:Bitslice 4:PRESENT_CRV)\n",cipher);
    printf("#shares: %d, Variant:%d  (1:Normal 0:Increasing shares) and PRG type: %d (2:robust 3:multiple PRG)\n",shares,scheme,type_PRG);
    printf("**********************************************\n");*/
  
    if(cipher==ORIGINAL||cipher==PRG||cipher==LRV||cipher==LRV_PRG||cipher==BITSLICE)
    {
        byte keyex[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
		byte inex[16] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
		//Expected result:{0x39,0x25,0x84,0x1d,0x02,0xdc,0x09,0xfb,0xdc,0x11,0x85,0x97,0x19,0x6a,0x0b,0x32};

		byte in1[16], in2[16], out1[16], out2[16];
		// In1 and In2 (out and key also) to represent the input to unshared and shared AES block cipher.
		byte key1[16], key2[16];

		for (i = 0; i < 16; i++)
		{
			key1[i] = keyex[i];
			key2[i] = keyex[i];
		}
		
		for (i = 0; i < 16; i++)
		{
			in1[i] = inex[i];
			in2[i] = inex[i];

		}


		for (k = 0; k < 16; k++)
		{
			out1[k] = 0x0;
		}
		
		run_aes(in1, out1, key1, nt);
		//printf("Pre-computation of 160 tables for AES-128\n");
		//gen_t_forall_third(shares, third_order_scheme);
		
		run_aes_shares_third(in2,out2,key2,shares,cipher,nt,time);
		
		if (compare_output(out1, out2, 16))
		{
			printf("Successful execution of LUT-based AES\n");
				
		}
		else
		{
			printf("Unsuccessful execution :(, pls check...");
		}

    }   
	if(cipher==PRESENT||cipher==PRESENT_THIRD||cipher==PRESENT_THIRD_PRG)
    {
            /*********Test Vectors*********/
        byte keyex[] ={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
        byte inex[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};//{0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d};


        byte in1[8],in2[8],out1[8],out2[8];
		byte key1[10],key2[10];

		for(i=0;i<10;i++)
        {
            key1[i]=keyex[i];
            key2[i]=keyex[i];
        }


		for(i=0;i<8;i++)
        {
            in1[i]=inex[i];//rand()%256;
            in2[i]=inex[i];

        }


		for(k=0;k<8;k++)
        {
            out1[k]=0x0;
            out2[k]=0x0;
        }

        present(in1,out1,key1);
        //run_present_shares_prg(in2,out2,key2,shares,time,nt);

        //run_present_shares_prg(in2,out2,key2,shares,time,nt);
        run_present_shares_third(in2,out2,key2,shares,time,nt,cipher);
        if(compare_output(out1,out2,8))
        {
            printf("Successful execution of LUT-based PRESENT\n");
/*
            #if TRNG==0
            printf("#Milli seconds: Off-line: %f and Online: %f\n ",time[0],time[1]);
            #else
            printf("#Clock_cycles: Off-line: %f and Online: %f\n ",time[0],time[1]);
            #endif
*/
        }

        else
            printf("Unsuccessful execution :(, pls check...");



    }

   
  
  
	return 0;
}

