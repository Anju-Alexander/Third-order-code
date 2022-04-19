#include <stdio.h>
#include<stdint.h>

#include "aes.h"
#include "aes_htable_prg.h"
//#include "../Util/prg3.h"
#include "../Util/share.h"

#define TSIZE 256 //16 for PRESENT
typedef uint32_t word;
int w=4;
word sbox_Word[64]; //TSIZE/w=4


byte x_shares[160*(shares_N-1)]; //Shares of x used as part of pre-processing.  496 *(N2-1) fpr PRESENT
byte T[160*TSIZE]; //Size of pre-computed tables for 10*16=160 S-box calls for AES-128, 496*TSIZE for PRESENT.

byte gamma1[subset_k + 1];
byte gamma2[subset_k + 1];
//byte x_shares[160*(shares_N - 1)]; //Shares of x used as part of pre-processing.  496 *(N2-1) fpr PRESENT
byte y_shares[160*(shares_N - 2)]; //shares of y y_1 and y_2
byte y_prg[160]; //y2 and y3 are generated using prg
byte Y3[40960]; // Y3 array, with 256*160=40960 size
//byte T1[TSIZE];
byte T2[160*TSIZE];

byte R1[1];

word y_shares_word[160]; //y1 and y2 packed into one cell so instead of 320, just 160
word x_shares_word[160]; //x1,x2,x3 packed into one cell
word Y3_word[10240]; //256*160/4 =10240
word T2_word[160*64];
byte V_rand[160];

//************Functions for Normal variant using robust PRG *******************


//*************off-line functions******


void shift_tab(byte a,byte *Tp,int count)//Shift the entries of lookup table (LUT) by x_i
{
	unsigned int j,temp,temp1;

	for(j=0;j<TSIZE;j++)
	{
		temp=count*TSIZE;
		temp1=j^a;
    Tp[j]=T[temp+temp1];
	}
}

//************state shares*************


void subbytestate_share_prg(byte stateshare[16][shares_N],int n,void (*subbyte_share_call)(byte *,int,int),int round)
{
  unsigned int t,ind;

  for(int i=0;i<16;i++)
  {
	ind=16*round+i;
	t=ind*(shares_N-1);
    byte temp=0;

    for(int j=0;j<n-1;j++)
       temp=temp ^ stateshare[i][j] ^ x_shares[t+j];

    stateshare[i][n-1]=stateshare[i][n-1] ^ temp;
    subbyte_share_call(stateshare[i],n,ind);

  }

}
/*********specific to third order*************/
void init_subset_sum()
{
	//byte temp[subset_k+1];
	gen_rand(gamma1, subset_k+1);
	gen_rand(gamma2,subset_k+1);
	//for(int j=0;j<subset_k+1;j++)
	//{  gamma1[j]=temp[j];
	  
	//}

	
}

byte subset_sum(byte seed[subset_k+1],unsigned int b)
{
	byte t1 = 0, t2, temp; //k= log_2(160*256)=log_2(2^8 * 2^8) approx = 16 
	t2 = b; 
	temp = seed[0]; //y0
	for (int j = 1; j < subset_k + 1; j++)
	{
		t1 = b % 2;
		t2 = t2 / 2;
		if (t1)
		{
			temp = temp ^ seed[j];
			
		}
	}
	return temp;
	

}
/*
void shift_tab_third(byte a, byte *Tp)//shift_tab(x_shares[count][i],Tp,count);
{
	unsigned int j, temp1;
	for (j = 0; j < TSIZE; j++)
	{
		
		temp1 = j ^ a;
		Tp[j] = T1[temp1]; //T1[temp+temp1];
	}
}*/

void htable_third_prg(int n, int count, int choice)
{
	unsigned int j, i, t, t1, temp, temp1, tmp1, k = 0;
	byte Tp[TSIZE], v[1], d, b;
	temp = count*TSIZE;
	t = count*(shares_N - 1);
	t1 = count*(shares_N - 3);
	temp1 = x_shares[t];
	for (j = 0; j < TSIZE; j++)
	{
		//T1[temp+j]=sbox[j] ^ y_shares[t1] ^ y_shares[t1+1]; //y1 ^ y2
		Tp[j] = sbox[j ^ temp1] ^ y_prg[t1] ;
	}
	//shift_tab_third(temp1, Tp); //Tp is T(Sbox) shifted by first input share
	gen_rand(v, 1);
    
	d = (x_shares[t + 1] ^ v[0]) ^ x_shares[t + 2];
	for (j = 0; j < TSIZE; j++)
	{
		b = j ^ d;
		
		//T2[temp + b] = Tp[v[0] ^ j] ^ subset_sum(gamma1, temp + b) ^ y_shares[t1 + 1]; //subset_sum
		T2[temp + b] =( Tp[v[0] ^ j]  ^ subset_sum(gamma2, temp + b)) ^ subset_sum(gamma1, temp + b);
	}
    
    
}


void htable_third(int n, int count, int choice)
{
	unsigned int j, i, t, t1, temp, temp1, k = 0;
	byte Tp[TSIZE], v[1], d, b;
    
	temp = count*TSIZE;
    
	t = count*(shares_N - 1);
	t1 = count*(shares_N - 2);
	temp1 = x_shares[t];
	for (j = 0; j < TSIZE; j++)
	{
		//T1[temp+j]=sbox[j] ^ y_shares[t1] ^ y_shares[t1+1]; //y1 ^ y2
		//T1[j] = sbox[j] ^ y_shares[t1] ^ y_shares[t1 + 1];
		Tp[j] = sbox[j ^ temp1] ^ y_shares[t1]; //S[x+a]+y1
	}
    
    
	//shift_tab_third(temp1, Tp); //Tp is T1 shifted by first input share
    
	
	gen_rand(v, 1);
    
	d = (x_shares[t + 1] ^ v[0]) ^ x_shares[t + 2];
    
	for (j = 0; j < TSIZE; j++)
	{
		b = j ^ d;
		T2[temp + b] = (Tp[v[0] ^ j] ^ Y3[temp + b]) ^ y_shares[t1 + 1];   //T1[v+a]+y3[b]+y2
	}
    
}

void gen_t_forall_third(int n, int choice)
{
	unsigned int i, j, temp1, temp2, temp3;
	byte a[shares_N - 1], c[TSIZE], common;
	gen_rand(R1, 1);
	for (i = 0; i < 160; i++)
	{
		gen_rand(a, n - 1);
		temp1 = i*(shares_N - 1);
        for (j = 0; j < (n - 1); j++)
		{
			x_shares[temp1 + j] = a[j]; 
			//x_shares[temp1 + j] = 0; 
            
		}

		if (choice == ORIGINAL)
		{
			byte b[shares_N - 2];
			gen_rand(b, n - 2);
		    temp2 = i*(shares_N - 2);
		    for (j = 0; j < (n - 2); j++)
			{
			y_shares[temp2 + j] = b[j]; 
            
		    }
			gen_rand(c, TSIZE);
			temp3 = i*TSIZE;
			for (j = 0; j < TSIZE; j++)
			{
				Y3[temp3 + j] = c[j];
			}
			htable_third(n, i, choice);



		}
	
		else if (choice == PRG)
		{
			byte b[shares_N-3];
			gen_rand(b, n - 3);
		    temp2 = i*(shares_N - 3);
		    for (j = 0; j < (n - 3); j++)
			{
			y_prg[temp2 + j] = b[j]; 
            
		    }
            
			//htable(n,i,choice);
			htable_third_prg(n, i, choice);
		}
	
        
	}

}


void subbyte_htable_third(byte y[shares_N], int n, int ind, int choice)
{
	
	byte x4;
	
	unsigned int t2 = ind*TSIZE;
	x4 = y[n - 1];
    
	y[0] = T2[t2 + x4]; //y4
	if(choice!=PRG)
	{
		unsigned int t1 = ind*(shares_N - 2);
		for (int i = 1; i < shares_N - 1; i++) //n=4, thus y0 and y1
		{
		y[i] = y_shares[t1 + i - 1]; //y1 y2
        }
	    y[n - 1] = Y3[t2 + x4];
	}
	if (choice == PRG)
	{	//y[n - 1] = get_robprg2(n, t2 + x4);
	    unsigned int t1 = ind*(shares_N - 3);
		y[1]=y_prg[t1];
        y[n-1]=subset_sum(gamma1,t2+x4); //y3
		y[2]=subset_sum(gamma2,t2+x4);
	}
		
	//y[2]=0;
    
    
}


void subbytestate_share_prg_third(byte stateshare[16][shares_N], int n, void(*subbyte_share_call)(byte *, int, int, int), int round, int choice)
{
	unsigned int i, j;
	unsigned int t, ind;
  
	for (i = 0; i < 16; i++)
	{
		ind = 16*round + i;
		t = ind*(shares_N - 1);
		byte temp = 0;
		for (j = 0; j < n - 1; j++)
			temp = temp ^ stateshare[i][j] ^ x_shares[t + j];
    
	
		stateshare[i][n - 1] = stateshare[i][n - 1] ^ temp;
	
		subbyte_share_call(stateshare[i], n, ind, choice); 
	
	}

}

/********************code specific to LRV*******************/
void init_table_sbox()
{
  for(int k=0;k<TSIZE/w; k++)
  {
    word r=0;
    for(int j=w-1;j>=0;j--)
    {
      r=r << 8;
      r^=sbox[k*w+j];
    }
    sbox_Word[k]=r;
    
  }
}

void subbyte_htable_third_LRV(byte y[shares_N], int n, int ind, int choice)
{
	int size=TSIZE/w;
	unsigned int i;
	unsigned int t=ind *size;
	unsigned int t1=ind*(shares_N-1);
	unsigned int t2=ind*(shares_N-2);
	byte T1_small[4], T2_small[4], y3_small[4], y3_small1[4];

	byte x1=x_shares[t1];
	byte x1_m=x1>>2;
	byte x1_l=x1&3;

	byte x2=x_shares[t1+1];
	byte x2_m=x2>>2;
	byte x2_l=x2&3;

	byte x3=x_shares[t1+2];
	byte x3_m=x3>>2;
	byte x3_l=x3&3;

	byte x4= y[n-1];
	byte x4_m=x4>>2;
	byte x4_l=x4&3;
    
	byte x=x1^x2^x3^x4;
    
    for(i=0;i<4;i++)
	{
	  T2_small[i ]=(T2_word[t+ (x4_m)]>>(i*8))&0xff;
	  y3_small[i ]=(Y3_word[t+ (x4_m)]>>(i*8))&0xff;
	}

	
	
	for(i=0;i<shares_N-2;i++)
	  y[i]=y_shares[t2+i];
  
	for(i=0;i<4;i++)
	{
		T1_small[i] = T2_small[i ^ x1_l];
		y3_small1[i] =y3_small[i ^ x1_l];
	}
    byte v_l, d_l, b2=0;
    v_l=V_rand[ind]&3;

	d_l=(x2_l ^ v_l) ^x3_l;

	for(i=0;i<4;i++)
	{
		b2=d_l^i;
		T2_small[b2] = T1_small[v_l ^ i];
		y3_small[b2] = y3_small1[v_l ^ i];
	}

	
    
    
	y[2]=y3_small[x4_l];   //y3
    //y[2]=0;
	//y[2]=((Y3_word[t + x4_m])>>(x4_l*8))&(0xff);
	y[n-1]=T2_small[x4_l]; //y4
   
	
    
	

}


void htable_third_LRV(int n, int count, int choice)
{
unsigned int t=count*(TSIZE/w);
unsigned int t1=count*(shares_N-1), i;
unsigned int t2=count*(shares_N-2);
word y1,y2;
int size=TSIZE/w;
y1=0^(y_shares[t2]<<24)^(y_shares[t2]<<16)^(y_shares[t2]<<8)^y_shares[t2];
y2=0^(y_shares[t2+1]<<24)^(y_shares[t2+1]<<16)^(y_shares[t2+1]<<8)^y_shares[t2+1];

byte x1=x_shares[t1], x2=x_shares[t1+1], x3=x_shares[t1+2];
byte x1_m=x1>>2, x2_m=x2>>2, x3_m=x3>>2;

word T1_small[64];
   for (i=0;i<64;i++)
   {
      T1_small[i]=sbox_Word[i ^ x1_m] ^ y1;
	  
   }

//genereating Y3_word out of Y3

int temp=count*TSIZE;

   byte v[1], v_m, d_m, b1=0, d, b;
   
   gen_rand(v,1);
   v_m=v[0]>>2;
   V_rand[count]=v[0];
   d_m= (x2_m ^ v_m) ^ x3_m;
   for(i=0;i<64;i++)
   {
	   b1=d_m ^ i;
	   T2_word[t+b1]=(T1_small[i ^ v_m] ^ Y3_word[t+b1]) ^y2;
   }
   
   
	
}



void gen_t_forall_third_LRV(int n, int choice)
{
	
	
	unsigned int i, j, temp1, temp2, temp3;
	byte a[shares_N - 1], b[shares_N - 2], common,c[TSIZE];
	int size=TSIZE/w;
	gen_rand(R1, 1);
	for (i = 0; i < 160; i++)
	{  
		gen_rand(a, n - 1);
		temp1 = i*(shares_N - 1);
        for (j = 0; j < (n - 1); j++)
		{
			x_shares[temp1 + j] = a[j]; 
			//x_shares[temp1 + j] = 0; 
			
			
		}
		
		gen_rand(b, n - 2);
		temp2 = i*(shares_N - 2);
		for (j = 0; j < (n - 2); j++)
		{
			
			y_shares[temp2 + j] = b[j]; 
		
			
            
		}
		
		if (choice == LRV)
		{
			
			temp3 = i*size;
			//gen_rand(c, TSIZE);
			for (j = 0; j < size; j++)
			{
				//Y3[temp3 + j] = c[j];
				Y3_word[temp3+j]= gen_rand32();
				//Y3_word[temp3+j]=0;
				
			}
		
			
			htable_third_LRV(n, i, choice);
			

		}
	
		else if (choice == PRG)
		{
            
			//htable(n,i,choice);
			htable_third_prg(n, i, choice);
		}
	
      
	}


}