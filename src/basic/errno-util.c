/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "errno-util.h"

स्थिरांक अक्षर* strerror_or_eof(पूर्णांक errnum, अक्षर *buf, माप_प्रकार buflen) {
        जर (errnum != 0)
                परत strerror_r(ABS(errnum), buf, buflen);

        परत "Unexpected EOF";
}
