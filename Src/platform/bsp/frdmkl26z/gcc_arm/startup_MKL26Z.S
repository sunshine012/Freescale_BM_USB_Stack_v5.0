/*****************************************************************************/
/* startup_MKL26Z4.s: Startup file for MKL26Z4 device series                   */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/


    .syntax unified
    .arch armv6-m

    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x00000400
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

    .section .heap
    .align 3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0x00000000
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size __HeapBase, . - __HeapBase
__HeapLimit:
    .size __HeapLimit, . - __HeapLimit

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                  /* Top of Stack */
    .long   Reset_Handler               /* Reset Handler */
    .long   NMI_Handler                 /* NMI Handler                  */
    .long   HardFault_Handler           /* Hard Fault Handler           */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   SVC_Handler                 /* SVCall Handler               */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   PendSV_Handler              /* PendSV Handler               */
    .long   SysTick_Handler             /* SysTick Handler              */

    /* External Interrupts */
    .long   DMA0_IRQHandler  /* DMA channel 0 transfer complete/error interrupt */
    .long   DMA1_IRQHandler  /* DMA channel 1 transfer complete/error interrupt */
    .long   DMA2_IRQHandler  /* DMA channel 2 transfer complete/error interrupt */
    .long   DMA3_IRQHandler  /* DMA channel 3 transfer complete/error interrupt */
    .long   Reserved20_IRQHandler  /* Reserved interrupt 20 */
    .long   FTFA_IRQHandler  /* FTFA command complete/read collision interrupt */
    .long   LVD_LVW_IRQHandler  /* Low Voltage Detect, Low Voltage Warning */
    .long   LLW_IRQHandler  /* Low Leakage Wakeup */
    .long   I2C0_IRQHandler  /* I2C0 interrupt */
    .long   I2C1_IRQHandler  /* I2C0 interrupt 26 */
    .long   SPI0_IRQHandler  /* SPI0 interrupt */
    .long   SPI1_IRQHandler  /* SPI1 interrupt */
    .long   UART0_IRQHandler  /* UART0 status/error interrupt */
    .long   UART1_IRQHandler  /* UART1 status/error interrupt */
    .long   UART2_IRQHandler  /* UART2 status/error interrupt */
    .long   ADC0_IRQHandler  /* ADC0 interrupt */
    .long   CMP0_IRQHandler  /* CMP0 interrupt */
    .long   TPM0_IRQHandler  /* TPM0 fault, overflow and channels interrupt */
    .long   TPM1_IRQHandler  /* TPM1 fault, overflow and channels interrupt */
    .long   TPM2_IRQHandler  /* TPM2 fault, overflow and channels interrupt */
    .long   RTC_IRQHandler  /* RTC interrupt */
    .long   RTC_Seconds_IRQHandler  /* RTC seconds interrupt */
    .long   PIT_IRQHandler  /* PIT timer interrupt */
    .long   Reserved39_IRQHandler  /* Reserved interrupt 39 */
    .long   USB_ISR  /* USB0 interrupt */
    .long   DAC0_IRQHandler  /* DAC0 interrupt */
    .long   TSI0_IRQHandler  /* TSI0 interrupt */
    .long   MCG_IRQHandler  /* MCG interrupt */
    .long   LPTimer_IRQHandler  /* LPTimer interrupt */
    .long   Reserved45_IRQHandler  /* Reserved interrupt 45 */
    .long   PORTA_IRQHandler  /* Port A interrupt */
    .long   PORTD_IRQHandler  /* Port D interrupt */


    .size    __isr_vector, . - __isr_vector

/* Flash Configuration */

  	.long	0xFFFFFFFF
  	.long	0xFFFFFFFF
  	.long	0xFFFFFFFF
  	.long	0xFFFFFFFE

    .thumb

    .weak   init_data_bss
    .type   init_data_bss, %function
init_data_bss:
    bx     r14

/* Reset Handler */

    .text
    .thumb
    .thumb_func
    .align 2
    .globl   Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
#ifndef __NO_SYSTEM_INIT
    bl	SystemInit
#endif
#ifndef __NO_INIT_DATA_BSS
	bl init_data_bss
#endif

/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__


    subs    r3, r2
    ble    .LC1
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:


#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    movs    r0, 0
.LC2:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC2
#endif /* __STARTUP_CLEAR_BSS */

#ifndef __START
#define __START _start
#endif
    bl    main
    .pool
    .size Reset_Handler, . - Reset_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm

    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    Default_Handler

/* IRQ Handlers */

    def_irq_handler     DMA0_IRQHandler
    def_irq_handler     DMA1_IRQHandler
    def_irq_handler     DMA2_IRQHandler
    def_irq_handler     DMA3_IRQHandler
    def_irq_handler     Reserved20_IRQHandler
    def_irq_handler     FTFA_IRQHandler
    def_irq_handler     LVD_LVW_IRQHandler
    def_irq_handler     LLW_IRQHandler
    def_irq_handler     I2C0_IRQHandler
    def_irq_handler     I2C1_IRQHandler
    def_irq_handler     SPI0_IRQHandler
    def_irq_handler     SPI1_IRQHandler
    def_irq_handler     UART0_IRQHandler
    def_irq_handler     UART1_IRQHandler
    def_irq_handler     UART2_IRQHandler
    def_irq_handler     ADC0_IRQHandler
    def_irq_handler     CMP0_IRQHandler
    def_irq_handler     TPM0_IRQHandler
    def_irq_handler     TPM1_IRQHandler
    def_irq_handler     TPM2_IRQHandler
    def_irq_handler     RTC_IRQHandler
    def_irq_handler     RTC_Seconds_IRQHandler
    def_irq_handler     PIT_IRQHandler
    def_irq_handler     Reserved39_IRQHandler
    def_irq_handler     USB_ISR
    def_irq_handler     DAC0_IRQHandler
    def_irq_handler     TSI0_IRQHandler
    def_irq_handler     MCG_IRQHandler
    def_irq_handler     LPTimer_IRQHandler
    def_irq_handler     Reserved45_IRQHandler
    def_irq_handler     PORTA_IRQHandler
    def_irq_handler     PORTD_IRQHandler

    .end
