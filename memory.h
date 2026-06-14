#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

struct MemoryDescriptor {
  uint32_t type;
  uint32_t padding;
  uint64_t physical_start;
  uint64_t virtual_start;
  uint64_t pages;
  uint64_t attributes;
} __attribute__((packed));

enum AllocateType {
  EFI_ALLOCATE_ANY_PAGES,
  EFI_ALLOCATE_MAX_ADDRESS,
  EFI_ALLOCATE_ADDRESS,
};

enum MemoryType {
  EFI_LOADER_CODE,
  EFI_LOADER_DATA,
};

typedef struct MemoryMapInfo{
  uint64_t buffer_address; // Absolute physical memory address of the mmap data (0x5000000)
  uint64_t total_size;     // Total bytes populated in the buffer
  uint64_t descriptor_size;// Unique hardware stepping size (usually 40 or 48 bytes)
}MemoryMapInfo;

void *copy_memory(void *destination, const void *source, size_t size);

void allocate_memory(uint64_t size, void **memory);

void *set_memory(void *pointer, int value, size_t size);

u64 get_memory_map_key();

#endif
