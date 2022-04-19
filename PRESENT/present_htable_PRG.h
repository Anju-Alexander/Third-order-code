#include "../Util/share.h"
#include "../Util/common.h"

void gen_t_forall_present(int n);

void locality_refresh_m_present(byte *a,int n);

void subbyte_htable_inc_mprg_present(byte *a,int n,int count);

void subbytestate_share_prg_present(byte stateshare[8][shares_N],int n,void (*subbyte_share_call)(byte *,int,int,int),int round, int type);

byte get_mprg_lr_present(int index,int n, unsigned int val);


//*************************third order*********************
void gen_t_forall_present_third(int n, int type);
void subbyte_htable_present_third(byte *a,int n,int count,int type);
void htable_m_present_third_prg(int n,int count);
byte subset_sum_prg(byte seed[subset_k_p+1],unsigned int b);

