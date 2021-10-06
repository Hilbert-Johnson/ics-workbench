#include <stdint.h>

static inline uint64_t mul2(uint64_t a, uint64_t m){
  uint64_t tmp1=(m<<1);
  uint64_t tmp2=m;

  while(a>=m){
    while(a>=tmp1){
      if(tmp1<tmp2){
        break;
      }
      tmp2=tmp1;
      tmp1<<=1;
    }
    a-=tmp2;
  }
  return a;
}

static inline uint64_t add_mult(uint64_t a, uint64_t b, uint64_t m){
  if((a+b)<a){
    uint64_t tmp1=mul2(a+b+1,m);
    uint64_t tmp2=mul2(-1ULL,m);
    return add_mult(tmp1,tmp2,m);
  }else{
    return mul2(a+b,m);
  }
}
      
uint64_t multimod(uint64_t a, uint64_t b, uint64_t m) {
  a=mul2(a,m);
  b=mul2(b,m);
  uint64_t result=0;
  for(int i = 0; i < 64;i++){
    uint64_t tmp1 = a & 1;
    if(tmp1==1){
      result=add_mult(result,b,m);
    }
    b=add_mult(b,b,m);
    a >>= 1;  
  }
  return result;
}
