#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;
uint64_t block_num = 0;
uint64_t associate = 0;

int tag_bit = 0;
int max_index_bit = 0;

struct line{
  bool valid,dirty;
  uint8_t tag;
  uint8_t memory[64];
};

struct line *cache=NULL;

void cycle_increase(int n) { cycle_cnt += n; }

#define BLOCK_MASK 0x3f

uint32_t cache_read(uintptr_t addr) {
  uintptr_t in_block_addr = addr & BLOCK_MASK;
  uintptr_t index_addr = (addr<<tag_bit)>>(tag_bit+6);
  uintptr_t tag_addr = addr>>(max_index_bit+6);

  for(int i = 0; i<associate; i++){
    if(cache[associate*index_addr + i].valid==true && cache[associate*index_addr + i].tag==tag_addr){
      return cache[associate*index_addr + i].memory[in_block_addr];
    }
  }

  uint8_t buf[64];
  mem_read(addr>>6, buf);

  for(int i = 0; i<associate; i++){
    if(cache[associate*index_addr + i].valid==false){
      cache[associate*index_addr + i].valid = true;
      cache[associate*index_addr + i].dirty = false;
      cache[associate*index_addr + i].tag = tag_addr;
      for(int j = 0; j<64; j++) cache[associate*index_addr + i].memory[j] = buf[j];
      return buf[in_block_addr];
    }
  }

  int replace = rand() % associate;
  if(cache[associate*index_addr + replace].dirty == true)
    mem_write((cache[associate*index_addr + replace].tag<<max_index_bit)|index_addr, cache[associate*index_addr + replace].memory);
  cache[associate*index_addr + replace].valid = true;
  cache[associate*index_addr + replace].dirty = false;
  cache[associate*index_addr + replace].tag = tag_addr;
  for(int i=0; i<64; i++) cache[associate*index_addr + replace].memory[i] = buf[i];
  return buf[in_block_addr];
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  uintptr_t in_block_addr = addr & BLOCK_MASK;
  uintptr_t index_addr = (addr<<tag_bit)>>(tag_bit+6);
  uintptr_t tag_addr = addr>>(max_index_bit+6);
}

void init_cache(int total_size_width, int associativity_width) {
  block_num = 1 << (total_size_width - 6);
  associate = 1 << associativity_width;
  max_index_bit = total_size_width - 6 - associativity_width;
  tag_bit = sizeof(uintptr_t)*8 - 6 - max_index_bit;
  cache = malloc(sizeof(struct line) * block_num);
  for(int i = 0; i < block_num;i++){
    cache[i].valid = false;
    cache[i].dirty = false;
  }
}

void display_statistic(void) {
}
