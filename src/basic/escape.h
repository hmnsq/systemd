/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

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

প্রকারলেখ গণনা UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

প্রকারলেখ গণনা ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

পূর্ণসংখ্যা cescape_char(অক্ষর c, অক্ষর *buf);
অক্ষর* cescape_দৈর্ঘ্য(ধ্রুবক অক্ষর *s, size_t n) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃরেখ অক্ষর* cescape(ধ্রুবক অক্ষর *s) {
        ফেরত cescape_দৈর্ঘ্য(s, SIZE_MAX);
}

পূর্ণসংখ্যা cunescape_one(ধ্রুবক অক্ষর *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_দৈর্ঘ্য_with_prefix(ধ্রুবক অক্ষর *s, size_t length, ধ্রুবক অক্ষর *prefix, UnescapeFlags flags, অক্ষর **ret);
স্থির অন্তঃরেখ ssize_t cunescape_দৈর্ঘ্য(ধ্রুবক অক্ষর *s, size_t length, UnescapeFlags flags, অক্ষর **ret) {
        ফেরত cunescape_দৈর্ঘ্য_with_prefix(s, length, ফাঁকা, flags, ret);
}
স্থির অন্তঃরেখ ssize_t cunescape(ধ্রুবক অক্ষর *s, UnescapeFlags flags, অক্ষর **ret) {
        ফেরত cunescape_দৈর্ঘ্য(s, SIZE_MAX, flags, ret);
}

প্রকারলেখ গণনা XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

অক্ষর* xescape_full(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad, size_t console_width, XEscapeFlags flags);
স্থির অন্তঃরেখ অক্ষর* xescape(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad) {
        ফেরত xescape_full(s, bad, SIZE_MAX, 0);
}
অক্ষর* octescape_full(ধ্রুবক অক্ষর *s, size_t len, ধ্রুবক অক্ষর *bad);
স্থির অন্তঃরেখ অক্ষর* octescape(ধ্রুবক অক্ষর *s, size_t len) {
        ফেরত octescape_full(s, len, ফাঁকা);
}
অক্ষর* decescape(ধ্রুবক অক্ষর *s, size_t len, ধ্রুবক অক্ষর *bad) _nonnull_if_nonzero_(1, 2);
অক্ষর* escape_non_printable_full(ধ্রুবক অক্ষর *str, size_t console_width, XEscapeFlags flags);

অক্ষর* shell_escape(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad);
অক্ষর* shell_maybe_quote(ধ্রুবক অক্ষর *s, ShellEscapeFlags flags);
অক্ষর* quote_command_সারি(অক্ষর * ধ্রুবক *argv, ShellEscapeFlags flags);
