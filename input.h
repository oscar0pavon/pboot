#ifndef __INPUT_H__
#define __INPUT_H__

#include "efi.h"
#include "types.h"

#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID \
		{0xdd9e7533, 0x7762, 0x4698, \
			{0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa}}

#define KEY_CODE_UP			0x01
#define KEY_CODE_DOWN		0x02
#define KEY_CODE_RIGHT	0x03
#define KEY_CODE_LEFT		0x04


typedef struct{
	uint16_t scan_code;
	uint16_t unicode_char;
}InputKey;


struct InputProtocol{
	void		(*reset)();
	Status	(*read_key_stroke)(InputProtocol *self, InputKey* key);
	void		(*wait_for_key)();
};

void init_input();

#endif
