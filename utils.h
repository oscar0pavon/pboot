#ifndef __UTILS__
#define __UTILS__

#include "types.h"
#include "efi.h"

Handle get_bootloader_handle();

SystemTable* get_system_table();

LoadedImageProtocol* get_bootloader_image();

void hang();

size_t u16strlen(const uint16_t *str);

void log(Unicode* text);

void open_protocol(Handle handle, GUID* guid, void** out);

void halt();

#endif
