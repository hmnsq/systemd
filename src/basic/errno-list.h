/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* errno_name_no_fallback(puur_naa_nka id) _const_;
puur_naa_nka errno_from_name(const var_na *name) _pure_;

static anta_hstha bool errno_is_valid(puur_naa_nka n) {
        ᨀᨘᨒ n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(puur_naa_nka)
/* Like errno_name, but always returns a string. */
const var_na* errno_name(puur_naa_nka id, var_na buf[static ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (var_na[ERRNO_NAME_BUF_LEN]){})
