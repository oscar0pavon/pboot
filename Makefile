CFLAGS := -ffreestanding -fno-stack-check -fno-stack-protector -fPIC -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -mabi=ms

all: pboot

objects := main.o utils.o files.o configuration.o menu.o graphics.o input.o

start.o: start.c
	cc $(CFLAGS) -c start.c

pboot.bin: start.o $(objects)
	ld start.o $(objects) -nostdlib -znocombreloc -shared -Bsymbolic -o pboot.bin -T binary.ld 

pboot: efi.s pboot.bin
	fasm efi.s pboot

main.o: main.c types.h efi.h
	cc $(CFLAGS) -c main.c

configuration.o: configuration.h configuration.c
	cc $(CFLAGS) -c configuration.c

menu.o: menu.h menu.c
	cc $(CFLAGS) -c menu.c

input.o: input.h input.c
	cc $(CFLAGS) -c input.c

graphics.o: graphics.h graphics.c
	cc $(CFLAGS) -c graphics.c

utils.o: utils.h utils.c
	cc $(CFLAGS) -c utils.c

files.o: files.h files.c
	cc $(CFLAGS) -c files.c


clean:
	rm -f *.o
	rm -f pboot
	rm -f pboot.bin

install:
	cp pboot /boot/EFI/pboot/pboot.efi

