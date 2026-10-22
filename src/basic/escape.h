/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHELL_NEED_ESCAPE_POSIX "\\\'"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_ਖਾਲੀ_ਥਾਵਾਂ = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

ਪੂਰਨ cescape_char(ਅੱਖਰ c, ਅੱਖਰ *buf);
ਅੱਖਰ* cescape_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *s, size_t n) _nonnull_if_nonzero_(1, 2);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* cescape(ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ cescape_ਲੰਮਾਈ(s, SIZE_MAX);
}

ਪੂਰਨ cunescape_one(ਅਚਲ ਅੱਖਰ *p, size_t length, char32_t *ret, bool *eight_ਦੋਆਧਾਰੀ_ਅੰਕ, bool accept_nul);

ssize_t cunescape_ਲੰਮਾਈ_with_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *s, size_t length, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, UnescapeFlags flags, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ssize_t cunescape_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *s, size_t length, UnescapeFlags flags, ਅੱਖਰ **ret) {
        ਵਾਪਸ cunescape_ਲੰਮਾਈ_with_ਅਗੇਤਰ(s, length, ਨਿਰਾ, flags, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ssize_t cunescape(ਅਚਲ ਅੱਖਰ *s, UnescapeFlags flags, ਅੱਖਰ **ret) {
        ਵਾਪਸ cunescape_ਲੰਮਾਈ(s, SIZE_MAX, flags, ret);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ XEscapeFlags {
        XESCAPE_8_ਦੋਆਧਾਰੀ_ਅੰਕ          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

ਅੱਖਰ* xescape_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad, size_t console_ਚੌੜਾਈ, XEscapeFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* xescape(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad) {
        ਵਾਪਸ xescape_ਭਰਿਆ(s, bad, SIZE_MAX, 0);
}
ਅੱਖਰ* octescape_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, size_t len, ਅਚਲ ਅੱਖਰ *bad);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* octescape(ਅਚਲ ਅੱਖਰ *s, size_t len) {
        ਵਾਪਸ octescape_ਭਰਿਆ(s, len, ਨਿਰਾ);
}
ਅੱਖਰ* decescape(ਅਚਲ ਅੱਖਰ *s, size_t len, ਅਚਲ ਅੱਖਰ *bad) _nonnull_if_nonzero_(1, 2);
ਅੱਖਰ* escape_non_printable_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *str, size_t console_ਚੌੜਾਈ, XEscapeFlags flags);

ਅੱਖਰ* shell_escape(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad);
ਅੱਖਰ* shell_maybe_quote(ਅਚਲ ਅੱਖਰ *s, ShellEscapeFlags flags);
ਅੱਖਰ* quote_command_ਸਤਰ(ਅੱਖਰ * ਅਚਲ *argv, ShellEscapeFlags flags);
