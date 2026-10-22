/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

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

প্রকারনির্দেশ গণনা UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

প্রকারনির্দেশ গণনা ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

পূর্ণাঙ্ক cescape_char(ৱর্ণ c, ৱর্ণ *buf);
ৱর্ণ* cescape_length(ধ্রুৱ ৱর্ণ *s, size_t n) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃস্থ ৱর্ণ* cescape(ধ্রুৱ ৱর্ণ *s) {
        ঘূৰাই cescape_length(s, SIZE_MAX);
}

পূর্ণাঙ্ক cunescape_one(ধ্রুৱ ৱর্ণ *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ধ্রুৱ ৱর্ণ *s, size_t length, ধ্রুৱ ৱর্ণ *prefix, UnescapeFlags flags, ৱর্ণ **ret);
স্থির অন্তঃস্থ ssize_t cunescape_length(ধ্রুৱ ৱর্ণ *s, size_t length, UnescapeFlags flags, ৱর্ণ **ret) {
        ঘূৰাই cunescape_length_with_prefix(s, length, রিক্ত, flags, ret);
}
স্থির অন্তঃস্থ ssize_t cunescape(ধ্রুৱ ৱর্ণ *s, UnescapeFlags flags, ৱর্ণ **ret) {
        ঘূৰাই cunescape_length(s, SIZE_MAX, flags, ret);
}

প্রকারনির্দেশ গণনা XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

ৱর্ণ* xescape_full(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad, size_t console_width, XEscapeFlags flags);
স্থির অন্তঃস্থ ৱর্ণ* xescape(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad) {
        ঘূৰাই xescape_full(s, bad, SIZE_MAX, 0);
}
ৱর্ণ* octescape_full(ধ্রুৱ ৱর্ণ *s, size_t len, ধ্রুৱ ৱর্ণ *bad);
স্থির অন্তঃস্থ ৱর্ণ* octescape(ধ্রুৱ ৱর্ণ *s, size_t len) {
        ঘূৰাই octescape_full(s, len, রিক্ত);
}
ৱর্ণ* decescape(ধ্রুৱ ৱর্ণ *s, size_t len, ধ্রুৱ ৱর্ণ *bad) _nonnull_if_nonzero_(1, 2);
ৱর্ণ* escape_non_printable_full(ধ্রুৱ ৱর্ণ *str, size_t console_width, XEscapeFlags flags);

ৱর্ণ* shell_escape(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad);
ৱর্ণ* shell_maybe_quote(ধ্রুৱ ৱর্ণ *s, ShellEscapeFlags flags);
ৱর্ণ* quote_command_সাৰি(ৱর্ণ * ধ্রুৱ *argv, ShellEscapeFlags flags);
