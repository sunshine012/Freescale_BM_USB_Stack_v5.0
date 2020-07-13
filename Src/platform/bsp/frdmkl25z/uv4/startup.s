;/*****************************************************************************
; * @file:    startup_MKL26Z.s
; * @purpose: CMSIS Cortex-M0+ Core Device Startup File for the
; *           MKL26Z
; * @version: 1.2
; * @date:    2012-12-4
; *
; * Copyright: 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
;*
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; *****************************************************************************/
#include "user_config.h"

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
#if DEVICE_APP==1
Stack_Size      EQU     0x00000200
#else
Stack_Size      EQU     0x00001000
#endif
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
#if DEVICE_APP==1
Heap_Size       EQU     0x00000000
#else
Heap_Size       EQU     0x00002000
#endif

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
				EXPORT  ___VECTOR_RAM
                EXPORT  __Vectors_Size
				IMPORT  __main
___VECTOR_RAM
__Vectors       DCD     __initial_sp  ; Top of Stack
                DCD     Reset_Handler  ; Reset Handler
                DCD     NMI_Handler  ; NMI Handler
                DCD     HardFault_Handler  ; Hard Fault Handler

        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler
        ; External Interrupts
        DCD     DMA0_IRQHandler           ; 0:  DMA Channel 0 transfer complete
        DCD     DMA1_IRQHandler           ; 1:  DMA Channel 1 transfer complete
        DCD     DMA2_IRQHandler           ; 2:  DMA Channel 2 transfer complete
        DCD     DMA3_IRQHandler           ; 3:  DMA Channel 3 transfer complete
        DCD     0                         ; Reserved
        DCD     FLASH_RC_IRQHandler       ; 5:  Flash memory read collision
        DCD     VLD_IRQHandler            ; 6:  Low Voltage Detect, Low Voltage Warning
        DCD     LLWU_IRQHandler           ; 7:  Low Leakage Wakeup
        DCD     I2C0_IRQHandler           ; 8:  I2C0 interrupt
        DCD     I2C1_IRQHandler           ; 9:  I2C1 interrupt
        DCD     SPI0_IRQHandler           ;10:  SPI 0 interrupt
        DCD     SPI1_IRQHandler           ;11:  SPI 1 interrupt
        DCD     UART0_ERR_IRQHandler	  ;12:  UART 0 error intertrupt
        DCD     UART1_ERR_IRQHandler	  ;13:  UART 1 error intertrupt
        DCD     UART2_ERR_IRQHandler      ;14:  UART 2 error intertrupt
        DCD     ADC0_IRQHandler 	  ;15:  ADC 0 interrupt
        DCD     CMP0_IRQHandler		  ;16:  CMP 0 High-speed comparator interrupt
        DCD     FTM0_IRQHandler           ;17:  FTM 0 interrupt
        DCD     FTM1_IRQHandler           ;18:  FTM 1 interrupt
        DCD     FTM2_IRQHandler           ;19:  FTM 2 interrupt
        DCD     RTC_IRQHandler            ;20:  RTC intrrupt
        DCD     RTC_SE_IRQHandler         ;21:  RTC second interrupt
        DCD     PIT0_ISR            ;22:  PIT interrupt
        DCD     0                         ; Reserved
        DCD     USB_ISR                   ;24:  USB 0 interrupt
        DCD     DAC0_IRQHandler           ;25:  DAC 0 interrupt
        DCD     TSI0_IRQHandler           ;26:  TSI 0 interrupt
        DCD     MCG_IRQHandler         	  ;27:  MCG interrupt
        DCD     LPT_IRQHandler            ;28:  LPT interrupt
        DCD     0                         ; Reserved
        DCD     IRQ_ISR_PORTA          ;30:  PORT A interrupt
        DCD     PORTD_IRQHandler          ;31:  PORT D interrupt

__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors

; <h> Flash Configuration
;   <i> 16-byte flash configuration field that stores default protection settings (loaded on reset)
;   <i> and security information that allows the MCU to restrict acces to the FTFL module.
;   <h> Backdoor Comparison Key
;     <o0>  Backdoor Key 0  <0x0-0xFF:2>
;     <o1>  Backdoor Key 1  <0x0-0xFF:2>
;     <o2>  Backdoor Key 2  <0x0-0xFF:2>
;     <o3>  Backdoor Key 3  <0x0-0xFF:2>
;     <o4>  Backdoor Key 4  <0x0-0xFF:2>
;     <o5>  Backdoor Key 5  <0x0-0xFF:2>
;     <o6>  Backdoor Key 6  <0x0-0xFF:2>
;     <o7>  Backdoor Key 7  <0x0-0xFF:2>
BackDoorK0      EQU     0xFF
BackDoorK1      EQU     0xFF
BackDoorK2      EQU     0xFF
BackDoorK3      EQU     0xFF
BackDoorK4      EQU     0xFF
BackDoorK5      EQU     0xFF
BackDoorK6      EQU     0xFF
BackDoorK7      EQU     0xFF
;   </h>
;   <h> Program flash protection bytes (FPROT)
;     <i> Each program flash region can be protected from program and erase operation by setting the associated PROT bit.
;     <i> Each bit protects a 1/32 region of the program flash memory.
;     <h> FPROT0
;       <i> Program flash protection bytes
;       <i> 1/32 - 8/32 region
;       <o.0>   FPROT0.0
;       <o.1>   FPROT0.1
;       <o.2>   FPROT0.2
;       <o.3>   FPROT0.3
;       <o.4>   FPROT0.4
;       <o.5>   FPROT0.5
;       <o.6>   FPROT0.6
;       <o.7>   FPROT0.7
nFPROT0         EQU     0x00
FPROT0          EQU     nFPROT0:EOR:0xFF
;     </h>
;     <h> FPROT1
;       <i> Program Flash Region Protect Register 1
;       <i> 9/32 - 16/32 region
;       <o.0>   FPROT1.0
;       <o.1>   FPROT1.1
;       <o.2>   FPROT1.2
;       <o.3>   FPROT1.3
;       <o.4>   FPROT1.4
;       <o.5>   FPROT1.5
;       <o.6>   FPROT1.6
;       <o.7>   FPROT1.7
nFPROT1         EQU     0x00
FPROT1          EQU     nFPROT1:EOR:0xFF
;     </h>
;     <h> FPROT2
;       <i> Program Flash Region Protect Register 2
;       <i> 17/32 - 24/32 region
;       <o.0>   FPROT2.0
;       <o.1>   FPROT2.1
;       <o.2>   FPROT2.2
;       <o.3>   FPROT2.3
;       <o.4>   FPROT2.4
;       <o.5>   FPROT2.5
;       <o.6>   FPROT2.6
;       <o.7>   FPROT2.7
nFPROT2         EQU     0x00
FPROT2          EQU     nFPROT2:EOR:0xFF
;     </h>
;     <h> FPROT3
;       <i> Program Flash Region Protect Register 3
;       <i> 25/32 - 32/32 region
;       <o.0>   FPROT3.0
;       <o.1>   FPROT3.1
;       <o.2>   FPROT3.2
;       <o.3>   FPROT3.3
;       <o.4>   FPROT3.4
;       <o.5>   FPROT3.5
;       <o.6>   FPROT3.6
;       <o.7>   FPROT3.7
nFPROT3         EQU     0x00
FPROT3          EQU     nFPROT3:EOR:0xFF
;     </h>
;   </h>
;   <h> Data flash protection byte (FDPROT)
;     <i> Each bit protects a 1/8 region of the data flash memory.
;     <i> (Program flash only devices: Reserved)
;     <o.0>   FDPROT.0
;     <o.1>   FDPROT.1
;     <o.2>   FDPROT.2
;     <o.3>   FDPROT.3
;     <o.4>   FDPROT.4
;     <o.5>   FDPROT.5
;     <o.6>   FDPROT.6
;     <o.7>   FDPROT.7
nFDPROT         EQU     0x00
FDPROT          EQU     nFDPROT:EOR:0xFF
;   </h>
;   <h> EEPROM protection byte (FEPROT)
;     <i> FlexNVM devices: Each bit protects a 1/8 region of the EEPROM.
;     <i> (Program flash only devices: Reserved)
;     <o.0>   FEPROT.0
;     <o.1>   FEPROT.1
;     <o.2>   FEPROT.2
;     <o.3>   FEPROT.3
;     <o.4>   FEPROT.4
;     <o.5>   FEPROT.5
;     <o.6>   FEPROT.6
;     <o.7>   FEPROT.7
nFEPROT         EQU     0x00
FEPROT          EQU     nFEPROT:EOR:0xFF
;   </h>
;   <h> Flash nonvolatile option byte (FOPT)
;     <i> Allows the user to customize the operation of the MCU at boot time.
;     <o.0>  LPBOOT
;       <0=> Low-power boot
;       <1=> normal boot
;     <o.1>  EZPORT_DIS
;       <0=> EzPort operation is enabled
;       <1=> EzPort operation is disabled
FOPT            EQU     0xFF
;   </h>
;   <h> Flash security byte (FSEC)
;     <i> WARNING: If SEC field is configured as "MCU security status is secure" and MEEN field is configured as "Mass erase is disabled",
;     <i> MCU's security status cannot be set back to unsecure state since Mass erase via the debugger is blocked !!!
;     <o.0..1> SEC
;       <2=> MCU security status is unsecure
;       <3=> MCU security status is secure
;         <i> Flash Security
;         <i> This bits define the security state of the MCU.
;     <o.2..3> FSLACC
;       <2=> Freescale factory access denied
;       <3=> Freescale factory access granted
;         <i> Freescale Failure Analysis Access Code
;         <i> This bits define the security state of the MCU.
;     <o.4..5> MEEN
;       <2=> Mass erase is disabled
;       <3=> Mass erase is enabled
;         <i> Mass Erase Enable Bits
;         <i> Enables and disables mass erase capability of the FTFL module
;     <o.6..7> KEYEN
;       <2=> Backdoor key access enabled
;       <3=> Backdoor key access disabled
;         <i> Backdoor key Security Enable
;         <i> These bits enable and disable backdoor key access to the FTFL module.
FSEC            EQU     0xFE
;   </h>
; </h>
                IF      :LNOT::DEF:RAM_TARGET
                AREA    |.ARM.__at_0x400|, CODE, READONLY
                DCB     BackDoorK0, BackDoorK1, BackDoorK2, BackDoorK3
                DCB     BackDoorK4, BackDoorK5, BackDoorK6, BackDoorK7
                DCB     FPROT0,     FPROT1,     FPROT2,     FPROT3
                DCB     FSEC,       FOPT,       FEPROT,     FDPROT
                ENDIF

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
				LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  DMA0_IRQHandler     [WEAK]
                EXPORT  DMA1_IRQHandler     [WEAK]
                EXPORT  DMA2_IRQHandler     [WEAK]
                EXPORT  DMA3_IRQHandler     [WEAK]
                EXPORT  FLASH_RC_IRQHandler     [WEAK]
                EXPORT  VLD_IRQHandler     [WEAK]
                EXPORT  LLWU_IRQHandler     [WEAK]
                EXPORT  I2C0_IRQHandler     [WEAK]
                EXPORT  I2C1_IRQHandler     [WEAK]
                EXPORT  SPI0_IRQHandler     [WEAK]
                EXPORT  SPI1_IRQHandler     [WEAK]
                EXPORT  UART0_ERR_IRQHandler     [WEAK]
                EXPORT  UART1_ERR_IRQHandler     [WEAK]
                EXPORT  UART2_ERR_IRQHandler     [WEAK]
                EXPORT  ADC0_IRQHandler     [WEAK]
                EXPORT  CMP0_IRQHandler     [WEAK]
                EXPORT  FTM0_IRQHandler     [WEAK]
                EXPORT  FTM1_IRQHandler     [WEAK]
                EXPORT  FTM2_IRQHandler     [WEAK]
                EXPORT  PIT0_ISR     [WEAK]
                EXPORT  USB_ISR     [WEAK]
                EXPORT  DAC0_IRQHandler     [WEAK]
                EXPORT  TSI0_IRQHandler     [WEAK]
                EXPORT  MCG_IRQHandler     [WEAK]
                EXPORT  IRQ_ISR_PORTA     [WEAK]
                EXPORT  PORTD_IRQHandler     [WEAK]
                EXPORT  DefaultISR                      [WEAK]

DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
FLASH_RC_IRQHandler
VLD_IRQHandler
LLWU_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
UART0_ERR_IRQHandler
UART1_ERR_IRQHandler
UART2_ERR_IRQHandler
ADC0_IRQHandler
CMP0_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
FTM2_IRQHandler
RTC_IRQHandler
RTC_SE_IRQHandler
PIT0_ISR
USB_ISR
DAC0_IRQHandler
TSI0_IRQHandler
MCG_IRQHandler
LPT_IRQHandler
IRQ_ISR_PORTA
PORTD_IRQHandler
DefaultISR

                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
