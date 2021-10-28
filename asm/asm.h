#include <stdint.h>
#include <stddef.h>

#define asm_jmp_buf jmp_buf
typedef struct _jmp_buf {
    int64_t callee_saved[8];
} jmp_buf[1];

int64_t asm_add(int64_t a, int64_t b);
int     asm_popcnt(uint64_t x);
void   *asm_memcpy(void *dest, const void *src, size_t n);
int     asm_setjmp(asm_jmp_buf env);
void    asm_longjmp(asm_jmp_buf env, int val);
