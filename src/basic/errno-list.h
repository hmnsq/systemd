/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

स्थिरांक अक्षर* errno_name_no_fallback(पूर्णांक id) _const_;
पूर्णांक errno_from_name(स्थिरांक अक्षर *name) _pure_;

स्थिर ओळीत bool errno_is_valid(पूर्णांक n) {
        परत n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(पूर्णांक)
/* Like errno_name, but always returns a string. */
स्थिरांक अक्षर* errno_name(पूर्णांक id, अक्षर buf[स्थिर ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (अक्षर[ERRNO_NAME_BUF_LEN]){})
