OUTPUT_FORMAT(binary)
ENTRY(start)
SECTIONS {

  .text : ALIGN(4096) {
    start.o(.text)
    *(.text*) 
  }

  .rodata : ALIGN(4096) {
    *(.rodata*) 
  }

  .data : ALIGN(4096) {
    *(.data*)
    *(.bss*)
    *(COMMON)
  }

}


