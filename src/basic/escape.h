/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#define SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#define SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#define SHELL_NEED_ESCAPE_POSIX "\\\'"

प्रकारनिर्देश गणना UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

प्रकारनिर्देश गणना ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

पूर्णाङ्क cescape_char(वर्ण c, वर्ण *buf);
वर्ण* cescape_length(ध्रुव वर्ण *s, size_t n) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ वर्ण* cescape(ध्रुव वर्ण *s) {
        प्रत्यागम cescape_length(s, SIZE_MAX);
}

पूर्णाङ्क cunescape_one(ध्रुव वर्ण *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ध्रुव वर्ण *s, size_t length, ध्रुव वर्ण *prefix, UnescapeFlags flags, वर्ण **ret);
स्थिर अन्तःस्थ ssize_t cunescape_length(ध्रुव वर्ण *s, size_t length, UnescapeFlags flags, वर्ण **ret) {
        प्रत्यागम cunescape_length_with_prefix(s, length, रिक्त, flags, ret);
}
स्थिर अन्तःस्थ ssize_t cunescape(ध्रुव वर्ण *s, UnescapeFlags flags, वर्ण **ret) {
        प्रत्यागम cunescape_length(s, SIZE_MAX, flags, ret);
}

प्रकारनिर्देश गणना XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

वर्ण* xescape_full(ध्रुव वर्ण *s, ध्रुव वर्ण *bad, size_t console_width, XEscapeFlags flags);
स्थिर अन्तःस्थ वर्ण* xescape(ध्रुव वर्ण *s, ध्रुव वर्ण *bad) {
        प्रत्यागम xescape_full(s, bad, SIZE_MAX, 0);
}
वर्ण* octescape_full(ध्रुव वर्ण *s, size_t len, ध्रुव वर्ण *bad);
स्थिर अन्तःस्थ वर्ण* octescape(ध्रुव वर्ण *s, size_t len) {
        प्रत्यागम octescape_full(s, len, रिक्त);
}
वर्ण* decescape(ध्रुव वर्ण *s, size_t len, ध्रुव वर्ण *bad) _nonnull_if_nonzero_(1, 2);
वर्ण* escape_non_printable_full(ध्रुव वर्ण *str, size_t console_width, XEscapeFlags flags);

वर्ण* shell_escape(ध्रुव वर्ण *s, ध्रुव वर्ण *bad);
वर्ण* shell_maybe_quote(ध्रुव वर्ण *s, ShellEscapeFlags flags);
वर्ण* quote_command_पङ्क्तिः(वर्ण * ध्रुव *argv, ShellEscapeFlags flags);
