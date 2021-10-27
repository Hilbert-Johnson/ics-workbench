#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
  asm(  "addq	%%rbx, %%rax ;"
        :  "=a"(a)
        :  "a"(a),"b"(b)
  );
  return a;
}

int asm_popcnt(uint64_t x) {
  int s = 0;
  int i = 0;
  asm(
        "cmpl $63 , %%ecx \n\t"
        "jg end \n"
        "loop: \n\t"
        "movq %%rbx , %%rdx \n\t"
        "shrq %%cl , %%rdx \n\t"
        "andq $1 , %%rdx \n\t"
        "testq %%rdx , %%rdx \n\t"
        "je update \n\t"
        "addl $1 , %%eax \n"
        "update: \n\t"
        "addl $1 , %%ecx \n\t"
        "cmpl $63 , %%ecx \n\t"
        "jle loop \n"
        "end: \n\t"
        : "=a" (s)
        : "a"(s),"b"(x),"c"(i)
        : "rdx"
  );
  return s;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
  size_t i = 0;
    asm(
        "cmpq %%rdi , %%rcx \n\t"
        "jbe end\n"
        "loop :\n\t"
        "movb (%%rbx,%%rdi,1), %%dl \n\t"
        "movb %%dl , (%%rax,%%rdi,1) \n\t"
        "addq $1 , %%rdi\n\t"
        "cmpq %%rdi, %%rcx \n\t"
        "ja loop\n"
        "end :\n\t" 
        : "=a" (dest)
        : "a"(dest),"b"(src),"c"(n),"D"(i)
        : "memory"
    );
	return dest;
}

int asm_setjmp(asm_jmp_buf env) {
  return setjmp(env);
}

void asm_longjmp(asm_jmp_buf env, int val) {
  longjmp(env, val);
}
