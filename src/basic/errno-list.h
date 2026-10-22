/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

const var_na* errno_name_no_fallback(ຈຳນວນເຕັມ id) _const_;
ຈຳນວນເຕັມ errno_from_name(const var_na *name) _pure_;

static anta_hstha bool errno_is_valid(ຈຳນວນເຕັມ n) {
        ສົ່ງຄືນ n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(ຈຳນວນເຕັມ)
/* Like errno_name, but always returns a string. */
const var_na* errno_name(ຈຳນວນເຕັມ id, var_na buf[static ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (var_na[ERRNO_NAME_BUF_LEN]){})
