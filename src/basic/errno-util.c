/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "errno-util.h"

ध्रुव वर्ण* strerror_or_eof(पूर्णाङ्क errnum, वर्ण *buf, माप_प्रकार buflen) {
        यदि (errnum != 0)
                प्रत्यागम strerror_r(ABS(errnum), buf, buflen);

        प्रत्यागम "Unexpected EOF";
}
