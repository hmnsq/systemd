/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"

पूर्णांक cescape_char(अक्षर c, अक्षर *buf) {
        अक्षर *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        निवड (c) {

                स्थिती '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        थांबा;
                स्थिती '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        थांबा;
                स्थिती '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        थांबा;
                स्थिती '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        थांबा;
                स्थिती '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        थांबा;
                स्थिती '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        थांबा;
                स्थिती '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        थांबा;
                स्थिती '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        थांबा;
                स्थिती '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        थांबा;
                स्थिती '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        थांबा;

                पूर्वनिर्धारित:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        जर ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((अचिन्हित अक्षर) c >> 6);
                                *(buf++) = octchar((अचिन्हित अक्षर) c >> 3);
                                *(buf++) = octchar((अचिन्हित अक्षर) c);
                        } अन्यथा
                                *(buf++) = c;
                        थांबा;
        }

        परत buf - buf_old;
}

अक्षर* cescape_length(स्थिरांक अक्षर *s, माप_प्रकार n) {
        स्थिरांक अक्षर *f;
        अक्षर *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        प्रतिज्ञा(s || n == 0);

        जर (n == SIZE_MAX)
                n = म_माप(s);

        जर (n > (SIZE_MAX - 1) / 4)
                परत रिक्त;

        r = new(अक्षर, n*4 + 1);
        जर (!r)
                परत रिक्त;

        साठी (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        परत r;
}

पूर्णांक cunescape_one(स्थिरांक अक्षर *p, माप_प्रकार length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        पूर्णांक r = 1;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);
        प्रतिज्ञा(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        जर (length != SIZE_MAX && length < 1)
                परत -EINVAL;

        निवड (p[0]) {

        स्थिती 'a':
                *ret = '\a';
                थांबा;
        स्थिती 'b':
                *ret = '\b';
                थांबा;
        स्थिती 'f':
                *ret = '\f';
                थांबा;
        स्थिती 'n':
                *ret = '\n';
                थांबा;
        स्थिती 'r':
                *ret = '\r';
                थांबा;
        स्थिती 't':
                *ret = '\t';
                थांबा;
        स्थिती 'v':
                *ret = '\v';
                थांबा;
        स्थिती '\\':
                *ret = '\\';
                थांबा;
        स्थिती '"':
                *ret = '"';
                थांबा;
        स्थिती '\'':
                *ret = '\'';
                थांबा;

        स्थिती 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                थांबा;

        स्थिती 'x': {
                /* hexadecimal encoding */
                पूर्णांक a, b;

                जर (length != SIZE_MAX && length < 3)
                        परत -EINVAL;

                a = unhexchar(p[1]);
                जर (a < 0)
                        परत -EINVAL;

                b = unhexchar(p[2]);
                जर (b < 0)
                        परत -EINVAL;

                /* Don't allow NUL bytes */
                जर (a == 0 && b == 0 && !accept_nul)
                        परत -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                थांबा;
        }

        स्थिती 'u': {
                /* C++11 style 16-bit unicode */

                पूर्णांक a[4];
                माप_प्रकार i;
                uint32_t c;

                जर (length != SIZE_MAX && length < 5)
                        परत -EINVAL;

                साठी (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        जर (a[i] < 0)
                                परत a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                जर (c == 0 && !accept_nul)
                        परत -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                जर (utf16_is_surrogate(c))
                        परत -EINVAL;

                *ret = c;
                r = 5;
                थांबा;
        }

        स्थिती 'U': {
                /* C++11 style 32-bit unicode */

                पूर्णांक a[8];
                माप_प्रकार i;
                char32_t c;

                जर (length != SIZE_MAX && length < 9)
                        परत -EINVAL;

                साठी (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        जर (a[i] < 0)
                                परत a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                जर (c == 0 && !accept_nul)
                        परत -EINVAL;

                /* Don't allow invalid code points */
                जर (!unichar_is_valid(c))
                        परत -EINVAL;

                *ret = c;
                r = 9;
                थांबा;
        }

        स्थिती '0':
        स्थिती '1':
        स्थिती '2':
        स्थिती '3':
        स्थिती '4':
        स्थिती '5':
        स्थिती '6':
        स्थिती '7': {
                /* octal encoding */
                पूर्णांक a, b, c;
                char32_t m;

                जर (length != SIZE_MAX && length < 3)
                        परत -EINVAL;

                a = unoctchar(p[0]);
                जर (a < 0)
                        परत -EINVAL;

                b = unoctchar(p[1]);
                जर (b < 0)
                        परत -EINVAL;

                c = unoctchar(p[2]);
                जर (c < 0)
                        परत -EINVAL;

                /* don't allow NUL bytes */
                जर (a == 0 && b == 0 && c == 0 && !accept_nul)
                        परत -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                जर (m > 255)
                        परत -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                थांबा;
        }

        पूर्वनिर्धारित:
                परत -EINVAL;
        }

        परत r;
}

ssize_t cunescape_length_with_prefix(स्थिरांक अक्षर *s, माप_प्रकार length, स्थिरांक अक्षर *prefix, UnescapeFlags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *ans = रिक्त;
        अक्षर *t;
        स्थिरांक अक्षर *f;
        माप_प्रकार pl;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        जर (length == SIZE_MAX)
                length = म_माप(s);

        pl = strlen_ptr(prefix);

        ans = new(अक्षर, pl+length+1);
        जर (!ans)
                परत -ENOMEM;

        जर (prefix)
                स_नकल(ans, prefix, pl);

        साठी (f = s, t = ans + pl; f < s + length; f++) {
                माप_प्रकार remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                प्रतिज्ञा(remaining > 0);

                जर (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        सुरूठेवा;
                }

                जर (remaining == 1) {
                        जर (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                सुरूठेवा;
                        }

                        परत -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                जर (r < 0) {
                        जर (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                सुरूठेवा;
                        }

                        परत r;
                }

                f += r;
                जर (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                अन्यथा
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        प्रतिज्ञा(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        परत t - *ret;
}

अक्षर* xescape_full(स्थिरांक अक्षर *s, स्थिरांक अक्षर *bad, माप_प्रकार console_width, XEscapeFlags flags) {
        अक्षर *ans, *t, *prev, *prev2;
        स्थिरांक अक्षर *f;

        प्रतिज्ञा(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        जर (console_width == 0)
                परत strdup("");

        माप_प्रकार len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        माप_प्रकार len_s = म_माप(s);

        माप_प्रकार len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        जर (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(अक्षर, len_body + 1);
        जर (!ans)
                परत रिक्त;

        साठी (f = s, t = prev = prev2 = ans; ; f++) {
                अक्षर *tmp_t = t;

                जर (!*f) {
                        जर (len_forced_ellipsis != 0)
                                थांबा;

                        *t = 0;
                        परत ans;
                }

                जर ((अचिन्हित अक्षर) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (अचिन्हित अक्षर) *f >= 127) ||
                    *f == '\\' || (bad && म_अक्षर(bad, *f))) {
                        जर ((माप_प्रकार) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                थांबा;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अन्यथा {
                        जर ((माप_प्रकार) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                थांबा;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        माप_प्रकार c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        माप_प्रकार off;
        जर (len_body - c >= (माप_प्रकार) (t - ans))
                off = (माप_प्रकार) (t - ans);
        अन्यथा जर (len_body - c >= (माप_प्रकार) (prev - ans))
                off = (माप_प्रकार) (prev - ans);
        अन्यथा जर (len_body - c >= (माप_प्रकार) (prev2 - ans))
                off = (माप_प्रकार) (prev2 - ans);
        अन्यथा
                off = len_body - c;
        प्रतिज्ञा(off <= (माप_प्रकार) (t - ans));

        स_नकल(ans + off, "...", c);
        ans[off + c] = '\0';
        परत ans;
}

अक्षर* escape_non_printable_full(स्थिरांक अक्षर *str, माप_प्रकार console_width, XEscapeFlags flags) {
        जर (FLAGS_SET(flags, XESCAPE_8_BIT))
                परत xescape_full(str, /* bad= */ रिक्त, console_width, flags);
        अन्यथा
                परत utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

अक्षर* octescape_full(स्थिरांक अक्षर *s, माप_प्रकार len, स्थिरांक अक्षर *bad) {
        अक्षर *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        प्रतिज्ञा(s || len == 0);

        जर (len == SIZE_MAX)
                len = म_माप(s);

        जर (len > (SIZE_MAX - 1) / 4)
                परत रिक्त;

        t = buf = new(अक्षर, len * 4 + 1);
        जर (!buf)
                परत रिक्त;

        साठी (माप_प्रकार i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                जर (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && म_अक्षर(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } अन्यथा
                        *(t++) = u;
        }

        *t = 0;
        परत buf;
}

अक्षर* decescape(स्थिरांक अक्षर *s, माप_प्रकार len, स्थिरांक अक्षर *bad) {
        अक्षर *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        प्रतिज्ञा(s || len == 0);

        जर (len == SIZE_MAX)
                len = म_माप(s);

        जर (len > (SIZE_MAX - 1) / 4)
                परत रिक्त;

        t = buf = new(अक्षर, len * 4 + 1);
        जर (!buf)
                परत रिक्त;

        साठी (माप_प्रकार i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                जर (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || म_अक्षर(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } अन्यथा
                        *(t++) = u;
        }

        *t = 0;
        परत buf;
}

स्थिर अक्षर* strcpy_backslash_escaped(अक्षर *t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *bad) {
        प्रतिज्ञा(bad);
        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        पर्यंत (*s) {
                पूर्णांक l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                जर (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                अन्यथा जर (l == 1) {
                        जर (*s == '\\' || म_अक्षर(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } अन्यथा {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        परत t;
}

अक्षर* shell_escape(स्थिरांक अक्षर *s, स्थिरांक अक्षर *bad) {
        अक्षर *buf, *t;

        buf = new(अक्षर, म_माप(s)*4+1);
        जर (!buf)
                परत रिक्त;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        परत buf;
}

अक्षर* shell_maybe_quote(स्थिरांक अक्षर *s, ShellEscapeFlags flags) {
        स्थिरांक अक्षर *p;
        अक्षर *buf, *t;

        प्रतिज्ञा(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        जर (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                परत strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        साठी (p = s; *p; ) {
                पूर्णांक l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                जर (char_is_cc(*p) || l < 0 ||
                    म_अक्षर(WHITESPACE SHELL_NEED_QUOTES, *p))
                        थांबा;

                p += l;
        }

        जर (!*p)
                परत strdup(s);

        buf = new(अक्षर, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + म_माप(s)*4 + 1 + 1);
        जर (!buf)
                परत रिक्त;

        t = buf;
        जर (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } अन्यथा
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        जर (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        अन्यथा
                *(t++) = '"';
        *t = 0;

        परत str_realloc(buf);
}

अक्षर* quote_command_ओळ(अक्षर * स्थिरांक *argv, ShellEscapeFlags flags) {
        _cleanup_free_ अक्षर *result = रिक्त;

        प्रतिज्ञा(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ अक्षर *t = रिक्त;

                t = shell_maybe_quote(*a, flags);
                जर (!t)
                        परत रिक्त;

                जर (!strextend_with_separator(&result, " ", t))
                        परत रिक्त;
        }

        परत str_realloc(TAKE_PTR(result));
}
