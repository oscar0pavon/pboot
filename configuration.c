#include "menu.h"
#include "types.h"
#include "pboot.h"
#include "files.h"
#include "memory.h"

static int current_parsing_entry = 0;

int strncmp(const char* s1, const char* s2, size_t n) {
  while (n && *s1 && (*s1 == *s2)) {
      s1++;
      s2++;
      n--;
  }
  if (n == 0) {
      return 0;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strcmp(const char* s1, const char* s2) {
  while (*s1 && (*s1 == *s2)) {
      s1++;
      s2++;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

//Moves pointer past spaces and tabs
const char* skip_spaces(const char* p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

//Reads a word until a space, newline, or null terminator is hit
const char* read_word(const char* src, char* dest, int max_len) {
    src = skip_spaces(src);
    int i = 0;
    while (*src && *src != ' ' && *src != '\t' &&
        *src != '\n' && *src != '\r' && i < max_len - 1) {
        dest[i++] = *src++;
    }
    dest[i] = '\0';
    return src;
}

Unicode ascii_to_unicode(char character){
  return (Unicode)(unsigned char)character;
}

const char* parse_string(const char* word, Unicode* output, int max_len){
	int char_count = 0;
	while(*word != '\"'){
		if(char_count >= max_len - 1)
			break;
		Unicode new_character = ascii_to_unicode(*word);
		output[char_count] = new_character;
		word++;
		char_count++;
		if(!*word || *word == 10)
			break;
	}
	Unicode zero = ascii_to_unicode('\0');
	output[char_count] = zero;

	return word;
}

void load_configuration(){
	FileProtocol* config_file;
	open_file(&config_file, u"pboot.conf");
	const char* config = read_file(config_file);

	uint8_t default_entry = 0;
	while(*config){
		if(*config == 'm'){
			config++;
			config++;
			if(*config == '1'){
				set_show_menu(true);
			}else if(*config == '0'){
				set_show_menu(false);
			}
		}else if(*config == 'e'){
			config++;
			config++;
			default_entry = *config - '0';

		}else if(*config == 'n'){
			config++;
			config++;
			config++;
			BootLoaderEntry* entries = get_entries();
			config = parse_string(config, entries[current_parsing_entry].entry_name, 20);
		}else if(*config == 'k'){
			config++;
			config++;
			config++;
			BootLoaderEntry* entries = get_entries();
			config = parse_string(config, entries[current_parsing_entry].kernel_name, 20);
		}else if(*config == 'p'){
			config++;
			config++;
			config++;
			BootLoaderEntry* entries = get_entries();
			config = parse_string(config, entries[current_parsing_entry].kernel_parameters, 100);
			current_parsing_entry++;
		}
		config++;
	}

	if(current_parsing_entry == 0){
		set_number_of_entries(0);
		set_default_entry(0);
		return;
	}

	uint8_t entries_count = current_parsing_entry;
	set_number_of_entries(entries_count);

	if(default_entry >= entries_count){
		default_entry = entries_count - 1;
	}

	set_default_entry(default_entry);
}
