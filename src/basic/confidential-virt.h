/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/confidential-virt.h"   /* IWYU pragma: export */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ConfidentialVirtualization {
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

ConfidentialVirtualization detect_confidential_virtualization(ಶೂನ್ಯ);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(confidential_virtualization, ConfidentialVirtualization);
