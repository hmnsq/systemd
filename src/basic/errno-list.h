/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* errno_name_no_fallback(ಪೂರ್ಣಾಂಕ id) _const_;
ಪೂರ್ಣಾಂಕ errno_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool errno_is_valid(ಪೂರ್ಣಾಂಕ n) {
        ಹಿಂತಿರುಗಿ n > 0 && n <= ERRNO_MAX;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ)
/* Like errno_name, but always returns a string. */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* errno_name(ಪೂರ್ಣಾಂಕ id, ಅಕ್ಷರ buf[ಸ್ಥಿರ ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#ವ್ಯಾಖ್ಯಾನಿಸು ERRNO_NAME(errnum) errno_name(errnum, (ಅಕ್ಷರ[ERRNO_NAME_BUF_LEN]){})
