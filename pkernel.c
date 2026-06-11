#include "efi.h"
#include "pboot.h"
#include "graphics.h"
#include "utils.h"
#include "types.h"
#include "memory.h"

static uint64_t pkernel_physical_address = 0x4000000;

static uint64_t pkernel_file_size;

void get_memory_address_for_pkernel(){
 
  pkernel_file_size = get_file_size(get_kernel_file());

  // Calculate how many 4KB pages your kernel file needs 
  // (e.g., a 40KB file needs 10 pages)
  uint64_t pages_needed = (pkernel_file_size + 4095) / 4096;

  SystemTable* system_table = get_system_table();

  Status status = system_table->boot_table->allocate_pages(
    EFI_ALLOCATE_ADDRESS,
    EFI_LOADER_CODE,
    pages_needed,
    &pkernel_physical_address
  );

  if (status != EFI_SUCCESS) {
    // If UEFI returns an error, 
    // this memory address is occupied by firmware.
    // this is 64MB address
    log(u"ERROR: Could not allocate memory at 0x4000000");
    hang();
  }

}


void boot_pkernel() {

  get_graphics_output_protocol();
  
	load_kernel_file();

  get_memory_address_for_pkernel();

  void *kernel_in_efi_memory = read_file(get_kernel_file());


  copy_memory((void*)pkernel_physical_address,
      kernel_in_efi_memory,
      get_file_size(get_kernel_file()));

	
  void (*run_kernel)(void*,uint64_t);

	run_kernel = (void (*)(void*,uint64_t))pkernel_physical_address;
  
  void* framebuffer = get_framebuffer();

  log(u"launching pkernel..");

  log(u"Get memory");

  exit_boot_services();

  //execute
	(*run_kernel)(framebuffer,0xFFFFFFFF);

  log(u"executed");
  
  //we never got here
  hang();
}
