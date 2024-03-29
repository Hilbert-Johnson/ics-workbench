#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;
uint64_t block_num = 0;
uint64_t associate = 0;

int tag_bit = 0;
int max_index_bit = 0;

struct line {
  uint32_t memory[16]; 
  bool valid,dirty;  
  uint32_t tag;
};

struct line *cache=NULL;

void cycle_increase(int n) { cycle_cnt += n; }

#define BLOCK_MASK 0x3f

void m2c(uintptr_t addr, int line_index){
  cache[line_index].valid = true;
  cache[line_index].tag = addr>>(BLOCK_WIDTH+max_index_bit);
  mem_read(addr >> BLOCK_WIDTH,(uint8_t*)cache[line_index].memory);
  cache[line_index].dirty = false;
}

void c2m(int group_num, int line_index){
  if(cache[line_index].dirty)
    mem_write((cache[line_index].tag << max_index_bit)|group_num, (uint8_t*)cache[line_index].memory);
  cache[line_index].valid = false;
}

int give_index(uintptr_t addr) {
    uint32_t tag = addr>>(max_index_bit+BLOCK_WIDTH);
    uint32_t group_num = (addr<<tag_bit)>>(tag_bit+BLOCK_WIDTH);
    for (int i = group_num*associate; i<group_num*associate+associate; i++){
        if (cache[i].valid && cache[i].tag == tag)
          return i;
    }
    for(int i = group_num*associate;i< group_num*associate+associate;i++){
      if(!cache[i].valid){
        m2c(addr,i);
        return i;
      }
    }
    int line_index = group_num*associate + rand()%associate;
    c2m(group_num,line_index);
    m2c(addr,line_index);
    return line_index;
}

uint32_t cache_read(uintptr_t addr) {
  int line_index = give_index(addr);
  int data_index = (addr & BLOCK_MASK)/sizeof(uint32_t);
  return cache[line_index].memory[data_index];
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  int line_index = give_index(addr);
  int data_index = (addr & BLOCK_MASK)/sizeof(uint32_t);
  cache[line_index].memory[data_index] -= cache[line_index].memory[data_index]&wmask;
  cache[line_index].memory[data_index] += data&wmask;
  cache[line_index].dirty = true;
}

void init_cache(int total_size_width, int associativity_width) {
  block_num = exp2(total_size_width - BLOCK_WIDTH);
  associate = exp2(associativity_width);
  max_index_bit = total_size_width - BLOCK_WIDTH - associativity_width;
  tag_bit = sizeof(uintptr_t)*8 - BLOCK_WIDTH - max_index_bit;
  cache = malloc(sizeof(struct line) * block_num);
  //printf("block_num:%lu associate:%lu index_bit:%d tag_bit:%d",block_num,associate,max_index_bit,tag_bit);
  for(int i = 0; i < block_num;i++){
    cache[i].valid = false;
    cache[i].dirty = false;
  }
}

void display_statistic(void) {
}
