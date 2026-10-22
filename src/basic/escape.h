/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cescape_char(ଅକ୍ଷର c, ଅକ୍ଷର *buf);
ଅକ୍ଷର* cescape_length(ଅଚଳ ଅକ୍ଷର *s, size_t n) _nonnull_if_nonzero_(1, 2);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* cescape(ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ cescape_length(s, SIZE_MAX);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cunescape_one(ଅଚଳ ଅକ୍ଷର *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(ଅଚଳ ଅକ୍ଷର *s, size_t length, ଅଚଳ ଅକ୍ଷର *prefix, UnescapeFlags flags, ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ssize_t cunescape_length(ଅଚଳ ଅକ୍ଷର *s, size_t length, UnescapeFlags flags, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ cunescape_length_with_prefix(s, length, ଖାଲି, flags, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ssize_t cunescape(ଅଚଳ ଅକ୍ଷର *s, UnescapeFlags flags, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ cunescape_length(s, SIZE_MAX, flags, ret);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

ଅକ୍ଷର* xescape_full(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad, size_t console_width, XEscapeFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* xescape(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad) {
        ଫେରନ୍ତୁ xescape_full(s, bad, SIZE_MAX, 0);
}
ଅକ୍ଷର* octescape_full(ଅଚଳ ଅକ୍ଷର *s, size_t len, ଅଚଳ ଅକ୍ଷର *bad);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* octescape(ଅଚଳ ଅକ୍ଷର *s, size_t len) {
        ଫେରନ୍ତୁ octescape_full(s, len, ଖାଲି);
}
ଅକ୍ଷର* decescape(ଅଚଳ ଅକ୍ଷର *s, size_t len, ଅଚଳ ଅକ୍ଷର *bad) _nonnull_if_nonzero_(1, 2);
ଅକ୍ଷର* escape_non_printable_full(ଅଚଳ ଅକ୍ଷର *str, size_t console_width, XEscapeFlags flags);

ଅକ୍ଷର* shell_escape(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad);
ଅକ୍ଷର* shell_maybe_quote(ଅଚଳ ଅକ୍ଷର *s, ShellEscapeFlags flags);
ଅକ୍ଷର* quote_command_ଧାଡ଼ି(ଅକ୍ଷର * ଅଚଳ *argv, ShellEscapeFlags flags);
