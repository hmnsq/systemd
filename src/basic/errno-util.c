/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "errno-util.h"

ध्रुव वर्ण* strerror_or_eof(पूर्णाङ्क errnum, वर्ण *buf, size_t buflen) {
        सचे (errnum != 0)
                पटिनिवत्त strerror_r(ABS(errnum), buf, buflen);

        पटिनिवत्त "Unexpected EOF";
}
