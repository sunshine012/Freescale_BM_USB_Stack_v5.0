/**************************************************
 *
 * Copyright 2010 IAR Systems. All rights reserved.
 *
 * $Revision: #1 $
 *
 **************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup
        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:ROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  ___VECTOR_RAM
        PUBLIC  __vector_table
        EXTERN  PIT0_ISR
        EXTERN  PIT1_ISR
        EXTERN  I2S0_TX_ISR
        EXTERN  USB_ISR
        ;;EXTERN  IRQ_ISR_PORTC
        EXTERN  PORTA_ISR
        EXTERN  UART1_ISR

        DATA
___VECTOR_RAM
__vector_table
        DCD     sfe(CSTACK)               ; Top of Stack
        DCD     __iar_program_start       ; Reset Handler
        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler
        ; External Interrupts
        DCD     DMA0_IRQHandler           ; 0:  DMA Channel 0 transfer complete
        DCD     DMA1_IRQHandler           ; 1:  DMA Channel 1 transfer complete
        DCD     DMA2_IRQHandler           ; 2:  DMA Channel 2 transfer complete
        DCD     DMA3_IRQHandler           ; 3:  DMA Channel 3 transfer complete
        DCD     DMA4_IRQHandler           ; 4:  DMA Channel 4 transfer complete
        DCD     DMA5_IRQHandler           ; 5:  DMA Channel 5 transfer complete
        DCD     DMA6_IRQHandler           ; 6:  DMA Channel 6 transfer complete
        DCD     DMA7_IRQHandler           ; 7:  DMA Channel 7 transfer complete
        DCD     DMA8_IRQHandler           ; 8:  DMA Channel 8 transfer complete
        DCD     DMA9_IRQHandler           ; 9:  DMA Channel 9 transfer complete
        DCD     DMA10_IRQHandler          ;10:  DMA Channel 10 transfer complete
        DCD     DMA11_IRQHandler          ;11:  DMA Channel 11 transfer complete
        DCD     DMA12_IRQHandler          ;12:  DMA Channel 12 transfer complete
        DCD     DMA13_IRQHandler          ;13:  DMA Channel 13 transfer complete
        DCD     DMA14_IRQHandler          ;14:  DMA Channel 14 transfer complete
        DCD     DMA15_IRQHandler          ;15:  DMA Channel 15 transfer complete
        DCD     DMA_ERR_IRQHandler        ;16:  DMA Error Interrupt Channels 0-31
        DCD     MCM_IRQHandler            ;17:  MCM Normal interrupt
        DCD     FLASH_CC_IRQHandler       ;18:  Flash memory command complete
        DCD     FLASH_RC_IRQHandler       ;19:  Flash memory read collision
        DCD     VLD_IRQHandler            ;20:  Low Voltage Detect, Low Voltage Warning
        DCD     LLWU_IRQHandler           ;21:  Low Leakage Wakeup
        DCD     WDOG_IRQHandler           ;22:  WDOG interrupt
        DCD     RNG_IRQHandler            ;23:  Random Number Generator
        DCD     I2C0_IRQHandler           ;24:  I2C0 interrupt
        DCD     I2C1_IRQHandler           ;25:  I2C1 interrupt
        DCD     SPI0_IRQHandler           ;26:  SPI 0 interrupt
        DCD     SPI1_IRQHandler           ;27:  SPI 1 interrupt
        DCD     I2S0_TX_ISR               ;28:
        DCD     I2S0_Receive              ;29:
        DCD     0                         ;30:  Reserved
        DCD     UART0_IRQHandler          ;31:  UART 0 intertrupt
        DCD     UART0_ERR_IRQHandler      ;32:  UART 0 error intertrupt
        DCD     UART1_ISR                 ;33:  UART 1 intertrupt
        DCD     UART1_ERR_IRQHandler      ;34:  UART 1 error intertrupt
        DCD     UART2_IRQHandler          ;35:  UART 2 intertrupt
        DCD     UART2_ERR_IRQHandler      ;36:  UART 2 error intertrupt
        DCD     UART3_IRQHandler          ;37:  UART 3 intertrupt
        DCD     UART3_ERR_IRQHandler      ;38:  UART 3 error intertrupt
        DCD     ADC0_IRQHandler           ;39:  ADC 0 interrupt
        DCD     CMP0_IRQHandler           ;40:  CMP 0 High-speed comparator interrupt
        DCD     CMP1_IRQHandler           ;41:  CMP 1 interrupt
        DCD     FTM0_IRQHandler           ;42:  FTM 0 interrupt
        DCD     FTM1_IRQHandler           ;43:  FTM 1 interrupt
        DCD     FTM2_IRQHandler           ;44:  FTM 2 interrupt
        DCD     0                         ;45:  reserved
        DCD     RTC_alarm_IRQHandler      ;46:  RTC interrupt
        DCD     RTC_seconds_IRQHandler    ;47:  RTC interrupt
        DCD     PIT0_ISR                  ;48:  PIT 0 interrupt
        DCD     PIT1_ISR                  ;49:  PIT 1 interrupt
        DCD     PIT2_IRQHandler           ;50:  PIT 2 interrupt
        DCD     PIT3_IRQHandler           ;51:  PIT 3 interrupt
        DCD     PDB_IRQHandler            ;52:  PDB interrupt
        DCD     USB_ISR                   ;53:  USB OTG interrupt
        DCD     0                         ;54:  reserved
        DCD     0                         ;55:  reserved
        DCD     DAC0_IRQHandler           ;56:  DAC 0 interrupt
        DCD     MCG_IRQHandler            ;57:  MCG interrupt
        DCD     LPT_IRQHandler            ;58:  LPT interrupt
        DCD     PORTA_ISR                 ;59:  PORT A interrupt
        DCD     PORTB_IRQHandler          ;60:  PORT B interrupt
        ;DCD     IRQ_ISR_PORTC             ;61:  PORT C interrupt
        DCD     0                         ;61:  PORT C interrupt
        DCD     IRQ_ISR_PORTD             ;62:  PORT D interrupt
        DCD     IRQ_ISR_PORTE             ;63:  PORT E interrupt
        DCD     Software_IRQHandler       ;64:
        DCD     0                         ;65:  reserved
        DCD     0                         ;66:  reserved
        DCD     0                         ;67:  reserved
        DCD     0                         ;68:  reserved
        DCD     0                         ;69:  reserved
        DCD     0                         ;70:  reserved
        DCD     FTM3_IRQHandler           ;71:
        DCD     DAC1_IRQHandler           ;72:
        DCD     ADC1_IRQHandler           ;73:
        DCD     0                         ;74:  reserved
        DCD     0                         ;75:  reserved
        DCD     0                         ;76:  reserved
        DCD     0                         ;77:  reserved
        DCD     0                         ;78:  reserved
        DCD     0                         ;79:  reserved
        DCD     0                         ;80:  reserved

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
      PUBWEAK NMI_Handler
      PUBWEAK HardFault_Handler
      PUBWEAK MemManage_Handler
      PUBWEAK BusFault_Handler
      PUBWEAK UsageFault_Handler
      PUBWEAK SVC_Handler
      PUBWEAK DebugMon_Handler
      PUBWEAK PendSV_Handler
      PUBWEAK SysTick_Handler
      PUBWEAK DMA0_IRQHandler
      PUBWEAK DMA1_IRQHandler
      PUBWEAK DMA2_IRQHandler
      PUBWEAK DMA3_IRQHandler
      PUBWEAK DMA4_IRQHandler
      PUBWEAK DMA5_IRQHandler
      PUBWEAK DMA6_IRQHandler
      PUBWEAK DMA7_IRQHandler
      PUBWEAK DMA8_IRQHandler
      PUBWEAK DMA9_IRQHandler
      PUBWEAK DMA10_IRQHandler
      PUBWEAK DMA11_IRQHandler
      PUBWEAK DMA12_IRQHandler
      PUBWEAK DMA13_IRQHandler
      PUBWEAK DMA14_IRQHandler
      PUBWEAK DMA15_IRQHandler
      PUBWEAK DMA_ERR_IRQHandler
      PUBWEAK MCM_IRQHandler
      PUBWEAK FLASH_CC_IRQHandler
      PUBWEAK FLASH_RC_IRQHandler
      PUBWEAK VLD_IRQHandler
      PUBWEAK LLWU_IRQHandler
      PUBWEAK WDOG_IRQHandler
      PUBWEAK RNG_IRQHandler
      PUBWEAK I2C0_IRQHandler
      PUBWEAK I2C1_IRQHandler
      PUBWEAK SPI0_IRQHandler
      PUBWEAK SPI1_IRQHandler
      PUBWEAK I2S0_Transmit
      PUBWEAK I2S0_Receive
      PUBWEAK UART0_IRQHandler
      PUBWEAK UART0_ERR_IRQHandler
      PUBWEAK UART1_IRQHandler
      PUBWEAK UART1_ERR_IRQHandler
      PUBWEAK UART2_IRQHandler
      PUBWEAK UART2_ERR_IRQHandler
      PUBWEAK UART3_IRQHandler
      PUBWEAK UART3_ERR_IRQHandler
      PUBWEAK ADC0_IRQHandler
      PUBWEAK CMP0_IRQHandler
      PUBWEAK CMP1_IRQHandler
      PUBWEAK FTM0_IRQHandler
      PUBWEAK FTM1_IRQHandler
      PUBWEAK FTM2_IRQHandler
      PUBWEAK RTC_alarm_IRQHandler
      PUBWEAK RTC_seconds_IRQHandler
      PUBWEAK PIT2_IRQHandler
      PUBWEAK PIT3_IRQHandler
      PUBWEAK PDB_IRQHandler
      PUBWEAK DAC0_IRQHandler
      PUBWEAK MCG_IRQHandler
      PUBWEAK LPT_IRQHandler
      PUBWEAK IRQ_ISR_PORTA
      PUBWEAK PORTB_IRQHandler
      PUBWEAK IRQ_ISR_PORTD
      PUBWEAK IRQ_ISR_PORTE
      PUBWEAK Software_IRQHandler
      PUBWEAK FTM3_IRQHandler
      PUBWEAK DAC1_IRQHandler
      PUBWEAK ADC1_IRQHandler

      SECTION .text:CODE:REORDER(1)
      THUMB
NMI_Handler
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
DMA4_IRQHandler
DMA5_IRQHandler
DMA6_IRQHandler
DMA7_IRQHandler
DMA8_IRQHandler
DMA9_IRQHandler
DMA10_IRQHandler
DMA11_IRQHandler
DMA12_IRQHandler
DMA13_IRQHandler
DMA14_IRQHandler
DMA15_IRQHandler
DMA_ERR_IRQHandler
MCM_IRQHandler
FLASH_CC_IRQHandler
FLASH_RC_IRQHandler
VLD_IRQHandler
LLWU_IRQHandler
WDOG_IRQHandler
RNG_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
I2S0_Transmit
I2S0_Receive
UART0_IRQHandler
UART0_ERR_IRQHandler
UART1_IRQHandler
UART1_ERR_IRQHandler
UART2_IRQHandler
UART2_ERR_IRQHandler
UART3_IRQHandler
UART3_ERR_IRQHandler
ADC0_IRQHandler
CMP0_IRQHandler
CMP1_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
FTM2_IRQHandler
RTC_alarm_IRQHandler
RTC_seconds_IRQHandler
PIT2_IRQHandler
PIT3_IRQHandler
PDB_IRQHandler
DAC0_IRQHandler
MCG_IRQHandler
LPT_IRQHandler
IRQ_ISR_PORTA
PORTB_IRQHandler
IRQ_ISR_PORTD
IRQ_ISR_PORTE
Software_IRQHandler
FTM3_IRQHandler
DAC1_IRQHandler
ADC1_IRQHandler
Default_Handler

        B Default_Handler
        END
