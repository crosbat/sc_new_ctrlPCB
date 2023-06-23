/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


//*****************************************************************************
//
// Check if compiler is GNU Compiler
//
//*****************************************************************************
#if !(defined(__GNUC__))
#error "startup_cc13x2_cc26x2_gcc.c: Unsupported compiler!"
#endif

#include <string.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/setup.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)

#include "project-conf.h"
#if BOARD_CONF_ENABLE_LOGS_ON_UART1
    #include "uart1-arch.h"
#else
    #include "uart0-arch.h"
#endif
#ifndef RF_TEST_APP_WD_DEBUG_DISABLE
#include "sf_persistentDataStorage.h"
#endif

//*****************************************************************************
//
// Macro for weak symbol aliasing
//
//*****************************************************************************
#define WEAK_ALIAS(x) __attribute__ ((weak, alias(#x)))

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void resetISR(void);
static void nmiISR(void);
static void faultISR(void);
static void intDefaultHandler( void );

// Default interrupt handlers
void MPUFaultIntHandler(void) WEAK_ALIAS(intDefaultHandler);
void BusFaultIntHandler(void) WEAK_ALIAS(intDefaultHandler);
void UsageFaultIntHandler(void) WEAK_ALIAS(intDefaultHandler);
void SVCallIntHandler(void) WEAK_ALIAS(intDefaultHandler);
void DebugMonIntHandler(void) WEAK_ALIAS(intDefaultHandler);
void PendSVIntHandler(void) WEAK_ALIAS(intDefaultHandler);
void SysTickIntHandler(void) WEAK_ALIAS(intDefaultHandler);

//*****************************************************************************
//
// External declaration for the reset handler that is to be called when the
// processor is started
//
//*****************************************************************************
extern void _c_int00(void);

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern uint32_t __bss_start__, __bss_end__;
extern uint32_t __data_load__, __data_start__, __data_end__;

//*****************************************************************************
//
// linker variable that marks the top of stack.
//
//*****************************************************************************
extern unsigned long _stack_end;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".resetVecs"))) __attribute__ ((used))
static void (* const resetVectors[])(void) =
{
    (void (*)(void))((uint32_t)&_stack_end),
                                         // The initial stack pointer
    resetISR,                            // The reset handler
    nmiISR,                              // The NMI handler
    faultISR,                            // The hard fault handler
    MPUFaultIntHandler,                     //  4 Memory Management (MemManage) Fault
    BusFaultIntHandler,                     //  5 The bus fault handler
    UsageFaultIntHandler,                   //  6 The usage fault handler
    0,                                      //  7 Reserved
    0,                                      //  8 Reserved
    0,                                      //  9 Reserved
    0,                                      // 10 Reserved
    SVCallIntHandler,                       // 11 Supervisor Call (SVCall)
    DebugMonIntHandler,                     // 12 Debug monitor handler
    0,                                      // 13 Reserved
    PendSVIntHandler,                       // 14 The PendSV handler
    SysTickIntHandler,                      // 15 The SysTick handler
};

//*****************************************************************************
//
// The following global variable is required for C++ support.
//
//*****************************************************************************
void * __dso_handle = (void *) &__dso_handle;

//*****************************************************************************
//
// Initialize the .data and .bss sections and copy the first 16 vectors from
// the read-only/reset table to the runtime RAM table. Fill the remaining
// vectors with a stub. This vector table will be updated at runtime.
//
//*****************************************************************************
//
void localProgramStart(void)
{
    uint32_t *pSrc;
    uint32_t *pDest;

    //
    // Final trim of device
    //
    SetupTrimDevice();

    //
    // Copy the data segment initializers from FLASH to SRAM.
    //
    pSrc = &__data_load__;
    for(pDest = &__data_start__; pDest < &__data_end__; )
    {
        *pDest++ = *pSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =__bss_start__\n"
          "    ldr     r1, =__bss_end__\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    //
    // Enable the FPU
    // CPACR is located at address 0xE000ED88
    // Set bits 20-23 in CPACR to enable CP10 and CP11 coprocessors
    //
    __asm("    ldr.w   r0, =0xE000ED88\n"
          "    ldr     r1, [r0]\n"
          "    orr     r1, r1, #(0xF << 20)\n"
          "    str     r1, [r0]\n");

    //
    // Call the application's entry point.
    //
    main();

    /* If we ever return signal Error */
    faultISR();
}

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void __attribute__((naked)) resetISR(void)
{
    __asm__ __volatile__ (
        " movw r0, #:lower16:resetVectors\n"
        " movt r0, #:upper16:resetVectors\n"
        " ldr r0, [r0]\n"
        " mov sp, r0\n"
        " bl localProgramStart"
    );
}

/*---------------------------------------------------------------------------*/
/*
 * \brief     Debug stack pointer.
 * \param sp  Stack pointer.
 *
 * Provide a view into the CPU state from the provided stack pointer.
 */
static void
debugWatchdogInt(uint32_t *sp)
{
#ifndef RF_TEST_APP_WD_DEBUG_DISABLE
    /*
    * (R0-R3, R12): Reconstruct the condition of the instruction at the
                    time the instruction executed.
    * Program Counter (PC): Points to the offending instruction.
    * Link Register (LR): Provides some context about what the program
                            was doing when the fault occurred. It tells
                            more about the calling function.
    */
    volatile uint32_t r0;  /**< R0 register */
    volatile uint32_t r1;  /**< R1 register */
    volatile uint32_t r2;  /**< R2 register */
    volatile uint32_t r3;  /**< R3 register */
    volatile uint32_t r12; /**< R12 register */
    volatile uint32_t lr;  /**< LR register */
    volatile uint32_t pc;  /**< PC register */

    (void)(r0  = sp[0]);
    (void)(r1  = sp[1]);
    (void)(r2  = sp[2]);
    (void)(r3  = sp[3]);
    (void)(r12 = sp[4]);
    (void)(lr  = sp[5]);
    (void)(pc  = sp[6]);

    /*
    | Bit | Description                                                            |
    |:--- |:---------------------------------------------------------------------- |
    | 7   | Valid fault address is stored in the Memory Management Address         |
    |     | Register (MMADR) at 0xE000ED34.                                        |
    | 6:5 | unused                                                                 |
    | 4   | Memory access violation occurred while stacking on exception entry.    |
    | 3   | Memory access violation occurred while unstacking on exception return. |
    | 2   | unused.                                                                |
    | 1   | Data access violation.                                                 |
    | 0   | Instruction access violation.                                          |
    */
    volatile uint32_t MMFSR_register = HWREG(0xE000ED28);
    /*
    | Bit | Description                                                 |
    |:--- |:----------------------------------------------------------- |
    | 7   | Valid fault address is stored in Bus Fault Address Register |
    |     | (FAULTADDR) at 0xE000ED38.                                  |
    | 6:5 | unused                                                      |
    | 4   | Bus fault occurred while stacking on exception entry        |
    | 3   | Bus fault occurred while unstacking on exception return.    |
    | 2   | Stacked PC does not indicate exact address of bus fault     |
    | 1   | Stacked PC indicates exact address of bus fault.            |
    | 0   | Instruction bus error.                                      |
    */
    volatile uint32_t BFSR_register = HWREG(0xE000ED29);
    /*
    | Bit | Description                                           |
    |:--- |:----------------------------------------------------- |
    | 7-4 | unused                                                |
    | 3   | Attempted to execute a coprocessor instruction.       |
    | 2   | Attempted exception return caused illegal load of PC. |
    | 1   | Invalid processing/instruction state.                 |
    | 0   | Attempt to execute an undefined instruction.          |
    */
    volatile uint32_t UFSR_register = HWREG(0xE000ED2A);

    uint8_t watchdogIntLog[33] = {0xFF};
    watchdogIntLog[0] = 0x57; // W
    watchdogIntLog[1] = 0x44; // D
    watchdogIntLog[2] = (uint8_t) MMFSR_register;
    watchdogIntLog[3] = (uint8_t) BFSR_register;
    watchdogIntLog[4] = (uint8_t) UFSR_register;
    watchdogIntLog[5] = (uint8_t) (lr >> 24U);
    watchdogIntLog[6] = (uint8_t) (lr >> 16U);
    watchdogIntLog[7] = (uint8_t) (lr >> 8U);
    watchdogIntLog[8] = (uint8_t) lr;
    watchdogIntLog[9] = (uint8_t) (pc >> 24U);
    watchdogIntLog[10] =(uint8_t) (pc >> 16U);
    watchdogIntLog[11] =(uint8_t) (pc >> 8U);
    watchdogIntLog[12] = (uint8_t) pc;
    watchdogIntLog[13] = (uint8_t) (r0 >> 24U);
    watchdogIntLog[14] = (uint8_t) (r0 >> 16U);
    watchdogIntLog[15] = (uint8_t) (r0 >> 8U);
    watchdogIntLog[16] = (uint8_t) r0;
    watchdogIntLog[17] = (uint8_t) (r1 >> 24U);
    watchdogIntLog[18] = (uint8_t) (r1 >> 16U);
    watchdogIntLog[19] = (uint8_t) (r1 >> 8U);
    watchdogIntLog[20] = (uint8_t) r1;
    watchdogIntLog[21] = (uint8_t) (r2 >> 24U);
    watchdogIntLog[22] = (uint8_t) (r2 >> 16U);
    watchdogIntLog[23] = (uint8_t) (r2 >> 8U);
    watchdogIntLog[24] = (uint8_t) r2;
    watchdogIntLog[25] = (uint8_t) (r3 >> 24U);
    watchdogIntLog[26] = (uint8_t) (r3 >> 16U);
    watchdogIntLog[27] = (uint8_t) (r3 >> 8U);
    watchdogIntLog[28] = (uint8_t) r3;
    watchdogIntLog[29] = (uint8_t) (r12 >> 24U);
    watchdogIntLog[30] = (uint8_t) (r12 >> 16U);
    watchdogIntLog[31] = (uint8_t) (r12 >> 8U);
    watchdogIntLog[32] = (uint8_t) r12;

    /*
    | FIELD | WD  | MMFSR | BFSR | UFSR | lr  | pc  | r0  | r1  | r2  | r3  | r12 |
    | ----- | --- | ----- | ---- | ---- | --- | --- | --- | --- | --- | --- | --- |
    | BYTES | 2   | 1     | 1    | 1    | 4   | 4   | 4   | 4   | 4   | 4   | 4   |
    */
//  #if BOARD_CONF_ENABLE_LOGS_ON_UART1
//      uart1_write(watchdogIntLog, sizeof(watchdogIntLog));
//  #else
//      uart0_write(watchdogIntLog, sizeof(watchdogIntLog));
//  #endif
// _eraseSensorList();
    _write(0x00002000, watchdogIntLog, sizeof(watchdogIntLog));
#endif

    /* Enter an infinite loop. */
    for(;;) { /* hang */ }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
nmiISR(void)
{
    __asm__ __volatile__
    (
        "tst lr, #4        \n"
        "ite eq            \n"
        "mrseq r0, msp     \n"
        "mrsne r0, psp     \n"
        "bx %0             \n"
        : /* output */
        : /* input */
        "r"(debugWatchdogInt)
    );
}

/*---------------------------------------------------------------------------*/
/*
 * \brief     Debug stack pointer.
 * \param sp  Stack pointer.
 *
 * Provide a view into the CPU state from the provided stack pointer.
 */
static void
debugHardfault(uint32_t *sp)
{
#ifndef RF_TEST_APP_WD_DEBUG_DISABLE
    /*
        * (R0-R3, R12): Reconstruct the condition of the instruction at the
                        time the instruction executed.
        * Program Counter (PC): Points to the offending instruction.
        * Link Register (LR): Provides some context about what the program
                              was doing when the fault occurred. It tells
                              more about the calling function.
    */
    volatile uint32_t r0;  /**< R0 register */
    volatile uint32_t r1;  /**< R1 register */
    volatile uint32_t r2;  /**< R2 register */
    volatile uint32_t r3;  /**< R3 register */
    volatile uint32_t r12; /**< R12 register */
    volatile uint32_t lr;  /**< LR register */
    volatile uint32_t pc;  /**< PC register */

    (void)(r0  = sp[0]);
    (void)(r1  = sp[1]);
    (void)(r2  = sp[2]);
    (void)(r3  = sp[3]);
    (void)(r12 = sp[4]);
    (void)(lr  = sp[5]);
    (void)(pc  = sp[6]);

    /*
    | Bit | Description                                                            |
    |:--- |:---------------------------------------------------------------------- |
    | 7   | Valid fault address is stored in the Memory Management Address         |
    |     | Register (MMADR) at 0xE000ED34.                                        |
    | 6:5 | unused                                                                 |
    | 4   | Memory access violation occurred while stacking on exception entry.    |
    | 3   | Memory access violation occurred while unstacking on exception return. |
    | 2   | unused.                                                                |
    | 1   | Data access violation.                                                 |
    | 0   | Instruction access violation.                                          |
    */
    volatile uint32_t MMFSR_register = HWREG(0xE000ED28);
    /*
    | Bit | Description                                                 |
    |:--- |:----------------------------------------------------------- |
    | 7   | Valid fault address is stored in Bus Fault Address Register |
    |     | (FAULTADDR) at 0xE000ED38.                                  |
    | 6:5 | unused                                                      |
    | 4   | Bus fault occurred while stacking on exception entry        |
    | 3   | Bus fault occurred while unstacking on exception return.    |
    | 2   | Stacked PC does not indicate exact address of bus fault     |
    | 1   | Stacked PC indicates exact address of bus fault.            |
    | 0   | Instruction bus error.                                      |
    */
    volatile uint32_t BFSR_register = HWREG(0xE000ED29);
    /*
    | Bit | Description                                           |
    |:--- |:----------------------------------------------------- |
    | 7-4 | unused                                                |
    | 3   | Attempted to execute a coprocessor instruction.       |
    | 2   | Attempted exception return caused illegal load of PC. |
    | 1   | Invalid processing/instruction state.                 |
    | 0   | Attempt to execute an undefined instruction.          |
    */
    volatile uint32_t UFSR_register = HWREG(0xE000ED2A);

    uint8_t hardfaultLog[33] = {0xFF};
    hardfaultLog[0] = 0x48; // H
    hardfaultLog[1] = 0x46; // F
    hardfaultLog[2] = (uint8_t) MMFSR_register;
    hardfaultLog[3] = (uint8_t) BFSR_register;
    hardfaultLog[4] = (uint8_t) UFSR_register;
    hardfaultLog[5] = (uint8_t) (lr >> 24U);
    hardfaultLog[6] = (uint8_t) (lr >> 16U);
    hardfaultLog[7] = (uint8_t) (lr >> 8U);
    hardfaultLog[8] = (uint8_t) lr;
    hardfaultLog[9] = (uint8_t) (pc >> 24U);
    hardfaultLog[10] = (uint8_t) (pc >> 16U);
    hardfaultLog[11] = (uint8_t) (pc >> 8U);
    hardfaultLog[12] = (uint8_t) pc;
    hardfaultLog[13] = (uint8_t) (r0 >> 24U);
    hardfaultLog[14] = (uint8_t) (r0 >> 16U);
    hardfaultLog[15] = (uint8_t) (r0 >> 8U);
    hardfaultLog[16] = (uint8_t) r0;
    hardfaultLog[17] = (uint8_t) (r1 >> 24U);
    hardfaultLog[18] = (uint8_t) (r1 >> 16U);
    hardfaultLog[19] = (uint8_t) (r1 >> 8U);
    hardfaultLog[20] = (uint8_t) r1;
    hardfaultLog[21] = (uint8_t) (r2 >> 24U);
    hardfaultLog[22] = (uint8_t) (r2 >> 16U);
    hardfaultLog[23] = (uint8_t) (r2 >> 8U);
    hardfaultLog[24] = (uint8_t) r2;
    hardfaultLog[25] = (uint8_t) (r3 >> 24U);
    hardfaultLog[26] = (uint8_t) (r3 >> 16U);
    hardfaultLog[27] = (uint8_t) (r3 >> 8U);
    hardfaultLog[28] = (uint8_t) r3;
    hardfaultLog[29] = (uint8_t) (r12 >> 24U);
    hardfaultLog[30] = (uint8_t) (r12 >> 16U);
    hardfaultLog[31] = (uint8_t) (r12 >> 8U);
    hardfaultLog[32] = (uint8_t) r12;

    /*
    | FIELD | HF  | MMFSR | BFSR | UFSR | lr  | pc  | r0  | r1  | r2  | r3  | r12 |
    | ----- | --- | ----- | ---- | ---- | --- | --- | --- | --- | --- | --- | --- |
    | BYTES | 2   | 1     | 1    | 1    | 4   | 4   | 4   | 4   | 4   | 4   | 4   |
    */
//  #if BOARD_CONF_ENABLE_LOGS_ON_UART1
//      uart1_write(hardfaultLog, sizeof(hardfaultLog));
//  #else
//      uart0_write(hardfaultLog, sizeof(hardfaultLog));
//  #endif
//_eraseSensorList();
    _write(0x00002000, hardfaultLog, sizeof(hardfaultLog));
#endif

    /* Enter an infinite loop. */
    for(;;) { /* hang */ }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
faultISR(void)
{
    __asm__ __volatile__
    (
        "tst lr, #4        \n"
        "ite eq            \n"
        "mrseq r0, msp     \n"
        "mrsne r0, psp     \n"
        "bx %0             \n"
        : /* output */
        : /* input */
        "r"(debugHardfault)
    );
}

//*****************************************************************************
//
//! This is the code that gets called when the processor receives an unexpected
//! interrupt. This simply enters an infinite loop, preserving the system state
//! for examination by a debugger.
//
//*****************************************************************************
static void
intDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This function is called by __libc_fini_array which gets called when exit()
// is called. In order to support exit(), an empty _fini() stub function is
// required.
//
//*****************************************************************************
void _fini(void)
{
    /* Function body left empty intentionally */
}
