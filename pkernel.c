#include "pboot.h"
#include "graphics.h"

void boot_pkernel() {

  get_graphics_output_protocol();
  
	load_kernel_file();

  void *kernel_in_memory = read_file(get_kernel_file());

	void (*kernel)(void*,uint64_t);

	kernel = (void (*)(void*,uint64_t))kernel_in_memory;
  
  void* framebuffer = get_framebuffer();

  log(u"launching pkernel..");

  log(u"Get memory");

  exit_boot_services();

  //execute
	(*kernel)(framebuffer,0xFFFFFFFF);

  log(u"executed");
  
  //we never got here
  hang();
}
