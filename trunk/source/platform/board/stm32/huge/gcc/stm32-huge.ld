/*
 *    Copyright(C) 2013-2015, Fans-rt development team.
 *
 *    All rights reserved.
 *
 *    This is open source software.
 *    Learning and research can be unrestricted to  modification, use and dissemination.
 *    If you need for commercial purposes, you should get the author's permission.
 *      Configuration:
 *      System global core stack                  NO
 *      The local core stack of general task      YES
 *      The loacl core stack of kernel task       YES
 *      The local user stack of general task      NO
 *      Hardware supported task switch IRQ        YES
 *      Hardware supported double stack           NO
 *    date           author          notes
 *    2015-07-19     JiangYong       new file
 */

/* The boot stack size if 1536 bytes */
__boot_stack_size = 1536 ;

PROVIDE ( _Stack_Size = __boot_stack_size ) ;

__boot_stack_buffer = _estack  - __boot_stack_size ;

PROVIDE ( _boot_stack_buffer = __boot_stack_buffer ) ;

_estack = 0x20000600;

/* The memory region definitions */

MEMORY
{
  CORE_ROM (rx): ORIGIN = 0x08000000, LENGTH = 0xf000
  STACK_RAM (xrw): ORIGIN = 0x20000000, LENGTH = 0xec00
  USER_RAM (xrw):  ORIGIN = 0x2000ec00, LENGTH = 0x0200
  CORE_RAM (xrw):  ORIGIN = 0x2000ee00, LENGTH = 0x1200
}

/* Sections definitions */

SECTIONS
{
    /* The boot startup vector */
    .isr_vector :
    {
	. = ALIGN(4);
        KEEP(*(.isr_vector))            /* Startup code */
	. = ALIGN(4);
    } >CORE_ROM

    /* the program code is stored in the .text section, which goes to Flash */
    .text :
    {
    . = ALIGN(4);

        *(.text)
        *(.text.*)
        *(.CoreText)
        *(.UserText)
        *(.rodata)
        *(.rodata*)
        *(.data)
        *(.data.*)
        *(.data.rel.ro)
        *(.data.rel.ro.*)
        *(.CoreROdata)
        *(.UserROdata)
        *(.glue_7)
        *(.glue_7t)
        *(.igot)
        *(.igot.*)
        *(.got)
        *(.got.*)

    . = ALIGN(4);
    _etext = .;
    _sidata = _etext;
    } >CORE_ROM

    
    /* This is the uninitialized data section */
    .boot_stack :
    {
	    . = ALIGN(4);
 
        _sbstack = .;
        *(.BootStack)
        *(.IdleStack)
        /* *(.CoreStack) */
        *(.Region0)

	    . = ALIGN(4);
   	 _ebstack = . ;
    } >STACK_RAM

    .data  : AT ( _sidata )
    {
	    . = ALIGN(4);
        _sdata = . ;

        *(.UserRWdata)
	    . = ALIGN(4);

    } >USER_RAM
    
    .data  : AT ( _sidata )
    {
	    . = ALIGN(4);

        *(.CoreRWdata)
	    . = ALIGN(4);

   	    _edata = . ;
    } >CORE_RAM

    .bss :
    {
	    . = ALIGN(4);
        _sbss = .;
 
        *(.bss)
        *(COMMON)
        
	    . = ALIGN(4);
   	 _ebss = . ;
    } >CORE_RAM
    

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );
    
    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
