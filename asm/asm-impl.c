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
        "jg end1 \n"
        "loop1: \n\t"
        "movq %%rbx , %%rdx \n\t"
        "shrq %%cl , %%rdx \n\t"
        "andq $1 , %%rdx \n\t"
        "testq %%rdx , %%rdx \n\t"
        "je update \n\t"
        "addl $1 , %%eax \n"
        "update: \n\t"
        "addl $1 , %%ecx \n\t"
        "cmpl $63 , %%ecx \n\t"
        "jle loop1 \n"
        "end1: \n\t"
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
        "jbe end2\n"
        "loop2 :\n\t"
        "movb (%%rbx,%%rdi,1), %%dl \n\t"
        "movb %%dl , (%%rax,%%rdi,1) \n\t"
        "addq $1 , %%rdi\n\t"
        "cmpq %%rdi, %%rcx \n\t"
        "ja loop2\n"
        "end2 :\n\t" 
        : "=a" (dest)
        : "a"(dest),"b"(src),"c"(n),"D"(i)
        : "memory"
    );
	return dest;
}

int asm_setjmp(asm_jmp_buf env) {
  int ret = 0;
    asm volatile(
      "movq  %%rsp , (%%rdi) \n\t"
      "movq  %%rbx , 8(%%rdi) \n\t"
      "movq  %%rbp , 16(%%rdi) \n\t"
      "movq  %%r12 , 24(%%rdi) \n\t"
      "movq  %%r13 , 32(%%rdi) \n\t"
      "movq  %%r14 , 40(%%rdi) \n\t"
      "movq  %%r15 , 48(%%rdi) \n\t"
      "xorq  %%rax , %%rax \n\t"
      "leaq 4(%%rip), %%rcx \n\t"
      "movq %%rcx , 56(%%rdi) \n\t"
      : "=D" (env) , "=a"(ret)
      : "D" (env)
    );
  return ret;
}

void asm_longjmp(asm_jmp_buf env, int val) {
    asm(
      "testq    %%rax,%%rax" // val == 0?
      "jnz tmp"
      "addq $1,%%rax"      //  eax++
      "tmp:"
      "movq  8(%%edx),%%rbx" 
      "movq 24(%%edx),%%r12" 
      "movq 32(%%edx),%%r13" 
      "movq 40(%%edx),%%r14"
      "movq 48(%%edx),%%r15"
      "movq  16(%%edx),%%rbp"
      "movq (%%rdx),%%rcx" 
      "movq %%rcx,%%rsp"
      "movq 56(%%edx), %%rcx" 
      "jmp  *%%rcx " 
      : "=d"(env) , "=a"(val)
      : "d"(env) , "a"(val)     
    );
}

//int main(){}