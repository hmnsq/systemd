/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

மாறிலி எழுத்து* errno_name_no_fallback(முழு id) _const_;
முழு errno_from_name(மாறிலி எழுத்து *name) _pure_;

நிலையான உள்வரி bool errno_is_valid(முழு n) {
        திரும்பு n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(முழு)
/* Like errno_name, but always returns a string. */
மாறிலி எழுத்து* errno_name(முழு id, எழுத்து buf[நிலையான ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (எழுத்து[ERRNO_NAME_BUF_LEN]){})
