/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe/iota/iota_ppe42.S $            */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2017,2018                                                    */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#include "iota.h"
#include "iota_ppe42.h"
#include "std_irq_config.h"
#include "iota_panic_codes.h"

.global g_iota_execution_stack

.global __iota_boot
__iota_boot:
    # Enable 'trap' to cause a halt
    li      %r3, 0x0100
    slwi    %r3, %r3, 16
    mtdbcr  %r3

    # Mask timer interrupts
    li      %r3, 0
    mttcr   %r3

    # Load the sda and sda2 pointers to eabi-defined registers
    lis     %r13, _SDA_BASE_@h
    ori     %r13, %r13, _SDA_BASE_@l
    lis     %r2, _SDA2_BASE_@h
    ori     %r2, %r2, _SDA2_BASE_@l

    # Load the initial stack pointer
    lis     %r1, g_iota_execution_stack@h
    ori     %r1, %r1, g_iota_execution_stack@l
    addi    %r3, 0, IOTA_EXECUTION_STACK_SIZE
    add     %r1, %r1, %r3
    # 0 to indicate the end of the stack chain
    li      %r3, 0
    stwu    %r3, -8(%r1)

    b       _iota_boot


# This routine is called for every interrupt/exception which is a "reason"
# for iota to schedule a task. The current machine state is saved.
# A few pre-conditions must be met:
#   - original r3 value (yet to be saved!) is at _IOTA_TEMPORARY_R3_STACK_OFFSET(sp)
#   - the pointer to location of where to save machine state is in r9
#   - the schedule reason is in r3
#   - d8 is saved
# This routine sets up and calls _zbm_schedule (C-function), upon which's
# return the prior machine state is restored.

.global g_iota_curr_machine_state_ptr
.global _iota_schedule

.global __iota_save_interrupt_state_and_schedule
__iota_save_interrupt_state_and_schedule:
    # First, save off the original value of r3
    lwz     %r8, _IOTA_TEMPORARY_R3_STACK_OFFSET(%r1)
    stw     %r8, _IOTA_SAVE_R3_OFFSET(%r9)
    # Save r10
    stw     %r10, _IOTA_SAVE_R10_OFFSET(%r9)
    # Save d0, d4, d6, d28, and d31
    stvd    %d0, _IOTA_SAVE_D0_OFFSET(%r9)
    stvd    %d4, _IOTA_SAVE_D4_OFFSET(%r9)
    stvd    %d6, _IOTA_SAVE_D6_OFFSET(%r9)
    stvd    %d28, _IOTA_SAVE_D28_OFFSET(%r9)
    stvd    %d30, _IOTA_SAVE_D30_OFFSET(%r9)
    # Piecemeal save srr0+srr1 and lr+cr
    mfsrr0  %r4
    mfsrr1  %r5
    stvd    %d4, _IOTA_SAVE_SRR0_OFFSET(%r9)
    mflr    %r4
    mfcr    %r5
    stvd    %d4, _IOTA_SAVE_LR_OFFSET(%r9)
    mfctr   %r4
    stw     %r4, _IOTA_SAVE_CTR_OFFSET(%r9)
    # All state is saved, sp(r1) intact pointing to "Execution Stack"

    # _iota_schedule(uint32_t schedule_reason)
    # - schedule_reason is already in r3

    bl      _iota_schedule

    # Task completed, need to restore machine state now
    # Get pointer to machine state
    lwz      %r3, g_iota_curr_machine_state_ptr@sda21(0)
    # Restore d0 first (need sp in r1)
    lvd     %d0, _IOTA_SAVE_D0_OFFSET(%r3)
    # Save the restore value of r3 to _IOTA_TEMPORARY_R3_STACK_OFFSET(sp) 
    lwz     %r4, _IOTA_SAVE_R3_OFFSET(%r3)
    stw     %r4, _IOTA_TEMPORARY_R3_STACK_OFFSET(%r1)
    # Restore d4, d6, d28, and d30
    lvd     %d4, _IOTA_SAVE_D4_OFFSET(%r3)
    lvd     %d6, _IOTA_SAVE_D6_OFFSET(%r3)
    lvd     %d28, _IOTA_SAVE_D28_OFFSET(%r3)
    lvd     %d30, _IOTA_SAVE_D30_OFFSET(%r3)
    # Restore r10
    lwz     %r10, _IOTA_SAVE_R10_OFFSET(%r3)
    # Restore lr and cr
    lvd     %d8, _IOTA_SAVE_LR_OFFSET(%r3)
    mtlr    %r8
    mtcr0   %r9
    lwz     %r9, _IOTA_SAVE_CTR_OFFSET(%r3)
    mtctr   %r9
    lvd     %d8, _IOTA_SAVE_SRR0_OFFSET(%r3)
    mtsrr0  %r8
    mtsrr1  %r9
    # Restore d8
    lvd     %d8, _IOTA_SAVE_D8_OFFSET(%r3)
    # Restore r3
    lwz     %r3, _IOTA_TEMPORARY_R3_STACK_OFFSET(%r1)

    rfi

.global __iota_halt
__iota_halt:
    _pk_panic IOTA_SOFTWARE_HALT

## pull in the macro that calculates all the external interrupt config
## defined in pk/std/std_irq_config.h
    .std_irq_cfg_bitmaps
