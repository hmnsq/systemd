/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#تعریف_کرو SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#تعریف_کرو SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#تعریف_کرو SHELL_NEED_ESCAPE_POSIX "\\\'"

قسمتعریف شمار UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

قسمتعریف شمار ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_خالی_خانے = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

صحیح cescape_char(حرف c, حرف *buf);
حرف* cescape_لمبائی(ثابت حرف *s, size_t n) _nonnull_if_nonzero_(1, 2);
ساکن اندرونی حرف* cescape(ثابت حرف *s) {
        واپس cescape_لمبائی(s, SIZE_MAX);
}

صحیح cunescape_one(ثابت حرف *p, size_t length, char32_t *ret, bool *eight_ثنائی_ہندسہ, bool accept_nul);

ssize_t cunescape_لمبائی_with_سابقہ(ثابت حرف *s, size_t length, ثابت حرف *سابقہ, UnescapeFlags flags, حرف **ret);
ساکن اندرونی ssize_t cunescape_لمبائی(ثابت حرف *s, size_t length, UnescapeFlags flags, حرف **ret) {
        واپس cunescape_لمبائی_with_سابقہ(s, length, تہی, flags, ret);
}
ساکن اندرونی ssize_t cunescape(ثابت حرف *s, UnescapeFlags flags, حرف **ret) {
        واپس cunescape_لمبائی(s, SIZE_MAX, flags, ret);
}

قسمتعریف شمار XEscapeFlags {
        XESCAPE_8_ثنائی_ہندسہ          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

حرف* xescape_بھرا(ثابت حرف *s, ثابت حرف *bad, size_t console_چوڑائی, XEscapeFlags flags);
ساکن اندرونی حرف* xescape(ثابت حرف *s, ثابت حرف *bad) {
        واپس xescape_بھرا(s, bad, SIZE_MAX, 0);
}
حرف* octescape_بھرا(ثابت حرف *s, size_t len, ثابت حرف *bad);
ساکن اندرونی حرف* octescape(ثابت حرف *s, size_t len) {
        واپس octescape_بھرا(s, len, تہی);
}
حرف* decescape(ثابت حرف *s, size_t len, ثابت حرف *bad) _nonnull_if_nonzero_(1, 2);
حرف* escape_non_printable_بھرا(ثابت حرف *str, size_t console_چوڑائی, XEscapeFlags flags);

حرف* shell_escape(ثابت حرف *s, ثابت حرف *bad);
حرف* shell_maybe_quote(ثابت حرف *s, ShellEscapeFlags flags);
حرف* quote_command_سطر(حرف * ثابت *argv, ShellEscapeFlags flags);
