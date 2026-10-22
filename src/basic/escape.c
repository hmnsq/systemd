/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>
#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "escape.h"
#ضمن "hexdecoct.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"

صحيح cescape_char(حرف c, حرف *buf) {
        حرف *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        بدل (c) {

                حالة '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        اكسر;
                حالة '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        اكسر;
                حالة '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        اكسر;
                حالة '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        اكسر;
                حالة '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        اكسر;
                حالة '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        اكسر;
                حالة '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        اكسر;
                حالة '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        اكسر;
                حالة '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        اكسر;
                حالة '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        اكسر;

                افتراضي:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ܐܢ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned حرف) c >> 6);
                                *(buf++) = octchar((unsigned حرف) c >> 3);
                                *(buf++) = octchar((unsigned حرف) c);
                        } ܐܚܪܢܐ
                                *(buf++) = c;
                        اكسر;
        }

        ܦܢܐ buf - buf_old;
}

حرف* cescape_length(ثابت حرف *s, size_t n) {
        ثابت حرف *f;
        حرف *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        أكد(s || n == 0);

        ܐܢ (n == SIZE_MAX)
                n = strlen(s);

        ܐܢ (n > (SIZE_MAX - 1) / 4)
                ܦܢܐ عدم;

        r = new(حرف, n*4 + 1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        لكل (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ܦܢܐ r;
}

صحيح cunescape_one(ثابت حرف *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        صحيح r = 1;

        أكد(p);
        أكد(ret);
        أكد(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ܐܢ (length != SIZE_MAX && length < 1)
                ܦܢܐ -EINVAL;

        بدل (p[0]) {

        حالة 'a':
                *ret = '\a';
                اكسر;
        حالة 'b':
                *ret = '\b';
                اكسر;
        حالة 'f':
                *ret = '\f';
                اكسر;
        حالة 'n':
                *ret = '\n';
                اكسر;
        حالة 'r':
                *ret = '\r';
                اكسر;
        حالة 't':
                *ret = '\t';
                اكسر;
        حالة 'v':
                *ret = '\v';
                اكسر;
        حالة '\\':
                *ret = '\\';
                اكسر;
        حالة '"':
                *ret = '"';
                اكسر;
        حالة '\'':
                *ret = '\'';
                اكسر;

        حالة 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                اكسر;

        حالة 'x': {
                /* hexadecimal encoding */
                صحيح a, b;

                ܐܢ (length != SIZE_MAX && length < 3)
                        ܦܢܐ -EINVAL;

                a = unhexchar(p[1]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unhexchar(p[2]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                /* Don't allow NUL bytes */
                ܐܢ (a == 0 && b == 0 && !accept_nul)
                        ܦܢܐ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                اكسر;
        }

        حالة 'u': {
                /* C++11 style 16-bit unicode */

                صحيح a[4];
                size_t i;
                uint32_t c;

                ܐܢ (length != SIZE_MAX && length < 5)
                        ܦܢܐ -EINVAL;

                لكل (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ܐܢ (a[i] < 0)
                                ܦܢܐ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ܐܢ (c == 0 && !accept_nul)
                        ܦܢܐ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ܐܢ (utf16_is_surrogate(c))
                        ܦܢܐ -EINVAL;

                *ret = c;
                r = 5;
                اكسر;
        }

        حالة 'U': {
                /* C++11 style 32-bit unicode */

                صحيح a[8];
                size_t i;
                char32_t c;

                ܐܢ (length != SIZE_MAX && length < 9)
                        ܦܢܐ -EINVAL;

                لكل (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ܐܢ (a[i] < 0)
                                ܦܢܐ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ܐܢ (c == 0 && !accept_nul)
                        ܦܢܐ -EINVAL;

                /* Don't allow invalid code points */
                ܐܢ (!unichar_is_valid(c))
                        ܦܢܐ -EINVAL;

                *ret = c;
                r = 9;
                اكسر;
        }

        حالة '0':
        حالة '1':
        حالة '2':
        حالة '3':
        حالة '4':
        حالة '5':
        حالة '6':
        حالة '7': {
                /* octal encoding */
                صحيح a, b, c;
                char32_t m;

                ܐܢ (length != SIZE_MAX && length < 3)
                        ܦܢܐ -EINVAL;

                a = unoctchar(p[0]);
                ܐܢ (a < 0)
                        ܦܢܐ -EINVAL;

                b = unoctchar(p[1]);
                ܐܢ (b < 0)
                        ܦܢܐ -EINVAL;

                c = unoctchar(p[2]);
                ܐܢ (c < 0)
                        ܦܢܐ -EINVAL;

                /* don't allow NUL bytes */
                ܐܢ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ܦܢܐ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ܐܢ (m > 255)
                        ܦܢܐ -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                اكسر;
        }

        افتراضي:
                ܦܢܐ -EINVAL;
        }

        ܦܢܐ r;
}

ssize_t cunescape_length_with_prefix(ثابت حرف *s, size_t length, ثابت حرف *prefix, UnescapeFlags flags, حرف **ret) {
        _cleanup_free_ حرف *ans = عدم;
        حرف *t;
        ثابت حرف *f;
        size_t pl;
        صحيح r;

        أكد(s);
        أكد(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ܐܢ (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(حرف, pl+length+1);
        ܐܢ (!ans)
                ܦܢܐ -ENOMEM;

        ܐܢ (prefix)
                memcpy(ans, prefix, pl);

        لكل (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                أكد(remaining > 0);

                ܐܢ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        استمر;
                }

                ܐܢ (remaining == 1) {
                        ܐܢ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                استمر;
                        }

                        ܦܢܐ -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                ܐܢ (r < 0) {
                        ܐܢ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                استمر;
                        }

                        ܦܢܐ r;
                }

                f += r;
                ܐܢ (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ܐܚܪܢܐ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        أكد(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ܦܢܐ t - *ret;
}

حرف* xescape_full(ثابت حرف *s, ثابت حرف *bad, size_t console_width, XEscapeFlags flags) {
        حرف *ans, *t, *prev, *prev2;
        ثابت حرف *f;

        أكد(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ܐܢ (console_width == 0)
                ܦܢܐ strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        ܐܢ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(حرف, len_body + 1);
        ܐܢ (!ans)
                ܦܢܐ عدم;

        لكل (f = s, t = prev = prev2 = ans; ; f++) {
                حرف *tmp_t = t;

                ܐܢ (!*f) {
                        ܐܢ (len_forced_ellipsis != 0)
                                اكسر;

                        *t = 0;
                        ܦܢܐ ans;
                }

                ܐܢ ((unsigned حرف) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned حرف) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ܐܢ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                اكسر;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ܐܚܪܢܐ {
                        ܐܢ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                اكسر;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ܐܢ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ܐܚܪܢܐ ܐܢ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        ܐܚܪܢܐ ܐܢ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ܐܚܪܢܐ
                off = len_body - c;
        أكد(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        ܦܢܐ ans;
}

حرف* escape_non_printable_full(ثابت حرف *str, size_t console_width, XEscapeFlags flags) {
        ܐܢ (FLAGS_SET(flags, XESCAPE_8_BIT))
                ܦܢܐ xescape_full(str, /* bad= */ عدم, console_width, flags);
        ܐܚܪܢܐ
                ܦܢܐ utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

حرف* octescape_full(ثابت حرف *s, size_t len, ثابت حرف *bad) {
        حرف *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        أكد(s || len == 0);

        ܐܢ (len == SIZE_MAX)
                len = strlen(s);

        ܐܢ (len > (SIZE_MAX - 1) / 4)
                ܦܢܐ عدم;

        t = buf = new(حرف, len * 4 + 1);
        ܐܢ (!buf)
                ܦܢܐ عدم;

        لكل (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ܐܢ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ܐܚܪܢܐ
                        *(t++) = u;
        }

        *t = 0;
        ܦܢܐ buf;
}

حرف* decescape(ثابت حرف *s, size_t len, ثابت حرف *bad) {
        حرف *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        أكد(s || len == 0);

        ܐܢ (len == SIZE_MAX)
                len = strlen(s);

        ܐܢ (len > (SIZE_MAX - 1) / 4)
                ܦܢܐ عدم;

        t = buf = new(حرف, len * 4 + 1);
        ܐܢ (!buf)
                ܦܢܐ عدم;

        لكل (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ܐܢ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ܐܚܪܢܐ
                        *(t++) = u;
        }

        *t = 0;
        ܦܢܐ buf;
}

ساكن حرف* strcpy_backslash_escaped(حرف *t, ثابت حرف *s, ثابت حرف *bad) {
        أكد(bad);
        أكد(t);
        أكد(s);

        ܟܡܐ (*s) {
                صحيح l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ܐܢ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ܐܚܪܢܐ ܐܢ (l == 1) {
                        ܐܢ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ܐܚܪܢܐ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ܦܢܐ t;
}

حرف* shell_escape(ثابت حرف *s, ثابت حرف *bad) {
        حرف *buf, *t;

        buf = new(حرف, strlen(s)*4+1);
        ܐܢ (!buf)
                ܦܢܐ عدم;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ܦܢܐ buf;
}

حرف* shell_maybe_quote(ثابت حرف *s, ShellEscapeFlags flags) {
        ثابت حرف *p;
        حرف *buf, *t;

        أكد(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ܐܢ (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                ܦܢܐ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        لكل (p = s; *p; ) {
                صحيح l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ܐܢ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        اكسر;

                p += l;
        }

        ܐܢ (!*p)
                ܦܢܐ strdup(s);

        buf = new(حرف, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        ܐܢ (!buf)
                ܦܢܐ عدم;

        t = buf;
        ܐܢ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ܐܚܪܢܐ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ܐܢ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ܐܚܪܢܐ
                *(t++) = '"';
        *t = 0;

        ܦܢܐ str_realloc(buf);
}

حرف* quote_command_line(حرف * ثابت *argv, ShellEscapeFlags flags) {
        _cleanup_free_ حرف *result = عدم;

        أكد(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ حرف *t = عدم;

                t = shell_maybe_quote(*a, flags);
                ܐܢ (!t)
                        ܦܢܐ عدم;

                ܐܢ (!strextend_with_separator(&result, " ", t))
                        ܦܢܐ عدم;
        }

        ܦܢܐ str_realloc(TAKE_PTR(result));
}
