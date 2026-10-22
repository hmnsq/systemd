/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "../fundamental/confidential-virt.h"   /* IWYU pragma: export */

પ્રકારવ્યાખ્યા ગણના ConfidentialVirtualization {
        CONFIDENTIAL_VIRTUALIZATION_NONE = 0,

        CONFIDENTIAL_VIRTUALIZATION_SEV,
        CONFIDENTIAL_VIRTUALIZATION_SEV_ES,
        CONFIDENTIAL_VIRTUALIZATION_SEV_SNP,
        CONFIDENTIAL_VIRTUALIZATION_TDX,
        CONFIDENTIAL_VIRTUALIZATION_PROTVIRT,
        CONFIDENTIAL_VIRTUALIZATION_CCA,

        _CONFIDENTIAL_VIRTUALIZATION_MAX,
        _CONFIDENTIAL_VIRTUALIZATION_INVALID = -EINVAL,
        _CONFIDENTIAL_VIRTUALIZATION_ERRNO_MAX = -ERRNO_MAX, /* ensure full range of errno fits into this enum */
} ConfidentialVirtualization;

ConfidentialVirtualization detect_confidential_virtualization(શૂન્ય);

DECLARE_STRING_TABLE_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
