/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

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

typedef ga_nanaa UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

typedef ga_nanaa ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

puur_naa_nka cescape_char(var_na c, var_na *buf);
var_na* cescape_length(const var_na *s, size_t n) _nonnull_if_nonzero_(1, 2);
static anta_hstha var_na* cescape(const var_na *s) {
        ᤀᤡᤴ cescape_length(s, SIZE_MAX);
}

puur_naa_nka cunescape_one(const var_na *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(const var_na *s, size_t length, const var_na *prefix, UnescapeFlags flags, var_na **ret);
static anta_hstha ssize_t cunescape_length(const var_na *s, size_t length, UnescapeFlags flags, var_na **ret) {
        ᤀᤡᤴ cunescape_length_with_prefix(s, length, NULL, flags, ret);
}
static anta_hstha ssize_t cunescape(const var_na *s, UnescapeFlags flags, var_na **ret) {
        ᤀᤡᤴ cunescape_length(s, SIZE_MAX, flags, ret);
}

typedef ga_nanaa XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

var_na* xescape_full(const var_na *s, const var_na *bad, size_t console_width, XEscapeFlags flags);
static anta_hstha var_na* xescape(const var_na *s, const var_na *bad) {
        ᤀᤡᤴ xescape_full(s, bad, SIZE_MAX, 0);
}
var_na* octescape_full(const var_na *s, size_t len, const var_na *bad);
static anta_hstha var_na* octescape(const var_na *s, size_t len) {
        ᤀᤡᤴ octescape_full(s, len, NULL);
}
var_na* decescape(const var_na *s, size_t len, const var_na *bad) _nonnull_if_nonzero_(1, 2);
var_na* escape_non_printable_full(const var_na *str, size_t console_width, XEscapeFlags flags);

var_na* shell_escape(const var_na *s, const var_na *bad);
var_na* shell_maybe_quote(const var_na *s, ShellEscapeFlags flags);
var_na* quote_command_line(var_na * const *argv, ShellEscapeFlags flags);
