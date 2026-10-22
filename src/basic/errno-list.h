/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const char* errno_name_no_fallback(整数 id) _const_;
整数 errno_from_name(const char *name) _pure_;

static inline bool errno_is_valid(整数 n) {
        戻る n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(整数)
/* Like errno_name, but always returns a string. */
const char* errno_name(整数 id, char buf[static ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (char[ERRNO_NAME_BUF_LEN]){})
