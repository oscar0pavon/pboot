#ifndef __EFI_H__
#define __EFI_H__

#include "types.h"
#include "console.h"

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
	{ 0x5b1b31a1, 0x9562, 0x11d2, \
	  { 0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }

#define EFI_ACPI_20_TABLE_GUID \
	{0x8868e871,0xe4f1,0x11d3,\
		{0xbc,0x22,0x00,0x80,0xc7,0x3c,0x88,0x81}}


typedef void* Event;
typedef void (*EventNotify)(Event* event, void* context);

typedef enum {
	TimerCancel,
	TimerPeriodic,
	TimerRelative
} TimerDelay;

#define EFI_EVENT_TIMER 0x80000000

#define EFI_SUCCESS 0

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL		0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL					0x00000002


struct MemoryDescriptor{
	uint32_t type;
	uint64_t physical_start;
	uint64_t virtual_start;
	uint64_t pages;
	uint64_t attributes;
};

enum AllocateType{
	EFI_ALLOCATE_ANY_PAGES,
};

enum MemoryType{
	EFI_LOADER_CODE,
	EFI_LOADER_DATA,
};

struct GUID{
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
};

struct TableHeader{
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
};


typedef struct{
	uint8_t type;
	uint8_t sub_type;
	uint8_t length[2];
}DevicePathProtocol;


typedef struct {
	EFI_GUID vendor_guid;
	void* vendor_table;
}ConfigurationTable;


struct SystemTable{
	struct TableHeader		 header;
	uint16_t							*unused1;//firmware vendor
	uint32_t							 unused2;//firmware revision
	void									*unused3;//console in handle
	InputProtocol					*input; 
	void									*unused5;//console out handle
	TextOutputProtocol		*out;
	void									*unused6;//standard error handle
	void									*unused7;//standard error
	void									*unused8;//runtime services
	BootTable							*boot_table;
	uint64_t							 number_of_table_entries;
	ConfigurationTable    *configuration_tables;
};


typedef struct LoadedImageProtocol{
	uint32_t revision;
	Handle parent;
	SystemTable *system;

	// Source location of the image
	Handle device;
	DevicePathProtocol *file_path;
	void *reserved;

	uint32_t load_options_size;
	void *load_options;

	// Location of the image in memory
	void *image_base;
	uint64_t image_size;
	enum MemoryType image_code_type;
	enum MemoryType image_data_type;
	void (*unused)();
}LoadedImageProtocol;


struct BootTable 
{
	struct TableHeader header;
	void (*unused1)();
	void (*unused2)();

	Status (*allocate_pages)(enum AllocateType,enum MemoryType,
														uint64_t,uint64_t*);

	Status (*free_pages)(uint64_t, uint64_t);

	Status (*get_memory_map)(uint64_t*,struct MemoryDescriptor*,
														uint64_t*,uint64_t*,uint32_t *);

	Status (*allocate_pool)(enum MemoryType, uint64_t, void **);
	Status (*free_pool)(void *);

	// Events and Timer services
	Status (*create_event)(uint32_t type, uint64_t TPL, EventNotify notify,
													void* context, Event* out_event);

	Status (*set_timer)(Event* event, TimerDelay type, uint64_t trigger_time);

	Status (*wait_for_event)(uint64_t number_of_events, 
			Event* events_array, uint64_t* out_index);

	void (*unused10)();//signal event

	Status (*close_event)(Event* event);
	Status (*check_event)(Event event);

	//Protocol Handler Services
	void (*unused13)();
	void (*unused14)();
	void (*unused15)();
	Status (*handle_protocol)(Handle, GUID*, void** interface);
	void *reserved;
	void (*unused17)();
	void (*unused18)();//locate handle
	void (*unused19)();//locate device path
	void (*unused20)();//install configuration table

	Status (*image_load)(bool boot_policy, Handle parent_image_handle,
			DevicePathProtocol *device_path, void* source_buffer,
			uint64_t source_size, Handle* image_handle);

	Status (*start_image)(Handle image_handle,
			uint64_t * exit_data_size, 
			uint16_t **exit_data);

	void (*unused23)();
	void (*unused24)();
	Status (*exit_boot_services)(Handle, uint64_t);
	void (*unused26)();
	void (*unused27)();
	void (*unused28)();
	void (*unused29)();
	void (*unused30)();

	Status (*open_protocol)(
		Handle handle,
		GUID* protocol,
		void** interface,//optional
		Handle agent_handle,
		Handle controller_handle,
		uint32_t attributes);

	Status (*close_protocol)(Handle,GUID*,Handle agent_handle,
														Handle controller_handle);
	void (*unused33)();
	Status (*protocols_per_handle)(Handle, GUID ***, uint64_t *);
	void (*unused35)();
	Status (*locate_protocol)(GUID* protocol_guid, void* registration,
														void** interface);
	void (*unused37)();
	void (*unused38)();
	void (*unused39)();
	void (*unused40)();
	void (*unused41)();
	void (*unused42)();
};



#endif
