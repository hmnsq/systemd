/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"

पूर्णाङ्क cescape_char(वर्ण c, वर्ण *buf) {
        वर्ण *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        विकल्प (c) {

                पक्ष '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        रोक;
                पक्ष '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        रोक;
                पक्ष '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        रोक;
                पक्ष '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        रोक;
                पक्ष '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        रोक;
                पक्ष '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        रोक;
                पक्ष '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        रोक;
                पक्ष '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        रोक;
                पक्ष '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        रोक;
                पक्ष '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        रोक;

                स्वभाव:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        यदि ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((अचिन्हित वर्ण) c >> 6);
                                *(buf++) = octchar((अचिन्हित वर्ण) c >> 3);
                                *(buf++) = octchar((अचिन्हित वर्ण) c);
                        } अन्यथा
                                *(buf++) = c;
                        रोक;
        }

        फर्काऊ buf - buf_old;
}

वर्ण* cescape_length(अचर वर्ण *s, माप_प्रकार n) {
        अचर वर्ण *f;
        वर्ण *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        प्रतिज्ञा(s || n == 0);

        यदि (n == SIZE_MAX)
                n = म_माप(s);

        यदि (n > (SIZE_MAX - 1) / 4)
                फर्काऊ रिक्त;

        r = new(वर्ण, n*4 + 1);
        यदि (!r)
                फर्काऊ रिक्त;

        लागि (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        फर्काऊ r;
}

पूर्णाङ्क cunescape_one(अचर वर्ण *p, माप_प्रकार length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        पूर्णाङ्क r = 1;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);
        प्रतिज्ञा(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        यदि (length != SIZE_MAX && length < 1)
                फर्काऊ -EINVAL;

        विकल्प (p[0]) {

        पक्ष 'a':
                *ret = '\a';
                रोक;
        पक्ष 'b':
                *ret = '\b';
                रोक;
        पक्ष 'f':
                *ret = '\f';
                रोक;
        पक्ष 'n':
                *ret = '\n';
                रोक;
        पक्ष 'r':
                *ret = '\r';
                रोक;
        पक्ष 't':
                *ret = '\t';
                रोक;
        पक्ष 'v':
                *ret = '\v';
                रोक;
        पक्ष '\\':
                *ret = '\\';
                रोक;
        पक्ष '"':
                *ret = '"';
                रोक;
        पक्ष '\'':
                *ret = '\'';
                रोक;

        पक्ष 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                रोक;

        पक्ष 'x': {
                /* hexadecimal encoding */
                पूर्णाङ्क a, b;

                यदि (length != SIZE_MAX && length < 3)
                        फर्काऊ -EINVAL;

                a = unhexchar(p[1]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unhexchar(p[2]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                /* Don't allow NUL bytes */
                यदि (a == 0 && b == 0 && !accept_nul)
                        फर्काऊ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                रोक;
        }

        पक्ष 'u': {
                /* C++11 style 16-bit unicode */

                पूर्णाङ्क a[4];
                माप_प्रकार i;
                uint32_t c;

                यदि (length != SIZE_MAX && length < 5)
                        फर्काऊ -EINVAL;

                लागि (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        यदि (a[i] < 0)
                                फर्काऊ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                यदि (c == 0 && !accept_nul)
                        फर्काऊ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                यदि (utf16_is_surrogate(c))
                        फर्काऊ -EINVAL;

                *ret = c;
                r = 5;
                रोक;
        }

        पक्ष 'U': {
                /* C++11 style 32-bit unicode */

                पूर्णाङ्क a[8];
                माप_प्रकार i;
                char32_t c;

                यदि (length != SIZE_MAX && length < 9)
                        फर्काऊ -EINVAL;

                लागि (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        यदि (a[i] < 0)
                                फर्काऊ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                यदि (c == 0 && !accept_nul)
                        फर्काऊ -EINVAL;

                /* Don't allow invalid code points */
                यदि (!unichar_is_valid(c))
                        फर्काऊ -EINVAL;

                *ret = c;
                r = 9;
                रोक;
        }

        पक्ष '0':
        पक्ष '1':
        पक्ष '2':
        पक्ष '3':
        पक्ष '4':
        पक्ष '5':
        पक्ष '6':
        पक्ष '7': {
                /* octal encoding */
                पूर्णाङ्क a, b, c;
                char32_t m;

                यदि (length != SIZE_MAX && length < 3)
                        फर्काऊ -EINVAL;

                a = unoctchar(p[0]);
                यदि (a < 0)
                        फर्काऊ -EINVAL;

                b = unoctchar(p[1]);
                यदि (b < 0)
                        फर्काऊ -EINVAL;

                c = unoctchar(p[2]);
                यदि (c < 0)
                        फर्काऊ -EINVAL;

                /* don't allow NUL bytes */
                यदि (a == 0 && b == 0 && c == 0 && !accept_nul)
                        फर्काऊ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                यदि (m > 255)
                        फर्काऊ -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                रोक;
        }

        स्वभाव:
                फर्काऊ -EINVAL;
        }

        फर्काऊ r;
}

ssize_t cunescape_length_with_prefix(अचर वर्ण *s, माप_प्रकार length, अचर वर्ण *prefix, UnescapeFlags flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *ans = रिक्त;
        वर्ण *t;
        अचर वर्ण *f;
        माप_प्रकार pl;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        यदि (length == SIZE_MAX)
                length = म_माप(s);

        pl = strlen_ptr(prefix);

        ans = new(वर्ण, pl+length+1);
        यदि (!ans)
                फर्काऊ -ENOMEM;

        यदि (prefix)
                स_नकल(ans, prefix, pl);

        लागि (f = s, t = ans + pl; f < s + length; f++) {
                माप_प्रकार remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                प्रतिज्ञा(remaining > 0);

                यदि (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        जारी;
                }

                यदि (remaining == 1) {
                        यदि (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                जारी;
                        }

                        फर्काऊ -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                यदि (r < 0) {
                        यदि (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                जारी;
                        }

                        फर्काऊ r;
                }

                f += r;
                यदि (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                अन्यथा
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        प्रतिज्ञा(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        फर्काऊ t - *ret;
}

वर्ण* xescape_full(अचर वर्ण *s, अचर वर्ण *bad, माप_प्रकार console_width, XEscapeFlags flags) {
        वर्ण *ans, *t, *prev, *prev2;
        अचर वर्ण *f;

        प्रतिज्ञा(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        यदि (console_width == 0)
                फर्काऊ strdup("");

        माप_प्रकार len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        माप_प्रकार len_s = म_माप(s);

        माप_प्रकार len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        यदि (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(वर्ण, len_body + 1);
        यदि (!ans)
                फर्काऊ रिक्त;

        लागि (f = s, t = prev = prev2 = ans; ; f++) {
                वर्ण *tmp_t = t;

                यदि (!*f) {
                        यदि (len_forced_ellipsis != 0)
                                रोक;

                        *t = 0;
                        फर्काऊ ans;
                }

                यदि ((अचिन्हित वर्ण) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (अचिन्हित वर्ण) *f >= 127) ||
                    *f == '\\' || (bad && म_अक्षर(bad, *f))) {
                        यदि ((माप_प्रकार) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                रोक;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अन्यथा {
                        यदि ((माप_प्रकार) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                रोक;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        माप_प्रकार c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        माप_प्रकार off;
        यदि (len_body - c >= (माप_प्रकार) (t - ans))
                off = (माप_प्रकार) (t - ans);
        अन्यथा यदि (len_body - c >= (माप_प्रकार) (prev - ans))
                off = (माप_प्रकार) (prev - ans);
        अन्यथा यदि (len_body - c >= (माप_प्रकार) (prev2 - ans))
                off = (माप_प्रकार) (prev2 - ans);
        अन्यथा
                off = len_body - c;
        प्रतिज्ञा(off <= (माप_प्रकार) (t - ans));

        स_नकल(ans + off, "...", c);
        ans[off + c] = '\0';
        फर्काऊ ans;
}

वर्ण* escape_non_printable_full(अचर वर्ण *str, माप_प्रकार console_width, XEscapeFlags flags) {
        यदि (FLAGS_SET(flags, XESCAPE_8_BIT))
                फर्काऊ xescape_full(str, /* bad= */ रिक्त, console_width, flags);
        अन्यथा
                फर्काऊ utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

वर्ण* octescape_full(अचर वर्ण *s, माप_प्रकार len, अचर वर्ण *bad) {
        वर्ण *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        प्रतिज्ञा(s || len == 0);

        यदि (len == SIZE_MAX)
                len = म_माप(s);

        यदि (len > (SIZE_MAX - 1) / 4)
                फर्काऊ रिक्त;

        t = buf = new(वर्ण, len * 4 + 1);
        यदि (!buf)
                फर्काऊ रिक्त;

        लागि (माप_प्रकार i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                यदि (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && म_अक्षर(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } अन्यथा
                        *(t++) = u;
        }

        *t = 0;
        फर्काऊ buf;
}

वर्ण* decescape(अचर वर्ण *s, माप_प्रकार len, अचर वर्ण *bad) {
        वर्ण *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        प्रतिज्ञा(s || len == 0);

        यदि (len == SIZE_MAX)
                len = म_माप(s);

        यदि (len > (SIZE_MAX - 1) / 4)
                फर्काऊ रिक्त;

        t = buf = new(वर्ण, len * 4 + 1);
        यदि (!buf)
                फर्काऊ रिक्त;

        लागि (माप_प्रकार i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                यदि (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || म_अक्षर(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } अन्यथा
                        *(t++) = u;
        }

        *t = 0;
        फर्काऊ buf;
}

स्थिर वर्ण* strcpy_backslash_escaped(वर्ण *t, अचर वर्ण *s, अचर वर्ण *bad) {
        प्रतिज्ञा(bad);
        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        जबसम्म (*s) {
                पूर्णाङ्क l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                यदि (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                अन्यथा यदि (l == 1) {
                        यदि (*s == '\\' || म_अक्षर(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } अन्यथा {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        फर्काऊ t;
}

वर्ण* shell_escape(अचर वर्ण *s, अचर वर्ण *bad) {
        वर्ण *buf, *t;

        buf = new(वर्ण, म_माप(s)*4+1);
        यदि (!buf)
                फर्काऊ रिक्त;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        फर्काऊ buf;
}

वर्ण* shell_maybe_quote(अचर वर्ण *s, ShellEscapeFlags flags) {
        अचर वर्ण *p;
        वर्ण *buf, *t;

        प्रतिज्ञा(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        यदि (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                फर्काऊ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        लागि (p = s; *p; ) {
                पूर्णाङ्क l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                यदि (char_is_cc(*p) || l < 0 ||
                    म_अक्षर(WHITESPACE SHELL_NEED_QUOTES, *p))
                        रोक;

                p += l;
        }

        यदि (!*p)
                फर्काऊ strdup(s);

        buf = new(वर्ण, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + म_माप(s)*4 + 1 + 1);
        यदि (!buf)
                फर्काऊ रिक्त;

        t = buf;
        यदि (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } अन्यथा
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        यदि (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        अन्यथा
                *(t++) = '"';
        *t = 0;

        फर्काऊ str_realloc(buf);
}

वर्ण* quote_command_पङ्क्ति(वर्ण * अचर *argv, ShellEscapeFlags flags) {
        _cleanup_free_ वर्ण *result = रिक्त;

        प्रतिज्ञा(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ वर्ण *t = रिक्त;

                t = shell_maybe_quote(*a, flags);
                यदि (!t)
                        फर्काऊ रिक्त;

                यदि (!strextend_with_separator(&result, " ", t))
                        फर्काऊ रिक्त;
        }

        फर्काऊ str_realloc(TAKE_PTR(result));
}
