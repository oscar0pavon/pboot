#ifndef PKERNEL_H
#define PKERNEL_H

#include "graphics.h"
#include "memory.h"
#include <stdint.h>

typedef struct BootInfo{
  FrameBuffer frame_buffer;
  MemoryMapInfo memory_info;
  uint64_t xsdt_address;
}BootInfo;

BootInfo* get_boot_info();

void boot_pkernel();

#endif
