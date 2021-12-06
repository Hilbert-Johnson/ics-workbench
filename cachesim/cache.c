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
  uintptr_t tag;
  uint8_t memory[BLOCK_SIZE];
};

struct line *cache=NULL;

void cycle_increase(int n) { cycle_cnt += n; }

#define BLOCK_MASK 0x3f

uint32_t cache_read(uintptr_t addr) {
  uintptr_t in_block_addr = addr & BLOCK_MASK;
  uintptr_t index_addr = (addr<<tag_bit)>>(tag_bit+BLOCK_WIDTH);
  uintptr_t tag_addr = addr>>(max_index_bit+BLOCK_WIDTH);

  for(int i = 0; i<associate; i++){
    if(cache[associate*index_addr + i].valid==true && cache[associate*index_addr + i].tag==tag_addr){
      return *(uint32_t *)(cache[associate*index_addr + i].memory+in_block_addr);
    }
  }

  uint8_t buf[BLOCK_SIZE];
  mem_read(addr>>BLOCK_WIDTH, buf);

  for(int i = 0; i<associate; i++){
    if(cache[associate*index_addr + i].valid==false){
      cache[associate*index_addr + i].valid = true;
      cache[associate*index_addr + i].dirty = false;
      cache[associate*index_addr + i].tag = tag_addr;
      for(int j = 0; j<BLOCK_SIZE; j++) cache[associate*index_addr + i].memory[j] = buf[j];
      return *(uint32_t *)(buf+in_block_addr);
    }
  }

  int replace = rand() % associate;
  if(cache[associate*index_addr + replace].dirty == true)
    mem_write((cache[associate*index_addr + replace].tag<<max_index_bit)|index_addr, cache[associate*index_addr + replace].memory);
  
  cache[associate*index_addr + replace].valid = true;
  cache[associate*index_addr + replace].dirty = false;
  cache[associate*index_addr + replace].tag = tag_addr;
  for(int i=0; i<BLOCK_SIZE; i++) cache[associate*index_addr + replace].memory[i] = buf[i];

  return *(uint32_t *)(buf+in_block_addr);
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  uintptr_t in_block_addr = addr & BLOCK_MASK;
  uintptr_t index_addr = (addr<<tag_bit)>>(tag_bit+BLOCK_WIDTH);
  uintptr_t tag_addr = addr>>(max_index_bit+BLOCK_WIDTH);
  int i;
  for(i = 0; i<associate; i++){
    if(cache[associate*index_addr + i].valid==true && cache[associate*index_addr + i].tag==tag_addr){
      uint32_t *tmp = (uint32_t *)(cache[associate*index_addr + i].memory+in_block_addr);
      *tmp=data&wmask;
      cache[associate*index_addr + i].dirty = true;
      break;
    }
  }
  if(i == associate){
    uint8_t buf[BLOCK_SIZE];
    mem_read(addr>>BLOCK_WIDTH, buf);
    int j;
    for(j = 0; i<associate; j++){
      if(cache[associate*index_addr + j].valid == false){
        cache[associate*index_addr + j].valid = true;
        cache[associate*index_addr + j].dirty = true;
        cache[associate*index_addr + j].tag = tag_addr;
        for(int k = 0; k<BLOCK_SIZE; k++) cache[associate*index_addr + j].memory[k] = buf[k];
        uint32_t *tmp = (uint32_t *)(cache[associate*index_addr + j].memory+in_block_addr);
        *tmp=data&wmask;
        break;
      }
    }
    if(j == associate){
      int replace = rand() % associate;
      if(cache[associate*index_addr + replace].dirty == true)
        mem_write((cache[associate*index_addr + replace].tag<<max_index_bit)|index_addr, cache[associate*index_addr + replace].memory);
  
      cache[associate*index_addr + replace].valid = true;
      cache[associate*index_addr + replace].dirty = true;
      cache[associate*index_addr + replace].tag = tag_addr;
      for(int k=0; k<BLOCK_SIZE; k++) cache[associate*index_addr + replace].memory[k] = buf[k];
      uint32_t *tmp = (uint32_t *)(cache[associate*index_addr + replace].memory+in_block_addr);
      *tmp=data&wmask;
    }
  }
}

void init_cache(int total_size_width, int associativity_width) {
  block_num = exp2(total_size_width - BLOCK_WIDTH);
  associate = exp2(associativity_width);
  max_index_bit = total_size_width - BLOCK_WIDTH - associativity_width;
  tag_bit = sizeof(uintptr_t)*8 - BLOCK_WIDTH - max_index_bit;
  cache = malloc(sizeof(struct line) * block_num);
  printf("block_num:%lu associate:%lu index_bit:%d tag_bit:%d",block_num,associate,max_index_bit,tag_bit);
  for(int i = 0; i < block_num;i++){
    cache[i].valid = false;
    cache[i].dirty = false;
  }
}

void display_statistic(void) {
}
