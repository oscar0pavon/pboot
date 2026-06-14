OUTPUT_FORMAT(binary)
SECTIONS {
  .text : ALIGN(4096) {
    *(.text*) 
  }

  .rodata : ALIGN(4096) {
    *(.rodata*) 
  }

  .data : ALIGN(4096) {
    *(.data*)
  }

  .bss : ALIGN(4096) {
    /* Track the start of the BSS segment */
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    _bss_end = .;
  }

}


