/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

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

प्रकारव्याख्या गणना UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

प्रकारव्याख्या गणना ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

पूर्णांक cescape_char(अक्षर c, अक्षर *buf);
अक्षर* cescape_length(स्थिरांक अक्षर *s, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);
स्थिर ओळीत अक्षर* cescape(स्थिरांक अक्षर *s) {
        परत cescape_length(s, SIZE_MAX);
}

पूर्णांक cunescape_one(स्थिरांक अक्षर *p, माप_प्रकार length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(स्थिरांक अक्षर *s, माप_प्रकार length, स्थिरांक अक्षर *prefix, UnescapeFlags flags, अक्षर **ret);
स्थिर ओळीत ssize_t cunescape_length(स्थिरांक अक्षर *s, माप_प्रकार length, UnescapeFlags flags, अक्षर **ret) {
        परत cunescape_length_with_prefix(s, length, रिक्त, flags, ret);
}
स्थिर ओळीत ssize_t cunescape(स्थिरांक अक्षर *s, UnescapeFlags flags, अक्षर **ret) {
        परत cunescape_length(s, SIZE_MAX, flags, ret);
}

प्रकारव्याख्या गणना XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

अक्षर* xescape_full(स्थिरांक अक्षर *s, स्थिरांक अक्षर *bad, माप_प्रकार console_width, XEscapeFlags flags);
स्थिर ओळीत अक्षर* xescape(स्थिरांक अक्षर *s, स्थिरांक अक्षर *bad) {
        परत xescape_full(s, bad, SIZE_MAX, 0);
}
अक्षर* octescape_full(स्थिरांक अक्षर *s, माप_प्रकार len, स्थिरांक अक्षर *bad);
स्थिर ओळीत अक्षर* octescape(स्थिरांक अक्षर *s, माप_प्रकार len) {
        परत octescape_full(s, len, रिक्त);
}
अक्षर* decescape(स्थिरांक अक्षर *s, माप_प्रकार len, स्थिरांक अक्षर *bad) _nonnull_if_nonzero_(1, 2);
अक्षर* escape_non_printable_full(स्थिरांक अक्षर *str, माप_प्रकार console_width, XEscapeFlags flags);

अक्षर* shell_escape(स्थिरांक अक्षर *s, स्थिरांक अक्षर *bad);
अक्षर* shell_maybe_quote(स्थिरांक अक्षर *s, ShellEscapeFlags flags);
अक्षर* quote_command_ओळ(अक्षर * स्थिरांक *argv, ShellEscapeFlags flags);
