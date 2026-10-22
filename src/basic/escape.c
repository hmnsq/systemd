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
                        إذا ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((بینښه حرف) c >> 6);
                                *(buf++) = octchar((بینښه حرف) c >> 3);
                                *(buf++) = octchar((بینښه حرف) c);
                        } وإلا
                                *(buf++) = c;
                        اكسر;
        }

        أرجع buf - buf_old;
}

حرف* cescape_اوږدتیا(ثابت حرف *s, size_t n) {
        ثابت حرف *f;
        حرف *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        أكد(s || n == 0);

        إذا (n == SIZE_MAX)
                n = مزي_اوږدوالی(s);

        إذا (n > (SIZE_MAX - 1) / 4)
                أرجع عدم;

        r = new(حرف, n*4 + 1);
        إذا (!r)
                أرجع عدم;

        لكل (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        أرجع r;
}

صحيح cunescape_one(ثابت حرف *p, size_t length, char32_t *ret, bool *eight_بټ, bool accept_nul) {
        صحيح r = 1;

        أكد(p);
        أكد(ret);
        أكد(eight_بټ);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        إذا (length != SIZE_MAX && length < 1)
                أرجع -EINVAL;

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

                إذا (length != SIZE_MAX && length < 3)
                        أرجع -EINVAL;

                a = unhexchar(p[1]);
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unhexchar(p[2]);
                إذا (b < 0)
                        أرجع -EINVAL;

                /* Don't allow NUL bytes */
                إذا (a == 0 && b == 0 && !accept_nul)
                        أرجع -EINVAL;

                *ret = (a << 4U) | b;
                *eight_بټ = true;
                r = 3;
                اكسر;
        }

        حالة 'u': {
                /* C++11 style 16-bit unicode */

                صحيح a[4];
                size_t i;
                uint32_t c;

                إذا (length != SIZE_MAX && length < 5)
                        أرجع -EINVAL;

                لكل (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        إذا (a[i] < 0)
                                أرجع a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                إذا (c == 0 && !accept_nul)
                        أرجع -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                إذا (utf16_is_surrogate(c))
                        أرجع -EINVAL;

                *ret = c;
                r = 5;
                اكسر;
        }

        حالة 'U': {
                /* C++11 style 32-bit unicode */

                صحيح a[8];
                size_t i;
                char32_t c;

                إذا (length != SIZE_MAX && length < 9)
                        أرجع -EINVAL;

                لكل (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        إذا (a[i] < 0)
                                أرجع a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                إذا (c == 0 && !accept_nul)
                        أرجع -EINVAL;

                /* Don't allow invalid code points */
                إذا (!unichar_is_valid(c))
                        أرجع -EINVAL;

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

                إذا (length != SIZE_MAX && length < 3)
                        أرجع -EINVAL;

                a = unoctchar(p[0]);
                إذا (a < 0)
                        أرجع -EINVAL;

                b = unoctchar(p[1]);
                إذا (b < 0)
                        أرجع -EINVAL;

                c = unoctchar(p[2]);
                إذا (c < 0)
                        أرجع -EINVAL;

                /* don't allow NUL bytes */
                إذا (a == 0 && b == 0 && c == 0 && !accept_nul)
                        أرجع -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                إذا (m > 255)
                        أرجع -EINVAL;

                *ret = m;
                *eight_بټ = true;
                r = 3;
                اكسر;
        }

        افتراضي:
                أرجع -EINVAL;
        }

        أرجع r;
}

ssize_t cunescape_اوږدتیا_with_مختاړی(ثابت حرف *s, size_t length, ثابت حرف *مختاړی, UnescapeFlags flags, حرف **ret) {
        _cleanup_free_ حرف *ans = عدم;
        حرف *t;
        ثابت حرف *f;
        size_t pl;
        صحيح r;

        أكد(s);
        أكد(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        إذا (length == SIZE_MAX)
                length = مزي_اوږدوالی(s);

        pl = strlen_ptr(مختاړی);

        ans = new(حرف, pl+length+1);
        إذا (!ans)
                أرجع -ENOMEM;

        إذا (مختاړی)
                حافظه_نقل(ans, مختاړی, pl);

        لكل (f = s, t = ans + pl; f < s + length; f++) {
                size_t پاتې_شوی;
                bool eight_بټ = false;
                char32_t u;

                پاتې_شوی = s + length - f;
                أكد(پاتې_شوی > 0);

                إذا (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        استمر;
                }

                إذا (پاتې_شوی == 1) {
                        إذا (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                استمر;
                        }

                        أرجع -EINVAL;
                }

                r = cunescape_one(f + 1, پاتې_شوی - 1, &u, &eight_بټ, flags & UNESCAPE_ACCEPT_NUL);
                إذا (r < 0) {
                        إذا (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                استمر;
                        }

                        أرجع r;
                }

                f += r;
                إذا (eight_بټ)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                وإلا
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        أكد(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        أرجع t - *ret;
}

حرف* xescape_ډک(ثابت حرف *s, ثابت حرف *bad, size_t console_پلنوالی, XEscapeFlags flags) {
        حرف *ans, *t, *prev, *prev2;
        ثابت حرف *f;

        أكد(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        إذا (console_پلنوالی == 0)
                أرجع strdup("");

        size_t len_forced_ellipsis = FLAGS_سېټ(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = مزي_اوږدوالی(s);

        size_t len_body = MIN(console_پلنوالی, SIZE_MAX - 1); /* We need room for the NUL byte */
        إذا (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(حرف, len_body + 1);
        إذا (!ans)
                أرجع عدم;

        لكل (f = s, t = prev = prev2 = ans; ; f++) {
                حرف *tmp_t = t;

                إذا (!*f) {
                        إذا (len_forced_ellipsis != 0)
                                اكسر;

                        *t = 0;
                        أرجع ans;
                }

                إذا ((بینښه حرف) *f < ' ' ||
                    (!FLAGS_سېټ(flags, XESCAPE_8_بټ) && (بینښه حرف) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        إذا ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                اكسر;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } وإلا {
                        إذا ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
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
        إذا (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        وإلا إذا (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        وإلا إذا (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        وإلا
                off = len_body - c;
        أكد(off <= (size_t) (t - ans));

        حافظه_نقل(ans + off, "...", c);
        ans[off + c] = '\0';
        أرجع ans;
}

حرف* escape_non_printable_ډک(ثابت حرف *str, size_t console_پلنوالی, XEscapeFlags flags) {
        إذا (FLAGS_سېټ(flags, XESCAPE_8_بټ))
                أرجع xescape_ډک(str, /* bad= */ عدم, console_پلنوالی, flags);
        وإلا
                أرجع utf8_escape_non_printable_ډک(str,
                                                      console_پلنوالی,
                                                      FLAGS_سېټ(flags, XESCAPE_FORCE_ELLIPSIS));
}

حرف* octescape_ډک(ثابت حرف *s, size_t len, ثابت حرف *bad) {
        حرف *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = مزي_اوږدوالی(s);

        إذا (len > (SIZE_MAX - 1) / 4)
                أرجع عدم;

        t = buf = new(حرف, len * 4 + 1);
        إذا (!buf)
                أرجع عدم;

        لكل (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                إذا (u < ' ' || u >= 127 || دننه_سېټ(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } وإلا
                        *(t++) = u;
        }

        *t = 0;
        أرجع buf;
}

حرف* decescape(ثابت حرف *s, size_t len, ثابت حرف *bad) {
        حرف *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        أكد(s || len == 0);

        إذا (len == SIZE_MAX)
                len = مزي_اوږدوالی(s);

        إذا (len > (SIZE_MAX - 1) / 4)
                أرجع عدم;

        t = buf = new(حرف, len * 4 + 1);
        إذا (!buf)
                أرجع عدم;

        لكل (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                إذا (u < ' ' || u >= 127 || دننه_سېټ(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } وإلا
                        *(t++) = u;
        }

        *t = 0;
        أرجع buf;
}

ساكن حرف* strcpy_backslash_escaped(حرف *t, ثابت حرف *s, ثابت حرف *bad) {
        أكد(bad);
        أكد(t);
        أكد(s);

        طالما (*s) {
                صحيح l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                إذا (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                وإلا إذا (l == 1) {
                        إذا (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } وإلا {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        أرجع t;
}

حرف* shell_escape(ثابت حرف *s, ثابت حرف *bad) {
        حرف *buf, *t;

        buf = new(حرف, مزي_اوږدوالی(s)*4+1);
        إذا (!buf)
                أرجع عدم;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        أرجع buf;
}

حرف* shell_maybe_quote(ثابت حرف *s, ShellEscapeFlags flags) {
        ثابت حرف *p;
        حرف *buf, *t;

        أكد(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        إذا (FLAGS_سېټ(flags, SHELL_ESCAPE_تشه) && isempty(s))
                أرجع strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        لكل (p = s; *p; ) {
                صحيح l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                إذا (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        اكسر;

                p += l;
        }

        إذا (!*p)
                أرجع strdup(s);

        buf = new(حرف, FLAGS_سېټ(flags, SHELL_ESCAPE_POSIX) + 1 + مزي_اوږدوالی(s)*4 + 1 + 1);
        إذا (!buf)
                أرجع عدم;

        t = buf;
        إذا (FLAGS_سېټ(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } وإلا
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_سېټ(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        إذا (FLAGS_سېټ(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        وإلا
                *(t++) = '"';
        *t = 0;

        أرجع str_realloc(buf);
}

حرف* quote_command_کرښه(حرف * ثابت *argv, ShellEscapeFlags flags) {
        _cleanup_free_ حرف *پایله = عدم;

        أكد(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ حرف *t = عدم;

                t = shell_maybe_quote(*a, flags);
                إذا (!t)
                        أرجع عدم;

                إذا (!strextend_with_separator(&پایله, " ", t))
                        أرجع عدم;
        }

        أرجع str_realloc(TAKE_PTR(پایله));
}
