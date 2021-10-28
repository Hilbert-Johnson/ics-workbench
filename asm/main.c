#include "asm.h"
#include <assert.h>
#include <stdio.h>

asm_jmp_buf buf;

int main() {
  //printf("%ld\n",asm_add(1,1));
  //printf("%d\n",asm_popcnt(0xffffffffffffffff)); 
  int r = asm_setjmp(buf);
  printf("r:%d\n",r);
  if (r == 0) {
    assert(asm_add(1234, 5678) == 6912);
    assert(asm_popcnt(0x0123456789abcdefULL) == 32);
    // TODO: add more tests here.
    asm_longjmp(buf, 123);
    printf("do not reach here\n");
  } else {
    assert(r == 123);
    printf("PASSED.\n");
  }
}
