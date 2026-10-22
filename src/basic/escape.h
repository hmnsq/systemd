/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

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

வகைவரை எண்ணிக்கை UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

வகைவரை எண்ணிக்கை ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_EMPTY = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

முழு cescape_char(எழுத்து c, எழுத்து *buf);
எழுத்து* cescape_length(மாறிலி எழுத்து *s, size_t n) _nonnull_if_nonzero_(1, 2);
நிலையான உள்வரி எழுத்து* cescape(மாறிலி எழுத்து *s) {
        திரும்பு cescape_length(s, SIZE_MAX);
}

முழு cunescape_one(மாறிலி எழுத்து *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul);

ssize_t cunescape_length_with_prefix(மாறிலி எழுத்து *s, size_t length, மாறிலி எழுத்து *prefix, UnescapeFlags flags, எழுத்து **ret);
நிலையான உள்வரி ssize_t cunescape_length(மாறிலி எழுத்து *s, size_t length, UnescapeFlags flags, எழுத்து **ret) {
        திரும்பு cunescape_length_with_prefix(s, length, வெற்று, flags, ret);
}
நிலையான உள்வரி ssize_t cunescape(மாறிலி எழுத்து *s, UnescapeFlags flags, எழுத்து **ret) {
        திரும்பு cunescape_length(s, SIZE_MAX, flags, ret);
}

வகைவரை எண்ணிக்கை XEscapeFlags {
        XESCAPE_8_BIT          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

எழுத்து* xescape_full(மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad, size_t console_width, XEscapeFlags flags);
நிலையான உள்வரி எழுத்து* xescape(மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad) {
        திரும்பு xescape_full(s, bad, SIZE_MAX, 0);
}
எழுத்து* octescape_full(மாறிலி எழுத்து *s, size_t len, மாறிலி எழுத்து *bad);
நிலையான உள்வரி எழுத்து* octescape(மாறிலி எழுத்து *s, size_t len) {
        திரும்பு octescape_full(s, len, வெற்று);
}
எழுத்து* decescape(மாறிலி எழுத்து *s, size_t len, மாறிலி எழுத்து *bad) _nonnull_if_nonzero_(1, 2);
எழுத்து* escape_non_printable_full(மாறிலி எழுத்து *str, size_t console_width, XEscapeFlags flags);

எழுத்து* shell_escape(மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad);
எழுத்து* shell_maybe_quote(மாறிலி எழுத்து *s, ShellEscapeFlags flags);
எழுத்து* quote_command_வரி(எழுத்து * மாறிலி *argv, ShellEscapeFlags flags);
