#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;/*
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
      return cache[associate*index_addr + i].memory[in_block_addr];
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
      return buf[in_block_addr];
    }
  }

  int replace = rand() % associate;
  if(cache[associate*index_addr + replace].dirty == true)
    mem_write((cache[associate*index_addr + replace].tag<<max_index_bit)|index_addr, cache[associate*index_addr + replace].memory);
  
  cache[associate*index_addr + replace].valid = true;
  cache[associate*index_addr + replace].dirty = false;
  cache[associate*index_addr + replace].tag = tag_addr;
  for(int i=0; i<BLOCK_SIZE; i++) cache[associate*index_addr + replace].memory[i] = buf[i];

  return buf[in_block_addr];
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  uintptr_t in_block_addr = addr & BLOCK_MASK;
  uintptr_t index_addr = (addr<<tag_bit)>>(tag_bit+BLOCK_WIDTH);
  uintptr_t tag_addr = addr>>(max_index_bit+BLOCK_WIDTH);
  int i;
  for(i = 0; i<associate; i++){
    if(cache[associate*index_addr + i].valid==true && cache[associate*index_addr + i].tag==tag_addr){
      *(uint32_t *)(cache[associate*index_addr + i].memory+in_block_addr)=data&wmask;
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
        *(uint32_t *)(cache[associate*index_addr + j].memory+in_block_addr)=data&wmask;
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
      *(uint32_t *)(cache[associate*index_addr + replace].memory+in_block_addr)=data&wmask; 
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
*/

#define OFFSET 0x3f //　块内偏移量,0-63,即最最低6位
#define max_cache_line 2048   // 11+6 可以满足　２^17Byte的数据要求

//　cache结构
struct Cache {
  uint32_t data[16]; //  序号代表块内地址
  bool valid,dirty;  //　有效位，脏位
  uint32_t tag;      //　标记，代表取模后的内存地址高位
}line[max_cache_line];// 序号代表行号

//　三个全局宽度
static int group_lines; //  组内行数
static int total_width; //　数据总宽度
static int group_width; //　组号宽度
// 统计量
static uint64_t hit,miss;
uint64_t real_time;

// 获取高位地址
uint32_t get_tag(uintptr_t addr){
  int mod_width = group_width+BLOCK_WIDTH;
  // 通过右移实现取模
  addr >>= mod_width;
  return addr;
}
// 获取组号
uint32_t get_group_index(uintptr_t addr){
  int mod_width = BLOCK_WIDTH;
  //　右移取模
  addr >>= mod_width;
  //　遮盖获取组号
  addr &= mask_with_len(group_width);
  return addr;
}
// 获取块内地址偏移量
uint32_t get_data_index(uintptr_t addr) {
  return (addr & OFFSET)/sizeof(uint32_t);
}
// 主存块调入cache
void m2c(uintptr_t addr, int line_index){
  // 有效位设置
  line[line_index].valid = true;
  //　由地址获取tag
  line[line_index].tag = get_tag(addr);
  // 获得地址对应的内存快的起始地址
  uintptr_t start = addr >> BLOCK_WIDTH;
  mem_read(start,(uint8_t*)(&line[line_index].data[0]));
  // 脏位设置
  line[line_index].dirty = false;
  
}
// cache放入主存
void c2m(int group_num, int line_index){
  // 若修改过则采用回写法，将新内容写如主存
  if(line[line_index].dirty){
    // 主存地址某一行起始等于，tag与组号的拼接
    uintptr_t start = (line[line_index].tag << group_width)+group_num;
    mem_write(start, (uint8_t*)&line[line_index].data[0]);
  }
  // 有效位置0
  line[line_index].valid = false;
}
//检查地址对应主存是否在它对应的组当中，若在返回行号，缺失返回负数标记
int give_me_an_index(uintptr_t addr) {
    // 获取地址tag
    uint32_t tag = get_tag(addr);
    uint32_t group_num = get_group_index(addr);
    // 本组起始结束行号
    int group_start = group_num * group_lines;
    int group_end = group_start + group_lines;
    // 查找
    for (int i = group_start; i<group_end;i++){
        //　如果地址tag与行tag相同，说明命中
        if (line[i].valid && line[i].tag == tag){
          hit ++;
          return i;
        }
    }

    //进行到这儿，发现没有命中，进行内存与cache替换
    //如果有空行，将所需内存块直接放入空行中
    for(int i = group_start;i< group_end;i++){
      if(!line[i].valid){
        m2c(addr,i);
        return i;
      }
    }
    //如果没有空行，则随机替换，rand用于生成0-0x7fff随机数，进行随机替换
    int line_index = group_start + rand()%group_lines;
    c2m(group_num,line_index);
    m2c(addr,line_index);
    miss ++;

    return line_index;
}



uint32_t cache_read(uintptr_t addr) {
  int line_index = give_me_an_index(addr);
  int data_index = get_data_index(addr);

  return line[line_index].data[data_index];
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  int line_index = give_me_an_index(addr);
  int data_index = (addr & OFFSET)/sizeof(uint32_t);

  line[line_index].data[data_index] -= line[line_index].data[data_index]&wmask;
  line[line_index].data[data_index] += data&wmask;
  line[line_index].dirty = true;
}

void init_cache(int total_size_width, int associativity_width) {
  //　初始化统计量
  hit=miss=cycle_cnt=real_time=0;
  // 初始化三个宽度
  total_width = total_size_width;
  group_lines = exp2(associativity_width);
  group_width = total_width - associativity_width - BLOCK_WIDTH;
  // 初始化有效位脏位为0
  for (int i=0;i<exp2(total_width-BLOCK_WIDTH);i++) 
    line[i].valid = line[i].dirty = false;
}

void display_statistic(void) {
}
