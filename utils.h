#ifndef __UTILS__
#define __UTILS__

#include "types.h"
#include "efi.h"

void hang();

void exit_boot_services();

size_t u16strlen(const uint16_t *str);

void *set_memory(void *pointer, int value, size_t size);

void log(Unicode* text);

void *copy_memory(void *destination, const void *source, size_t size);

void allocate_memory(uint64_t size, void** memory);

void open_protocol(Handle handle, GUID* guid, void** out);

#endif
