#include "graphics.h"
#include "efi.h"
#include "pboot.h"
#include "memory.h"

static EfiGraphicsOutputProtocol* gop;
static FrameBuffer frame_buffer;
static GraphicsOutputProtocol* graphics;

static void* framebuffer_in_memory;

void* get_framebuffer(){
	return &frame_buffer;
}

void plot_pixel(int x, int y, uint32_t pixel){
	FrameBuffer* frame = (FrameBuffer*)framebuffer_in_memory;
*((uint32_t*)(frame->frame_buffer
			+ 4 * frame->pixel_per_scan_line
			* y + 4 * x)) = pixel;
}

void get_graphics_output_protocol(){

  SystemTable* system_table = get_system_table();

	GUID guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	
	Status status;

	status = system_table->boot_table->locate_protocol(&guid,
			(void*)0, (void**)&gop);

	if(status != EFI_SUCCESS){
	  log(u"Can't get Graphics Output Protocol with locate_protocol");
	}

	graphics = gop;
	

	//get the current mode

	EfiGraphicsOutputModeInformation *info;
	uint64_t size_of_info, number_of_modes, native_mode;
	if(gop->mode == NULL){
		log(u"Graphics Output Protocol Mode is NULL");
		status = gop->query_mode(gop,
				0 , &size_of_info, &info);
	}else{
		status = gop->query_mode(gop,
				gop->mode->mode , &size_of_info, &info);
	}

	if(status != EFI_SUCCESS){
		log(u"Can't query mode");
	}
	
	native_mode = gop->mode->mode;
	number_of_modes = gop->mode->max_mode;

	//get best mode
	uint64_t best_horizontal = 0;
	uint64_t best_mode = 0;
	for(int i = 0; i<gop->mode->max_mode;i++){
		graphics->query_mode(graphics,i,&size_of_info,&info);
		if(info->horizontal_resolution > best_horizontal){
			best_mode = i;
			best_horizontal = info->horizontal_resolution;
			if(best_horizontal == 1920){
				break;//other resolution are too high
			}
		}
	}
	
	status = graphics->set_mode(graphics,best_mode)	;
	if(status != EFI_SUCCESS){
		log(u"Can't set best graphics mode");
	}

  // setup framebuffer
  frame_buffer.frame_buffer = gop->mode->frame_buffer_base_address;

  frame_buffer.pixel_per_scan_line =
      gop->mode->mode_info->pixel_per_scan_line;

  frame_buffer.horizontal_resolution =
      gop->mode->mode_info->horizontal_resolution;

  frame_buffer.vertical_resolution =
      gop->mode->mode_info->vertical_resolution;

}

