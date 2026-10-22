/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <string.h>

#include "alloc-util.h"
#include "escape.h"
#include "hexdecoct.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

puur_naa_nka cescape_char(var_na c, var_na *buf) {
        var_na *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        switch (c) {

                pak_sa '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        bha_nga;
                pak_sa '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        bha_nga;
                pak_sa '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        bha_nga;
                pak_sa '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        bha_nga;
                pak_sa '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        bha_nga;
                pak_sa '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        bha_nga;
                pak_sa '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        bha_nga;
                pak_sa '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        bha_nga;
                pak_sa '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        bha_nga;
                pak_sa '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        bha_nga;

                default:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ᯀᯮᯝ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned var_na) c >> 6);
                                *(buf++) = octchar((unsigned var_na) c >> 3);
                                *(buf++) = octchar((unsigned var_na) c);
                        } ᯀᯮᯝᯂᯪ
                                *(buf++) = c;
                        bha_nga;
        }

        ᯀᯮᯞ buf - buf_old;
}

var_na* cescape_length(const var_na *s, size_t n) {
        const var_na *f;
        var_na *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        assert(s || n == 0);

        ᯀᯮᯝ (n == SIZE_MAX)
                n = strlen(s);

        ᯀᯮᯝ (n > (SIZE_MAX - 1) / 4)
                ᯀᯮᯞ NULL;

        r = new(var_na, n*4 + 1);
        ᯀᯮᯝ (!r)
                ᯀᯮᯞ NULL;

        for (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ᯀᯮᯞ r;
}

puur_naa_nka cunescape_one(const var_na *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        puur_naa_nka r = 1;

        assert(p);
        assert(ret);
        assert(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ᯀᯮᯝ (length != SIZE_MAX && length < 1)
                ᯀᯮᯞ -EINVAL;

        switch (p[0]) {

        pak_sa 'a':
                *ret = '\a';
                bha_nga;
        pak_sa 'b':
                *ret = '\b';
                bha_nga;
        pak_sa 'f':
                *ret = '\f';
                bha_nga;
        pak_sa 'n':
                *ret = '\n';
                bha_nga;
        pak_sa 'r':
                *ret = '\r';
                bha_nga;
        pak_sa 't':
                *ret = '\t';
                bha_nga;
        pak_sa 'v':
                *ret = '\v';
                bha_nga;
        pak_sa '\\':
                *ret = '\\';
                bha_nga;
        pak_sa '"':
                *ret = '"';
                bha_nga;
        pak_sa '\'':
                *ret = '\'';
                bha_nga;

        pak_sa 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                bha_nga;

        pak_sa 'x': {
                /* hexadecimal encoding */
                puur_naa_nka a, b;

                ᯀᯮᯝ (length != SIZE_MAX && length < 3)
                        ᯀᯮᯞ -EINVAL;

                a = unhexchar(p[1]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unhexchar(p[2]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                /* Don't allow NUL bytes */
                ᯀᯮᯝ (a == 0 && b == 0 && !accept_nul)
                        ᯀᯮᯞ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                bha_nga;
        }

        pak_sa 'u': {
                /* C++11 style 16-bit unicode */

                puur_naa_nka a[4];
                size_t i;
                uint32_t c;

                ᯀᯮᯝ (length != SIZE_MAX && length < 5)
                        ᯀᯮᯞ -EINVAL;

                for (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ᯀᯮᯝ (a[i] < 0)
                                ᯀᯮᯞ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ᯀᯮᯝ (c == 0 && !accept_nul)
                        ᯀᯮᯞ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ᯀᯮᯝ (utf16_is_surrogate(c))
                        ᯀᯮᯞ -EINVAL;

                *ret = c;
                r = 5;
                bha_nga;
        }

        pak_sa 'U': {
                /* C++11 style 32-bit unicode */

                puur_naa_nka a[8];
                size_t i;
                char32_t c;

                ᯀᯮᯝ (length != SIZE_MAX && length < 9)
                        ᯀᯮᯞ -EINVAL;

                for (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ᯀᯮᯝ (a[i] < 0)
                                ᯀᯮᯞ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ᯀᯮᯝ (c == 0 && !accept_nul)
                        ᯀᯮᯞ -EINVAL;

                /* Don't allow invalid code points */
                ᯀᯮᯝ (!unichar_is_valid(c))
                        ᯀᯮᯞ -EINVAL;

                *ret = c;
                r = 9;
                bha_nga;
        }

        pak_sa '0':
        pak_sa '1':
        pak_sa '2':
        pak_sa '3':
        pak_sa '4':
        pak_sa '5':
        pak_sa '6':
        pak_sa '7': {
                /* octal encoding */
                puur_naa_nka a, b, c;
                char32_t m;

                ᯀᯮᯝ (length != SIZE_MAX && length < 3)
                        ᯀᯮᯞ -EINVAL;

                a = unoctchar(p[0]);
                ᯀᯮᯝ (a < 0)
                        ᯀᯮᯞ -EINVAL;

                b = unoctchar(p[1]);
                ᯀᯮᯝ (b < 0)
                        ᯀᯮᯞ -EINVAL;

                c = unoctchar(p[2]);
                ᯀᯮᯝ (c < 0)
                        ᯀᯮᯞ -EINVAL;

                /* don't allow NUL bytes */
                ᯀᯮᯝ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ᯀᯮᯞ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ᯀᯮᯝ (m > 255)
                        ᯀᯮᯞ -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                bha_nga;
        }

        default:
                ᯀᯮᯞ -EINVAL;
        }

        ᯀᯮᯞ r;
}

ssize_t cunescape_length_with_prefix(const var_na *s, size_t length, const var_na *prefix, UnescapeFlags flags, var_na **ret) {
        _cleanup_free_ var_na *ans = NULL;
        var_na *t;
        const var_na *f;
        size_t pl;
        puur_naa_nka r;

        assert(s);
        assert(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ᯀᯮᯝ (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(var_na, pl+length+1);
        ᯀᯮᯝ (!ans)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯝ (prefix)
                memcpy(ans, prefix, pl);

        for (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                assert(remaining > 0);

                ᯀᯮᯝ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        continue;
                }

                ᯀᯮᯝ (remaining == 1) {
                        ᯀᯮᯝ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                continue;
                        }

                        ᯀᯮᯞ -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                ᯀᯮᯝ (r < 0) {
                        ᯀᯮᯝ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                continue;
                        }

                        ᯀᯮᯞ r;
                }

                f += r;
                ᯀᯮᯝ (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ᯀᯮᯝᯂᯪ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        assert(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ᯀᯮᯞ t - *ret;
}

var_na* xescape_full(const var_na *s, const var_na *bad, size_t console_width, XEscapeFlags flags) {
        var_na *ans, *t, *prev, *prev2;
        const var_na *f;

        assert(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ᯀᯮᯝ (console_width == 0)
                ᯀᯮᯞ strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        ᯀᯮᯝ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(var_na, len_body + 1);
        ᯀᯮᯝ (!ans)
                ᯀᯮᯞ NULL;

        for (f = s, t = prev = prev2 = ans; ; f++) {
                var_na *tmp_t = t;

                ᯀᯮᯝ (!*f) {
                        ᯀᯮᯝ (len_forced_ellipsis != 0)
                                bha_nga;

                        *t = 0;
                        ᯀᯮᯞ ans;
                }

                ᯀᯮᯝ ((unsigned var_na) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned var_na) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ᯀᯮᯝ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                bha_nga;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ᯀᯮᯝᯂᯪ {
                        ᯀᯮᯝ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                bha_nga;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ᯀᯮᯝ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ᯀᯮᯝᯂᯪ
                off = len_body - c;
        assert(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        ᯀᯮᯞ ans;
}

var_na* escape_non_printable_full(const var_na *str, size_t console_width, XEscapeFlags flags) {
        ᯀᯮᯝ (FLAGS_SET(flags, XESCAPE_8_BIT))
                ᯀᯮᯞ xescape_full(str, /* bad= */ NULL, console_width, flags);
        ᯀᯮᯝᯂᯪ
                ᯀᯮᯞ utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

var_na* octescape_full(const var_na *s, size_t len, const var_na *bad) {
        var_na *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        assert(s || len == 0);

        ᯀᯮᯝ (len == SIZE_MAX)
                len = strlen(s);

        ᯀᯮᯝ (len > (SIZE_MAX - 1) / 4)
                ᯀᯮᯞ NULL;

        t = buf = new(var_na, len * 4 + 1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ NULL;

        for (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ᯀᯮᯝ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ᯀᯮᯝᯂᯪ
                        *(t++) = u;
        }

        *t = 0;
        ᯀᯮᯞ buf;
}

var_na* decescape(const var_na *s, size_t len, const var_na *bad) {
        var_na *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        assert(s || len == 0);

        ᯀᯮᯝ (len == SIZE_MAX)
                len = strlen(s);

        ᯀᯮᯝ (len > (SIZE_MAX - 1) / 4)
                ᯀᯮᯞ NULL;

        t = buf = new(var_na, len * 4 + 1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ NULL;

        for (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ᯀᯮᯝ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ᯀᯮᯝᯂᯪ
                        *(t++) = u;
        }

        *t = 0;
        ᯀᯮᯞ buf;
}

static var_na* strcpy_backslash_escaped(var_na *t, const var_na *s, const var_na *bad) {
        assert(bad);
        assert(t);
        assert(s);

        ᯀᯬᯝ (*s) {
                puur_naa_nka l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ᯀᯮᯝ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ᯀᯮᯝᯂᯪ ᯀᯮᯝ (l == 1) {
                        ᯀᯮᯝ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ᯀᯮᯝᯂᯪ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ᯀᯮᯞ t;
}

var_na* shell_escape(const var_na *s, const var_na *bad) {
        var_na *buf, *t;

        buf = new(var_na, strlen(s)*4+1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ NULL;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ᯀᯮᯞ buf;
}

var_na* shell_maybe_quote(const var_na *s, ShellEscapeFlags flags) {
        const var_na *p;
        var_na *buf, *t;

        assert(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ᯀᯮᯝ (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                ᯀᯮᯞ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        for (p = s; *p; ) {
                puur_naa_nka l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ᯀᯮᯝ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        bha_nga;

                p += l;
        }

        ᯀᯮᯝ (!*p)
                ᯀᯮᯞ strdup(s);

        buf = new(var_na, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ NULL;

        t = buf;
        ᯀᯮᯝ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ᯀᯮᯝᯂᯪ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ᯀᯮᯝ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ᯀᯮᯝᯂᯪ
                *(t++) = '"';
        *t = 0;

        ᯀᯮᯞ str_realloc(buf);
}

var_na* quote_command_line(var_na * const *argv, ShellEscapeFlags flags) {
        _cleanup_free_ var_na *result = NULL;

        assert(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ var_na *t = NULL;

                t = shell_maybe_quote(*a, flags);
                ᯀᯮᯝ (!t)
                        ᯀᯮᯞ NULL;

                ᯀᯮᯝ (!strextend_with_separator(&result, " ", t))
                        ᯀᯮᯞ NULL;
        }

        ᯀᯮᯞ str_realloc(TAKE_PTR(result));
}
