/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

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

પ્રકારવ્યાખ્યા ગણના UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

પ્રકારવ્યાખ્યા ગણના ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

પૂર્ણાંક cescape_char(અક્ષર c, અક્ષર *buf);
અક્ષર* cescape_length(અચલ અક્ષર *s, size_t n) _nonnull_if_nonzero_(1, 2);
સ્થિર અંતઃરેખ અક્ષર* cescape(અચલ અક્ષર *s) {
        પાછા cescape_length(s, SIZE_MAX);
}

પૂર્ણાંક cunescape_one(અચલ અક્ષર *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(અચલ અક્ષર *s, size_t length, અચલ અક્ષર *prefix, UnescapeFlags flags, અક્ષર **ret);
સ્થિર અંતઃરેખ ssize_t cunescape_length(અચલ અક્ષર *s, size_t length, UnescapeFlags flags, અક્ષર **ret) {
        પાછા cunescape_length_with_prefix(s, length, ખાલી, flags, ret);
}
સ્થિર અંતઃરેખ ssize_t cunescape(અચલ અક્ષર *s, UnescapeFlags flags, અક્ષર **ret) {
        પાછા cunescape_length(s, SIZE_MAX, flags, ret);
}

પ્રકારવ્યાખ્યા ગણના XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

અક્ષર* xescape_full(અચલ અક્ષર *s, અચલ અક્ષર *bad, size_t console_width, XEscapeFlags flags);
સ્થિર અંતઃરેખ અક્ષર* xescape(અચલ અક્ષર *s, અચલ અક્ષર *bad) {
        પાછા xescape_full(s, bad, SIZE_MAX, 0);
}
અક્ષર* octescape_full(અચલ અક્ષર *s, size_t len, અચલ અક્ષર *bad);
સ્થિર અંતઃરેખ અક્ષર* octescape(અચલ અક્ષર *s, size_t len) {
        પાછા octescape_full(s, len, ખાલી);
}
અક્ષર* decescape(અચલ અક્ષર *s, size_t len, અચલ અક્ષર *bad) _nonnull_if_nonzero_(1, 2);
અક્ષર* escape_non_printable_full(અચલ અક્ષર *str, size_t console_width, XEscapeFlags flags);

અક્ષર* shell_escape(અચલ અક્ષર *s, અચલ અક્ષર *bad);
અક્ષર* shell_maybe_quote(અચલ અક્ષર *s, ShellEscapeFlags flags);
અક્ષર* quote_command_લીટી(અક્ષર * અચલ *argv, ShellEscapeFlags flags);
