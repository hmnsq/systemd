/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಧ್ರುವ ವರ್ಣ* errno_name_no_fallback(ಪೂರ್ಣಾಙ್ಕ id) _const_;
ಪೂರ್ಣಾಙ್ಕ errno_from_name(ಧ್ರುವ ವರ್ಣ *name) _pure_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool errno_is_valid(ಪೂರ್ಣಾಙ್ಕ n) {
        ಪ್ರತ್ಯಾಗಮ n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ)
/* Like errno_name, but always returns a string. */
ಧ್ರುವ ವರ್ಣ* errno_name(ಪೂರ್ಣಾಙ್ಕ id, ವರ್ಣ buf[ಸ್ಥಿರ ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (ವರ್ಣ[ERRNO_NAME_BUF_LEN]){})
