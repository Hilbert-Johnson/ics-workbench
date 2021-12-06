#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;
uint64_t block_num = 0;
uint64_t associate = 0;

int tag = 0;
int cache_index = 0;

struct line{
  bool valid,dirty;
  uint8_t tag;
  uint8_t memory[64];
};

struct line *cache=NULL;

void cycle_increase(int n) { cycle_cnt += n; }

// TODO: implement the following functions

uint32_t cache_read(uintptr_t addr) {
  return 0;
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
}

void init_cache(int total_size_width, int associativity_width) {
  block_num = 1 << (total_size_width - 6);
  associate = 1 << associativity_width;
  cache_index = total_size_width - 6 - associativity_width;
  tag = sizeof(uintptr_t)*8 - 6 - cache_index;
  cache = malloc(sizeof(struct line) * block_num);
  for(int i = 0; i < block_num;i++){
    cache[i].valid = false;
    cache[i].dirty = false;
  }
}

void display_statistic(void) {
}
