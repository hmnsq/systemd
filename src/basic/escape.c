/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "utf8.h"

പൂർണ്ണസംഖ്യ cescape_char(അക്ഷരം c, അക്ഷരം *buf) {
        അക്ഷരം *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        തിരഞ്ഞെടുപ്പ് (c) {

                അവസ്ഥ '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        നിർത്തുക;
                അവസ്ഥ '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        നിർത്തുക;
                അവസ്ഥ '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        നിർത്തുക;
                അവസ്ഥ '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        നിർത്തുക;
                അവസ്ഥ '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        നിർത്തുക;
                അവസ്ഥ '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        നിർത്തുക;
                അവസ്ഥ '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        നിർത്തുക;
                അവസ്ഥ '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        നിർത്തുക;
                അവസ്ഥ '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        നിർത്തുക;
                അവസ്ഥ '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        നിർത്തുക;

                സ്ഥിരസ്ഥിതി:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        എങ്കിൽ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned അക്ഷരം) c >> 6);
                                *(buf++) = octchar((unsigned അക്ഷരം) c >> 3);
                                *(buf++) = octchar((unsigned അക്ഷരം) c);
                        } അല്ലെങ്കിൽ
                                *(buf++) = c;
                        നിർത്തുക;
        }

        തിരികെ buf - buf_old;
}

അക്ഷരം* cescape_length(സ്ഥിരം അക്ഷരം *s, size_t n) {
        സ്ഥിരം അക്ഷരം *f;
        അക്ഷരം *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        പ്രതിജ്ഞാ(s || n == 0);

        എങ്കിൽ (n == SIZE_MAX)
                n = strlen(s);

        എങ്കിൽ (n > (SIZE_MAX - 1) / 4)
                തിരികെ ശൂന്യ;

        r = new(അക്ഷരം, n*4 + 1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        വേണ്ടി (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ cunescape_one(സ്ഥിരം അക്ഷരം *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        പൂർണ്ണസംഖ്യ r = 1;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        എങ്കിൽ (length != SIZE_MAX && length < 1)
                തിരികെ -EINVAL;

        തിരഞ്ഞെടുപ്പ് (p[0]) {

        അവസ്ഥ 'a':
                *ret = '\a';
                നിർത്തുക;
        അവസ്ഥ 'b':
                *ret = '\b';
                നിർത്തുക;
        അവസ്ഥ 'f':
                *ret = '\f';
                നിർത്തുക;
        അവസ്ഥ 'n':
                *ret = '\n';
                നിർത്തുക;
        അവസ്ഥ 'r':
                *ret = '\r';
                നിർത്തുക;
        അവസ്ഥ 't':
                *ret = '\t';
                നിർത്തുക;
        അവസ്ഥ 'v':
                *ret = '\v';
                നിർത്തുക;
        അവസ്ഥ '\\':
                *ret = '\\';
                നിർത്തുക;
        അവസ്ഥ '"':
                *ret = '"';
                നിർത്തുക;
        അവസ്ഥ '\'':
                *ret = '\'';
                നിർത്തുക;

        അവസ്ഥ 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                നിർത്തുക;

        അവസ്ഥ 'x': {
                /* hexadecimal encoding */
                പൂർണ്ണസംഖ്യ a, b;

                എങ്കിൽ (length != SIZE_MAX && length < 3)
                        തിരികെ -EINVAL;

                a = unhexchar(p[1]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unhexchar(p[2]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                /* Don't allow NUL bytes */
                എങ്കിൽ (a == 0 && b == 0 && !accept_nul)
                        തിരികെ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                നിർത്തുക;
        }

        അവസ്ഥ 'u': {
                /* C++11 style 16-bit unicode */

                പൂർണ്ണസംഖ്യ a[4];
                size_t i;
                uint32_t c;

                എങ്കിൽ (length != SIZE_MAX && length < 5)
                        തിരികെ -EINVAL;

                വേണ്ടി (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        എങ്കിൽ (a[i] < 0)
                                തിരികെ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                എങ്കിൽ (c == 0 && !accept_nul)
                        തിരികെ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                എങ്കിൽ (utf16_is_surrogate(c))
                        തിരികെ -EINVAL;

                *ret = c;
                r = 5;
                നിർത്തുക;
        }

        അവസ്ഥ 'U': {
                /* C++11 style 32-bit unicode */

                പൂർണ്ണസംഖ്യ a[8];
                size_t i;
                char32_t c;

                എങ്കിൽ (length != SIZE_MAX && length < 9)
                        തിരികെ -EINVAL;

                വേണ്ടി (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        എങ്കിൽ (a[i] < 0)
                                തിരികെ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                എങ്കിൽ (c == 0 && !accept_nul)
                        തിരികെ -EINVAL;

                /* Don't allow invalid code points */
                എങ്കിൽ (!unichar_is_valid(c))
                        തിരികെ -EINVAL;

                *ret = c;
                r = 9;
                നിർത്തുക;
        }

        അവസ്ഥ '0':
        അവസ്ഥ '1':
        അവസ്ഥ '2':
        അവസ്ഥ '3':
        അവസ്ഥ '4':
        അവസ്ഥ '5':
        അവസ്ഥ '6':
        അവസ്ഥ '7': {
                /* octal encoding */
                പൂർണ്ണസംഖ്യ a, b, c;
                char32_t m;

                എങ്കിൽ (length != SIZE_MAX && length < 3)
                        തിരികെ -EINVAL;

                a = unoctchar(p[0]);
                എങ്കിൽ (a < 0)
                        തിരികെ -EINVAL;

                b = unoctchar(p[1]);
                എങ്കിൽ (b < 0)
                        തിരികെ -EINVAL;

                c = unoctchar(p[2]);
                എങ്കിൽ (c < 0)
                        തിരികെ -EINVAL;

                /* don't allow NUL bytes */
                എങ്കിൽ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        തിരികെ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                എങ്കിൽ (m > 255)
                        തിരികെ -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                നിർത്തുക;
        }

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EINVAL;
        }

        തിരികെ r;
}

ssize_t cunescape_length_with_prefix(സ്ഥിരം അക്ഷരം *s, size_t length, സ്ഥിരം അക്ഷരം *prefix, UnescapeFlags flags, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *ans = ശൂന്യ;
        അക്ഷരം *t;
        സ്ഥിരം അക്ഷരം *f;
        size_t pl;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        എങ്കിൽ (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(അക്ഷരം, pl+length+1);
        എങ്കിൽ (!ans)
                തിരികെ -ENOMEM;

        എങ്കിൽ (prefix)
                memcpy(ans, prefix, pl);

        വേണ്ടി (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                പ്രതിജ്ഞാ(remaining > 0);

                എങ്കിൽ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        തുടരുക;
                }

                എങ്കിൽ (remaining == 1) {
                        എങ്കിൽ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                തുടരുക;
                        }

                        തിരികെ -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                തുടരുക;
                        }

                        തിരികെ r;
                }

                f += r;
                എങ്കിൽ (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                അല്ലെങ്കിൽ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        പ്രതിജ്ഞാ(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        തിരികെ t - *ret;
}

അക്ഷരം* xescape_full(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad, size_t console_width, XEscapeFlags flags) {
        അക്ഷരം *ans, *t, *prev, *prev2;
        സ്ഥിരം അക്ഷരം *f;

        പ്രതിജ്ഞാ(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        എങ്കിൽ (console_width == 0)
                തിരികെ strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        എങ്കിൽ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(അക്ഷരം, len_body + 1);
        എങ്കിൽ (!ans)
                തിരികെ ശൂന്യ;

        വേണ്ടി (f = s, t = prev = prev2 = ans; ; f++) {
                അക്ഷരം *tmp_t = t;

                എങ്കിൽ (!*f) {
                        എങ്കിൽ (len_forced_ellipsis != 0)
                                നിർത്തുക;

                        *t = 0;
                        തിരികെ ans;
                }

                എങ്കിൽ ((unsigned അക്ഷരം) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned അക്ഷരം) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        എങ്കിൽ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                നിർത്തുക;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } അല്ലെങ്കിൽ {
                        എങ്കിൽ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                നിർത്തുക;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        എങ്കിൽ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        അല്ലെങ്കിൽ എങ്കിൽ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        അല്ലെങ്കിൽ എങ്കിൽ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        അല്ലെങ്കിൽ
                off = len_body - c;
        പ്രതിജ്ഞാ(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        തിരികെ ans;
}

അക്ഷരം* escape_non_printable_full(സ്ഥിരം അക്ഷരം *str, size_t console_width, XEscapeFlags flags) {
        എങ്കിൽ (FLAGS_SET(flags, XESCAPE_8_BIT))
                തിരികെ xescape_full(str, /* bad= */ ശൂന്യ, console_width, flags);
        അല്ലെങ്കിൽ
                തിരികെ utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

അക്ഷരം* octescape_full(സ്ഥിരം അക്ഷരം *s, size_t len, സ്ഥിരം അക്ഷരം *bad) {
        അക്ഷരം *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        പ്രതിജ്ഞാ(s || len == 0);

        എങ്കിൽ (len == SIZE_MAX)
                len = strlen(s);

        എങ്കിൽ (len > (SIZE_MAX - 1) / 4)
                തിരികെ ശൂന്യ;

        t = buf = new(അക്ഷരം, len * 4 + 1);
        എങ്കിൽ (!buf)
                തിരികെ ശൂന്യ;

        വേണ്ടി (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                എങ്കിൽ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } അല്ലെങ്കിൽ
                        *(t++) = u;
        }

        *t = 0;
        തിരികെ buf;
}

അക്ഷരം* decescape(സ്ഥിരം അക്ഷരം *s, size_t len, സ്ഥിരം അക്ഷരം *bad) {
        അക്ഷരം *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        പ്രതിജ്ഞാ(s || len == 0);

        എങ്കിൽ (len == SIZE_MAX)
                len = strlen(s);

        എങ്കിൽ (len > (SIZE_MAX - 1) / 4)
                തിരികെ ശൂന്യ;

        t = buf = new(അക്ഷരം, len * 4 + 1);
        എങ്കിൽ (!buf)
                തിരികെ ശൂന്യ;

        വേണ്ടി (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                എങ്കിൽ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } അല്ലെങ്കിൽ
                        *(t++) = u;
        }

        *t = 0;
        തിരികെ buf;
}

സ്ഥിതം അക്ഷരം* strcpy_backslash_escaped(അക്ഷരം *t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad) {
        പ്രതിജ്ഞാ(bad);
        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(s);

        ആകുമ്പോൾ (*s) {
                പൂർണ്ണസംഖ്യ l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                എങ്കിൽ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                അല്ലെങ്കിൽ എങ്കിൽ (l == 1) {
                        എങ്കിൽ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } അല്ലെങ്കിൽ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        തിരികെ t;
}

അക്ഷരം* shell_escape(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad) {
        അക്ഷരം *buf, *t;

        buf = new(അക്ഷരം, strlen(s)*4+1);
        എങ്കിൽ (!buf)
                തിരികെ ശൂന്യ;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        തിരികെ buf;
}

അക്ഷരം* shell_maybe_quote(സ്ഥിരം അക്ഷരം *s, ShellEscapeFlags flags) {
        സ്ഥിരം അക്ഷരം *p;
        അക്ഷരം *buf, *t;

        പ്രതിജ്ഞാ(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        എങ്കിൽ (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                തിരികെ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        വേണ്ടി (p = s; *p; ) {
                പൂർണ്ണസംഖ്യ l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                എങ്കിൽ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        നിർത്തുക;

                p += l;
        }

        എങ്കിൽ (!*p)
                തിരികെ strdup(s);

        buf = new(അക്ഷരം, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        എങ്കിൽ (!buf)
                തിരികെ ശൂന്യ;

        t = buf;
        എങ്കിൽ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } അല്ലെങ്കിൽ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        എങ്കിൽ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        അല്ലെങ്കിൽ
                *(t++) = '"';
        *t = 0;

        തിരികെ str_realloc(buf);
}

അക്ഷരം* quote_command_വരി(അക്ഷരം * സ്ഥിരം *argv, ShellEscapeFlags flags) {
        _cleanup_free_ അക്ഷരം *result = ശൂന്യ;

        പ്രതിജ്ഞാ(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ അക്ഷരം *t = ശൂന്യ;

                t = shell_maybe_quote(*a, flags);
                എങ്കിൽ (!t)
                        തിരികെ ശൂന്യ;

                എങ്കിൽ (!strextend_with_separator(&result, " ", t))
                        തിരികെ ശൂന്യ;
        }

        തിരികെ str_realloc(TAKE_PTR(result));
}
