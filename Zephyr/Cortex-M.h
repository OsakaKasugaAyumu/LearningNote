/**
 * Cortex-M内核:
 * 
 * 
 *                  Privileged       Unprivileged
 *  Thread          P + T            U + T
 *  Handler         P + H            -----
 * 
 * 
 *  CONTROL : 
 *      bit 0 : nPRIV
 *          - 0 特权
 *          - 1 非特权
 * 
 *      bit 1 : SPSEL
 *          - 0 MSP
 *          - 1 PSP
 * 
 *  CONTROL只在Thread mode下有含义, 在Handler mode下, 硬件忽略CONTROL的控制, 强制特权 + MSP
 *  异常返回时, LR不是普通地址, 而是特殊编码 EXC_RETURN
 *      bit 31:5        --
 *      bit 4           FTYPE   1=使用扩展帧(FPU)  0=基础帧
 *      bit 3           Mode    1=返回Thread mode 0=返回Handler mode
 *      bit 2           SPSEL   返回Thread mode使用的栈 : 1=PSP 0=MSP
 *      bit 1:0         --
 * 
 *  
 * 
 *  系统控制空间(SCS), 及PPB起始地址 0xE000E000
 *  ICSR (0xE000ED04) 设置/清除系统异常的挂起状态(NMI, PendSV, Systick)
 *  VTOR (0xE000ED08) 中断向量表的地址[31 : 7]
 *  MPU_BASE (0xE000ED90)
 * 
 * 
*/


