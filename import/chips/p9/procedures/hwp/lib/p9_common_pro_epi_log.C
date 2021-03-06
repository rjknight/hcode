/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/hwp/lib/p9_common_pro_epi_log.C $  */
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
///
/// @file  p9_common_pro_epi_log.C
/// @brief common procedure prologue/epilogue routines
///
/// *HWP HWP Owner          : David Du      <daviddu@us.ibm.com>
/// *HWP Backup HWP Owner   : Greg Still    <stillgs@us.ibm.com>
/// *HWP FW Owner           : Reshmi Nair   <resnair5@in.ibm.com>
/// *HWP Team               : PM
/// *HWP Consumed by        : SBE:SGPE
/// *HWP Level              : 1
///
/// Procedure Summary:
///

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>
#include "p9_common_pro_epi_log.H"

//------------------------------------------------------------------------------
// Constant Definitions:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Procedure:
//------------------------------------------------------------------------------

extern "C"
{

    fapi2::ReturnCode
    p9_common_pro_epi_log(
        const fapi2::Target < fapi2::TARGET_TYPE_EQ |
        fapi2::TARGET_TYPE_CORE > & i_target,
        int i_operation)
    {
        return fapi2::FAPI2_RC_SUCCESS;

    } // Procedure


} // extern C
