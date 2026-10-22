/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "stdio-util.h"

അക്ഷരം* asprintf_safe(സ്ഥിരം അക്ഷരം *restrict fmt, ...) {
        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, fmt);
        r = vasprintf(&buf, fmt, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ ശൂന്യ;
        തിരികെ TAKE_PTR(buf);
}
