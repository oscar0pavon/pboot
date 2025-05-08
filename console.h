#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "types.h"

typedef struct OutputMode{
	u32 max_mode;
	u32 mode;
	u32 attribute;
	u32 column;
	u32 row;
	bool cursor_visible;
}OutputMode;


struct TextOutputProtocol{
	void (*unused1)();
	Status (*print)(TextOutputProtocol*, Unicode*);
	void (*unused2)();
	void (*unused3)();
	void (*unused4)();
	void (*unused5)();
	Status (*clear_screen)(TextOutputProtocol*);
	Status (*set_cursor_position)(TextOutputProtocol* self, u32 column, u32 row);
	void (*unused7)();
	OutputMode* mode;
};

#endif
