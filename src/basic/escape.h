/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#घोषणा SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#घोषणा SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#घोषणा SHELL_NEED_ESCAPE_POSIX "\\\'"

प्रकार क्रमागत UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

प्रकार क्रमागत ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

पूर्णांक cescape_char(अक्षर c, अक्षर *buf);
अक्षर* cescape_लंबाई(स्थिर अक्षर *s, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);
अटल अंतरभूत अक्षर* cescape(स्थिर अक्षर *s) {
        वापस cescape_लंबाई(s, SIZE_MAX);
}

पूर्णांक cunescape_one(स्थिर अक्षर *p, माप_प्रकार length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_लंबाई_with_prefix(स्थिर अक्षर *s, माप_प्रकार length, स्थिर अक्षर *prefix, UnescapeFlags flags, अक्षर **ret);
अटल अंतरभूत ssize_t cunescape_लंबाई(स्थिर अक्षर *s, माप_प्रकार length, UnescapeFlags flags, अक्षर **ret) {
        वापस cunescape_लंबाई_with_prefix(s, length, शून्य, flags, ret);
}
अटल अंतरभूत ssize_t cunescape(स्थिर अक्षर *s, UnescapeFlags flags, अक्षर **ret) {
        वापस cunescape_लंबाई(s, SIZE_MAX, flags, ret);
}

प्रकार क्रमागत XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

अक्षर* xescape_full(स्थिर अक्षर *s, स्थिर अक्षर *bad, माप_प्रकार console_width, XEscapeFlags flags);
अटल अंतरभूत अक्षर* xescape(स्थिर अक्षर *s, स्थिर अक्षर *bad) {
        वापस xescape_full(s, bad, SIZE_MAX, 0);
}
अक्षर* octescape_full(स्थिर अक्षर *s, माप_प्रकार len, स्थिर अक्षर *bad);
अटल अंतरभूत अक्षर* octescape(स्थिर अक्षर *s, माप_प्रकार len) {
        वापस octescape_full(s, len, शून्य);
}
अक्षर* decescape(स्थिर अक्षर *s, माप_प्रकार len, स्थिर अक्षर *bad) _nonnull_if_nonzero_(1, 2);
अक्षर* escape_non_printable_full(स्थिर अक्षर *str, माप_प्रकार console_width, XEscapeFlags flags);

अक्षर* shell_escape(स्थिर अक्षर *s, स्थिर अक्षर *bad);
अक्षर* shell_maybe_quote(स्थिर अक्षर *s, ShellEscapeFlags flags);
अक्षर* quote_command_पंक्ति(अक्षर * स्थिर *argv, ShellEscapeFlags flags);
