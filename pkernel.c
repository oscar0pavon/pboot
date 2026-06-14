#include "efi.h"
#include "pboot.h"
#include "graphics.h"
#include "utils.h"
#include "types.h"
#include "memory.h"

static uint64_t pkernel_physical_address = 0x4000000;

static uint64_t pkernel_file_size;

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
	
	Handle handle = get_bootloader_handle();

	status = system_table->boot_table->exit_boot_services(handle, 
			mmap_key);

	if(status != EFI_SUCCESS){
		log(u"ERROR boot service not closed");
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


void boot_pkernel() {

  get_graphics_output_protocol();

  get_acpi_table();
  
	load_kernel_file();

  get_memory_address_for_pkernel();

  uint64_t kernel_size = get_file_size(get_kernel_file());

  read_file_to_memory(get_kernel_file(), kernel_size, 
      (void*)pkernel_physical_address);

	
  void (*run_kernel)(void*,uint64_t);

	run_kernel = (void (*)(void*,uint64_t))pkernel_physical_address;
  
  void* framebuffer = get_framebuffer();

  log(u"launching pkernel..");

  log(u"Get memory");

  exit_boot_services();

  //execute
	(*run_kernel)(framebuffer,acpi_table->XSDT_address);

  log(u"executed");
  
  //we never got here
  hang();
}
