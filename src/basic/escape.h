/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

ಪೂರ್ಣಾಙ್ಕ cescape_char(ವರ್ಣ c, ವರ್ಣ *buf);
ವರ್ಣ* cescape_length(ಧ್ರುವ ವರ್ಣ *s, size_t n) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* cescape(ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ cescape_length(s, SIZE_MAX);
}

ಪೂರ್ಣಾಙ್ಕ cunescape_one(ಧ್ರುವ ವರ್ಣ *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ಧ್ರುವ ವರ್ಣ *s, size_t length, ಧ್ರುವ ವರ್ಣ *prefix, UnescapeFlags flags, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ssize_t cunescape_length(ಧ್ರುವ ವರ್ಣ *s, size_t length, UnescapeFlags flags, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ cunescape_length_with_prefix(s, length, ರಿಕ್ತ, flags, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ssize_t cunescape(ಧ್ರುವ ವರ್ಣ *s, UnescapeFlags flags, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ cunescape_length(s, SIZE_MAX, flags, ret);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

ವರ್ಣ* xescape_full(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad, size_t console_width, XEscapeFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* xescape(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad) {
        ಪ್ರತ್ಯಾಗಮ xescape_full(s, bad, SIZE_MAX, 0);
}
ವರ್ಣ* octescape_full(ಧ್ರುವ ವರ್ಣ *s, size_t len, ಧ್ರುವ ವರ್ಣ *bad);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* octescape(ಧ್ರುವ ವರ್ಣ *s, size_t len) {
        ಪ್ರತ್ಯಾಗಮ octescape_full(s, len, ರಿಕ್ತ);
}
ವರ್ಣ* decescape(ಧ್ರುವ ವರ್ಣ *s, size_t len, ಧ್ರುವ ವರ್ಣ *bad) _nonnull_if_nonzero_(1, 2);
ವರ್ಣ* escape_non_printable_full(ಧ್ರುವ ವರ್ಣ *str, size_t console_width, XEscapeFlags flags);

ವರ್ಣ* shell_escape(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad);
ವರ್ಣ* shell_maybe_quote(ಧ್ರುವ ವರ್ಣ *s, ShellEscapeFlags flags);
ವರ್ಣ* quote_command_ಸಾಲು(ವರ್ಣ * ಧ್ರುವ *argv, ShellEscapeFlags flags);
