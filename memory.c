#include "memory.h"
#include "types.h"
#include "utils.h"

void *set_memory(void *pointer, int value, size_t size)
{
	char *to = pointer;

	for (size_t i = 0; i < size; ++i)
		*to++ = value;
	return pointer;
}

void *copy_memory(void *destination, const void *source, size_t size)
{
	const char *from = source;
	char *to = destination;

	for (size_t i = 0; i < size; ++i)
		*to++ = *from++;
	return destination;
}

void allocate_memory(uint64_t size, void** memory){

	SystemTable* system_table = get_system_table();
	system_table->boot_table->allocate_pool(
			EFI_LOADER_DATA, size, memory);
}

u64 get_memory_map_key(){

	struct MemoryDescriptor *mmap;
	u64 mmap_size = 4096;
	u64 mmap_key;
	u64 desc_size;
	uint32_t desc_version;


	SystemTable* system_table = get_system_table();
	
	Status status;

	while (1) {
    uint64_t safe_mmap_address = 0x5000000; 
    uint64_t pages_needed = (mmap_size + 4095) / 4096;

    status = system_table->boot_table->allocate_pages(
          EFI_ALLOCATE_ADDRESS,
          EFI_LOADER_DATA,
          pages_needed,
          &safe_mmap_address
      );

    mmap = (struct MemoryDescriptor*)safe_mmap_address;

		status = system_table->boot_table->get_memory_map(
			&mmap_size,
			mmap,
			&mmap_key,
			&desc_size,
			&desc_version);
		if (status == EFI_SUCCESS){
			break;
		}
	}
	
  return mmap_key;
}

