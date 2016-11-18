/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/cme/pstate_cme/p9_cme_pstate.h $ */
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

/// \file p9_cme_pstate.h
/// \brief Shared and global definitions for pstate H codes.
/// \owner  Rahul Batra Email: rbatra@us.ibm.com
///
#ifndef _P9_CME_PSTATE_H_
#define _P9_CME_PSTATE_H_

#include "pk.h"
#include "pstate_pgpe_cme_api.h"
#include "gpehw_common.h"

enum  PMCR_CONTROL
{
    PMCR0_DISABLE  =    0x1,
    PMCR1_DISABLE  =    0x2,
    PMCR0_ENABLE   =    0x4,
    PMCR1_ENABLE   =    0x8
};

void p9_cme_pstate_pmcr_thread(void*);
void p9_cme_pstate_db_thread(void*);
void p9_cme_pstate_pmcr_handler(void*, PkIrqId);
void p9_cme_pstate_db_handler(void*, PkIrqId);
void p9_cme_pstate_intercme_in0_handler(void*, PkIrqId);
int send_pig_packet(uint64_t data, uint32_t coreMask);


typedef struct
{
    PkSemaphore sem[2];
    uint32_t quadNum;
    uint32_t pstatesEnabled;
} CmePstateRecord;

typedef struct
{
    uint64_t seqNum;
} cme_pstate_pmcr_data_t;

typedef struct
{
    uint32_t qmFlag;
    uint32_t siblingCMEFlag;
    uint32_t cmeMaskGoodCore;
    uint32_t globalPS;
    uint32_t localPS;
    uint32_t resClkTblIdx;
    uint32_t qaccr21_23InitVal;
    uint32_t dpll_pstate0_value;
} cme_pstate_db_data_t;

#endif //_P9_CME_PSTATE_H_
