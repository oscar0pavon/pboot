#include "utils.h"
#include "efi.h"
#include "pboot.h"
#include "types.h"
#include "memory.h"


void halt(){

  while (1) {
    __asm__("hlt");
  }
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

void hang(){
	while (1) {};
}


