/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SHELL_NEED_ESCAPE_POSIX "\\\'"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_ಖಾಲಿಗಳು = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

ಪೂರ್ಣಾಂಕ cescape_char(ಅಕ್ಷರ c, ಅಕ್ಷರ *buf);
ಅಕ್ಷರ* cescape_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* cescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ cescape_ಉದ್ದಳತೆ(s, SIZE_MAX);
}

ಪೂರ್ಣಾಂಕ cunescape_one(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t length, char32_t *ret, bool *eight_ಬಿಟ್, bool accept_nul);

ssize_t cunescape_ಉದ್ದಳತೆ_with_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, UnescapeFlags flags, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ssize_t cunescape_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length, UnescapeFlags flags, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ cunescape_ಉದ್ದಳತೆ_with_ಪೂರ್ವಪ್ರತ್ಯಯ(s, length, ಖಾಲಿ, flags, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ssize_t cunescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, UnescapeFlags flags, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ cunescape_ಉದ್ದಳತೆ(s, SIZE_MAX, flags, ret);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ XEscapeFlags {
        XESCAPE_8_ಬಿಟ್          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

ಅಕ್ಷರ* xescape_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad, size_t console_ಅಗಲ, XEscapeFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* xescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಹಿಂತಿರುಗಿ xescape_ತುಂಬಿದ(s, bad, SIZE_MAX, 0);
}
ಅಕ್ಷರ* octescape_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* octescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len) {
        ಹಿಂತಿರುಗಿ octescape_ತುಂಬಿದ(s, len, ಖಾಲಿ);
}
ಅಕ್ಷರ* decescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) _nonnull_if_nonzero_(1, 2);
ಅಕ್ಷರ* escape_non_printable_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t console_ಅಗಲ, XEscapeFlags flags);

ಅಕ್ಷರ* shell_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad);
ಅಕ್ಷರ* shell_maybe_quote(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ShellEscapeFlags flags);
ಅಕ್ಷರ* quote_command_ಸಾಲು(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *argv, ShellEscapeFlags flags);
