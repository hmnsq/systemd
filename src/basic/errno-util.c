/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "errno-util.h"

अचर वर्ण* strerror_or_eof(पूर्णाङ्क errnum, वर्ण *buf, माप_प्रकार buflen) {
        यदि (errnum != 0)
                फर्काऊ strerror_r(ABS(errnum), buf, buflen);

        फर्काऊ "Unexpected EOF";
}
