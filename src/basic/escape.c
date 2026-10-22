/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "utf8.h"

పూర్ణాంకం cescape_char(అక్షరం c, అక్షరం *buf) {
        అక్షరం *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        ఎంపిక (c) {

                స్థితి '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        ఆపు;
                స్థితి '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        ఆపు;
                స్థితి '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        ఆపు;
                స్థితి '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        ఆపు;
                స్థితి '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        ఆపు;
                స్థితి '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        ఆపు;
                స్థితి '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        ఆపు;
                స్థితి '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        ఆపు;
                స్థితి '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        ఆపు;
                స్థితి '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        ఆపు;

                అప్రమేయ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ఒకవేళ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((గుర్తులేని అక్షరం) c >> 6);
                                *(buf++) = octchar((గుర్తులేని అక్షరం) c >> 3);
                                *(buf++) = octchar((గుర్తులేని అక్షరం) c);
                        } లేకపోతే
                                *(buf++) = c;
                        ఆపు;
        }

        తిరిగి buf - buf_old;
}

అక్షరం* cescape_నిడివి(స్థిరరాశి అక్షరం *s, size_t n) {
        స్థిరరాశి అక్షరం *f;
        అక్షరం *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        ప్రతిజ్ఞా(s || n == 0);

        ఒకవేళ (n == SIZE_MAX)
                n = అక్షరశ్రేణి_పొడవు(s);

        ఒకవేళ (n > (SIZE_MAX - 1) / 4)
                తిరిగి ఖాళీ;

        r = new(అక్షరం, n*4 + 1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        కొరకు (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        తిరిగి r;
}

పూర్ణాంకం cunescape_one(స్థిరరాశి అక్షరం *p, size_t length, char32_t *ret, bool *eight_బిట్టు, bool accept_nul) {
        పూర్ణాంకం r = 1;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(eight_బిట్టు);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ఒకవేళ (length != SIZE_MAX && length < 1)
                తిరిగి -EINVAL;

        ఎంపిక (p[0]) {

        స్థితి 'a':
                *ret = '\a';
                ఆపు;
        స్థితి 'b':
                *ret = '\b';
                ఆపు;
        స్థితి 'f':
                *ret = '\f';
                ఆపు;
        స్థితి 'n':
                *ret = '\n';
                ఆపు;
        స్థితి 'r':
                *ret = '\r';
                ఆపు;
        స్థితి 't':
                *ret = '\t';
                ఆపు;
        స్థితి 'v':
                *ret = '\v';
                ఆపు;
        స్థితి '\\':
                *ret = '\\';
                ఆపు;
        స్థితి '"':
                *ret = '"';
                ఆపు;
        స్థితి '\'':
                *ret = '\'';
                ఆపు;

        స్థితి 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                ఆపు;

        స్థితి 'x': {
                /* hexadecimal encoding */
                పూర్ణాంకం a, b;

                ఒకవేళ (length != SIZE_MAX && length < 3)
                        తిరిగి -EINVAL;

                a = unhexchar(p[1]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unhexchar(p[2]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                /* Don't allow NUL bytes */
                ఒకవేళ (a == 0 && b == 0 && !accept_nul)
                        తిరిగి -EINVAL;

                *ret = (a << 4U) | b;
                *eight_బిట్టు = true;
                r = 3;
                ఆపు;
        }

        స్థితి 'u': {
                /* C++11 style 16-bit unicode */

                పూర్ణాంకం a[4];
                size_t i;
                uint32_t c;

                ఒకవేళ (length != SIZE_MAX && length < 5)
                        తిరిగి -EINVAL;

                కొరకు (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ఒకవేళ (a[i] < 0)
                                తిరిగి a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ఒకవేళ (c == 0 && !accept_nul)
                        తిరిగి -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ఒకవేళ (utf16_is_surrogate(c))
                        తిరిగి -EINVAL;

                *ret = c;
                r = 5;
                ఆపు;
        }

        స్థితి 'U': {
                /* C++11 style 32-bit unicode */

                పూర్ణాంకం a[8];
                size_t i;
                char32_t c;

                ఒకవేళ (length != SIZE_MAX && length < 9)
                        తిరిగి -EINVAL;

                కొరకు (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ఒకవేళ (a[i] < 0)
                                తిరిగి a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ఒకవేళ (c == 0 && !accept_nul)
                        తిరిగి -EINVAL;

                /* Don't allow invalid code points */
                ఒకవేళ (!unichar_is_valid(c))
                        తిరిగి -EINVAL;

                *ret = c;
                r = 9;
                ఆపు;
        }

        స్థితి '0':
        స్థితి '1':
        స్థితి '2':
        స్థితి '3':
        స్థితి '4':
        స్థితి '5':
        స్థితి '6':
        స్థితి '7': {
                /* octal encoding */
                పూర్ణాంకం a, b, c;
                char32_t m;

                ఒకవేళ (length != SIZE_MAX && length < 3)
                        తిరిగి -EINVAL;

                a = unoctchar(p[0]);
                ఒకవేళ (a < 0)
                        తిరిగి -EINVAL;

                b = unoctchar(p[1]);
                ఒకవేళ (b < 0)
                        తిరిగి -EINVAL;

                c = unoctchar(p[2]);
                ఒకవేళ (c < 0)
                        తిరిగి -EINVAL;

                /* don't allow NUL bytes */
                ఒకవేళ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        తిరిగి -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ఒకవేళ (m > 255)
                        తిరిగి -EINVAL;

                *ret = m;
                *eight_బిట్టు = true;
                r = 3;
                ఆపు;
        }

        అప్రమేయ:
                తిరిగి -EINVAL;
        }

        తిరిగి r;
}

ssize_t cunescape_నిడివి_with_ఉపసర్గ(స్థిరరాశి అక్షరం *s, size_t length, స్థిరరాశి అక్షరం *ఉపసర్గ, UnescapeFlags flags, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *ans = ఖాళీ;
        అక్షరం *t;
        స్థిరరాశి అక్షరం *f;
        size_t pl;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ఒకవేళ (length == SIZE_MAX)
                length = అక్షరశ్రేణి_పొడవు(s);

        pl = strlen_ptr(ఉపసర్గ);

        ans = new(అక్షరం, pl+length+1);
        ఒకవేళ (!ans)
                తిరిగి -ENOMEM;

        ఒకవేళ (ఉపసర్గ)
                జ్ఞాపకం_నకలు(ans, ఉపసర్గ, pl);

        కొరకు (f = s, t = ans + pl; f < s + length; f++) {
                size_t మిగిలిన;
                bool eight_బిట్టు = false;
                char32_t u;

                మిగిలిన = s + length - f;
                ప్రతిజ్ఞా(మిగిలిన > 0);

                ఒకవేళ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        కొనసాగు;
                }

                ఒకవేళ (మిగిలిన == 1) {
                        ఒకవేళ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                కొనసాగు;
                        }

                        తిరిగి -EINVAL;
                }

                r = cunescape_one(f + 1, మిగిలిన - 1, &u, &eight_బిట్టు, flags & UNESCAPE_ACCEPT_NUL);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                కొనసాగు;
                        }

                        తిరిగి r;
                }

                f += r;
                ఒకవేళ (eight_బిట్టు)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                లేకపోతే
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        ప్రతిజ్ఞా(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        తిరిగి t - *ret;
}

అక్షరం* xescape_నిండిన(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad, size_t console_వెడల్పు, XEscapeFlags flags) {
        అక్షరం *ans, *t, *prev, *prev2;
        స్థిరరాశి అక్షరం *f;

        ప్రతిజ్ఞా(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ఒకవేళ (console_వెడల్పు == 0)
                తిరిగి strdup("");

        size_t len_forced_ellipsis = FLAGS_సమితి(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = అక్షరశ్రేణి_పొడవు(s);

        size_t len_body = MIN(console_వెడల్పు, SIZE_MAX - 1); /* We need room for the NUL byte */
        ఒకవేళ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(అక్షరం, len_body + 1);
        ఒకవేళ (!ans)
                తిరిగి ఖాళీ;

        కొరకు (f = s, t = prev = prev2 = ans; ; f++) {
                అక్షరం *tmp_t = t;

                ఒకవేళ (!*f) {
                        ఒకవేళ (len_forced_ellipsis != 0)
                                ఆపు;

                        *t = 0;
                        తిరిగి ans;
                }

                ఒకవేళ ((గుర్తులేని అక్షరం) *f < ' ' ||
                    (!FLAGS_సమితి(flags, XESCAPE_8_బిట్టు) && (గుర్తులేని అక్షరం) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ఒకవేళ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                ఆపు;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } లేకపోతే {
                        ఒకవేళ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                ఆపు;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ఒకవేళ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        లేకపోతే ఒకవేళ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        లేకపోతే ఒకవేళ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        లేకపోతే
                off = len_body - c;
        ప్రతిజ్ఞా(off <= (size_t) (t - ans));

        జ్ఞాపకం_నకలు(ans + off, "...", c);
        ans[off + c] = '\0';
        తిరిగి ans;
}

అక్షరం* escape_non_printable_నిండిన(స్థిరరాశి అక్షరం *str, size_t console_వెడల్పు, XEscapeFlags flags) {
        ఒకవేళ (FLAGS_సమితి(flags, XESCAPE_8_బిట్టు))
                తిరిగి xescape_నిండిన(str, /* bad= */ ఖాళీ, console_వెడల్పు, flags);
        లేకపోతే
                తిరిగి utf8_escape_non_printable_నిండిన(str,
                                                      console_వెడల్పు,
                                                      FLAGS_సమితి(flags, XESCAPE_FORCE_ELLIPSIS));
}

అక్షరం* octescape_నిండిన(స్థిరరాశి అక్షరం *s, size_t len, స్థిరరాశి అక్షరం *bad) {
        అక్షరం *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        ప్రతిజ్ఞా(s || len == 0);

        ఒకవేళ (len == SIZE_MAX)
                len = అక్షరశ్రేణి_పొడవు(s);

        ఒకవేళ (len > (SIZE_MAX - 1) / 4)
                తిరిగి ఖాళీ;

        t = buf = new(అక్షరం, len * 4 + 1);
        ఒకవేళ (!buf)
                తిరిగి ఖాళీ;

        కొరకు (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ఒకవేళ (u < ' ' || u >= 127 || లోపల_సమితి(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } లేకపోతే
                        *(t++) = u;
        }

        *t = 0;
        తిరిగి buf;
}

అక్షరం* decescape(స్థిరరాశి అక్షరం *s, size_t len, స్థిరరాశి అక్షరం *bad) {
        అక్షరం *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        ప్రతిజ్ఞా(s || len == 0);

        ఒకవేళ (len == SIZE_MAX)
                len = అక్షరశ్రేణి_పొడవు(s);

        ఒకవేళ (len > (SIZE_MAX - 1) / 4)
                తిరిగి ఖాళీ;

        t = buf = new(అక్షరం, len * 4 + 1);
        ఒకవేళ (!buf)
                తిరిగి ఖాళీ;

        కొరకు (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ఒకవేళ (u < ' ' || u >= 127 || లోపల_సమితి(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } లేకపోతే
                        *(t++) = u;
        }

        *t = 0;
        తిరిగి buf;
}

స్థిర అక్షరం* strcpy_backslash_escaped(అక్షరం *t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad) {
        ప్రతిజ్ఞా(bad);
        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(s);

        అయితే (*s) {
                పూర్ణాంకం l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ఒకవేళ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                లేకపోతే ఒకవేళ (l == 1) {
                        ఒకవేళ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } లేకపోతే {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        తిరిగి t;
}

అక్షరం* shell_escape(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad) {
        అక్షరం *buf, *t;

        buf = new(అక్షరం, అక్షరశ్రేణి_పొడవు(s)*4+1);
        ఒకవేళ (!buf)
                తిరిగి ఖాళీ;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        తిరిగి buf;
}

అక్షరం* shell_maybe_quote(స్థిరరాశి అక్షరం *s, ShellEscapeFlags flags) {
        స్థిరరాశి అక్షరం *p;
        అక్షరం *buf, *t;

        ప్రతిజ్ఞా(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ఒకవేళ (FLAGS_సమితి(flags, SHELL_ESCAPE_ఖాళీలు) && isempty(s))
                తిరిగి strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        కొరకు (p = s; *p; ) {
                పూర్ణాంకం l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ఒకవేళ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        ఆపు;

                p += l;
        }

        ఒకవేళ (!*p)
                తిరిగి strdup(s);

        buf = new(అక్షరం, FLAGS_సమితి(flags, SHELL_ESCAPE_POSIX) + 1 + అక్షరశ్రేణి_పొడవు(s)*4 + 1 + 1);
        ఒకవేళ (!buf)
                తిరిగి ఖాళీ;

        t = buf;
        ఒకవేళ (FLAGS_సమితి(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } లేకపోతే
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_సమితి(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ఒకవేళ (FLAGS_సమితి(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        లేకపోతే
                *(t++) = '"';
        *t = 0;

        తిరిగి str_realloc(buf);
}

అక్షరం* quote_command_పంక్తి(అక్షరం * స్థిరరాశి *argv, ShellEscapeFlags flags) {
        _cleanup_free_ అక్షరం *ఫలితం = ఖాళీ;

        ప్రతిజ్ఞా(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ అక్షరం *t = ఖాళీ;

                t = shell_maybe_quote(*a, flags);
                ఒకవేళ (!t)
                        తిరిగి ఖాళీ;

                ఒకవేళ (!strextend_with_separator(&ఫలితం, " ", t))
                        తిరిగి ఖాళీ;
        }

        తిరిగి str_realloc(TAKE_PTR(ఫలితం));
}
