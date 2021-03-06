/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe/hwpf/include/plat/plat_error_scope.H $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2012,2017                                                    */
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
/**
 * @file plat_error_scope.H
 * @brief platform definitions which create a scope for automatic error handling
 */

#ifndef __FAPI2_PLAT_ERROR_SCOPE__
#define __FAPI2_PLAT_ERROR_SCOPE__

/// @cond
#define PLAT_FAPI_TRY_NO_TRACE( __operation__ )                         \
    (__operation__)

#define PLAT_FAPI_TRY_TRACE( __operation__, ... )                       \
    FAPI_DBG(__VA_ARGS));                                               \
    (__operation__)

///
/// @brief Assert a conditional is true.
/// If it is not, the FFDC gathering function is called and the
/// trace is output as a FAPI error trace.
/// @param[in] __conditional__ the condition to assert
/// @param[in] __ffdc__ the FFDC gathering function
/// @param[in] ... varargs, as input to FAPI_ERR
///
#define PLAT_FAPI_ASSERT( __conditional__, __ffdc__, ... )      \
    if (! (__conditional__))                                    \
    {                                                           \
        (__ffdc__).execute();                                   \
        FAPI_ERR(__VA_ARGS__);                                  \
        goto fapi_try_exit;                                     \
    }


///
/// @brief Temporary macro for error label until all are removed.
/// @todo REMOVE this in time.
#define FAPI_CLEANUP()   \
    fapi_try_exit:
/// @endcond

#endif
