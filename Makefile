CFLAGS := -ffreestanding -fno-stack-check -fno-stack-protector 
CFLAGS += -fPIC -fshort-wchar -mno-red-zone 
CFLAGS += -maccumulate-outgoing-args -mabi=ms

LDFLAGS := -nostdlib -znocombreloc -shared -Bsymbolic

SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)

all: pboot.efi

%.o : %.c
	@echo "Compiling $@"
	$(CC) $(CFLAGS) -c $<

pboot.bin: $(OBJS)
	ld $(OBJS) $(LDFLAGS) -o pboot.bin -T binary.ld 

pboot.efi: pboot.bin efi.s
	@echo "Creating pboot using fasm"
	./tools/fasm efi.s pboot.efi
	chmod +x pboot.efi
	@echo "You have pboot.efi"


clean:
	rm -f *.o
	rm -f pboot.efi
	rm -f pboot.bin

install:
	cp pboot.efi /boot/pboot.efi
	@echo "Installed on /boot/pboot.efi"

$(LOG).SILENT:
