/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <string.h>

#include "alloc-util.h"
#include "escape.h"
#include "hexdecoct.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

정수 cescape_char(char c, char *buf) {
        char *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        switch (c) {

                case '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        break;
                case '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        break;
                case '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        break;
                case '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        break;
                case '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        break;
                case '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        break;
                case '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        break;
                case '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        break;
                case '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        break;
                case '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        break;

                default:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        만약 ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned char) c >> 6);
                                *(buf++) = octchar((unsigned char) c >> 3);
                                *(buf++) = octchar((unsigned char) c);
                        } 아니면
                                *(buf++) = c;
                        break;
        }

        반환 buf - buf_old;
}

char* cescape_length(const char *s, size_t n) {
        const char *f;
        char *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        assert(s || n == 0);

        만약 (n == SIZE_MAX)
                n = strlen(s);

        만약 (n > (SIZE_MAX - 1) / 4)
                반환 NULL;

        r = new(char, n*4 + 1);
        만약 (!r)
                반환 NULL;

        위해 (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        반환 r;
}

정수 cunescape_one(const char *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        정수 r = 1;

        assert(p);
        assert(ret);
        assert(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        만약 (length != SIZE_MAX && length < 1)
                반환 -EINVAL;

        switch (p[0]) {

        case 'a':
                *ret = '\a';
                break;
        case 'b':
                *ret = '\b';
                break;
        case 'f':
                *ret = '\f';
                break;
        case 'n':
                *ret = '\n';
                break;
        case 'r':
                *ret = '\r';
                break;
        case 't':
                *ret = '\t';
                break;
        case 'v':
                *ret = '\v';
                break;
        case '\\':
                *ret = '\\';
                break;
        case '"':
                *ret = '"';
                break;
        case '\'':
                *ret = '\'';
                break;

        case 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                break;

        case 'x': {
                /* hexadecimal encoding */
                정수 a, b;

                만약 (length != SIZE_MAX && length < 3)
                        반환 -EINVAL;

                a = unhexchar(p[1]);
                만약 (a < 0)
                        반환 -EINVAL;

                b = unhexchar(p[2]);
                만약 (b < 0)
                        반환 -EINVAL;

                /* Don't allow NUL bytes */
                만약 (a == 0 && b == 0 && !accept_nul)
                        반환 -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                break;
        }

        case 'u': {
                /* C++11 style 16-bit unicode */

                정수 a[4];
                size_t i;
                uint32_t c;

                만약 (length != SIZE_MAX && length < 5)
                        반환 -EINVAL;

                위해 (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        만약 (a[i] < 0)
                                반환 a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                만약 (c == 0 && !accept_nul)
                        반환 -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                만약 (utf16_is_surrogate(c))
                        반환 -EINVAL;

                *ret = c;
                r = 5;
                break;
        }

        case 'U': {
                /* C++11 style 32-bit unicode */

                정수 a[8];
                size_t i;
                char32_t c;

                만약 (length != SIZE_MAX && length < 9)
                        반환 -EINVAL;

                위해 (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        만약 (a[i] < 0)
                                반환 a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                만약 (c == 0 && !accept_nul)
                        반환 -EINVAL;

                /* Don't allow invalid code points */
                만약 (!unichar_is_valid(c))
                        반환 -EINVAL;

                *ret = c;
                r = 9;
                break;
        }

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7': {
                /* octal encoding */
                정수 a, b, c;
                char32_t m;

                만약 (length != SIZE_MAX && length < 3)
                        반환 -EINVAL;

                a = unoctchar(p[0]);
                만약 (a < 0)
                        반환 -EINVAL;

                b = unoctchar(p[1]);
                만약 (b < 0)
                        반환 -EINVAL;

                c = unoctchar(p[2]);
                만약 (c < 0)
                        반환 -EINVAL;

                /* don't allow NUL bytes */
                만약 (a == 0 && b == 0 && c == 0 && !accept_nul)
                        반환 -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                만약 (m > 255)
                        반환 -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                break;
        }

        default:
                반환 -EINVAL;
        }

        반환 r;
}

ssize_t cunescape_length_with_prefix(const char *s, size_t length, const char *prefix, UnescapeFlags flags, char **ret) {
        _cleanup_free_ char *ans = NULL;
        char *t;
        const char *f;
        size_t pl;
        정수 r;

        assert(s);
        assert(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        만약 (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(char, pl+length+1);
        만약 (!ans)
                반환 -ENOMEM;

        만약 (prefix)
                memcpy(ans, prefix, pl);

        위해 (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                assert(remaining > 0);

                만약 (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        continue;
                }

                만약 (remaining == 1) {
                        만약 (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                continue;
                        }

                        반환 -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                만약 (r < 0) {
                        만약 (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                continue;
                        }

                        반환 r;
                }

                f += r;
                만약 (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                아니면
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        assert(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        반환 t - *ret;
}

char* xescape_full(const char *s, const char *bad, size_t console_width, XEscapeFlags flags) {
        char *ans, *t, *prev, *prev2;
        const char *f;

        assert(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        만약 (console_width == 0)
                반환 strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        만약 (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(char, len_body + 1);
        만약 (!ans)
                반환 NULL;

        위해 (f = s, t = prev = prev2 = ans; ; f++) {
                char *tmp_t = t;

                만약 (!*f) {
                        만약 (len_forced_ellipsis != 0)
                                break;

                        *t = 0;
                        반환 ans;
                }

                만약 ((unsigned char) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned char) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        만약 ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                break;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } 아니면 {
                        만약 ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                break;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        만약 (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        아니면 만약 (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        아니면 만약 (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        아니면
                off = len_body - c;
        assert(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        반환 ans;
}

char* escape_non_printable_full(const char *str, size_t console_width, XEscapeFlags flags) {
        만약 (FLAGS_SET(flags, XESCAPE_8_BIT))
                반환 xescape_full(str, /* bad= */ NULL, console_width, flags);
        아니면
                반환 utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

char* octescape_full(const char *s, size_t len, const char *bad) {
        char *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        assert(s || len == 0);

        만약 (len == SIZE_MAX)
                len = strlen(s);

        만약 (len > (SIZE_MAX - 1) / 4)
                반환 NULL;

        t = buf = new(char, len * 4 + 1);
        만약 (!buf)
                반환 NULL;

        위해 (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                만약 (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } 아니면
                        *(t++) = u;
        }

        *t = 0;
        반환 buf;
}

char* decescape(const char *s, size_t len, const char *bad) {
        char *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        assert(s || len == 0);

        만약 (len == SIZE_MAX)
                len = strlen(s);

        만약 (len > (SIZE_MAX - 1) / 4)
                반환 NULL;

        t = buf = new(char, len * 4 + 1);
        만약 (!buf)
                반환 NULL;

        위해 (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                만약 (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } 아니면
                        *(t++) = u;
        }

        *t = 0;
        반환 buf;
}

static char* strcpy_backslash_escaped(char *t, const char *s, const char *bad) {
        assert(bad);
        assert(t);
        assert(s);

        동안 (*s) {
                정수 l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                만약 (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                아니면 만약 (l == 1) {
                        만약 (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } 아니면 {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        반환 t;
}

char* shell_escape(const char *s, const char *bad) {
        char *buf, *t;

        buf = new(char, strlen(s)*4+1);
        만약 (!buf)
                반환 NULL;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        반환 buf;
}

char* shell_maybe_quote(const char *s, ShellEscapeFlags flags) {
        const char *p;
        char *buf, *t;

        assert(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        만약 (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                반환 strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        위해 (p = s; *p; ) {
                정수 l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                만약 (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        break;

                p += l;
        }

        만약 (!*p)
                반환 strdup(s);

        buf = new(char, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        만약 (!buf)
                반환 NULL;

        t = buf;
        만약 (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } 아니면
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        만약 (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        아니면
                *(t++) = '"';
        *t = 0;

        반환 str_realloc(buf);
}

char* quote_command_line(char * const *argv, ShellEscapeFlags flags) {
        _cleanup_free_ char *result = NULL;

        assert(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ char *t = NULL;

                t = shell_maybe_quote(*a, flags);
                만약 (!t)
                        반환 NULL;

                만약 (!strextend_with_separator(&result, " ", t))
                        반환 NULL;
        }

        반환 str_realloc(TAKE_PTR(result));
}
