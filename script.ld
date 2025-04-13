SECTIONS
{
    /* Align .text to page size */
    . = 0x100000; /* or your base virtual address */
    .text ALIGN(0x1000) : {
        __ehdr_start = .;
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata)
        *(.rodata.*)
    }

    .init_array ALIGN(8) : {
        __init_array_start = .;
        KEEP(*(.init_array*))
        __init_array_end = .;
    }

    .preinit_array ALIGN(8) : {
        __preinit_array_start = .;
        KEEP(*(.preinit_array*))
        __preinit_array_end = .;
    }

    .data ALIGN(0x1000) : {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(0x1000) (NOLOAD) : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
}
