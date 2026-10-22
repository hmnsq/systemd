/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

ध्रुव वर्ण* errno_name_no_fallback(पूर्णाङ्क id) _const_;
पूर्णाङ्क errno_from_name(ध्रुव वर्ण *name) _pure_;

स्थिर अन्तःस्थ bool errno_is_valid(पूर्णाङ्क n) {
        पटिनिवत्त n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(पूर्णाङ्क)
/* Like errno_name, but always returns a string. */
ध्रुव वर्ण* errno_name(पूर्णाङ्क id, वर्ण buf[स्थिर ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (वर्ण[ERRNO_NAME_BUF_LEN]){})
