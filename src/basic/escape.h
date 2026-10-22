/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* What characters are special in the shell? */
/* must be escaped outside and inside double-quotes */
#నిర్వచించు SHELL_NEED_ESCAPE "\"\\`$"

/* Those that can be escaped or double-quoted.
 *
 * Strictly speaking, ! does not need to be escaped, except in interactive
 * mode, but let's be extra nice to the user and quote ! in case this
 * output is ever used in interactive mode. */
#నిర్వచించు SHELL_NEED_QUOTES SHELL_NEED_ESCAPE GLOB_CHARS "'()<>|&;!"

/* Note that we assume control characters would need to be escaped too in
 * addition to the "special" characters listed here, if they appear in the
 * string. Current users disallow control characters. Also '"' shall not
 * be escaped.
 */
#నిర్వచించు SHELL_NEED_ESCAPE_POSIX "\\\'"

రకనిర్వచనం గణన UnescapeFlags {
        UNESCAPE_RELAX      = 1 << 0,
        UNESCAPE_ACCEPT_NUL = 1 << 1,
} UnescapeFlags;

రకనిర్వచనం గణన ShellEscapeFlags {
        /* The default is to add shell quotes ("") so the shell will consider this a single argument.
         * Tabs and newlines are escaped. */

        SHELL_ESCAPE_POSIX = 1 << 1, /* Use POSIX shell escape syntax (a string enclosed in $'') instead of plain quotes. */
        SHELL_ESCAPE_ఖాళీలు = 1 << 2, /* Format empty arguments as "". */
} ShellEscapeFlags;

పూర్ణాంకం cescape_char(అక్షరం c, అక్షరం *buf);
అక్షరం* cescape_నిడివి(స్థిరరాశి అక్షరం *s, size_t n) _nonnull_if_nonzero_(1, 2);
స్థిర అంతఃవరుస అక్షరం* cescape(స్థిరరాశి అక్షరం *s) {
        తిరిగి cescape_నిడివి(s, SIZE_MAX);
}

పూర్ణాంకం cunescape_one(స్థిరరాశి అక్షరం *p, size_t length, char32_t *ret, bool *eight_బిట్టు, bool accept_nul);

ssize_t cunescape_నిడివి_with_ఉపసర్గ(స్థిరరాశి అక్షరం *s, size_t length, స్థిరరాశి అక్షరం *ఉపసర్గ, UnescapeFlags flags, అక్షరం **ret);
స్థిర అంతఃవరుస ssize_t cunescape_నిడివి(స్థిరరాశి అక్షరం *s, size_t length, UnescapeFlags flags, అక్షరం **ret) {
        తిరిగి cunescape_నిడివి_with_ఉపసర్గ(s, length, ఖాళీ, flags, ret);
}
స్థిర అంతఃవరుస ssize_t cunescape(స్థిరరాశి అక్షరం *s, UnescapeFlags flags, అక్షరం **ret) {
        తిరిగి cunescape_నిడివి(s, SIZE_MAX, flags, ret);
}

రకనిర్వచనం గణన XEscapeFlags {
        XESCAPE_8_బిట్టు          = 1 << 0,
        XESCAPE_FORCE_ELLIPSIS = 1 << 1,
} XEscapeFlags;

అక్షరం* xescape_నిండిన(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad, size_t console_వెడల్పు, XEscapeFlags flags);
స్థిర అంతఃవరుస అక్షరం* xescape(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad) {
        తిరిగి xescape_నిండిన(s, bad, SIZE_MAX, 0);
}
అక్షరం* octescape_నిండిన(స్థిరరాశి అక్షరం *s, size_t len, స్థిరరాశి అక్షరం *bad);
స్థిర అంతఃవరుస అక్షరం* octescape(స్థిరరాశి అక్షరం *s, size_t len) {
        తిరిగి octescape_నిండిన(s, len, ఖాళీ);
}
అక్షరం* decescape(స్థిరరాశి అక్షరం *s, size_t len, స్థిరరాశి అక్షరం *bad) _nonnull_if_nonzero_(1, 2);
అక్షరం* escape_non_printable_నిండిన(స్థిరరాశి అక్షరం *str, size_t console_వెడల్పు, XEscapeFlags flags);

అక్షరం* shell_escape(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad);
అక్షరం* shell_maybe_quote(స్థిరరాశి అక్షరం *s, ShellEscapeFlags flags);
అక్షరం* quote_command_పంక్తి(అక్షరం * స్థిరరాశి *argv, ShellEscapeFlags flags);
