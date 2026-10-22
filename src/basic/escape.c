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
                        भङ्ग;
                पक्ष '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        भङ्ग;
                पक्ष '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        भङ्ग;
                पक्ष '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        भङ्ग;
                पक्ष '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        भङ्ग;
                पक्ष '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        भङ्ग;
                पक्ष '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        भङ्ग;
                पक्ष '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        भङ्ग;
                पक्ष '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        भङ्ग;
                पक्ष '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        भङ्ग;

                स्वभाव:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        जइ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned वर्ण) c >> 6);
                                *(buf++) = octchar((unsigned वर्ण) c >> 3);
                                *(buf++) = octchar((unsigned वर्ण) c);
                        } अण्णहा
                                *(buf++) = c;
                        भङ्ग;
        }

        प्रत्यागम buf - buf_old;
}

वर्ण* cescape_length(ध्रुव वर्ण *s, size_t n) {
        ध्रुव वर्ण *f;
        वर्ण *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        प्रतिज्ञा(s || n == 0);

        जइ (n == SIZE_MAX)
                n = strlen(s);

        जइ (n > (SIZE_MAX - 1) / 4)
                प्रत्यागम रिक्त;

        r = new(वर्ण, n*4 + 1);
        जइ (!r)
                प्रत्यागम रिक्त;

        प्रति (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        प्रत्यागम r;
}

पूर्णाङ्क cunescape_one(ध्रुव वर्ण *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        पूर्णाङ्क r = 1;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);
        प्रतिज्ञा(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        जइ (length != SIZE_MAX && length < 1)
                प्रत्यागम -EINVAL;

        विकल्प (p[0]) {

        पक्ष 'a':
                *ret = '\a';
                भङ्ग;
        पक्ष 'b':
                *ret = '\b';
                भङ्ग;
        पक्ष 'f':
                *ret = '\f';
                भङ्ग;
        पक्ष 'n':
                *ret = '\n';
                भङ्ग;
        पक्ष 'r':
                *ret = '\r';
                भङ्ग;
        पक्ष 't':
                *ret = '\t';
                भङ्ग;
        पक्ष 'v':
                *ret = '\v';
                भङ्ग;
        पक्ष '\\':
                *ret = '\\';
                भङ्ग;
        पक्ष '"':
                *ret = '"';
                भङ्ग;
        पक्ष '\'':
                *ret = '\'';
                भङ्ग;

        पक्ष 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                भङ्ग;

        पक्ष 'x': {
                /* hexadecimal encoding */
                पूर्णाङ्क a, b;

                जइ (length != SIZE_MAX && length < 3)
                        प्रत्यागम -EINVAL;

                a = unhexchar(p[1]);
                जइ (a < 0)
                        प्रत्यागम -EINVAL;

                b = unhexchar(p[2]);
                जइ (b < 0)
                        प्रत्यागम -EINVAL;

                /* Don't allow NUL bytes */
                जइ (a == 0 && b == 0 && !accept_nul)
                        प्रत्यागम -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                भङ्ग;
        }

        पक्ष 'u': {
                /* C++11 style 16-bit unicode */

                पूर्णाङ्क a[4];
                size_t i;
                uint32_t c;

                जइ (length != SIZE_MAX && length < 5)
                        प्रत्यागम -EINVAL;

                प्रति (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        जइ (a[i] < 0)
                                प्रत्यागम a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                जइ (c == 0 && !accept_nul)
                        प्रत्यागम -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                जइ (utf16_is_surrogate(c))
                        प्रत्यागम -EINVAL;

                *ret = c;
                r = 5;
                भङ्ग;
        }

        पक्ष 'U': {
                /* C++11 style 32-bit unicode */

                पूर्णाङ्क a[8];
                size_t i;
                char32_t c;

                जइ (length != SIZE_MAX && length < 9)
                        प्रत्यागम -EINVAL;

                प्रति (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        जइ (a[i] < 0)
                                प्रत्यागम a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                जइ (c == 0 && !accept_nul)
                        प्रत्यागम -EINVAL;

                /* Don't allow invalid code points */
                जइ (!unichar_is_valid(c))
                        प्रत्यागम -EINVAL;

                *ret = c;
                r = 9;
                भङ्ग;
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

                जइ (length != SIZE_MAX && length < 3)
                        प्रत्यागम -EINVAL;

                a = unoctchar(p[0]);
                जइ (a < 0)
                        प्रत्यागम -EINVAL;

                b = unoctchar(p[1]);
                जइ (b < 0)
                        प्रत्यागम -EINVAL;

                c = unoctchar(p[2]);
                जइ (c < 0)
                        प्रत्यागम -EINVAL;

                /* don't allow NUL bytes */
                जइ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        प्रत्यागम -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                जइ (m > 255)
                        प्रत्यागम -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                भङ्ग;
        }

        स्वभाव:
                प्रत्यागम -EINVAL;
        }

        प्रत्यागम r;
}

ssize_t cunescape_length_with_prefix(ध्रुव वर्ण *s, size_t length, ध्रुव वर्ण *prefix, UnescapeFlags flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *ans = रिक्त;
        वर्ण *t;
        ध्रुव वर्ण *f;
        size_t pl;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        जइ (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(वर्ण, pl+length+1);
        जइ (!ans)
                प्रत्यागम -ENOMEM;

        जइ (prefix)
                memcpy(ans, prefix, pl);

        प्रति (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                प्रतिज्ञा(remaining > 0);

                जइ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        अनुवर्त;
                }

                जइ (remaining == 1) {
                        जइ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                अनुवर्त;
                        }

                        प्रत्यागम -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                जइ (r < 0) {
                        जइ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                अनुवर्त;
                        }

                        प्रत्यागम r;
                }

                f += r;
                जइ (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                अण्णहा
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        प्रतिज्ञा(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        प्रत्यागम t - *ret;
}

वर्ण* xescape_full(ध्रुव वर्ण *s, ध्रुव वर्ण *bad, size_t console_width, XEscapeFlags flags) {
        वर्ण *ans, *t, *prev, *prev2;
        ध्रुव वर्ण *f;

        प्रतिज्ञा(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        जइ (console_width == 0)
                प्रत्यागम strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        जइ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(वर्ण, len_body + 1);
        जइ (!ans)
                प्रत्यागम रिक्त;

        प्रति (f = s, t = prev = prev2 = ans; ; f++) {
                वर्ण *tmp_t = t;

                जइ (!*f) {
                        जइ (len_forced_ellipsis != 0)
                                भङ्ग;

                        *t = 0;
                        प्रत्यागम ans;
                }

                जइ ((unsigned वर्ण) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned वर्ण) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        जइ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                भङ्ग;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } अण्णहा {
                        जइ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                भङ्ग;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        जइ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        अण्णहा जइ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        अण्णहा जइ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        अण्णहा
                off = len_body - c;
        प्रतिज्ञा(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        प्रत्यागम ans;
}

वर्ण* escape_non_printable_full(ध्रुव वर्ण *str, size_t console_width, XEscapeFlags flags) {
        जइ (FLAGS_SET(flags, XESCAPE_8_BIT))
                प्रत्यागम xescape_full(str, /* bad= */ रिक्त, console_width, flags);
        अण्णहा
                प्रत्यागम utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

वर्ण* octescape_full(ध्रुव वर्ण *s, size_t len, ध्रुव वर्ण *bad) {
        वर्ण *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        प्रतिज्ञा(s || len == 0);

        जइ (len == SIZE_MAX)
                len = strlen(s);

        जइ (len > (SIZE_MAX - 1) / 4)
                प्रत्यागम रिक्त;

        t = buf = new(वर्ण, len * 4 + 1);
        जइ (!buf)
                प्रत्यागम रिक्त;

        प्रति (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                जइ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } अण्णहा
                        *(t++) = u;
        }

        *t = 0;
        प्रत्यागम buf;
}

वर्ण* decescape(ध्रुव वर्ण *s, size_t len, ध्रुव वर्ण *bad) {
        वर्ण *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        प्रतिज्ञा(s || len == 0);

        जइ (len == SIZE_MAX)
                len = strlen(s);

        जइ (len > (SIZE_MAX - 1) / 4)
                प्रत्यागम रिक्त;

        t = buf = new(वर्ण, len * 4 + 1);
        जइ (!buf)
                प्रत्यागम रिक्त;

        प्रति (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                जइ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } अण्णहा
                        *(t++) = u;
        }

        *t = 0;
        प्रत्यागम buf;
}

स्थिर वर्ण* strcpy_backslash_escaped(वर्ण *t, ध्रुव वर्ण *s, ध्रुव वर्ण *bad) {
        प्रतिज्ञा(bad);
        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        जाव (*s) {
                पूर्णाङ्क l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                जइ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                अण्णहा जइ (l == 1) {
                        जइ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } अण्णहा {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        प्रत्यागम t;
}

वर्ण* shell_escape(ध्रुव वर्ण *s, ध्रुव वर्ण *bad) {
        वर्ण *buf, *t;

        buf = new(वर्ण, strlen(s)*4+1);
        जइ (!buf)
                प्रत्यागम रिक्त;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        प्रत्यागम buf;
}

वर्ण* shell_maybe_quote(ध्रुव वर्ण *s, ShellEscapeFlags flags) {
        ध्रुव वर्ण *p;
        वर्ण *buf, *t;

        प्रतिज्ञा(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        जइ (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                प्रत्यागम strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        प्रति (p = s; *p; ) {
                पूर्णाङ्क l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                जइ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        भङ्ग;

                p += l;
        }

        जइ (!*p)
                प्रत्यागम strdup(s);

        buf = new(वर्ण, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        जइ (!buf)
                प्रत्यागम रिक्त;

        t = buf;
        जइ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } अण्णहा
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        जइ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        अण्णहा
                *(t++) = '"';
        *t = 0;

        प्रत्यागम str_realloc(buf);
}

वर्ण* quote_command_पङ्क्तिः(वर्ण * ध्रुव *argv, ShellEscapeFlags flags) {
        _cleanup_free_ वर्ण *result = रिक्त;

        प्रतिज्ञा(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ वर्ण *t = रिक्त;

                t = shell_maybe_quote(*a, flags);
                जइ (!t)
                        प्रत्यागम रिक्त;

                जइ (!strextend_with_separator(&result, " ", t))
                        प्रत्यागम रिक्त;
        }

        प्रत्यागम str_realloc(TAKE_PTR(result));
}
