#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void *copy_memory(void *destination, const void *source, size_t size);

void allocate_memory(uint64_t size, void** memory);

void *set_memory(void *pointer, int value, size_t size);

#endif
