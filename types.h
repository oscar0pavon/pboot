
#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint16_t Unicode;

typedef struct {
	Unicode entry_name[20];
	Unicode kernel_name[20];
	Unicode kernel_parameters[100];
}BootLoaderEntry;

typedef struct TextOutputProtocol			TextOutputProtocol;
typedef struct InputProtocol					InputProtocol;
typedef struct BootTable							BootTable;
typedef struct SystemTable						SystemTable;
typedef struct FileProtocol						FileProtocol;
typedef struct FileSystemProtocol			FileSystemProtocol;
typedef struct GUID										GUID;
typedef struct GUID										EFI_GUID;

typedef void * Handle;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t	u8;

typedef uint64_t Status;

#endif
