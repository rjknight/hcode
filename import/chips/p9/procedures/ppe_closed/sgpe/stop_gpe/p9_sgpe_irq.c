/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/sgpe/stop_gpe/p9_sgpe_irq.c $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2015,2017                                                    */
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
//-----------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2014
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
//-----------------------------------------------------------------------------

/// \file   p9_sgpe_irq.c
/// \brief  Shared and global file for SGPE H-codes.
/// \owner  Michael Olsen   Email: cmolsen@us.ibm.com
/// \owner  David Du        Email: daviddu@us.ibm.com
///

#include "pk.h"
#include "p9_sgpe_irq.h"

//-------------------------------------------------------------------//
//            DO NOT modify this file unless you're the owner        //
//-------------------------------------------------------------------//

// Notes:
//   The following two lists,
//     ext_irq_vectors_sgpe[][] and IDX_PRTY_LVL_<task_abbr>, must match.
//   IDX_PRTY_LVL_<task_abbr> is the tasks priority level and serves
//   as the index into the ext_irq_vectors_sgpe[][] table.

const uint64_t ext_irq_vectors_sgpe[NUM_EXT_IRQ_PRTY_LEVELS][2] =
{
    /* 0: IDX_PRTY_VEC    1: IDX_MASK_VEC */
    {
        IRQ_VEC_PRTY0_SGPE, /* 0: IDX_PRTY_LVL_HIPRTY */
        IRQ_VEC_PRTY4_SGPE |
        IRQ_VEC_PRTY3_SGPE |
        IRQ_VEC_PRTY2_SGPE |
        IRQ_VEC_PRTY1_SGPE |
        IRQ_VEC_PRTY0_SGPE
    },
    {
        IRQ_VEC_PRTY1_SGPE, /* 1: IDX_PRTY_LVL_IPI3_HIGH */
        IRQ_VEC_PRTY4_SGPE |
        IRQ_VEC_PRTY3_SGPE |
        IRQ_VEC_PRTY2_SGPE |
        IRQ_VEC_PRTY1_SGPE
    },
    {
        IRQ_VEC_PRTY2_SGPE, /* 2: IDX_PRTY_LVL_PIG_TYPE */
        IRQ_VEC_PRTY4_SGPE |
        IRQ_VEC_PRTY3_SGPE |
        IRQ_VEC_PRTY2_SGPE
    },
    {
        IRQ_VEC_PRTY3_SGPE, /* 3: IDX_PRTY_LVL_IPI3_LOW */
        IRQ_VEC_PRTY4_SGPE |
        IRQ_VEC_PRTY3_SGPE
    },
    {
        IRQ_VEC_PRTY4_SGPE, /* 4: IDX_PRTY_LVL_DISABLED */
        IRQ_VEC_PRTY4_SGPE
    }
};

uint8_t       g_current_prty_level = NUM_EXT_IRQ_PRTY_LEVELS - 1;
uint8_t       g_oimr_stack[NUM_EXT_IRQ_PRTY_LEVELS];
int           g_oimr_stack_ctr = -1;
uint64_t      g_oimr_override_stack[NUM_EXT_IRQ_PRTY_LEVELS];
uint64_t      g_oimr_override = 0x0000000000000000;

// Unified IRQ priority and masking handler.
// - Locates the highest priority IRQ task vector that has at least one of its
//   interrupts in the current external PK interrupt vector.
void pk_unified_irq_prty_mask_handler(void)
{
    uint8_t   iPrtyLvl, bFound;
    uint64_t  ext_irq_vector_pk;

    // 1. Identify the priority level of the interrupt.
    ext_irq_vector_pk = ((uint64_t)in32(OCB_G0ISR0 + APPCFG_OCC_INSTANCE_ID * 8)) << 32 |
                        (uint64_t)in32(OCB_G0ISR1 + APPCFG_OCC_INSTANCE_ID * 8);
    bFound = FALSE;
    iPrtyLvl = 0;

    do
    {
        if (ext_irq_vectors_sgpe[iPrtyLvl][IDX_PRTY_VEC] & ext_irq_vector_pk)
        {
            bFound = TRUE;
            break;
        }
    }
    while(++iPrtyLvl < (NUM_EXT_IRQ_PRTY_LEVELS - 1)); //No need to check DISABLED.

    // Only manipulate EIMR masks for task level prty levels.
    // Let shared non-task IRQs (iPrtyLvl=0) be processed by
    // the PK kernel in usual fashion.
    if (bFound)
    {
        // 2. Save current mask (assume current prty level).
        //    Note, reading EIMR is NOT safe because overrides may already have
        //      happened to the EIMR. And we always want to restore
        //      the EIMR to a known value when we exit our thread.
        if (++g_oimr_stack_ctr < NUM_EXT_IRQ_PRTY_LEVELS)
        {
            // Make a note of present prty level and
            // then update tracker to new prty level.
            g_oimr_stack[g_oimr_stack_ctr] = g_current_prty_level;
            g_current_prty_level = iPrtyLvl; // Update prty level tracker.
            g_oimr_override_stack[g_oimr_stack_ctr] = g_oimr_override;
        }
        else
        {
            MY_TRACE_ERR("Code bug: EIMR S/R stack counter=%d  >=  max=%d.",
                         g_oimr_stack_ctr, NUM_EXT_IRQ_PRTY_LEVELS);
            pk_halt();
        }

        // 3. Write the new mask for this priority level.

        // First, clear all those IRQs that could possibly interrupt this instance.
        //  This includes all those IRQs which belong to this instance as well as
        //  those high-prty IRQs shared with the other instances.
        out32(OCB_OIMR0_CLR, (uint32_t)(IRQ_VEC_ALL_OUR_IRQS >> 32));
        out32(OCB_OIMR1_CLR, (uint32_t)IRQ_VEC_ALL_OUR_IRQS);

        // Second, mask IRQs belonging to this task and lower prty tasks.
        // Note, that we do not modify the permanently disabled IRQs, such as the
        //  _RESERVED_ ones. Nor do we touch other instances' IRQs. Iow, the
        //  IDX_PRTY_LVL_DISABLED mask is  NOT  part of the mask we apply below.
        out32(OCB_OIMR0_OR, (uint32_t)((ext_irq_vectors_sgpe[iPrtyLvl][IDX_MASK_VEC] |
                                        g_oimr_override) >> 32) );
        out32(OCB_OIMR1_OR, (uint32_t)(ext_irq_vectors_sgpe[iPrtyLvl][IDX_MASK_VEC] |
                                       g_oimr_override) );

    }
    else
    {
        MY_TRACE_ERR("A disabled IRQ fired");
        MY_TRACE_ERR("ext_irq_vector_pk=0x%08x%08x", ext_irq_vector_pk);
#if !EPM_P9_TUNING
        pk_halt();
#endif
    }


    // 4. Return the priority vector in d5 and let hwmacro_get_ext_irq do the
    //    rest, i.e. route first found IRQ in the returned priority vector
    //    to the registered [unified] interrupt handler.
    uint32_t  register l_vecH asm("r5");
    //uint32_t  register l_vecL asm("r6");

    //l_vecL = 0;
    asm volatile ("lvd %[data], 0(%[addr]) \n" \
                  : [data]"=r"(l_vecH) \
                  : [addr]"r"(&(ext_irq_vectors_sgpe[iPrtyLvl][IDX_PRTY_VEC])) );
}