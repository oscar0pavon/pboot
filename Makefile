CFLAGS := -ffreestanding -fno-stack-check -fno-stack-protector 
CFLAGS += -fPIC -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -mabi=ms

LDFLAGS := -nostdlib -znocombreloc -shared -Bsymbolic

SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)
OBJS := $(filter-out start.o, $(OBJS))

all: pboot

%.o : %.c
	@echo "Compiling $@"
	$(CC) $(CFLAGS) -c $<

start.o: start.c
	$(CC) $(CFLAGS) -c start.c

pboot.bin: start.o $(OBJS)
	ld start.o $(OBJS) $(LDFLAGS) -o pboot.bin -T binary.ld 

pboot: efi.s pboot.bin
	@echo "Creating pboot using fasm"
	./tools/fasm efi.s pboot
	chmod +x pboot
	@echo "You have pboot"


clean:
	rm -f *.o
	rm -f pboot
	rm -f pboot.bin

install:
	cp pboot /boot/EFI/pboot/pboot.efi

$(LOG).SILENT:
