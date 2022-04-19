#include "bs.h"
#include "aes.h"
#include "../Util/common.h"
#include "../Util/share.h"

#define N 4

#define lenn 16 //number of bits of register in use

void MOVn(unsigned int dst[N],unsigned int src[N], byte n)
{
    byte i;

    for(i=0;i<n;i++)
	{
       dst[i] = src[i];
	}

}

void XOR1n(unsigned int c[N],unsigned int a[N],unsigned int b[N],byte n)
{
    int i;
    for(i=0;i<n;i++)
        c[i] = a[i] ^ b[i];

}

void fullRefresh(unsigned int c[N])
{
	int i,j;
	for(int i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			unsigned int temp = gen_rand32();
			c[i] = c[i] ^ temp;
			c[j] = c[j] ^ temp;
		
		}
	}

}

void ANDn(unsigned int c[N],unsigned int a[N],unsigned int b[N],byte n)
{

    unsigned int i,j;

    for(i=0;i<n;i++)
        c[i]=a[i] & b[i];

	  
    for(i=0;i<n;i++)
    {
        for(j=i+1;j<n;j++)
        {
						
            unsigned int tmp=gen_rand32()%(pow_cust(2,16)); // rand();
            unsigned int tmp2=(tmp ^ (a[i] & b[j]) ^ (a[j] & b[i]));
            c[i]^=tmp;
            c[j]^=tmp2;
        }
    }
	
	fullRefresh(c);
		
		
	}


void rotate_sharen(unsigned int dst[N],unsigned int src[N],unsigned int bits,byte n)	{

    int i;
    for(i=0;i<n;i++)
    {
        dst[i] = ((src[i] << bits) | (src[i] >> (lenn - bits)));
        dst[i]&= (0xFFFF);
    }

}

void NOTn(unsigned int a[N],byte n)	{
    a[0] = a[0] ^ (0xFFFF);
}

void swapBits_sharen(unsigned int n[N], int k1, int k2, byte nshr)
{
    int i=0;

    for(i=0;i<nshr;i++)
    {
        unsigned int b1 =  (n[i] >> k1) & 1;
        unsigned int b2 =  (n[i] >> k2) & 1;
        unsigned int x = (b1 ^ b2);
        x = (x << k1) | (x << k2);
        n[i] = (n[i] ^ x)&(0xFFFF);
    }
}

void left_swap_rotate_sharen(unsigned int state[8][N],byte n)
{
	unsigned int temp[N];
	int i;

	//temp=(unsigned int*) malloc(n*sizeof(unsigned int));
	for(i=0;i<8;i++)
	{
		MOVn(temp,state[i],n);
		swapBits_sharen(temp,3,15,n);
		swapBits_sharen(temp,3,11,n);
		swapBits_sharen(temp,3,7,n);
		rotate_sharen(state[i],temp,1,n);
	}
}

/**************************Encodnigs**************/


void encode_bskeyn(byte n) //encode_bskeyn(wshare,wshare_bs,n);
{
}



void encode_bsn(byte bit_array[16][N],unsigned int arr_b[8][N],byte n){
byte i,k,t,j=15,l=7,m;

    for(m=0;m<n;m++)
    {
        j=15,l=7;

        for(k=0;k<8;k++)
        {
            arr_b[k][m]=0;
            j=15;

            for(i=0;i<16;i++)
            {
                t=(1 & (bit_array[i][m] >> l));
                arr_b[k][m]=arr_b[k][m]+t*pow_cust(2,j);
                j--;
            }

            l--;
        }

    }

}

void decode_bsn(byte bit_array[16][N],unsigned int arr_b[8][N],byte n)
{
int i,k,t,j=0,m,pos=15,l=7;

    for(m=0;m<n;m++)
    {
        pos=15,l=7;

        for(i=0;i<16;i++)
        {
            bit_array[i][m]=0;
            l=7;
            for(k=0;k<8;k++)
            {
                t=(1&(arr_b[k][m]>>pos));
                bit_array[i][m]=bit_array[i][m]+t*pow_cust(2,l);
                l--;
            }
            pos--;
        }

    }
}


/****************end of encodings*********************/

void bs_addroundkeyn(unsigned int state[8][N],int round,byte n)
{
	int i;
	for(i=0; i<8; i++){}
		//XOR1n(state[i],state[i],wshare_bs[round*8+i],n);
}


void bs_shiftrowsn(unsigned int X[8][N],byte n)
{
	int i;
	for(i=0;i<8;i++)
	{//for each int in BS array
		swapBits_sharen(X[i],2,14,n);
		swapBits_sharen(X[i],6,10,n);
		swapBits_sharen(X[i],14,6,n);

		swapBits_sharen(X[i],8,12,n);
		swapBits_sharen(X[i],0,4,n);
		swapBits_sharen(X[i],12,4,n);

		swapBits_sharen(X[i],13,5,n);
		swapBits_sharen(X[i],9,1,n);
    }
}

void bs_mixColumnsn(unsigned int state[8][N],byte n)
{
    unsigned int temp[8][N],temp1[N];
	unsigned int of[N],i,t1;

	/*for(i=0;i<8;i++)
         temp[i]=(unsigned int*) malloc(n*sizeof(unsigned int));

    temp1=(unsigned int*) malloc(n*sizeof(unsigned int));
    of=(unsigned int*) malloc(n*sizeof(unsigned int));*/

	MOVn(temp1,state[0],n);
	swapBits_sharen(temp1,3,15,n);
	swapBits_sharen(temp1,3,11,n);
	swapBits_sharen(temp1,3,7,n);
    rotate_sharen(temp1,temp1,1,n);


	MOVn(of, state[0],n);
	XOR1n(of,of, temp1,n);

	for(i=0;i<7;i++)
		MOVn(temp[i],state[((i+1)%8)],n);
   	left_swap_rotate_sharen(state,n);//shift by 1


	for(i=0;i<8;i++)
	{
		XOR1n(temp[i],temp[i],state[((i+1)%8)],n);
	}

	XOR1n(temp[3],temp[3],of,n);
	XOR1n(temp[4],temp[4],of,n);
	XOR1n(temp[6],temp[6],of,n);
	MOVn(temp[7],of,n);

	for(i=0;i<8;i++)
		XOR1n(temp[i],temp[i],state[i],n);

	left_swap_rotate_sharen(state,n);//Shift by 2

	for(i=0;i<8;i++)
		XOR1n(temp[i],temp[i],state[i],n);

	left_swap_rotate_sharen(state,n);//Shift by 3

	for(i=0;i<8;i++)
		XOR1n(temp[i],temp[i],state[i],n);

	for(i=0;i<8;i++)
		MOVn(state[i],temp[i],n);
}

void bs_bitslicen(unsigned int X[8][N],byte n)
{
	int i;
	unsigned int Y[22][N],T[68][N],Z[18][N],S[8][N];

	/*for(i=0;i<22;i++)
	{
		Y[i]=(unsigned int*) malloc(n*sizeof(unsigned int));
	}

	for(i=0;i<68;i++)
	{
		T[i]=(unsigned int*) malloc(n*sizeof(unsigned int));
	}
	for(i=0;i<18;i++)
	{
		Z[i]=(unsigned int*) malloc(n*sizeof(unsigned int));
	}
	for(i=0;i<8;i++)
	{
		S[i]=(unsigned int*) malloc(n*sizeof(unsigned int));
	}*/

	XOR1n(Y[14] , X[3] , X[5], n);
	XOR1n(Y[13] , X[0] , X[6], n);
	XOR1n(Y[12] , Y[13] , Y[14], n);
	XOR1n(Y[9] , X[0] , X[3], n);
	XOR1n(Y[8] , X[0] , X[5], n);
	XOR1n(T[0] , X[1] , X[2], n);
	XOR1n(Y[1] , T[0] , X[7], n);
	XOR1n(Y[4] , Y[1] , X[3], n);
	XOR1n(Y[2] , Y[1] , X[0], n);
	XOR1n(Y[5] , Y[1] , X[6], n);
	XOR1n(T[1] , X[4] , Y[12], n);
	XOR1n(Y[3] , Y[5] , Y[8], n);
	XOR1n(Y[15] , T[1] , X[5], n);
	XOR1n(Y[20] , T[1] , X[1], n);
	XOR1n(Y[6] , Y[15] , X[7], n);
	XOR1n(Y[10] , Y[15] , T[0], n);
	XOR1n(Y[11] , Y[20] , Y[9], n);
	XOR1n(Y[7] , X[7] , Y[11], n);
	XOR1n(Y[17] , Y[10] , Y[11], n);
	XOR1n(Y[19] , Y[10] , Y[8], n);
	XOR1n(Y[16] , T[0] , Y[11], n);
	XOR1n(Y[21] , Y[13] , Y[16], n);
	XOR1n(Y[18] , X[0] , Y[16], n);


	//middle non-linear

	ANDn(T[2] , Y[12] , Y[15], n);
	ANDn(T[3] , Y[3] , Y[6], n);
	ANDn(T[5] , Y[4] , X[7] , n);
	ANDn(T[7] , Y[13] , Y[16], n);
	ANDn(T[8] , Y[5] , Y[1] , n);
	ANDn(T[10] , Y[2] , Y[7] , n);
	ANDn(T[12] , Y[9] , Y[11] , n);
	ANDn(T[13] , Y[14] , Y[17], n);
	XOR1n(T[4] , T[3] , T[2] , n);
	XOR1n(T[6] , T[5] , T[2], n);
	XOR1n(T[9] , T[8] , T[7], n);
	XOR1n(T[11] , T[10] , T[7], n);
	XOR1n(T[14] , T[13] , T[12], n);
	XOR1n(T[17] , T[4] , T[14], n);
	XOR1n(T[19] , T[9] , T[14] , n);
	XOR1n(T[21] , T[17] , Y[20], n);

	XOR1n(T[23] , T[19] , Y[21] , n);
	ANDn(T[15] , Y[8] , Y[10], n);
	ANDn(T[26] , T[21] , T[23], n);
	XOR1n(T[16] , T[15] , T[12], n);
	XOR1n(T[18] , T[6] , T[16], n);
	XOR1n(T[20] , T[11] , T[16], n);
	XOR1n(T[24] , T[20] , Y[18], n);
	XOR1n(T[30] , T[23] , T[24], n);
	XOR1n(T[22] , T[18] , Y[19], n);
	XOR1n(T[25] , T[21] , T[22], n);
	XOR1n(T[27] , T[24] , T[26], n);
	XOR1n(T[31] , T[22] , T[26], n);
	ANDn(T[28] , T[25] , T[27], n);
	ANDn(T[32] , T[31] , T[30], n);
	XOR1n(T[29] , T[28] , T[22], n);
	XOR1n(T[33] , T[32] , T[24], n);
	XOR1n(T[34] , T[23] , T[33], n);
	XOR1n(T[35] , T[27] , T[33] , n);
	XOR1n(T[42] , T[29] , T[33], n);
	ANDn(Z[14] , T[29] , Y[2], n);
	ANDn(T[36] , T[24] , T[35], n);
	XOR1n(T[37] , T[36] , T[34], n);
	XOR1n(T[38] , T[27] , T[36], n);
	ANDn(T[39] , T[29] , T[38], n);
	ANDn(Z[5] , T[29] , Y[7], n);


	XOR1n(T[44] , T[33] , T[37] , n);
	XOR1n(T[40] , T[25] , T[39], n);
	XOR1n(T[41] , T[40] , T[37], n);
	XOR1n(T[43] , T[29] , T[40], n);
	XOR1n(T[45] , T[42] , T[41], n);
	ANDn(Z[0] , T[44] , Y[15], n);
	ANDn(Z[1] , T[37] , Y[6], n);

	ANDn(Z[2] , T[33] , X[7], n);
	ANDn(Z[3] , T[43] , Y[16], n);
	ANDn(Z[4] , T[40] , Y[1], n);
	ANDn(Z[6] , T[42] , Y[11], n);
	ANDn(Z[7] , T[45] , Y[17], n);
	ANDn(Z[8] , T[41] , Y[10], n);
	ANDn(Z[9] , T[44] , Y[12], n);
	ANDn(Z[10] , T[37] , Y[3], n);
	ANDn(Z[11] , T[33] , Y[4], n);
	ANDn(Z[12] , T[43] , Y[13], n);
	ANDn(Z[13] , T[40] , Y[5], n);
	ANDn(Z[15] , T[42] , Y[9], n);
	ANDn(Z[16] , T[45] , Y[14], n);
	ANDn(Z[17] , T[41] , Y[8], n);

	//bottom linear
	XOR1n(T[46] , Z[15] , Z[16], n);
	XOR1n(T[55] , Z[16] , Z[17] , n);
	XOR1n(T[52] , Z[7] , Z[8], n);
	XOR1n(T[54] , Z[6] , Z[7], n);
	XOR1n(T[58] , Z[4] , T[46], n);
	XOR1n(T[59] , Z[3] , T[54] , n);
	XOR1n(T[64] , Z[4] , T[59], n);
	XOR1n(T[47] , Z[10] , Z[11] , n);

	XOR1n(T[49] , Z[9] , Z[10], n);
	XOR1n(T[63] , T[49] , T[58] , n);
	XOR1n(T[66] , Z[1], T[63], n);
	XOR1n(T[62] , T[52] , T[58], n);
	XOR1n(T[53] , Z[0] , Z[3], n);
	XOR1n(T[50] , Z[2] , Z[12] , n);
	XOR1n(T[57] , T[50] , T[53], n);
	XOR1n(T[60] , T[46] , T[57] , n);

	XOR1n(T[61] , Z[14] , T[57], n);
	XOR1n(T[65] , T[61] , T[62] , n);
	XOR1n(S[0] , T[59] , T[63], n);
	XOR1n(T[51] , Z[2] , Z[5] , n);
	XOR1n(S[4] , T[51] , T[66], n);
	XOR1n(S[5] , T[47] , T[65] , n);
	XOR1n(T[67] , T[64] , T[65], n);

	XOR1n(S[2] , T[55] , T[67], n);

	NOTn(S[2], n);

	XOR1n(T[48] , Z[5] , Z[13], n);
	XOR1n(T[56] , Z[12] , T[48], n);
	XOR1n(S[3] , T[53] , T[66], n);
	XOR1n(S[1] , T[64] , S[3], n);

	NOTn(S[1], n);
	XOR1n(S[6] , T[56], T[62], n);
	NOTn(S[6], n);
	XOR1n(S[7] , T[48], T[60], n);
	NOTn(S[7], n);


	MOVn(X[0], S[0], n);
	MOVn(X[1], S[1], n);
	MOVn(X[2], S[2], n);
	MOVn(X[3], S[3], n);
	MOVn(X[4], S[4], n);
	MOVn(X[5], S[5], n);
	MOVn(X[6], S[6], n);
	MOVn(X[7], S[7], n);

	/*for(i=0;i<22;i++)
	{
		free(Y[i]);
	}

	for(i=0;i<68;i++)
	{
		free(T[i]);
	}
	for(i=0;i<18;i++)
	{
		free(Z[i]);
	}
	for(i=0;i<8;i++)
	{
		free(S[i]);
	}*/


}

/********************Share 16-bit bitslice****************************/


void aes_share_subkeys_bitslice(byte in[16],byte out[16],byte n)
{
  int i,k,j;
  int round=0;

  byte stateshare[16][N];

  unsigned int state_bs[8][N];

  for(i=0;i<16;i++)
  {
    //stateshare[i]=(byte*) malloc(n*sizeof(byte));
    share_rnga(in[i],stateshare[i],n);
    //refresh(stateshare[i],n);
  }

  /*for(i=0;i<8;i++)
	  state_bs[i]=(unsigned int*) malloc(n*sizeof(unsigned int));*/

    encode_bsn(stateshare,state_bs,n);
    bs_addroundkeyn(state_bs, 0,n);

	// AES Round Transformations (x 9)
	//------------------------------------------------------
	for(i=1; i<10; i++)
	{
        bs_bitslicen(state_bs,n);//(state_bs,n);
        bs_shiftrowsn(state_bs,n);
        bs_mixColumnsn(state_bs,n);
        bs_addroundkeyn(state_bs,i,n);
	}

	// AES Last Round
	//------------------------------------------------------

	bs_bitslicen(state_bs,n);
	bs_shiftrowsn(state_bs,n);
	bs_addroundkeyn(state_bs,10,n);

	decode_bsn(stateshare,state_bs,n);

    for(i=0;i<16;i++)
    {
        out[i]=decode(stateshare[i],n);
        //free(stateshare[i]);
    }


}

int run_aes_share_bitslice(byte in[16],byte out[16],byte key[16],byte n,int nt,double time[2]){

  int i,k;
  byte w[176];
	unsigned int begin1=0,end1=0;	
  byte wshare4[176][N]; // for key bitslice representation should contain [3][176] input and [88][3] output
  unsigned int wshare_bs[88][N];
	
  keyexpansion(key,w);

  for(i=0;i<176;i++)
  {
    //wshare[i]=(byte *) malloc(n*sizeof(byte));
    share_rnga(w[i],wshare4[i],n);
    
  }

	/*for(i=0;i<88;i++)
		wshare_bs[i]=(unsigned int *) malloc(n*sizeof(unsigned int));*/

	 //encode_bskeyn(n);
	
	
	int row=0,t,m,l=0;
	byte j=15;

	for(m=0;m<n;m++)
	{
		for(row=0;row<11;row++)
		{
		l=7;
		for(k=0;k<8;k++)
		{
           wshare_bs[8*row+k][m]=0;
           j=15;

            for(i=0;i<16;i++)
			{
                t=(1 & (wshare4[16*row+i][m] >> l));//(1 && (bit_array[i] >> (k+1 )));
                wshare_bs[8*row+k][m]=wshare_bs[8*row+k][m]+t*pow_cust(2,j);
                j--;
            }

            l--;
		}

		}
	}
	
	
	  #if TRNG==1
            reset_systick();
            begin1 = SysTick->VAL; // Obtains the start time
            #endif // TRNG


  for(int ij=0;ij<nt;ij++)
  {
    //aes_share_subkeys_bitslice(in,out,n);
		
		byte stateshare[16][N];

   unsigned int state_bs[8][N];

  for(i=0;i<16;i++)
  {
    //stateshare[i]=(byte*) malloc(n*sizeof(byte));
    share_rnga(in[i],stateshare[i],n);
    //refresh(stateshare[i],n);
  }

  /*for(i=0;i<8;i++)
	  state_bs[i]=(unsigned int*) malloc(n*sizeof(unsigned int));*/

    encode_bsn(stateshare,state_bs,n);
    //bs_addroundkeyn(state_bs, 0,n);
	   for(int ik=0; ik<8; ik++)
		XOR1n(state_bs[ik],state_bs[ik],wshare_bs[ik],n);
	

	// AES Round Transformations (x 9)
	//------------------------------------------------------
	for(i=1; i<10; i++)
	{
        bs_bitslicen(state_bs,n);//(state_bs,n);
        bs_shiftrowsn(state_bs,n);
        bs_mixColumnsn(state_bs,n);
        //bs_addroundkeyn(state_bs,i,n);
		for(int ik=0; ik<8; ik++)
		XOR1n(state_bs[ik],state_bs[ik],wshare_bs[i*8+ik],n);
	
	}

	// AES Last Round
	//------------------------------------------------------

	bs_bitslicen(state_bs,n);
	bs_shiftrowsn(state_bs,n);
	bs_addroundkeyn(state_bs,10,n);

	decode_bsn(stateshare,state_bs,n);

    for(i=0;i<16;i++)
    {
        out[i]=decode(stateshare[i],n);
        //free(stateshare[i]);
    }

		
		
		
  }

	  #if TRNG==1
            end1 = SysTick->VAL; // Obtains the stop time
            time[1] = ((double) (begin1-end1))/nt; // Calculates the time taken
            #endif // TRNG

	
  /*for(i=0;i<176;i++)
    free(wshare[i]);*/

  return (double) (0.00) ;

}

