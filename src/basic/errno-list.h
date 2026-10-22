/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

स्थिर अक्षर* errno_name_no_fallback(पूर्णांक id) _const_;
पूर्णांक errno_from_name(स्थिर अक्षर *name) _pure_;

अटल अंतरभूत bool errno_is_valid(पूर्णांक n) {
        वापस n > 0 && n <= ERRNO_MAX;
}

#घोषणा ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(पूर्णांक)
/* Like errno_name, but always returns a string. */
स्थिर अक्षर* errno_name(पूर्णांक id, अक्षर buf[अटल ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#घोषणा ERRNO_NAME(errnum) errno_name(errnum, (अक्षर[ERRNO_NAME_BUF_LEN]){})
