#ifndef __FILES_H__
#define __FILES_H__

#include "efi.h"

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
    { 0x0964e5b22, 0x6459, 0x11d2, \
      { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#define EFI_FILE_MODE_READ 0x0000000000000001
#define EFI_FILE_MODE_WRITE 0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

#define EFI_FILE_ARCHIVE 0x0000000000000020

static const uint64_t EFI_FILE_READ_ONLY = 0x1;

static const uint64_t MAX_BIT = 0x8000000000000000ULL;


typedef struct FileProtocol{
    uint64_t revision;
    Status (*open)(FileProtocol* self, FileProtocol** new_handle,
										uint16_t *file_name, uint64_t open_mode,
										uint64_t attributes);
    Status (*close)(FileProtocol*);
    void (*unused1)();
    Status (*read)(FileProtocol*, uint64_t*, void *);
    Status (*write)(FileProtocol*, uint64_t* buffer_size, void* buffer);
    Status (*get_position)(FileProtocol*, uint64_t*);
    Status (*set_position)(FileProtocol*, uint64_t);
    Status (*get_info)(FileProtocol*, GUID*, uint64_t*, void*);
    void (*unused6)();
    void (*unused7)();
    void (*unused8)();
    void (*unused9)();
    void (*unused10)();
    void (*unused11)();
}FileProtocol;


typedef struct FileSystemProtocol{
    uint64_t revision;
    Status (*open_volume)(FileSystemProtocol* self, FileProtocol** root);
}FileSystemProtocol;

void* read_file(FileProtocol* file);

uint64_t get_file_size(FileProtocol* file);

void open_file(FileProtocol** file, uint16_t* name);

void setup_file_system();

#endif
