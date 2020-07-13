/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL FREESCALE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */
/*
 * WARNING! DO NOT EDIT THIS FILE DIRECTLY!
 *
 * This file was generated automatically and any changes may be lost.
 */
#ifndef __HW_GPIO_REGISTERS_H__
#define __HW_GPIO_REGISTERS_H__

#include "regs.h"

/*
 * MK70F12 GPIO
 *
 * General Purpose Input/Output
 *
 * Registers defined in this header file:
 * - HW_GPIO_PDOR - Port Data Output Register
 * - HW_GPIO_PSOR - Port Set Output Register
 * - HW_GPIO_PCOR - Port Clear Output Register
 * - HW_GPIO_PTOR - Port Toggle Output Register
 * - HW_GPIO_PDIR - Port Data Input Register
 * - HW_GPIO_PDDR - Port Data Direction Register
 *
 * - hw_gpio_t - Struct containing all module registers.
 */

//! @name Module base addresses
//@{
#ifndef REGS_GPIO_BASE
#define HW_GPIO_INSTANCE_COUNT (6U) //!< Number of instances of the GPIO module.
#define HW_GPIOA (0U) //!< Instance number for GPIOA.
#define HW_GPIOB (1U) //!< Instance number for GPIOB.
#define HW_GPIOC (2U) //!< Instance number for GPIOC.
#define HW_GPIOD (3U) //!< Instance number for GPIOD.
#define HW_GPIOE (4U) //!< Instance number for GPIOE.
#define HW_GPIOF (5U) //!< Instance number for GPIOF.
#define REGS_GPIOA_BASE (0x400FF000U) //!< Base address for GPIOA.
#define REGS_GPIOB_BASE (0x400FF040U) //!< Base address for GPIOB.
#define REGS_GPIOC_BASE (0x400FF080U) //!< Base address for GPIOC.
#define REGS_GPIOD_BASE (0x400FF0C0U) //!< Base address for GPIOD.
#define REGS_GPIOE_BASE (0x400FF100U) //!< Base address for GPIOE.
#define REGS_GPIOF_BASE (0x400FF140U) //!< Base address for GPIOF.

//! @brief Table of base addresses for GPIO instances.
static const uint32_t __g_regs_GPIO_base_addresses[] = {
        REGS_GPIOA_BASE,
        REGS_GPIOB_BASE,
        REGS_GPIOC_BASE,
        REGS_GPIOD_BASE,
        REGS_GPIOE_BASE,
        REGS_GPIOF_BASE,
    };

//! @brief Get the base address of GPIO by instance number.
//! @param x GPIO instance number, from 0 through 5.
#define REGS_GPIO_BASE(x) (__g_regs_GPIO_base_addresses[(x)])

//! @brief Get the instance number given a base address.
//! @param b Base address for an instance of GPIO.
#define REGS_GPIO_INSTANCE(b) ((b) == REGS_GPIOA_BASE ? HW_GPIOA : (b) == REGS_GPIOB_BASE ? HW_GPIOB : (b) == REGS_GPIOC_BASE ? HW_GPIOC : (b) == REGS_GPIOD_BASE ? HW_GPIOD : (b) == REGS_GPIOE_BASE ? HW_GPIOE : (b) == REGS_GPIOF_BASE ? HW_GPIOF : 0)
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_GPIO_PDOR - Port Data Output Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_GPIO_PDOR - Port Data Output Register (RW)
 *
 * Reset value: 0x00000000U
 *
 * This register configures the logic levels that are driven on each
 * general-purpose output pins.
 */
typedef union _hw_gpio_pdor
{
    uint32_t U;
    struct _hw_gpio_pdor_bitfields
    {
        uint32_t PDO : 32;             //!< [31:0] Port Data Output
    } B;
} hw_gpio_pdor_t;
#endif

/*!
 * @name Constants and macros for entire GPIO_PDOR register
 */
//@{
#define HW_GPIO_PDOR_ADDR(x)     (REGS_GPIO_BASE(x) + 0x0U)

#ifndef __LANGUAGE_ASM__
#define HW_GPIO_PDOR(x)          (*(__IO hw_gpio_pdor_t *) HW_GPIO_PDOR_ADDR(x))
#define HW_GPIO_PDOR_RD(x)       (HW_GPIO_PDOR(x).U)
#define HW_GPIO_PDOR_WR(x, v)    (HW_GPIO_PDOR(x).U = (v))
#define HW_GPIO_PDOR_SET(x, v)   (HW_GPIO_PDOR_WR(x, HW_GPIO_PDOR_RD(x) |  (v)))
#define HW_GPIO_PDOR_CLR(x, v)   (HW_GPIO_PDOR_WR(x, HW_GPIO_PDOR_RD(x) & ~(v)))
#define HW_GPIO_PDOR_TOG(x, v)   (HW_GPIO_PDOR_WR(x, HW_GPIO_PDOR_RD(x) ^  (v)))
#endif
//@}

/*
 * Constants & macros for individual GPIO_PDOR bitfields
 */

/*!
 * @name Register GPIO_PDOR, field PDO[31:0] (RW)
 *
 * Unimplemented pins for a particular device read as zero.
 *
 * Values:
 * - 0 - Logic level 0 is driven on pin, provided pin is configured for
 *     general-purpose output.
 * - 1 - Logic level 1 is driven on pin, provided pin is configured for
 *     general-purpose output.
 */
//@{
#define BP_GPIO_PDOR_PDO     (0U)          //!< Bit position for GPIO_PDOR_PDO.
#define BM_GPIO_PDOR_PDO     (0xFFFFFFFFU) //!< Bit mask for GPIO_PDOR_PDO.
#define BS_GPIO_PDOR_PDO     (32U)         //!< Bit field size in bits for GPIO_PDOR_PDO.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the GPIO_PDOR_PDO field.
#define BR_GPIO_PDOR_PDO(x)  (HW_GPIO_PDOR(x).U)
#endif

//! @brief Format value for bitfield GPIO_PDOR_PDO.
#define BF_GPIO_PDOR_PDO(v)  (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_GPIO_PDOR_PDO), uint32_t) & BM_GPIO_PDOR_PDO)

#ifndef __LANGUAGE_ASM__
//! @brief Set the PDO field to a new value.
#define BW_GPIO_PDOR_PDO(x, v) (HW_GPIO_PDOR_WR(x, v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_GPIO_PSOR - Port Set Output Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_GPIO_PSOR - Port Set Output Register (WORZ)
 *
 * Reset value: 0x00000000U
 *
 * This register configures whether to set the fields of the PDOR.
 */
typedef union _hw_gpio_psor
{
    uint32_t U;
    struct _hw_gpio_psor_bitfields
    {
        uint32_t PTSO : 32;            //!< [31:0] Port Set Output
    } B;
} hw_gpio_psor_t;
#endif

/*!
 * @name Constants and macros for entire GPIO_PSOR register
 */
//@{
#define HW_GPIO_PSOR_ADDR(x)     (REGS_GPIO_BASE(x) + 0x4U)

#ifndef __LANGUAGE_ASM__
#define HW_GPIO_PSOR(x)          (*(__O hw_gpio_psor_t *) HW_GPIO_PSOR_ADDR(x))
#define HW_GPIO_PSOR_RD(x)       (HW_GPIO_PSOR(x).U)
#define HW_GPIO_PSOR_WR(x, v)    (HW_GPIO_PSOR(x).U = (v))
#endif
//@}

/*
 * Constants & macros for individual GPIO_PSOR bitfields
 */

/*!
 * @name Register GPIO_PSOR, field PTSO[31:0] (WORZ)
 *
 * Writing to this register will update the contents of the corresponding bit in
 * the PDOR as follows:
 *
 * Values:
 * - 0 - Corresponding bit in PDORn does not change.
 * - 1 - Corresponding bit in PDORn is set to logic 1.
 */
//@{
#define BP_GPIO_PSOR_PTSO    (0U)          //!< Bit position for GPIO_PSOR_PTSO.
#define BM_GPIO_PSOR_PTSO    (0xFFFFFFFFU) //!< Bit mask for GPIO_PSOR_PTSO.
#define BS_GPIO_PSOR_PTSO    (32U)         //!< Bit field size in bits for GPIO_PSOR_PTSO.

//! @brief Format value for bitfield GPIO_PSOR_PTSO.
#define BF_GPIO_PSOR_PTSO(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_GPIO_PSOR_PTSO), uint32_t) & BM_GPIO_PSOR_PTSO)

#ifndef __LANGUAGE_ASM__
//! @brief Set the PTSO field to a new value.
#define BW_GPIO_PSOR_PTSO(x, v) (HW_GPIO_PSOR_WR(x, v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_GPIO_PCOR - Port Clear Output Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_GPIO_PCOR - Port Clear Output Register (WORZ)
 *
 * Reset value: 0x00000000U
 *
 * This register configures whether to clear the fields of PDOR.
 */
typedef union _hw_gpio_pcor
{
    uint32_t U;
    struct _hw_gpio_pcor_bitfields
    {
        uint32_t PTCO : 32;            //!< [31:0] Port Clear Output
    } B;
} hw_gpio_pcor_t;
#endif

/*!
 * @name Constants and macros for entire GPIO_PCOR register
 */
//@{
#define HW_GPIO_PCOR_ADDR(x)     (REGS_GPIO_BASE(x) + 0x8U)

#ifndef __LANGUAGE_ASM__
#define HW_GPIO_PCOR(x)          (*(__O hw_gpio_pcor_t *) HW_GPIO_PCOR_ADDR(x))
#define HW_GPIO_PCOR_RD(x)       (HW_GPIO_PCOR(x).U)
#define HW_GPIO_PCOR_WR(x, v)    (HW_GPIO_PCOR(x).U = (v))
#endif
//@}

/*
 * Constants & macros for individual GPIO_PCOR bitfields
 */

/*!
 * @name Register GPIO_PCOR, field PTCO[31:0] (WORZ)
 *
 * Writing to this register will update the contents of the corresponding bit in
 * the Port Data Output Register (PDOR) as follows:
 *
 * Values:
 * - 0 - Corresponding bit in PDORn does not change.
 * - 1 - Corresponding bit in PDORn is cleared to logic 0.
 */
//@{
#define BP_GPIO_PCOR_PTCO    (0U)          //!< Bit position for GPIO_PCOR_PTCO.
#define BM_GPIO_PCOR_PTCO    (0xFFFFFFFFU) //!< Bit mask for GPIO_PCOR_PTCO.
#define BS_GPIO_PCOR_PTCO    (32U)         //!< Bit field size in bits for GPIO_PCOR_PTCO.

//! @brief Format value for bitfield GPIO_PCOR_PTCO.
#define BF_GPIO_PCOR_PTCO(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_GPIO_PCOR_PTCO), uint32_t) & BM_GPIO_PCOR_PTCO)

#ifndef __LANGUAGE_ASM__
//! @brief Set the PTCO field to a new value.
#define BW_GPIO_PCOR_PTCO(x, v) (HW_GPIO_PCOR_WR(x, v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_GPIO_PTOR - Port Toggle Output Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_GPIO_PTOR - Port Toggle Output Register (WORZ)
 *
 * Reset value: 0x00000000U
 */
typedef union _hw_gpio_ptor
{
    uint32_t U;
    struct _hw_gpio_ptor_bitfields
    {
        uint32_t PTTO : 32;            //!< [31:0] Port Toggle Output
    } B;
} hw_gpio_ptor_t;
#endif

/*!
 * @name Constants and macros for entire GPIO_PTOR register
 */
//@{
#define HW_GPIO_PTOR_ADDR(x)     (REGS_GPIO_BASE(x) + 0xCU)

#ifndef __LANGUAGE_ASM__
#define HW_GPIO_PTOR(x)          (*(__O hw_gpio_ptor_t *) HW_GPIO_PTOR_ADDR(x))
#define HW_GPIO_PTOR_RD(x)       (HW_GPIO_PTOR(x).U)
#define HW_GPIO_PTOR_WR(x, v)    (HW_GPIO_PTOR(x).U = (v))
#endif
//@}

/*
 * Constants & macros for individual GPIO_PTOR bitfields
 */

/*!
 * @name Register GPIO_PTOR, field PTTO[31:0] (WORZ)
 *
 * Writing to this register will update the contents of the corresponding bit in
 * the PDOR as follows:
 *
 * Values:
 * - 0 - Corresponding bit in PDORn does not change.
 * - 1 - Corresponding bit in PDORn is set to the inverse of its existing logic
 *     state.
 */
//@{
#define BP_GPIO_PTOR_PTTO    (0U)          //!< Bit position for GPIO_PTOR_PTTO.
#define BM_GPIO_PTOR_PTTO    (0xFFFFFFFFU) //!< Bit mask for GPIO_PTOR_PTTO.
#define BS_GPIO_PTOR_PTTO    (32U)         //!< Bit field size in bits for GPIO_PTOR_PTTO.

//! @brief Format value for bitfield GPIO_PTOR_PTTO.
#define BF_GPIO_PTOR_PTTO(v) (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_GPIO_PTOR_PTTO), uint32_t) & BM_GPIO_PTOR_PTTO)

#ifndef __LANGUAGE_ASM__
//! @brief Set the PTTO field to a new value.
#define BW_GPIO_PTOR_PTTO(x, v) (HW_GPIO_PTOR_WR(x, v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_GPIO_PDIR - Port Data Input Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_GPIO_PDIR - Port Data Input Register (RO)
 *
 * Reset value: 0x00000000U
 */
typedef union _hw_gpio_pdir
{
    uint32_t U;
    struct _hw_gpio_pdir_bitfields
    {
        uint32_t PDI : 32;             //!< [31:0] Port Data Input
    } B;
} hw_gpio_pdir_t;
#endif

/*!
 * @name Constants and macros for entire GPIO_PDIR register
 */
//@{
#define HW_GPIO_PDIR_ADDR(x)     (REGS_GPIO_BASE(x) + 0x10U)

#ifndef __LANGUAGE_ASM__
#define HW_GPIO_PDIR(x)          (*(__I hw_gpio_pdir_t *) HW_GPIO_PDIR_ADDR(x))
#define HW_GPIO_PDIR_RD(x)       (HW_GPIO_PDIR(x).U)
#endif
//@}

/*
 * Constants & macros for individual GPIO_PDIR bitfields
 */

/*!
 * @name Register GPIO_PDIR, field PDI[31:0] (RO)
 *
 * Reads 0 at the unimplemented pins for a particular device. Pins that are not
 * configured for a digital function read 0. If the Port Control and Interrupt
 * module is disabled, then the corresponding bit in PDIR does not update.
 *
 * Values:
 * - 0 - Pin logic level is logic 0 or, is not configured for use by digital
 *     function.
 * - 1 - Pin logic level is logic 1.
 */
//@{
#define BP_GPIO_PDIR_PDI     (0U)          //!< Bit position for GPIO_PDIR_PDI.
#define BM_GPIO_PDIR_PDI     (0xFFFFFFFFU) //!< Bit mask for GPIO_PDIR_PDI.
#define BS_GPIO_PDIR_PDI     (32U)         //!< Bit field size in bits for GPIO_PDIR_PDI.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the GPIO_PDIR_PDI field.
#define BR_GPIO_PDIR_PDI(x)  (HW_GPIO_PDIR(x).U)
#endif
//@}

//-------------------------------------------------------------------------------------------
// HW_GPIO_PDDR - Port Data Direction Register
//-------------------------------------------------------------------------------------------

#ifndef __LANGUAGE_ASM__
/*!
 * @brief HW_GPIO_PDDR - Port Data Direction Register (RW)
 *
 * Reset value: 0x00000000U
 *
 * The PDDR configures the individual port pins for input or output.
 */
typedef union _hw_gpio_pddr
{
    uint32_t U;
    struct _hw_gpio_pddr_bitfields
    {
        uint32_t PDD : 32;             //!< [31:0] Port data direction
    } B;
} hw_gpio_pddr_t;
#endif

/*!
 * @name Constants and macros for entire GPIO_PDDR register
 */
//@{
#define HW_GPIO_PDDR_ADDR(x)     (REGS_GPIO_BASE(x) + 0x14U)

#ifndef __LANGUAGE_ASM__
#define HW_GPIO_PDDR(x)          (*(__IO hw_gpio_pddr_t *) HW_GPIO_PDDR_ADDR(x))
#define HW_GPIO_PDDR_RD(x)       (HW_GPIO_PDDR(x).U)
#define HW_GPIO_PDDR_WR(x, v)    (HW_GPIO_PDDR(x).U = (v))
#define HW_GPIO_PDDR_SET(x, v)   (HW_GPIO_PDDR_WR(x, HW_GPIO_PDDR_RD(x) |  (v)))
#define HW_GPIO_PDDR_CLR(x, v)   (HW_GPIO_PDDR_WR(x, HW_GPIO_PDDR_RD(x) & ~(v)))
#define HW_GPIO_PDDR_TOG(x, v)   (HW_GPIO_PDDR_WR(x, HW_GPIO_PDDR_RD(x) ^  (v)))
#endif
//@}

/*
 * Constants & macros for individual GPIO_PDDR bitfields
 */

/*!
 * @name Register GPIO_PDDR, field PDD[31:0] (RW)
 *
 * Values:
 * - 0 - Pin is configured as general-purpose input, for the GPIO function.
 * - 1 - Pin is configured for general-purpose output, for the GPIO function.
 */
//@{
#define BP_GPIO_PDDR_PDD     (0U)          //!< Bit position for GPIO_PDDR_PDD.
#define BM_GPIO_PDDR_PDD     (0xFFFFFFFFU) //!< Bit mask for GPIO_PDDR_PDD.
#define BS_GPIO_PDDR_PDD     (32U)         //!< Bit field size in bits for GPIO_PDDR_PDD.

#ifndef __LANGUAGE_ASM__
//! @brief Read current value of the GPIO_PDDR_PDD field.
#define BR_GPIO_PDDR_PDD(x)  (HW_GPIO_PDDR(x).U)
#endif

//! @brief Format value for bitfield GPIO_PDDR_PDD.
#define BF_GPIO_PDDR_PDD(v)  (__REG_VALUE_TYPE((__REG_VALUE_TYPE((v), uint32_t) << BP_GPIO_PDDR_PDD), uint32_t) & BM_GPIO_PDDR_PDD)

#ifndef __LANGUAGE_ASM__
//! @brief Set the PDD field to a new value.
#define BW_GPIO_PDDR_PDD(x, v) (HW_GPIO_PDDR_WR(x, v))
#endif
//@}

//-------------------------------------------------------------------------------------------
// hw_gpio_t - module struct
//-------------------------------------------------------------------------------------------
/*!
 * @brief All GPIO module registers.
 */
#ifndef __LANGUAGE_ASM__
#pragma pack(1)
typedef struct _hw_gpio
{
    __IO hw_gpio_pdor_t PDOR;              //!< [0x0] Port Data Output Register
    __O hw_gpio_psor_t PSOR;               //!< [0x4] Port Set Output Register
    __O hw_gpio_pcor_t PCOR;               //!< [0x8] Port Clear Output Register
    __O hw_gpio_ptor_t PTOR;               //!< [0xC] Port Toggle Output Register
    __I hw_gpio_pdir_t PDIR;               //!< [0x10] Port Data Input Register
    __IO hw_gpio_pddr_t PDDR;              //!< [0x14] Port Data Direction Register
} hw_gpio_t;
#pragma pack()

//! @brief Macro to access all GPIO registers.
//! @param x GPIO instance number.
//! @return Reference (not a pointer) to the registers struct. To get a pointer to the struct,
//!     use the '&' operator, like <code>&HW_GPIO(0)</code>.
#define HW_GPIO(x)     (*(hw_gpio_t *) REGS_GPIO_BASE(x))
#endif

#endif // __HW_GPIO_REGISTERS_H__
// v22/130726/0.9
// EOF
