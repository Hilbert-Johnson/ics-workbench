#include <stdint.h>

static inline uint64_t multimod2(uint64_t b, uint64_t m){
  while(b>=m){
    b-=m;
  }
  return b;
}

static inline uint64_t add_mult(uint64_t a, uint64_t b, uint64_t m){
  //a=multimod2(a,m);
  //b=multimod2(b,m);
  if((a+b)<a){
    uint64_t tmp1=multimod2(a+b+1,m);
    uint64_t tmp2=multimod2(-1ULL,m);
    return add_mult(tmp1,tmp2,m);
  }else{
    return multimod2(a+b,m);
  }
}
      
uint64_t multimod(uint64_t a, uint64_t b, uint64_t m) {
  a=multimod2(a,m);
  b=multimod2(b,m);
  uint64_t result=0;
  for(int i = 0; i < 64;i++){
    uint64_t tmp1 = a & 1;
    b=add_mult(b,b,m);
    if(tmp1==1){
      result=add_mult(result,b,m);
    }
    a >>= 1;  
  }
  return result;
}
