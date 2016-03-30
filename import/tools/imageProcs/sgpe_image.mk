# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: import/tools/imageProcs/sgpe_image.mk $
#
# OpenPOWER HCODE Project
#
# COPYRIGHT 2016,2017
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
IMAGE=sgpe_image
$(call APPEND_EMPTY_SECTION,qpmr,1024)
$(call APPEND_EMPTY_SECTION,lvl1_bl,1024)
$(call APPEND_EMPTY_SECTION,lvl2_bl,1024)
$(call APPEND_EMPTY_SECTION,vect,1024)
$(call APPEND_EMPTY_SECTION,sgpe_image_header,1024)
$(call XIP_TOOL,append,.hcode,$(ROOTPATH)/chips/p9/procedures/ppe_closed/sgpe/stop_gpe/obj/stop_gpe/stop_gpe.bin)
$(call BUILD_IMAGE)