/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

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

تعريفنوع تعداد UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

تعريفنوع تعداد ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

صحيح cescape_char(حرف c, حرف *buf);
حرف* cescape_length(ثابت حرف *s, size_t n) _nonnull_if_nonzero_(1, 2);
ساكن مضمن حرف* cescape(ثابت حرف *s) {
        ܦܢܐ cescape_length(s, SIZE_MAX);
}

صحيح cunescape_one(ثابت حرف *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ثابت حرف *s, size_t length, ثابت حرف *prefix, UnescapeFlags flags, حرف **ret);
ساكن مضمن ssize_t cunescape_length(ثابت حرف *s, size_t length, UnescapeFlags flags, حرف **ret) {
        ܦܢܐ cunescape_length_with_prefix(s, length, عدم, flags, ret);
}
ساكن مضمن ssize_t cunescape(ثابت حرف *s, UnescapeFlags flags, حرف **ret) {
        ܦܢܐ cunescape_length(s, SIZE_MAX, flags, ret);
}

تعريفنوع تعداد XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

حرف* xescape_full(ثابت حرف *s, ثابت حرف *bad, size_t console_width, XEscapeFlags flags);
ساكن مضمن حرف* xescape(ثابت حرف *s, ثابت حرف *bad) {
        ܦܢܐ xescape_full(s, bad, SIZE_MAX, 0);
}
حرف* octescape_full(ثابت حرف *s, size_t len, ثابت حرف *bad);
ساكن مضمن حرف* octescape(ثابت حرف *s, size_t len) {
        ܦܢܐ octescape_full(s, len, عدم);
}
حرف* decescape(ثابت حرف *s, size_t len, ثابت حرف *bad) _nonnull_if_nonzero_(1, 2);
حرف* escape_non_printable_full(ثابت حرف *str, size_t console_width, XEscapeFlags flags);

حرف* shell_escape(ثابت حرف *s, ثابت حرف *bad);
حرف* shell_maybe_quote(ثابت حرف *s, ShellEscapeFlags flags);
حرف* quote_command_line(حرف * ثابت *argv, ShellEscapeFlags flags);
