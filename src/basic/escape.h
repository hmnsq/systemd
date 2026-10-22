/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#تعریف_کړه SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#تعریف_کړه SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#تعریف_کړه SHELL_NEED_ESCAPE_POSIX "\\\'"

تعريفنوع تعداد UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

تعريفنوع تعداد ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_تشه = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

صحيح cescape_char(حرف c, حرف *buf);
حرف* cescape_اوږدتیا(ثابت حرف *s, size_t n) _nonnull_if_nonzero_(1, 2);
ساكن مضمن حرف* cescape(ثابت حرف *s) {
        أرجع cescape_اوږدتیا(s, SIZE_MAX);
}

صحيح cunescape_one(ثابت حرف *p, size_t length, char32_t *ret, bool *eight_بټ, bool accept_nul);

ssize_t cunescape_اوږدتیا_with_مختاړی(ثابت حرف *s, size_t length, ثابت حرف *مختاړی, UnescapeFlags flags, حرف **ret);
ساكن مضمن ssize_t cunescape_اوږدتیا(ثابت حرف *s, size_t length, UnescapeFlags flags, حرف **ret) {
        أرجع cunescape_اوږدتیا_with_مختاړی(s, length, عدم, flags, ret);
}
ساكن مضمن ssize_t cunescape(ثابت حرف *s, UnescapeFlags flags, حرف **ret) {
        أرجع cunescape_اوږدتیا(s, SIZE_MAX, flags, ret);
}

تعريفنوع تعداد XEscapeFlags {
        XESCAPE_8_بټ          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

حرف* xescape_ډک(ثابت حرف *s, ثابت حرف *bad, size_t console_پلنوالی, XEscapeFlags flags);
ساكن مضمن حرف* xescape(ثابت حرف *s, ثابت حرف *bad) {
        أرجع xescape_ډک(s, bad, SIZE_MAX, 0);
}
حرف* octescape_ډک(ثابت حرف *s, size_t len, ثابت حرف *bad);
ساكن مضمن حرف* octescape(ثابت حرف *s, size_t len) {
        أرجع octescape_ډک(s, len, عدم);
}
حرف* decescape(ثابت حرف *s, size_t len, ثابت حرف *bad) _nonnull_if_nonzero_(1, 2);
حرف* escape_non_printable_ډک(ثابت حرف *str, size_t console_پلنوالی, XEscapeFlags flags);

حرف* shell_escape(ثابت حرف *s, ثابت حرف *bad);
حرف* shell_maybe_quote(ثابت حرف *s, ShellEscapeFlags flags);
حرف* quote_command_کرښه(حرف * ثابت *argv, ShellEscapeFlags flags);
