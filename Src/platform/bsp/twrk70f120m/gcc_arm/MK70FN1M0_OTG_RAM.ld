/* Entry Point */
ENTRY(Reset_Handler)

__HEAP_SIZE = 0x4000;      /* required amount of heap  */
_Stack_Size = 0x3800;      /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
    ram (rw)     : ORIGIN = 0x1FFF0000, LENGTH = 0x20000 
}

/* Define output sections */
SECTIONS
{

    /* The program code and other data goes into ram */
    .text :
    {
        __VECTOR_TABLE = .;
	__VECTOR_RAM = .;
        KEEP(*(.isr_vector))
        _stext_ = .;

        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
         *crtbegin.o(.dtors)
         *crtbegin?.o(.dtors)
         *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
         *(SORT(.dtors.*))
         *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > ram

    .ARM.extab : 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ram

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ram
    __exidx_end = .;

    __etext = .;

    __DATA_ROM = .;
        
    .data :
    {
        __DATA_RAM = .;
        __data_start__ = .;
        *(vtable)
        *(.data*)

        /* All data end */
        __data_end__ = .;

    } > ram
    __DATA_END = __DATA_ROM + SIZEOF(.data);
    .sram_data :
    {
        . = ALIGN(512);
        __BDT_BASE = .;
        *(.usb_bdt)
        __BDT_END = .;
    } > ram

    /* bss */
    .bss :
    {
        . = ALIGN(4);
        __START_BSS = .;
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        __END_BSS = .;
    } > ram

    /* heap */
 
    .heap (COPY):
    {
        __end__ = .;
		    PROVIDE(end = .);
		    *(.heap*)
		    __HeapLimit = .; 
    } > ram

    /* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		*(.stack*)
	} > ram

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(ram) + LENGTH(ram);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);
	
	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}

