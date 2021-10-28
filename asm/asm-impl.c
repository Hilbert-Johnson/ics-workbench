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
    asm (
      
      "movq %%rbx,(%%rdi) \n\t"       
	    "movq %%rbp,8(%%rdi) \n\t"
	    "movq %%r12,16(%%rdi) \n\t"
	    "movq %%r13,24(%%rdi) \n\t"
	    "movq %%r14,32(%%rdi) \n\t"
	    "movq %%r15,40(%%rdi) \n\t"
	    "leaq 8(%%rsp),%%rdx \n\t"       
	    "movq %%rdx,48(%%rdi) \n\t"
	    "movq (%%rsp),%%rdx \n\t"        
	    "movq %%rdx,56(%%rdi) \n\t"
	    "xorl %%eax,%%eax \n\t"        
      : "=a"(ret), "=D"(env)
      : "a"(ret), "D"(env)   
    ); 
  return ret;
}

void asm_longjmp(asm_jmp_buf env, int val) {
    asm(
    "xor %eax,%eax \n\t"
	  "cmp $1,%esi \n\t"            
	  "adc %esi,%eax \n\t"          
	  "mov (%rdi),%rbx \n\t"          
	  "mov 8(%rdi),%rbp \n\t"
	  "mov 16(%rdi),%r12 \n\t"
	  "mov 24(%rdi),%r13 \n\t"
	  "mov 32(%rdi),%r14 \n\t"
	  "mov 40(%rdi),%r15 \n\t"
	  "mov 48(%rdi),%rsp \n\t"
	  "jmp *56(%rdi) \n\t"    
    );
}