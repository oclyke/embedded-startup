/* stack: dynamic */
/* heap: dynamic */

ENTRY(Reset_Handler)

MEMORY
{
  flash (rx) : ORIGIN = 0x0000C000, LENGTH = 960K
  sram (rwx) : ORIGIN = 0x10000000, LENGTH = 384K
}

SECTIONS
{
  /* text: executable code */
  /* located in _flash_ */
  .text :
  {
    . = ALIGN(4);
    _stext = .;
    KEEP(*(.isr_vector))
    *(.text)
    *(.text*)

    /* .ctors */
    . = ALIGN(4);
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    . = ALIGN(4);
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    /* .rodata */
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)

    . = ALIGN(4);
    _etext = .;
  } > flash

  /* data: uniquely initialized symbols */
  /* loaded into flash region, copied to sram region at startup */
  /* VMA appears in sram region, LMA is in flash region for initialization */
  /* _init_data used by startup to locate flash region copy of data */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > sram AT>flash
  _init_data = LOADADDR(.data);

  /* bss: zero-initialized symbols */
  /* don't require flash memory to remember their value */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > sram

  /* heap: RAM memory that can be dynamically allocated in the upward direction (increasing memory addresses) */
  /* _sheap is used to identify the beginning of available dynamic memory */
  .heap :
  {
    . = ALIGN(4);
    __end__ = .;
    PROVIDE( end = . );
    _sheap = .;
  } >sram

  /* dmz: the no-mans-land between the stack and the heap */
  /* purpose is to place padding between initial (empty) stack and heap */
  .dmz :
  {
    . = ORIGIN(sram) + LENGTH(sram) - 4; /* Automatically give the rest of the space in sram to the stack */
    . = ALIGN(4);
  }

  /* stack: RAM memory that can be dynamically allocated in the downward direction (decreasing memory addresses) */
  /* _sstack is used to set the initial stack pointer */
  .stack :
  {
    . = ALIGN(4);
    _sstack = .;
  } >sram
}