/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

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

تعریفنوع شمارش UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

تعریفنوع شمارش ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

صحیح cescape_char(نویسه c, نویسه *buf);
نویسه* cescape_length(ثابت نویسه *s, size_t n) _nonnull_if_nonzero_(1, 2);
ایستا درونخطی نویسه* cescape(ثابت نویسه *s) {
        بازگشت cescape_length(s, SIZE_MAX);
}

صحیح cunescape_one(ثابت نویسه *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ثابت نویسه *s, size_t length, ثابت نویسه *prefix, UnescapeFlags flags, نویسه **ret);
ایستا درونخطی ssize_t cunescape_length(ثابت نویسه *s, size_t length, UnescapeFlags flags, نویسه **ret) {
        بازگشت cunescape_length_with_prefix(s, length, پوچ, flags, ret);
}
ایستا درونخطی ssize_t cunescape(ثابت نویسه *s, UnescapeFlags flags, نویسه **ret) {
        بازگشت cunescape_length(s, SIZE_MAX, flags, ret);
}

تعریفنوع شمارش XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

نویسه* xescape_full(ثابت نویسه *s, ثابت نویسه *bad, size_t console_width, XEscapeFlags flags);
ایستا درونخطی نویسه* xescape(ثابت نویسه *s, ثابت نویسه *bad) {
        بازگشت xescape_full(s, bad, SIZE_MAX, 0);
}
نویسه* octescape_full(ثابت نویسه *s, size_t len, ثابت نویسه *bad);
ایستا درونخطی نویسه* octescape(ثابت نویسه *s, size_t len) {
        بازگشت octescape_full(s, len, پوچ);
}
نویسه* decescape(ثابت نویسه *s, size_t len, ثابت نویسه *bad) _nonnull_if_nonzero_(1, 2);
نویسه* escape_non_printable_full(ثابت نویسه *str, size_t console_width, XEscapeFlags flags);

نویسه* shell_escape(ثابت نویسه *s, ثابت نویسه *bad);
نویسه* shell_maybe_quote(ثابت نویسه *s, ShellEscapeFlags flags);
نویسه* quote_command_سطر(نویسه * ثابت *argv, ShellEscapeFlags flags);
