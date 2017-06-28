/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/hwp/lib/p9_sbe_ppe_utils.C $       */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2017                                                         */
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
/// @file  p9_sbe_ppe_utils.C
/// @brief Commonly used PPE utilities on the SBE
///
/// *HWP HW Owner        : Greg Still <stillgs@us.ibm.com>
/// *HWP HW Backup Owner : Brian Vanderpool <vanderp@us.ibm.com>
/// *HWP FW Owner        : Amit Tendolkar <amit.tendolkar@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 1
/// *HWP Consumed by     : SBE
///
/// @verbatim
///
/// @endverbatim

#if 0
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <fapi2.H>
#include <p9_sbe_ppe_utils.H>
#include <p9_hcd_common.H>
//-----------------------------------------------------------------------------

uint32_t ppe_getMtsprInstruction( const uint16_t i_Rs, const uint16_t i_Spr )
{
    uint32_t mtsprInstOpcode = 0;
    uint32_t temp = (( i_Spr & 0x03FF ) << 11);
    mtsprInstOpcode = ( temp  & 0x0000F800 ) << 5;
    mtsprInstOpcode |= ( temp & 0x001F0000 ) >> 5;
    mtsprInstOpcode |= MTSPR_BASE_OPCODE;
    mtsprInstOpcode |= ( i_Rs & 0x001F ) << 21;

    return mtsprInstOpcode;
}
//-----------------------------------------------------------------------------

uint32_t ppe_getMfsprInstruction( const uint16_t i_Rt, const uint16_t i_Spr )
{
    uint32_t mtsprInstOpcode = 0;
    uint32_t temp = (( i_Spr & 0x03FF ) << 11);
    mtsprInstOpcode = ( temp  & 0x0000F800 ) << 5;
    mtsprInstOpcode |= ( temp & 0x001F0000 ) >> 5;
    mtsprInstOpcode |= MFSPR_BASE_OPCODE;
    mtsprInstOpcode |= ( i_Rt & 0x001F ) << 21;

    return mtsprInstOpcode;
}
//-----------------------------------------------------------------------------

fapi2::ReturnCode ppe_pollHaltState(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
    const uint64_t i_base_address)
{
    fapi2::buffer<uint64_t> l_data64;

    // Halt state entry should be very fast on PPEs (eg nanoseconds)
    // Try only using the SCOM access time to delay.
    uint32_t l_timeout_count = 20;

    do
    {
        FAPI_TRY(getScom ( i_target,
                           i_base_address + PPE_XIRAMDBG,
                           l_data64 ),
                 "Failed reading XIRAMDBG register!" );
    }
    while ((! l_data64.getBit<0>()) &&
           (--l_timeout_count != 0));

    FAPI_ASSERT ((l_data64.getBit<0>()),
                 fapi2::SBE_PPE_UTILS_HALT_TIMEOUT_ERR(),
                 "PPE Halt Timeout. Base Addr: 0x%16llX", i_base_address);

fapi_try_exit:
    return fapi2::current_err;
}
//-----------------------------------------------------------------------------

fapi2::ReturnCode ppe_halt(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
    const uint64_t i_base_address)
{
    fapi2::buffer<uint64_t> l_data64;

    FAPI_INF("   Send HALT command via XCR...");
    l_data64.flush<0>().insertFromRight(p9hcd::HALT, 1, 3);

    FAPI_TRY(putScom(i_target, i_base_address + PPE_XIXCR, l_data64),
             "Error in PUTSCOM in XCR to generate Halt condition");

    FAPI_TRY(ppe_pollHaltState(i_target, i_base_address));

fapi_try_exit:
    return fapi2::current_err;
}
//-----------------------------------------------------------------------------

fapi2::ReturnCode ppe_RAMRead(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
    const uint64_t i_base_address,
    const fapi2::buffer<uint64_t> i_instruction,
    fapi2::buffer<uint32_t>& o_data)
{
    fapi2::buffer<uint64_t> l_data64;
    FAPI_TRY(ppe_pollHaltState(i_target, i_base_address));
    FAPI_TRY(fapi2::putScom(i_target, i_base_address + PPE_XIRAMEDR, i_instruction));
    FAPI_DBG("    RAMREAD i_instruction: 0X%16llX", i_instruction);
    FAPI_TRY(ppe_pollHaltState(i_target, i_base_address));
    FAPI_TRY(fapi2::getScom(i_target, i_base_address + PPE_XIRAMDBG, l_data64),
             "Error in GETSCOM");
    l_data64.extractToRight(o_data, 32, 32);
    FAPI_DBG("    RAMREAD o_data: 0X%16llX", o_data);

fapi_try_exit:
    return fapi2::current_err;
}
//-----------------------------------------------------------------------------
#endif