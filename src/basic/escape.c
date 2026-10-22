/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "utf8.h"

পূর্ণসংখ্যা cescape_char(অক্ষর c, অক্ষর *buf) {
        অক্ষর *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        নির্বাচন (c) {

                ক্ষেত্র '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        বিরতি;
                ক্ষেত্র '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        বিরতি;
                ক্ষেত্র '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        বিরতি;
                ক্ষেত্র '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        বিরতি;
                ক্ষেত্র '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        বিরতি;
                ক্ষেত্র '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        বিরতি;
                ক্ষেত্র '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        বিরতি;
                ক্ষেত্র '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        বিরতি;
                ক্ষেত্র '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        বিরতি;
                ক্ষেত্র '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        বিরতি;

                পূর্বনির্ধারিত:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        যদি ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned অক্ষর) c >> 6);
                                *(buf++) = octchar((unsigned অক্ষর) c >> 3);
                                *(buf++) = octchar((unsigned অক্ষর) c);
                        } অন্যথা
                                *(buf++) = c;
                        বিরতি;
        }

        ফেরত buf - buf_old;
}

অক্ষর* cescape_দৈর্ঘ্য(ধ্রুবক অক্ষর *s, size_t n) {
        ধ্রুবক অক্ষর *f;
        অক্ষর *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        প্রতিজ্ঞা(s || n == 0);

        যদি (n == SIZE_MAX)
                n = strlen(s);

        যদি (n > (SIZE_MAX - 1) / 4)
                ফেরত ফাঁকা;

        r = new(অক্ষর, n*4 + 1);
        যদি (!r)
                ফেরত ফাঁকা;

        জন্য (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ফেরত r;
}

পূর্ণসংখ্যা cunescape_one(ধ্রুবক অক্ষর *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        পূর্ণসংখ্যা r = 1;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        যদি (length != SIZE_MAX && length < 1)
                ফেরত -EINVAL;

        নির্বাচন (p[0]) {

        ক্ষেত্র 'a':
                *ret = '\a';
                বিরতি;
        ক্ষেত্র 'b':
                *ret = '\b';
                বিরতি;
        ক্ষেত্র 'f':
                *ret = '\f';
                বিরতি;
        ক্ষেত্র 'n':
                *ret = '\n';
                বিরতি;
        ক্ষেত্র 'r':
                *ret = '\r';
                বিরতি;
        ক্ষেত্র 't':
                *ret = '\t';
                বিরতি;
        ক্ষেত্র 'v':
                *ret = '\v';
                বিরতি;
        ক্ষেত্র '\\':
                *ret = '\\';
                বিরতি;
        ক্ষেত্র '"':
                *ret = '"';
                বিরতি;
        ক্ষেত্র '\'':
                *ret = '\'';
                বিরতি;

        ক্ষেত্র 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                বিরতি;

        ক্ষেত্র 'x': {
                /* hexadecimal encoding */
                পূর্ণসংখ্যা a, b;

                যদি (length != SIZE_MAX && length < 3)
                        ফেরত -EINVAL;

                a = unhexchar(p[1]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unhexchar(p[2]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                /* Don't allow NUL bytes */
                যদি (a == 0 && b == 0 && !accept_nul)
                        ফেরত -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                বিরতি;
        }

        ক্ষেত্র 'u': {
                /* C++11 style 16-bit unicode */

                পূর্ণসংখ্যা a[4];
                size_t i;
                uint32_t c;

                যদি (length != SIZE_MAX && length < 5)
                        ফেরত -EINVAL;

                জন্য (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        যদি (a[i] < 0)
                                ফেরত a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                যদি (c == 0 && !accept_nul)
                        ফেরত -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                যদি (utf16_is_surrogate(c))
                        ফেরত -EINVAL;

                *ret = c;
                r = 5;
                বিরতি;
        }

        ক্ষেত্র 'U': {
                /* C++11 style 32-bit unicode */

                পূর্ণসংখ্যা a[8];
                size_t i;
                char32_t c;

                যদি (length != SIZE_MAX && length < 9)
                        ফেরত -EINVAL;

                জন্য (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        যদি (a[i] < 0)
                                ফেরত a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                যদি (c == 0 && !accept_nul)
                        ফেরত -EINVAL;

                /* Don't allow invalid code points */
                যদি (!unichar_is_valid(c))
                        ফেরত -EINVAL;

                *ret = c;
                r = 9;
                বিরতি;
        }

        ক্ষেত্র '0':
        ক্ষেত্র '1':
        ক্ষেত্র '2':
        ক্ষেত্র '3':
        ক্ষেত্র '4':
        ক্ষেত্র '5':
        ক্ষেত্র '6':
        ক্ষেত্র '7': {
                /* octal encoding */
                পূর্ণসংখ্যা a, b, c;
                char32_t m;

                যদি (length != SIZE_MAX && length < 3)
                        ফেরত -EINVAL;

                a = unoctchar(p[0]);
                যদি (a < 0)
                        ফেরত -EINVAL;

                b = unoctchar(p[1]);
                যদি (b < 0)
                        ফেরত -EINVAL;

                c = unoctchar(p[2]);
                যদি (c < 0)
                        ফেরত -EINVAL;

                /* don't allow NUL bytes */
                যদি (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ফেরত -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                যদি (m > 255)
                        ফেরত -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                বিরতি;
        }

        পূর্বনির্ধারিত:
                ফেরত -EINVAL;
        }

        ফেরত r;
}

ssize_t cunescape_দৈর্ঘ্য_with_prefix(ধ্রুবক অক্ষর *s, size_t length, ধ্রুবক অক্ষর *prefix, UnescapeFlags flags, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *ans = ফাঁকা;
        অক্ষর *t;
        ধ্রুবক অক্ষর *f;
        size_t pl;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        যদি (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(অক্ষর, pl+length+1);
        যদি (!ans)
                ফেরত -ENOMEM;

        যদি (prefix)
                memcpy(ans, prefix, pl);

        জন্য (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                প্রতিজ্ঞা(remaining > 0);

                যদি (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        চালু;
                }

                যদি (remaining == 1) {
                        যদি (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                চালু;
                        }

                        ফেরত -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                যদি (r < 0) {
                        যদি (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                চালু;
                        }

                        ফেরত r;
                }

                f += r;
                যদি (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                অন্যথা
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        প্রতিজ্ঞা(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ফেরত t - *ret;
}

অক্ষর* xescape_full(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad, size_t console_width, XEscapeFlags flags) {
        অক্ষর *ans, *t, *পূর্ব, *prev2;
        ধ্রুবক অক্ষর *f;

        প্রতিজ্ঞা(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        যদি (console_width == 0)
                ফেরত strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        যদি (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(অক্ষর, len_body + 1);
        যদি (!ans)
                ফেরত ফাঁকা;

        জন্য (f = s, t = পূর্ব = prev2 = ans; ; f++) {
                অক্ষর *tmp_t = t;

                যদি (!*f) {
                        যদি (len_forced_ellipsis != 0)
                                বিরতি;

                        *t = 0;
                        ফেরত ans;
                }

                যদি ((unsigned অক্ষর) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned অক্ষর) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        যদি ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                বিরতি;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } অন্যথা {
                        যদি ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                বিরতি;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = পূর্ব;
                পূর্ব = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        যদি (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        অন্যথা যদি (len_body - c >= (size_t) (পূর্ব - ans))
                off = (size_t) (পূর্ব - ans);
        অন্যথা যদি (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        অন্যথা
                off = len_body - c;
        প্রতিজ্ঞা(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        ফেরত ans;
}

অক্ষর* escape_non_printable_full(ধ্রুবক অক্ষর *str, size_t console_width, XEscapeFlags flags) {
        যদি (FLAGS_SET(flags, XESCAPE_8_BIT))
                ফেরত xescape_full(str, /* bad= */ ফাঁকা, console_width, flags);
        অন্যথা
                ফেরত utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

অক্ষর* octescape_full(ধ্রুবক অক্ষর *s, size_t len, ধ্রুবক অক্ষর *bad) {
        অক্ষর *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        প্রতিজ্ঞা(s || len == 0);

        যদি (len == SIZE_MAX)
                len = strlen(s);

        যদি (len > (SIZE_MAX - 1) / 4)
                ফেরত ফাঁকা;

        t = buf = new(অক্ষর, len * 4 + 1);
        যদি (!buf)
                ফেরত ফাঁকা;

        জন্য (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                যদি (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } অন্যথা
                        *(t++) = u;
        }

        *t = 0;
        ফেরত buf;
}

অক্ষর* decescape(ধ্রুবক অক্ষর *s, size_t len, ধ্রুবক অক্ষর *bad) {
        অক্ষর *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        প্রতিজ্ঞা(s || len == 0);

        যদি (len == SIZE_MAX)
                len = strlen(s);

        যদি (len > (SIZE_MAX - 1) / 4)
                ফেরত ফাঁকা;

        t = buf = new(অক্ষর, len * 4 + 1);
        যদি (!buf)
                ফেরত ফাঁকা;

        জন্য (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                যদি (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } অন্যথা
                        *(t++) = u;
        }

        *t = 0;
        ফেরত buf;
}

স্থির অক্ষর* strcpy_backslash_escaped(অক্ষর *t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad) {
        প্রতিজ্ঞা(bad);
        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        যখন (*s) {
                পূর্ণসংখ্যা l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                যদি (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                অন্যথা যদি (l == 1) {
                        যদি (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } অন্যথা {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ফেরত t;
}

অক্ষর* shell_escape(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad) {
        অক্ষর *buf, *t;

        buf = new(অক্ষর, strlen(s)*4+1);
        যদি (!buf)
                ফেরত ফাঁকা;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ফেরত buf;
}

অক্ষর* shell_maybe_quote(ধ্রুবক অক্ষর *s, ShellEscapeFlags flags) {
        ধ্রুবক অক্ষর *p;
        অক্ষর *buf, *t;

        প্রতিজ্ঞা(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        যদি (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                ফেরত strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        জন্য (p = s; *p; ) {
                পূর্ণসংখ্যা l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                যদি (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        বিরতি;

                p += l;
        }

        যদি (!*p)
                ফেরত strdup(s);

        buf = new(অক্ষর, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        যদি (!buf)
                ফেরত ফাঁকা;

        t = buf;
        যদি (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } অন্যথা
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        যদি (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        অন্যথা
                *(t++) = '"';
        *t = 0;

        ফেরত str_realloc(buf);
}

অক্ষর* quote_command_সারি(অক্ষর * ধ্রুবক *argv, ShellEscapeFlags flags) {
        _cleanup_free_ অক্ষর *ফলাফল = ফাঁকা;

        প্রতিজ্ঞা(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ অক্ষর *t = ফাঁকা;

                t = shell_maybe_quote(*a, flags);
                যদি (!t)
                        ফেরত ফাঁকা;

                যদি (!strextend_with_separator(&ফলাফল, " ", t))
                        ফেরত ফাঁকা;
        }

        ফেরত str_realloc(TAKE_PTR(ফলাফল));
}
