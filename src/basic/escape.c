/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>
#شامل <string.h>

#شامل "alloc-util.h"
#شامل "escape.h"
#شامل "hexdecoct.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"

صحیح cescape_char(نویسه c, نویسه *buf) {
        نویسه *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        انتخاب (c) {

                حالت '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        بشکن;
                حالت '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        بشکن;
                حالت '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        بشکن;
                حالت '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        بشکن;
                حالت '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        بشکن;
                حالت '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        بشکن;
                حالت '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        بشکن;
                حالت '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        بشکن;
                حالت '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        بشکن;
                حالت '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        بشکن;

                پیشفرض:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        اگر ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned نویسه) c >> 6);
                                *(buf++) = octchar((unsigned نویسه) c >> 3);
                                *(buf++) = octchar((unsigned نویسه) c);
                        } وگرنه
                                *(buf++) = c;
                        بشکن;
        }

        بازگشت buf - buf_old;
}

نویسه* cescape_length(ثابت نویسه *s, size_t n) {
        ثابت نویسه *f;
        نویسه *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        أكد(s || n == 0);

        اگر (n == SIZE_MAX)
                n = strlen(s);

        اگر (n > (SIZE_MAX - 1) / 4)
                بازگشت پوچ;

        r = new(نویسه, n*4 + 1);
        اگر (!r)
                بازگشت پوچ;

        برای (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        بازگشت r;
}

صحیح cunescape_one(ثابت نویسه *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        صحیح r = 1;

        أكد(p);
        أكد(ret);
        أكد(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        اگر (length != SIZE_MAX && length < 1)
                بازگشت -EINVAL;

        انتخاب (p[0]) {

        حالت 'a':
                *ret = '\a';
                بشکن;
        حالت 'b':
                *ret = '\b';
                بشکن;
        حالت 'f':
                *ret = '\f';
                بشکن;
        حالت 'n':
                *ret = '\n';
                بشکن;
        حالت 'r':
                *ret = '\r';
                بشکن;
        حالت 't':
                *ret = '\t';
                بشکن;
        حالت 'v':
                *ret = '\v';
                بشکن;
        حالت '\\':
                *ret = '\\';
                بشکن;
        حالت '"':
                *ret = '"';
                بشکن;
        حالت '\'':
                *ret = '\'';
                بشکن;

        حالت 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                بشکن;

        حالت 'x': {
                /* hexadecimal encoding */
                صحیح a, b;

                اگر (length != SIZE_MAX && length < 3)
                        بازگشت -EINVAL;

                a = unhexchar(p[1]);
                اگر (a < 0)
                        بازگشت -EINVAL;

                b = unhexchar(p[2]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                /* Don't allow NUL bytes */
                اگر (a == 0 && b == 0 && !accept_nul)
                        بازگشت -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                بشکن;
        }

        حالت 'u': {
                /* C++11 style 16-bit unicode */

                صحیح a[4];
                size_t i;
                uint32_t c;

                اگر (length != SIZE_MAX && length < 5)
                        بازگشت -EINVAL;

                برای (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        اگر (a[i] < 0)
                                بازگشت a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                اگر (c == 0 && !accept_nul)
                        بازگشت -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                اگر (utf16_is_surrogate(c))
                        بازگشت -EINVAL;

                *ret = c;
                r = 5;
                بشکن;
        }

        حالت 'U': {
                /* C++11 style 32-bit unicode */

                صحیح a[8];
                size_t i;
                char32_t c;

                اگر (length != SIZE_MAX && length < 9)
                        بازگشت -EINVAL;

                برای (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        اگر (a[i] < 0)
                                بازگشت a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                اگر (c == 0 && !accept_nul)
                        بازگشت -EINVAL;

                /* Don't allow invalid code points */
                اگر (!unichar_is_valid(c))
                        بازگشت -EINVAL;

                *ret = c;
                r = 9;
                بشکن;
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
                        بازگشت -EINVAL;

                a = unoctchar(p[0]);
                اگر (a < 0)
                        بازگشت -EINVAL;

                b = unoctchar(p[1]);
                اگر (b < 0)
                        بازگشت -EINVAL;

                c = unoctchar(p[2]);
                اگر (c < 0)
                        بازگشت -EINVAL;

                /* don't allow NUL bytes */
                اگر (a == 0 && b == 0 && c == 0 && !accept_nul)
                        بازگشت -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                اگر (m > 255)
                        بازگشت -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                بشکن;
        }

        پیشفرض:
                بازگشت -EINVAL;
        }

        بازگشت r;
}

ssize_t cunescape_length_with_prefix(ثابت نویسه *s, size_t length, ثابت نویسه *prefix, UnescapeFlags flags, نویسه **ret) {
        _cleanup_free_ نویسه *ans = پوچ;
        نویسه *t;
        ثابت نویسه *f;
        size_t pl;
        صحیح r;

        أكد(s);
        أكد(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        اگر (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(نویسه, pl+length+1);
        اگر (!ans)
                بازگشت -ENOMEM;

        اگر (prefix)
                memcpy(ans, prefix, pl);

        برای (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                أكد(remaining > 0);

                اگر (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        ادامه;
                }

                اگر (remaining == 1) {
                        اگر (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                ادامه;
                        }

                        بازگشت -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                اگر (r < 0) {
                        اگر (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                ادامه;
                        }

                        بازگشت r;
                }

                f += r;
                اگر (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                وگرنه
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        أكد(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        بازگشت t - *ret;
}

نویسه* xescape_full(ثابت نویسه *s, ثابت نویسه *bad, size_t console_width, XEscapeFlags flags) {
        نویسه *ans, *t, *prev, *prev2;
        ثابت نویسه *f;

        أكد(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        اگر (console_width == 0)
                بازگشت strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        اگر (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(نویسه, len_body + 1);
        اگر (!ans)
                بازگشت پوچ;

        برای (f = s, t = prev = prev2 = ans; ; f++) {
                نویسه *tmp_t = t;

                اگر (!*f) {
                        اگر (len_forced_ellipsis != 0)
                                بشکن;

                        *t = 0;
                        بازگشت ans;
                }

                اگر ((unsigned نویسه) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned نویسه) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        اگر ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                بشکن;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } وگرنه {
                        اگر ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                بشکن;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        اگر (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        وگرنه اگر (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        وگرنه اگر (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        وگرنه
                off = len_body - c;
        أكد(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        بازگشت ans;
}

نویسه* escape_non_printable_full(ثابت نویسه *str, size_t console_width, XEscapeFlags flags) {
        اگر (FLAGS_SET(flags, XESCAPE_8_BIT))
                بازگشت xescape_full(str, /* bad= */ پوچ, console_width, flags);
        وگرنه
                بازگشت utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

نویسه* octescape_full(ثابت نویسه *s, size_t len, ثابت نویسه *bad) {
        نویسه *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        أكد(s || len == 0);

        اگر (len == SIZE_MAX)
                len = strlen(s);

        اگر (len > (SIZE_MAX - 1) / 4)
                بازگشت پوچ;

        t = buf = new(نویسه, len * 4 + 1);
        اگر (!buf)
                بازگشت پوچ;

        برای (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                اگر (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } وگرنه
                        *(t++) = u;
        }

        *t = 0;
        بازگشت buf;
}

نویسه* decescape(ثابت نویسه *s, size_t len, ثابت نویسه *bad) {
        نویسه *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        أكد(s || len == 0);

        اگر (len == SIZE_MAX)
                len = strlen(s);

        اگر (len > (SIZE_MAX - 1) / 4)
                بازگشت پوچ;

        t = buf = new(نویسه, len * 4 + 1);
        اگر (!buf)
                بازگشت پوچ;

        برای (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                اگر (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } وگرنه
                        *(t++) = u;
        }

        *t = 0;
        بازگشت buf;
}

ایستا نویسه* strcpy_backslash_escaped(نویسه *t, ثابت نویسه *s, ثابت نویسه *bad) {
        أكد(bad);
        أكد(t);
        أكد(s);

        وقتی (*s) {
                صحیح l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                اگر (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                وگرنه اگر (l == 1) {
                        اگر (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } وگرنه {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        بازگشت t;
}

نویسه* shell_escape(ثابت نویسه *s, ثابت نویسه *bad) {
        نویسه *buf, *t;

        buf = new(نویسه, strlen(s)*4+1);
        اگر (!buf)
                بازگشت پوچ;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        بازگشت buf;
}

نویسه* shell_maybe_quote(ثابت نویسه *s, ShellEscapeFlags flags) {
        ثابت نویسه *p;
        نویسه *buf, *t;

        أكد(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        اگر (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                بازگشت strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        برای (p = s; *p; ) {
                صحیح l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                اگر (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        بشکن;

                p += l;
        }

        اگر (!*p)
                بازگشت strdup(s);

        buf = new(نویسه, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        اگر (!buf)
                بازگشت پوچ;

        t = buf;
        اگر (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } وگرنه
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        اگر (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        وگرنه
                *(t++) = '"';
        *t = 0;

        بازگشت str_realloc(buf);
}

نویسه* quote_command_سطر(نویسه * ثابت *argv, ShellEscapeFlags flags) {
        _cleanup_free_ نویسه *result = پوچ;

        أكد(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ نویسه *t = پوچ;

                t = shell_maybe_quote(*a, flags);
                اگر (!t)
                        بازگشت پوچ;

                اگر (!strextend_with_separator(&result, " ", t))
                        بازگشت پوچ;
        }

        بازگشت str_realloc(TAKE_PTR(result));
}
