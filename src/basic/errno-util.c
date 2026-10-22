/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "errno-util.h"

स्थिर अक्षर* strerror_or_eof(पूर्णांक errnum, अक्षर *buf, माप_प्रकार buflen) {
        अगर (errnum != 0)
                वापस strerror_r(ABS(errnum), buf, buflen);

        वापस "Unexpected EOF";
}
