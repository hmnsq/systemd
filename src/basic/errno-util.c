/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "errno-util.h"

സ്ഥിരം അക്ഷരം* strerror_or_eof(പൂർണ്ണസംഖ്യ errnum, അക്ഷരം *buf, size_t buflen) {
        എങ്കിൽ (errnum != 0)
                തിരികെ strerror_r(ABS(errnum), buf, buflen);

        തിരികെ "Unexpected EOF";
}
