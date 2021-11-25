#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define N 10000000

static int is_prime[N];
static int  primes[N];

int *sieve(int n) {
  

  for (int i = 3; i*i<= n; i+=2) {
    for (int j = i*i; j <= n; j += (i<<1)) {
      is_prime[j] = 1;
    }
  }

  int *p = primes;
  *p++ = 2;
  for (int i = 3; i <= n; i+=2)
    if (!is_prime[i]) {
      *p++ = i;
    }
  *p = 0;
  return primes;
}
