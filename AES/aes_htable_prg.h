#include "../Util/common.h"
#include "../Util/share.h"



void subbytestate_share_prg(byte stateshare[16][shares_N],int n,void (*subbyte_share_call)(byte *,int,int),int round);
/***********specific to third order**************/
void init_subset_sum();
void subbyte_htable_third(byte y[shares_N], int n, int ind, int choice);
void gen_t_forall_third(int n, int choice);
void subbytestate_share_prg_third(byte stateshare[16][shares_N], int n, void(*subbyte_share_call)(byte *, int, int, int), int round, int choice);

/***********specific to LRV third order**************/
void init_table_sbox();
void gen_t_forall_third_LRV(int n, int choice);
void subbyte_htable_third_LRV(byte y[shares_N], int n, int ind, int choice);
void htable_third_LRV(int n, int count, int choice);





