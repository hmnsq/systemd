/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>
#شامل <string.h>

#شامل "alloc-util.h"
#شامل "escape.h"
#شامل "hexdecoct.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"

صحیح cescape_char(حرف c, حرف *buf) {
        حرف *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        انتخاب (c) {

                حالت '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        توڑو;
                حالت '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        توڑو;
                حالت '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        توڑو;
                حالت '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        توڑو;
                حالت '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        توڑو;
                حالت '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        توڑو;
                حالت '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        توڑو;
                حالت '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        توڑو;
                حالت '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        توڑو;
                حالت '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        توڑو;

                طےشدہ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        اگر ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((غیرعلامتی حرف) c >> 6);
                                *(buf++) = octchar((غیرعلامتی حرف) c >> 3);
                                *(buf++) = octchar((غیرعلامتی حرف) c);
                        } ورنہ
                                *(buf++) = c;
                        توڑو;
        }

        واپس buf - buf_old;
}

حرف* cescape_لمبائی(ثابت حرف *s, size_t n) {
        ثابت حرف *f;
        حرف *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        أكد(s || n == 0);

        اگر (n == SIZE_MAX)
                n = لڑی_لمبائی(s);

        اگر (n > (SIZE_MAX - 1) / 4)
                واپس تہی;

        r = new(حرف, n*4 + 1);
        اگر (!r)
                واپس تہی;

        برائے (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        واپس r;
}

صحیح cunescape_one(ثابت حرف *p, size_t length, char32_t *ret, bool *eight_ثنائی_ہندسہ, bool accept_nul) {
        صحیح r = 1;

        أكد(p);
        أكد(ret);
        أكد(eight_ثنائی_ہندسہ);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        اگر (length != SIZE_MAX && length < 1)
                واپس -EINVAL;

        انتخاب (p[0]) {

        حالت 'a':
                *ret = '\a';
                توڑو;
        حالت 'b':
                *ret = '\b';
                توڑو;
        حالت 'f':
                *ret = '\f';
                توڑو;
        حالت 'n':
                *ret = '\n';
                توڑو;
        حالت 'r':
                *ret = '\r';
                توڑو;
        حالت 't':
                *ret = '\t';
                توڑو;
        حالت 'v':
                *ret = '\v';
                توڑو;
        حالت '\\':
                *ret = '\\';
                توڑو;
        حالت '"':
                *ret = '"';
                توڑو;
        حالت '\'':
                *ret = '\'';
                توڑو;

        حالت 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                توڑو;

        حالت 'x': {
                /* hexadecimal encoding */
                صحیح a, b;

                اگر (length != SIZE_MAX && length < 3)
                        واپس -EINVAL;

                a = unhexchar(p[1]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unhexchar(p[2]);
                اگر (b < 0)
                        واپس -EINVAL;

                /* Don't allow NUL bytes */
                اگر (a == 0 && b == 0 && !accept_nul)
                        واپس -EINVAL;

                *ret = (a << 4U) | b;
                *eight_ثنائی_ہندسہ = true;
                r = 3;
                توڑو;
        }

        حالت 'u': {
                /* C++11 style 16-bit unicode */

                صحیح a[4];
                size_t i;
                uint32_t c;

                اگر (length != SIZE_MAX && length < 5)
                        واپس -EINVAL;

                برائے (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        اگر (a[i] < 0)
                                واپس a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                اگر (c == 0 && !accept_nul)
                        واپس -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                اگر (utf16_is_surrogate(c))
                        واپس -EINVAL;

                *ret = c;
                r = 5;
                توڑو;
        }

        حالت 'U': {
                /* C++11 style 32-bit unicode */

                صحیح a[8];
                size_t i;
                char32_t c;

                اگر (length != SIZE_MAX && length < 9)
                        واپس -EINVAL;

                برائے (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        اگر (a[i] < 0)
                                واپس a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                اگر (c == 0 && !accept_nul)
                        واپس -EINVAL;

                /* Don't allow invalid code points */
                اگر (!unichar_is_valid(c))
                        واپس -EINVAL;

                *ret = c;
                r = 9;
                توڑو;
        }

        حالت '0':
        حالت '1':
        حالت '2':
        حالت '3':
        حالت '4':
        حالت '5':
        حالت '6':
        حالت '7': {
                /* octal encoding */
                صحیح a, b, c;
                char32_t m;

                اگر (length != SIZE_MAX && length < 3)
                        واپس -EINVAL;

                a = unoctchar(p[0]);
                اگر (a < 0)
                        واپس -EINVAL;

                b = unoctchar(p[1]);
                اگر (b < 0)
                        واپس -EINVAL;

                c = unoctchar(p[2]);
                اگر (c < 0)
                        واپس -EINVAL;

                /* don't allow NUL bytes */
                اگر (a == 0 && b == 0 && c == 0 && !accept_nul)
                        واپس -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                اگر (m > 255)
                        واپس -EINVAL;

                *ret = m;
                *eight_ثنائی_ہندسہ = true;
                r = 3;
                توڑو;
        }

        طےشدہ:
                واپس -EINVAL;
        }

        واپس r;
}

ssize_t cunescape_لمبائی_with_سابقہ(ثابت حرف *s, size_t length, ثابت حرف *سابقہ, UnescapeFlags flags, حرف **ret) {
        _cleanup_free_ حرف *ans = تہی;
        حرف *t;
        ثابت حرف *f;
        size_t pl;
        صحیح r;

        أكد(s);
        أكد(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        اگر (length == SIZE_MAX)
                length = لڑی_لمبائی(s);

        pl = strlen_ptr(سابقہ);

        ans = new(حرف, pl+length+1);
        اگر (!ans)
                واپس -ENOMEM;

        اگر (سابقہ)
                حافظہ_نقل(ans, سابقہ, pl);

        برائے (f = s, t = ans + pl; f < s + length; f++) {
                size_t بقایا;
                bool eight_ثنائی_ہندسہ = false;
                char32_t u;

                بقایا = s + length - f;
                أكد(بقایا > 0);

                اگر (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        جاری;
                }

                اگر (بقایا == 1) {
                        اگر (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                جاری;
                        }

                        واپس -EINVAL;
                }

                r = cunescape_one(f + 1, بقایا - 1, &u, &eight_ثنائی_ہندسہ, flags & UNESCAPE_ACCEPT_NUL);
                اگر (r < 0) {
                        اگر (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                جاری;
                        }

                        واپس r;
                }

                f += r;
                اگر (eight_ثنائی_ہندسہ)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ورنہ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        أكد(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        واپس t - *ret;
}

حرف* xescape_بھرا(ثابت حرف *s, ثابت حرف *bad, size_t console_چوڑائی, XEscapeFlags flags) {
        حرف *ans, *t, *پچھلا, *prev2;
        ثابت حرف *f;

        أكد(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        اگر (console_چوڑائی == 0)
                واپس strdup("");

        size_t len_forced_ellipsis = FLAGS_سیٹ(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = لڑی_لمبائی(s);

        size_t len_body = MIN(console_چوڑائی, SIZE_MAX - 1); /* We need room for the NUL byte */
        اگر (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(حرف, len_body + 1);
        اگر (!ans)
                واپس تہی;

        برائے (f = s, t = پچھلا = prev2 = ans; ; f++) {
                حرف *tmp_t = t;

                اگر (!*f) {
                        اگر (len_forced_ellipsis != 0)
                                توڑو;

                        *t = 0;
                        واپس ans;
                }

                اگر ((غیرعلامتی حرف) *f < ' ' ||
                    (!FLAGS_سیٹ(flags, XESCAPE_8_ثنائی_ہندسہ) && (غیرعلامتی حرف) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        اگر ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                توڑو;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ورنہ {
                        اگر ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                توڑو;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = پچھلا;
                پچھلا = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        اگر (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ورنہ اگر (len_body - c >= (size_t) (پچھلا - ans))
                off = (size_t) (پچھلا - ans);
        ورنہ اگر (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ورنہ
                off = len_body - c;
        أكد(off <= (size_t) (t - ans));

        حافظہ_نقل(ans + off, "...", c);
        ans[off + c] = '\0';
        واپس ans;
}

حرف* escape_non_printable_بھرا(ثابت حرف *str, size_t console_چوڑائی, XEscapeFlags flags) {
        اگر (FLAGS_سیٹ(flags, XESCAPE_8_ثنائی_ہندسہ))
                واپس xescape_بھرا(str, /* bad= */ تہی, console_چوڑائی, flags);
        ورنہ
                واپس utf8_escape_non_printable_بھرا(str,
                                                      console_چوڑائی,
                                                      FLAGS_سیٹ(flags, XESCAPE_FORCE_ELLIPSIS));
}

حرف* octescape_بھرا(ثابت حرف *s, size_t len, ثابت حرف *bad) {
        حرف *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        أكد(s || len == 0);

        اگر (len == SIZE_MAX)
                len = لڑی_لمبائی(s);

        اگر (len > (SIZE_MAX - 1) / 4)
                واپس تہی;

        t = buf = new(حرف, len * 4 + 1);
        اگر (!buf)
                واپس تہی;

        برائے (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                اگر (u < ' ' || u >= 127 || اندر_سیٹ(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ورنہ
                        *(t++) = u;
        }

        *t = 0;
        واپس buf;
}

حرف* decescape(ثابت حرف *s, size_t len, ثابت حرف *bad) {
        حرف *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        أكد(s || len == 0);

        اگر (len == SIZE_MAX)
                len = لڑی_لمبائی(s);

        اگر (len > (SIZE_MAX - 1) / 4)
                واپس تہی;

        t = buf = new(حرف, len * 4 + 1);
        اگر (!buf)
                واپس تہی;

        برائے (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                اگر (u < ' ' || u >= 127 || اندر_سیٹ(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ورنہ
                        *(t++) = u;
        }

        *t = 0;
        واپس buf;
}

ساکن حرف* strcpy_backslash_escaped(حرف *t, ثابت حرف *s, ثابت حرف *bad) {
        أكد(bad);
        أكد(t);
        أكد(s);

        جب (*s) {
                صحیح l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                اگر (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ورنہ اگر (l == 1) {
                        اگر (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ورنہ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        واپس t;
}

حرف* shell_escape(ثابت حرف *s, ثابت حرف *bad) {
        حرف *buf, *t;

        buf = new(حرف, لڑی_لمبائی(s)*4+1);
        اگر (!buf)
                واپس تہی;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        واپس buf;
}

حرف* shell_maybe_quote(ثابت حرف *s, ShellEscapeFlags flags) {
        ثابت حرف *p;
        حرف *buf, *t;

        أكد(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        اگر (FLAGS_سیٹ(flags, SHELL_ESCAPE_خالی_خانے) && isempty(s))
                واپس strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        برائے (p = s; *p; ) {
                صحیح l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                اگر (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        توڑو;

                p += l;
        }

        اگر (!*p)
                واپس strdup(s);

        buf = new(حرف, FLAGS_سیٹ(flags, SHELL_ESCAPE_POSIX) + 1 + لڑی_لمبائی(s)*4 + 1 + 1);
        اگر (!buf)
                واپس تہی;

        t = buf;
        اگر (FLAGS_سیٹ(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ورنہ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_سیٹ(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        اگر (FLAGS_سیٹ(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ورنہ
                *(t++) = '"';
        *t = 0;

        واپس str_realloc(buf);
}

حرف* quote_command_سطر(حرف * ثابت *argv, ShellEscapeFlags flags) {
        _cleanup_free_ حرف *نتیجہ = تہی;

        أكد(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ حرف *t = تہی;

                t = shell_maybe_quote(*a, flags);
                اگر (!t)
                        واپس تہی;

                اگر (!strextend_with_separator(&نتیجہ, " ", t))
                        واپس تہی;
        }

        واپس str_realloc(TAKE_PTR(نتیجہ));
}
