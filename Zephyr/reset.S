


/**
    #define GTEXT(sym) .global sym; .type sym, %function
    #define GDATA(sym) .global sym; .type sym, %object
    #define WTEXT(sym) .weak sym; .type sym, %function
    #define WDATA(sym) .weak sym; .type sym, %object
 */

/**
    _ASM_FILE_PROLOGUE
 */
.text 
.syntax unified
.thumb


.global z_arm_reset
.type z_arm_reset, %function

.global arch_early_memset
.type arch_early_memset, %function

.global z_interrupt_stack
.type z_interrupt_stack, %object

.global z_main_stack
.type z_main_stack, %object



/**
    入口定义两个别名:
        - z_arm_reset : zephyr内部使用的
        - __start : 兼容其他外部引导程序 MCUBoot U-Boot
 */
.section .TEXT._reset_section
PERFOPT_ALIGN
z_arm_reset:

.section .TEXT.__reset_section
PERFOPT_ALIGN
__start:

    /**
        链式加载场景下必须强制初始化硬件, 因为MCUBoot可能已经修改过CONTROL寄存器了 
        在ARMv7-M上, .word和.long完全一致
        movs.n 强制使用16 bit thumb编码(.n), 并且会更新APSR(s) N Z C V

        nPRIV[bit 0] : 0设置成特权级
        SPSEL[bit 1] : 0设置为MSP    
    */
    movs.n r0, #0
    msr CONTROL, r0
    isb


    /**
        在链式加载中, 需要强制初始化MSP, 避免指向MCUBoot任务的栈
     */
    ldr r0, =z_main_stack + CONFIG_MAIN_STACK_SIZE 
    msr msp, r0


    /**
        清除z_sys_post_kernel flag 
     */
    movs.n r0, #0
    ldr r1, =z_sys_post_kernel
    strb r0, [r1]    //写 1 byte到z_sys_post_kernel中

    /**
        除能MPU
     */
    movs.n r0, #0
    ldr r1, =_SCS_MPU_CTRL
    str r0, [r1]
    dsb

