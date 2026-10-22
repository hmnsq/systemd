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

পূর্ণাঙ্ক cescape_char(বর্ণ c, বর্ণ *buf);
বর্ণ* cescape_length(ধ্রুব বর্ণ *s, size_t n) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃস্থ বর্ণ* cescape(ধ্রুব বর্ণ *s) {
        প্রত্যাগম cescape_length(s, SIZE_MAX);
}

পূর্ণাঙ্ক cunescape_one(ধ্রুব বর্ণ *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ধ্রুব বর্ণ *s, size_t length, ধ্রুব বর্ণ *prefix, UnescapeFlags flags, বর্ণ **ret);
স্থির অন্তঃস্থ ssize_t cunescape_length(ধ্রুব বর্ণ *s, size_t length, UnescapeFlags flags, বর্ণ **ret) {
        প্রত্যাগম cunescape_length_with_prefix(s, length, রিক্ত, flags, ret);
}
স্থির অন্তঃস্থ ssize_t cunescape(ধ্রুব বর্ণ *s, UnescapeFlags flags, বর্ণ **ret) {
        প্রত্যাগম cunescape_length(s, SIZE_MAX, flags, ret);
}

প্রকারনির্দেশ গণনা XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

বর্ণ* xescape_full(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *bad, size_t console_width, XEscapeFlags flags);
স্থির অন্তঃস্থ বর্ণ* xescape(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *bad) {
        প্রত্যাগম xescape_full(s, bad, SIZE_MAX, 0);
}
বর্ণ* octescape_full(ধ্রুব বর্ণ *s, size_t len, ধ্রুব বর্ণ *bad);
স্থির অন্তঃস্থ বর্ণ* octescape(ধ্রুব বর্ণ *s, size_t len) {
        প্রত্যাগম octescape_full(s, len, রিক্ত);
}
বর্ণ* decescape(ধ্রুব বর্ণ *s, size_t len, ধ্রুব বর্ণ *bad) _nonnull_if_nonzero_(1, 2);
বর্ণ* escape_non_printable_full(ধ্রুব বর্ণ *str, size_t console_width, XEscapeFlags flags);

বর্ণ* shell_escape(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *bad);
বর্ণ* shell_maybe_quote(ধ্রুব বর্ণ *s, ShellEscapeFlags flags);
বর্ণ* quote_command_সারি(বর্ণ * ধ্রুব *argv, ShellEscapeFlags flags);
