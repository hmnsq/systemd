/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

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

തരനിർവചനം എണ്ണൽ UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

തരനിർവചനം എണ്ണൽ ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

പൂർണ്ണസംഖ്യ cescape_char(അക്ഷരം c, അക്ഷരം *buf);
അക്ഷരം* cescape_length(സ്ഥിരം അക്ഷരം *s, size_t n) _nonnull_if_nonzero_(1, 2);
സ്ഥിതം വരിയിൽ അക്ഷരം* cescape(സ്ഥിരം അക്ഷരം *s) {
        തിരികെ cescape_length(s, SIZE_MAX);
}

പൂർണ്ണസംഖ്യ cunescape_one(സ്ഥിരം അക്ഷരം *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(സ്ഥിരം അക്ഷരം *s, size_t length, സ്ഥിരം അക്ഷരം *prefix, UnescapeFlags flags, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ ssize_t cunescape_length(സ്ഥിരം അക്ഷരം *s, size_t length, UnescapeFlags flags, അക്ഷരം **ret) {
        തിരികെ cunescape_length_with_prefix(s, length, ശൂന്യ, flags, ret);
}
സ്ഥിതം വരിയിൽ ssize_t cunescape(സ്ഥിരം അക്ഷരം *s, UnescapeFlags flags, അക്ഷരം **ret) {
        തിരികെ cunescape_length(s, SIZE_MAX, flags, ret);
}

തരനിർവചനം എണ്ണൽ XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

അക്ഷരം* xescape_full(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad, size_t console_width, XEscapeFlags flags);
സ്ഥിതം വരിയിൽ അക്ഷരം* xescape(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad) {
        തിരികെ xescape_full(s, bad, SIZE_MAX, 0);
}
അക്ഷരം* octescape_full(സ്ഥിരം അക്ഷരം *s, size_t len, സ്ഥിരം അക്ഷരം *bad);
സ്ഥിതം വരിയിൽ അക്ഷരം* octescape(സ്ഥിരം അക്ഷരം *s, size_t len) {
        തിരികെ octescape_full(s, len, ശൂന്യ);
}
അക്ഷരം* decescape(സ്ഥിരം അക്ഷരം *s, size_t len, സ്ഥിരം അക്ഷരം *bad) _nonnull_if_nonzero_(1, 2);
അക്ഷരം* escape_non_printable_full(സ്ഥിരം അക്ഷരം *str, size_t console_width, XEscapeFlags flags);

അക്ഷരം* shell_escape(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad);
അക്ഷരം* shell_maybe_quote(സ്ഥിരം അക്ഷരം *s, ShellEscapeFlags flags);
അക്ഷരം* quote_command_വരി(അക്ഷരം * സ്ഥിരം *argv, ShellEscapeFlags flags);
