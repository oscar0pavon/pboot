#include "pkernel.h"
#include "efi.h"
#include "files.h"
#include "pboot.h"
#include "graphics.h"
#include "utils.h"
#include "types.h"
#include "memory.h"

static uint64_t pkernel_physical_address = 0x4000000;

static uint64_t pkernel_file_size;

static BootInfo* boot_info = NULL;

BootInfo* get_boot_info(){
  return boot_info;
}

void set_boot_info(BootInfo* in){
  boot_info = in; 
}

struct ACPISystemDescriptorTableHeader{
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OPEMRevision;
	uint32_t creator_id;
	uint32_t creator_revision;
}__attribute__ ((packed));



struct XSDP_t {
	char signature[8];
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	uint64_t XSDT_address;//XSDT(eXtended System Description Table)
	uint8_t extended_checksum;
	uint8_t reserved[3];
}__attribute__ ((packed));


struct XSDP_t* acpi_table = NULL;
struct XSDT_t* XSDT;

void exit_boot_services(){

  u64 mmap_key =  get_memory_map_key();

	SystemTable* system_table = get_system_table();
	
	Status status;

	Handle handle = get_bootloader_handle();

	status = system_table->boot_table->exit_boot_services(handle, 
			mmap_key);

	if(status != EFI_SUCCESS){
		log(u"ERROR boot service not closed");
    halt();
		return;
	}

}

bool compare_efi_guid(EFI_GUID* guid1, EFI_GUID* guid2){

	if(guid1->data1 != guid2->data1){
		return false;
	}
	if(guid1->data2 != guid2->data2){
		return false;
	}
	if(guid1->data3 != guid2->data3){
		return false;
	}

	for(int i = 0; i<8;i++){
		if(guid1->data4[i] != guid2->data4[i]){
			return false;
		}
	}

	return true;
}

void get_acpi_table(){

	//get ACPI 2.0 table
  
  SystemTable* system_table = get_system_table();

	EFI_GUID acpi_guid = EFI_ACPI_20_TABLE_GUID;

	for(int i = 0; i < system_table->number_of_table_entries; i++){
		ConfigurationTable* table = &system_table->configuration_tables[i];
		
		if(compare_efi_guid(&table->vendor_guid,&acpi_guid)){
			acpi_table = table->vendor_table;
			break;
		}

	}
	if(acpi_table == NULL){
		log(u"Acpi not work");
		hang();
	}


	//XSDT = (struct XSDT_t*)acpi_table->XSDT_address;
	//log(u"XSDT");
	

}

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

BootInfo* setup_boot_info(){
  FrameBuffer* framebuffer = get_framebuffer();

  BootInfo* new_boot_info;
  allocate_memory(sizeof(BootInfo),(void**)&new_boot_info);
  copy_memory(new_boot_info, boot_info, sizeof(BootInfo));

  set_boot_info(new_boot_info);

  BootInfo* boot_info = get_boot_info();
  boot_info->frame_buffer = *framebuffer;
  boot_info->xsdt_address = acpi_table->XSDT_address;

  return new_boot_info;
}

void boot_pkernel() {

  get_graphics_output_protocol();

  get_acpi_table();
  
	load_kernel_file();

  get_memory_address_for_pkernel();

  uint64_t kernel_size = get_file_size(get_kernel_file());

  read_file_to_memory(get_kernel_file(), kernel_size, 
      (void*)pkernel_physical_address);

  close_file(get_kernel_file());

	
  void (*run_kernel)(BootInfo*);

	run_kernel = (void (*)(BootInfo*))pkernel_physical_address;
  


  BootInfo* boot_info = setup_boot_info();

  log(u"launching pkernel..");

  exit_boot_services();


  //execute
	(*run_kernel)(boot_info);

  log(u"executed");
  
  //never reach here
  hang();
}
