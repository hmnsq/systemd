/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>
#समावेश <string.h>

#समावेश "alloc-util.h"
#समावेश "escape.h"
#समावेश "hexdecoct.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "utf8.h"

पूर्णांक cescape_char(अक्षर c, अक्षर *buf) {
        अक्षर *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        चयन (c) {

                हाल '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        अवरोध;
                हाल '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        अवरोध;
                हाल '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        अवरोध;
                हाल '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        अवरोध;
                हाल '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        अवरोध;
                हाल '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        अवरोध;
                हाल '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        अवरोध;
                हाल '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        अवरोध;
                हाल '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        अवरोध;
                हाल '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        अवरोध;

                शेष:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        अगर ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((अचिन्हित अक्षर) c >> 6);
                                *(buf++) = octchar((अचिन्हित अक्षर) c >> 3);
                                *(buf++) = octchar((अचिन्हित अक्षर) c);
                        } अन्यथा
                                *(buf++) = c;
                        अवरोध;
        }

        वापस buf - buf_old;
}

अक्षर* cescape_लंबाई(स्थिर अक्षर *s, माप_प्रकार n) {
        स्थिर अक्षर *f;
        अक्षर *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        निश्चित(s || n == 0);

        अगर (n == SIZE_MAX)
                n = म_माप(s);

        अगर (n > (SIZE_MAX - 1) / 4)
                वापस शून्य;

        r = new(अक्षर, n*4 + 1);
        अगर (!r)
                वापस शून्य;

        क्रम (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        वापस r;
}

पूर्णांक cunescape_one(स्थिर अक्षर *p, माप_प्रकार length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        पूर्णांक r = 1;

        निश्चित(p);
        निश्चित(ret);
        निश्चित(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        अगर (length != SIZE_MAX && length < 1)
                वापस -EINVAL;

        चयन (p[0]) {

        हाल 'a':
                *ret = '\a';
                अवरोध;
        हाल 'b':
                *ret = '\b';
                अवरोध;
        हाल 'f':
                *ret = '\f';
                अवरोध;
        हाल 'n':
                *ret = '\n';
                अवरोध;
        हाल 'r':
                *ret = '\r';
                अवरोध;
        हाल 't':
                *ret = '\t';
                अवरोध;
        हाल 'v':
                *ret = '\v';
                अवरोध;
        हाल '\\':
                *ret = '\\';
                अवरोध;
        हाल '"':
                *ret = '"';
                अवरोध;
        हाल '\'':
                *ret = '\'';
                अवरोध;

        हाल 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                अवरोध;

        हाल 'x': {
                /* hexadecimal encoding */
                पूर्णांक a, b;

                अगर (length != SIZE_MAX && length < 3)
                        वापस -EINVAL;

                a = unhexchar(p[1]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unhexchar(p[2]);
                अगर (b < 0)
                        वापस -EINVAL;

                /* Don't allow NUL bytes */
                अगर (a == 0 && b == 0 && !accept_nul)
                        वापस -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                अवरोध;
        }

        हाल 'u': {
                /* C++11 style 16-bit unicode */

                पूर्णांक a[4];
                माप_प्रकार i;
                uint32_t c;

                अगर (length != SIZE_MAX && length < 5)
                        वापस -EINVAL;

                क्रम (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        अगर (a[i] < 0)
                                वापस a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                अगर (c == 0 && !accept_nul)
                        वापस -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                अगर (utf16_is_surrogate(c))
                        वापस -EINVAL;

                *ret = c;
                r = 5;
                अवरोध;
        }

        हाल 'U': {
                /* C++11 style 32-bit unicode */

                पूर्णांक a[8];
                माप_प्रकार i;
                char32_t c;

                अगर (length != SIZE_MAX && length < 9)
                        वापस -EINVAL;

                क्रम (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        अगर (a[i] < 0)
                                वापस a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                अगर (c == 0 && !accept_nul)
                        वापस -EINVAL;

                /* Don't allow invalid code points */
                अगर (!unichar_is_valid(c))
                        वापस -EINVAL;

                *ret = c;
                r = 9;
                अवरोध;
        }

        हाल '0':
        हाल '1':
        हाल '2':
        हाल '3':
        हाल '4':
        हाल '5':
        हाल '6':
        हाल '7': {
                /* octal encoding */
                पूर्णांक a, b, c;
                char32_t m;

                अगर (length != SIZE_MAX && length < 3)
                        वापस -EINVAL;

                a = unoctchar(p[0]);
                अगर (a < 0)
                        वापस -EINVAL;

                b = unoctchar(p[1]);
                अगर (b < 0)
                        वापस -EINVAL;

                c = unoctchar(p[2]);
                अगर (c < 0)
                        वापस -EINVAL;

                /* don't allow NUL bytes */
                अगर (a == 0 && b == 0 && c == 0 && !accept_nul)
                        वापस -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                अगर (m > 255)
                        वापस -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                अवरोध;
        }

        शेष:
                वापस -EINVAL;
        }

        वापस r;
}

ssize_t cunescape_लंबाई_with_prefix(स्थिर अक्षर *s, माप_प्रकार length, स्थिर अक्षर *prefix, UnescapeFlags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *ans = शून्य;
        अक्षर *t;
        स्थिर अक्षर *f;
        माप_प्रकार pl;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        अगर (length == SIZE_MAX)
                length = म_माप(s);

        pl = strlen_ptr(prefix);

        ans = new(अक्षर, pl+length+1);
        अगर (!ans)
                वापस -ENOMEM;

        अगर (prefix)
                स_नकल(ans, prefix, pl);

        क्रम (f = s, t = ans + pl; f < s + length; f++) {
                माप_प्रकार remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                निश्चित(remaining > 0);

                अगर (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        जारी;
                }

                अगर (remaining == 1) {
                        अगर (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                जारी;
                        }

                        वापस -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                अगर (r < 0) {
                        अगर (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                जारी;
                        }

                        वापस r;
                }

                f += r;
                अगर (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                अन्यथा
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        निश्चित(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        वापस t - *ret;
}

अक्षर* xescape_full(स्थिर अक्षर *s, स्थिर अक्षर *bad, माप_प्रकार console_width, XEscapeFlags flags) {
        अक्षर *ans, *t, *पूर्व, *prev2;
        स्थिर अक्षर *f;

        निश्चित(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        अगर (console_width == 0)
                वापस strdup("");

        माप_प्रकार len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        माप_प्रकार len_s = म_माप(s);

        माप_प्रकार len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        अगर (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(अक्षर, len_body + 1);
        अगर (!ans)
                वापस शून्य;

        क्रम (f = s, t = पूर्व = prev2 = ans; ; f++) {
                अक्षर *tmp_t = t;

                अगर (!*f) {
                        अगर (len_forced_ellipsis != 0)
                                अवरोध;

                        *t = 0;
                        वापस ans;
                }

                अगर ((अचिन्हित अक्षर) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (अचिन्हित अक्षर) *f >= 127) ||
                    *f == '\\' || (bad && म_अक्षर(bad, *f))) {
                        अगर ((माप_प्रकार) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                अवरोध;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अन्यथा {
                        अगर ((माप_प्रकार) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                अवरोध;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = पूर्व;
                पूर्व = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        माप_प्रकार c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        माप_प्रकार off;
        अगर (len_body - c >= (माप_प्रकार) (t - ans))
                off = (माप_प्रकार) (t - ans);
        अन्यथा अगर (len_body - c >= (माप_प्रकार) (पूर्व - ans))
                off = (माप_प्रकार) (पूर्व - ans);
        अन्यथा अगर (len_body - c >= (माप_प्रकार) (prev2 - ans))
                off = (माप_प्रकार) (prev2 - ans);
        अन्यथा
                off = len_body - c;
        निश्चित(off <= (माप_प्रकार) (t - ans));

        स_नकल(ans + off, "...", c);
        ans[off + c] = '\0';
        वापस ans;
}

अक्षर* escape_non_printable_full(स्थिर अक्षर *str, माप_प्रकार console_width, XEscapeFlags flags) {
        अगर (FLAGS_SET(flags, XESCAPE_8_BIT))
                वापस xescape_full(str, /* bad= */ शून्य, console_width, flags);
        अन्यथा
                वापस utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

अक्षर* octescape_full(स्थिर अक्षर *s, माप_प्रकार len, स्थिर अक्षर *bad) {
        अक्षर *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        निश्चित(s || len == 0);

        अगर (len == SIZE_MAX)
                len = म_माप(s);

        अगर (len > (SIZE_MAX - 1) / 4)
                वापस शून्य;

        t = buf = new(अक्षर, len * 4 + 1);
        अगर (!buf)
                वापस शून्य;

        क्रम (माप_प्रकार i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                अगर (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && म_अक्षर(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } अन्यथा
                        *(t++) = u;
        }

        *t = 0;
        वापस buf;
}

अक्षर* decescape(स्थिर अक्षर *s, माप_प्रकार len, स्थिर अक्षर *bad) {
        अक्षर *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        निश्चित(s || len == 0);

        अगर (len == SIZE_MAX)
                len = म_माप(s);

        अगर (len > (SIZE_MAX - 1) / 4)
                वापस शून्य;

        t = buf = new(अक्षर, len * 4 + 1);
        अगर (!buf)
                वापस शून्य;

        क्रम (माप_प्रकार i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                अगर (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || म_अक्षर(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } अन्यथा
                        *(t++) = u;
        }

        *t = 0;
        वापस buf;
}

अटल अक्षर* strcpy_backslash_escaped(अक्षर *t, स्थिर अक्षर *s, स्थिर अक्षर *bad) {
        निश्चित(bad);
        निश्चित(t);
        निश्चित(s);

        जबतक (*s) {
                पूर्णांक l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                अगर (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                अन्यथा अगर (l == 1) {
                        अगर (*s == '\\' || म_अक्षर(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } अन्यथा {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        वापस t;
}

अक्षर* shell_escape(स्थिर अक्षर *s, स्थिर अक्षर *bad) {
        अक्षर *buf, *t;

        buf = new(अक्षर, म_माप(s)*4+1);
        अगर (!buf)
                वापस शून्य;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        वापस buf;
}

अक्षर* shell_maybe_quote(स्थिर अक्षर *s, ShellEscapeFlags flags) {
        स्थिर अक्षर *p;
        अक्षर *buf, *t;

        निश्चित(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        अगर (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                वापस strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        क्रम (p = s; *p; ) {
                पूर्णांक l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                अगर (char_is_cc(*p) || l < 0 ||
                    म_अक्षर(WHITESPACE SHELL_NEED_QUOTES, *p))
                        अवरोध;

                p += l;
        }

        अगर (!*p)
                वापस strdup(s);

        buf = new(अक्षर, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + म_माप(s)*4 + 1 + 1);
        अगर (!buf)
                वापस शून्य;

        t = buf;
        अगर (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } अन्यथा
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        अगर (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        अन्यथा
                *(t++) = '"';
        *t = 0;

        वापस str_realloc(buf);
}

अक्षर* quote_command_पंक्ति(अक्षर * स्थिर *argv, ShellEscapeFlags flags) {
        _cleanup_free_ अक्षर *परिणाम = शून्य;

        निश्चित(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ अक्षर *t = शून्य;

                t = shell_maybe_quote(*a, flags);
                अगर (!t)
                        वापस शून्य;

                अगर (!strextend_with_separator(&परिणाम, " ", t))
                        वापस शून्य;
        }

        वापस str_realloc(TAKE_PTR(परिणाम));
}
