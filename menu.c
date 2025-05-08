#include "menu.h"
#include "efi.h"
#include "pboot.h"
#include "types.h"

#include "input.h"
#include "utils.h"
#include <stdint.h>

static uint8_t default_entry = 2;

static uint8_t number_of_entries = 0;

static uint8_t entry_selected = 0;

static bool show_menu = false;

static uint64_t wait_time = 5000000;


static Unicode HORIZONTAL_UNICODE[2] = {0x2500,0};

static Unicode VERTICAL_UNICODE[2] = {0x2502,0};

static Unicode corner1[2] = {0x250C,0};
static Unicode corner2[2] = {0x2510,0};
static Unicode corner3[2] = {0x2518,0};
static Unicode corner4[2] = {0x2514,0};

#define MAX_ENTRIES 10

static BootLoaderEntry entries[MAX_ENTRIES] = {};

void set_number_of_entries(uint8_t number){
	number_of_entries = number;	
}

BootLoaderEntry* get_entries(){
	return entries;
}

void set_default_entry(uint8_t entry){
  default_entry = entry;
}

void set_show_menu(bool can_show){
	show_menu = can_show;
}

Unicode* get_selected_kernel() { 
	return entries[entry_selected].kernel_name; 
}

Unicode* get_selected_parameters() {

  return entries[entry_selected].kernel_parameters;
}


void print_entries() {
  SystemTable *system_table = get_system_table();
	TextOutputProtocol* output = system_table->out;	

	u8 entries_position_x = 4;
	u8 entries_position_y = 4;

  for (uint8_t i = 0; i < number_of_entries; i++) {

		output->set_cursor_position(output, entries_position_x,entries_position_y);
    output->print(system_table->out, entries[i].entry_name);
    if (i == entry_selected) {
      system_table->out->print(system_table->out, u"*");
    }
		entries_position_y++;
		output->set_cursor_position(output, entries_position_x,entries_position_y);
  }
}

void draw_menu(){
	SystemTable* system_table = get_system_table();
	u32 column = system_table->out->mode->column;
	u32 row = system_table->out->mode->row;

	TextOutputProtocol* output = system_table->out;	
	
	
	output->clear_screen(output);
	output->set_cursor_position(output,2,2);	
	output->print(output, corner1);
	
	output->set_cursor_position(output,78,2);	
	output->print(output, corner2);

	output->set_cursor_position(output,78,23);	
	output->print(output, corner3);

	output->set_cursor_position(output,2,23);	
	output->print(output, corner4);

	for(int i = 3; i < 78; i++){
		output->set_cursor_position(output,i,2);	
		output->print(output, HORIZONTAL_UNICODE);
		output->set_cursor_position(output,i,23);	
		output->print(output, HORIZONTAL_UNICODE);
	}

	for(int i = 3; i < 23; i++){
		output->set_cursor_position(output,2,i);	
		output->print(output, VERTICAL_UNICODE);
		output->set_cursor_position(output,78,i);	
		output->print(output, VERTICAL_UNICODE);
	}

	output->set_cursor_position(output,38,3);	

	output->print(output, u"pboot");
	
	output->set_cursor_position(output,4,22);	
	output->print(output, u"Up: w    Down: s    Boot: d");
}

void enter_in_menu_loop(){
	
	SystemTable* system_table = get_system_table();
	
	system_table->out->clear_screen(system_table->out);

	draw_menu();
	print_entries();

	InputKey key_pressed;
	while (1) {
	
	Status status;
	status = system_table->input->read_key_stroke(system_table->input, &key_pressed);

	if(status == EFI_SUCCESS){

		if(key_pressed.scan_code == KEY_CODE_UP || key_pressed.unicode_char == u'w'){
			if(entry_selected > 0){
				entry_selected--;
			}
		}

		if(key_pressed.scan_code == KEY_CODE_DOWN || key_pressed.unicode_char == u's'){

			if(entry_selected < number_of_entries-1){//-1 because start at 0
				entry_selected++;
			}	
		}
	
		system_table->out->clear_screen(system_table->out);
		draw_menu();
		print_entries();
	
		if(key_pressed.scan_code == KEY_CODE_RIGHT || key_pressed.unicode_char == u'd'){
			system_table->out->clear_screen(system_table->out);
			boot();		
		}
	}

	}
}

void can_show_menu() {

  entry_selected = default_entry;

  InputKey key_pressed;

  SystemTable *system_table = get_system_table();

  BootTable *boot_table = system_table->boot_table;

  InputProtocol *input = system_table->input;

  bool checked_event = false;

  Event timer_event;

  Event events[2];

  uint64_t event_index = 500;

  while (!checked_event) {
    boot_table->create_event(EFI_EVENT_TIMER, 0, NULL, NULL, &timer_event);
    boot_table->set_timer(timer_event, TimerRelative, wait_time); // 500 ms

    events[0] = input->wait_for_key;
    events[1] = timer_event;

    boot_table->wait_for_event(2, events, &event_index);
    if (event_index == 0) {
			boot_table->close_event(timer_event);
      system_table->input->read_key_stroke(system_table->input, &key_pressed);
      if (key_pressed.unicode_char == u'a') {
        enter_in_menu_loop();
      }
      checked_event = true;
    }

    if (event_index == 1) {
			boot_table->close_event(timer_event);
      if (show_menu) {
        enter_in_menu_loop();
      }
			checked_event = true;
    }

  }
}
