


/*
    _ASM_FILE_PROLOGUE
 */
.text
.syntax unified
.thumb

/**
    SECTION_SUBSEC_FUNC(image_header,_image_header_section,_image_header)
 */
.section .image_header._image_header_section, "ax"
PERFOPT_ALIGN
_image_header:
    .long z_main_stack + CONFIG_MAIN_STACK_SIZE
    .long z_arm_reset
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0x016f2818
    .long __rom_region_start
    .long __end




