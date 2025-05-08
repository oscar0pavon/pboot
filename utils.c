#include "utils.h"
#include "efi.h"
#include "pboot.h"
#include "types.h"

void allocate_memory(uint64_t size, void** memory){

	SystemTable* system_table = get_system_table();
	system_table->boot_table->allocate_pool(
			EFI_LOADER_DATA, size, memory);
}

void log(Unicode* text){
	SystemTable* system_table = get_system_table();
	
	system_table->out->print(system_table->out,text);
	system_table->out->print(system_table->out,u"\n\r");

}

void open_protocol(Handle handle, GUID* guid, void** out){

	LoadedImageProtocol* bootloader = get_bootloader_image();
	SystemTable* system_table = get_system_table();
	Handle bootloader_handle = get_bootloader_handle();

	Status status = system_table->boot_table->open_protocol(handle,
			guid, out, bootloader_handle, 0,
			EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL)	;

	if(status != EFI_SUCCESS){
		log(u"Can't open protocol");
	}
}


size_t u16strlen(const uint16_t *str)
{
	const uint16_t *pos = str;

	while (*pos++)
		;
	return pos - str - 1;
}

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


void hang(){
	while (1) {};
}



void exit_boot_services(){

	struct MemoryDescriptor *mmap;
	u64 mmap_size = 4096;
	u64 mmap_key;
	u64 desc_size;
	uint32_t desc_version;


	SystemTable* system_table = get_system_table();
	
	Status status;

	while (1) {

		allocate_memory(mmap_size,(void**)&mmap);

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
	
	Handle handle = get_bootloader_handle();

	status = system_table->boot_table->exit_boot_services(handle, 
			mmap_key);

	if(status != EFI_SUCCESS){
		log(u"ERROR boot service not closed");
		return;
	}

}

