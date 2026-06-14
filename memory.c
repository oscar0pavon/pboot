#include "memory.h"
#include "pkernel.h"
#include "types.h"
#include "utils.h"
#include <stdint.h>

#define EFI_BUFFER_TOO_SMALL 0x8000000000000005ULL

static uint64_t safe_mmap_address = 0x5000000;


void *set_memory(void *pointer, int value, size_t size) {
  char *to = pointer;

  for (size_t i = 0; i < size; ++i)
    *to++ = value;
  return pointer;
}

void *copy_memory(void *destination, const void *source, size_t size) {
  const char *from = source;
  char *to = destination;

  for (size_t i = 0; i < size; ++i)
    *to++ = *from++;
  return destination;
}

void allocate_memory(uint64_t size, void **memory) {

  SystemTable *system_table = get_system_table();
  system_table->boot_table->allocate_pool(EFI_LOADER_DATA, size, memory);
}

u64 get_memory_map_key() {

  struct MemoryDescriptor *mmap;
  u64 mmap_size = 4096;
  u64 mmap_key = 0;
  u64 desc_size = 0;
  uint32_t desc_version = 0;
  Status status;

  SystemTable *system_table = get_system_table();

  // Query UEFI once first to get the true baseline size tracking metrics
  system_table->boot_table->get_memory_map(&mmap_size, NULL, &mmap_key,
                                           &desc_size, &desc_version);

  // Add extra padding bytes (4 descriptors worth) because allocating
  // pages forces the UEFI map itself to grow larger right after the
  // query!
  mmap_size += (desc_size * 4);

  while (1) {
    uint64_t pages_needed = (mmap_size + 4095) / 4096;

    status = system_table->boot_table->allocate_pages(
        EFI_ALLOCATE_ADDRESS, EFI_LOADER_DATA, pages_needed,
        &safe_mmap_address);

    mmap = (struct MemoryDescriptor *)safe_mmap_address;

    status = system_table->boot_table->get_memory_map(
        &mmap_size, mmap, &mmap_key, &desc_size, &desc_version);

    if (status == EFI_SUCCESS) {
      //For pkernel parameters
      BootInfo* boot_info = get_boot_info();
      boot_info->memory_info.buffer_address = safe_mmap_address;
      boot_info->memory_info.total_size = mmap_size;
      boot_info->memory_info.descriptor_size = desc_size;

      return mmap_key;
      break;
    }

    // If the buffer was too small, free this failed page
    // block before allocating a larger one on the next loop iteration pass!
    system_table->boot_table->free_pages(safe_mmap_address, pages_needed);

    if (status == EFI_BUFFER_TOO_SMALL) {
      // Add extra space to the updated size parameter returned by UEFI
      mmap_size += (desc_size * 4);
      continue;
    }

    // Unrecoverable fallback loop freeze
    log(u"Error in map memory");

    halt();

  }

  return mmap_key;
}
