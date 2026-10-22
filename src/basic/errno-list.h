/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* errno_name_no_fallback(ကိန်းပြည့် id) _const_;
ကိန်းပြည့် errno_from_name(const var_na *name) _pure_;

static anta_hstha bool errno_is_valid(ကိန်းပြည့် n) {
        ပြန်ပေး n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(ကိန်းပြည့်)
/* Like errno_name, but always returns a string. */
const var_na* errno_name(ကိန်းပြည့် id, var_na buf[static ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (var_na[ERRNO_NAME_BUF_LEN]){})
