/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਪੂਰਨ cescape_char(ਅੱਖਰ c, ਅੱਖਰ *buf) {
        ਅੱਖਰ *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        ਚੋਣ (c) {

                ਹਾਲਤ '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        ਰੋਕੋ;
                ਹਾਲਤ '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        ਰੋਕੋ;
                ਹਾਲਤ '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        ਰੋਕੋ;
                ਹਾਲਤ '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        ਰੋਕੋ;
                ਹਾਲਤ '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        ਰੋਕੋ;
                ਹਾਲਤ '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        ਰੋਕੋ;
                ਹਾਲਤ '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        ਰੋਕੋ;
                ਹਾਲਤ '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        ਰੋਕੋ;
                ਹਾਲਤ '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        ਰੋਕੋ;
                ਹਾਲਤ '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        ਰੋਕੋ;

                ਮੂਲ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ਜੇ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c >> 6);
                                *(buf++) = octchar((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c >> 3);
                                *(buf++) = octchar((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c);
                        } ਨਹੀਂਤਾਂ
                                *(buf++) = c;
                        ਰੋਕੋ;
        }

        ਵਾਪਸ buf - buf_old;
}

ਅੱਖਰ* cescape_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *s, size_t n) {
        ਅਚਲ ਅੱਖਰ *f;
        ਅੱਖਰ *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        ਪ੍ਰਤਿਜ੍ਞਾ(s || n == 0);

        ਜੇ (n == SIZE_MAX)
                n = ਲੜੀ_ਲੰਬਾਈ(s);

        ਜੇ (n > (SIZE_MAX - 1) / 4)
                ਵਾਪਸ ਨਿਰਾ;

        r = new(ਅੱਖਰ, n*4 + 1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ਵਾਪਸ r;
}

ਪੂਰਨ cunescape_one(ਅਚਲ ਅੱਖਰ *p, size_t length, char32_t *ret, bool *eight_ਦੋਆਧਾਰੀ_ਅੰਕ, bool accept_nul) {
        ਪੂਰਨ r = 1;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(eight_ਦੋਆਧਾਰੀ_ਅੰਕ);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ਜੇ (length != SIZE_MAX && length < 1)
                ਵਾਪਸ -EINVAL;

        ਚੋਣ (p[0]) {

        ਹਾਲਤ 'a':
                *ret = '\a';
                ਰੋਕੋ;
        ਹਾਲਤ 'b':
                *ret = '\b';
                ਰੋਕੋ;
        ਹਾਲਤ 'f':
                *ret = '\f';
                ਰੋਕੋ;
        ਹਾਲਤ 'n':
                *ret = '\n';
                ਰੋਕੋ;
        ਹਾਲਤ 'r':
                *ret = '\r';
                ਰੋਕੋ;
        ਹਾਲਤ 't':
                *ret = '\t';
                ਰੋਕੋ;
        ਹਾਲਤ 'v':
                *ret = '\v';
                ਰੋਕੋ;
        ਹਾਲਤ '\\':
                *ret = '\\';
                ਰੋਕੋ;
        ਹਾਲਤ '"':
                *ret = '"';
                ਰੋਕੋ;
        ਹਾਲਤ '\'':
                *ret = '\'';
                ਰੋਕੋ;

        ਹਾਲਤ 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                ਰੋਕੋ;

        ਹਾਲਤ 'x': {
                /* hexadecimal encoding */
                ਪੂਰਨ a, b;

                ਜੇ (length != SIZE_MAX && length < 3)
                        ਵਾਪਸ -EINVAL;

                a = unhexchar(p[1]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unhexchar(p[2]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                /* Don't allow NUL bytes */
                ਜੇ (a == 0 && b == 0 && !accept_nul)
                        ਵਾਪਸ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_ਦੋਆਧਾਰੀ_ਅੰਕ = true;
                r = 3;
                ਰੋਕੋ;
        }

        ਹਾਲਤ 'u': {
                /* C++11 style 16-bit unicode */

                ਪੂਰਨ a[4];
                size_t i;
                uint32_t c;

                ਜੇ (length != SIZE_MAX && length < 5)
                        ਵਾਪਸ -EINVAL;

                ਲਈ (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ਜੇ (a[i] < 0)
                                ਵਾਪਸ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ਜੇ (c == 0 && !accept_nul)
                        ਵਾਪਸ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ਜੇ (utf16_is_surrogate(c))
                        ਵਾਪਸ -EINVAL;

                *ret = c;
                r = 5;
                ਰੋਕੋ;
        }

        ਹਾਲਤ 'U': {
                /* C++11 style 32-bit unicode */

                ਪੂਰਨ a[8];
                size_t i;
                char32_t c;

                ਜੇ (length != SIZE_MAX && length < 9)
                        ਵਾਪਸ -EINVAL;

                ਲਈ (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ਜੇ (a[i] < 0)
                                ਵਾਪਸ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ਜੇ (c == 0 && !accept_nul)
                        ਵਾਪਸ -EINVAL;

                /* Don't allow invalid code points */
                ਜੇ (!unichar_is_valid(c))
                        ਵਾਪਸ -EINVAL;

                *ret = c;
                r = 9;
                ਰੋਕੋ;
        }

        ਹਾਲਤ '0':
        ਹਾਲਤ '1':
        ਹਾਲਤ '2':
        ਹਾਲਤ '3':
        ਹਾਲਤ '4':
        ਹਾਲਤ '5':
        ਹਾਲਤ '6':
        ਹਾਲਤ '7': {
                /* octal encoding */
                ਪੂਰਨ a, b, c;
                char32_t m;

                ਜੇ (length != SIZE_MAX && length < 3)
                        ਵਾਪਸ -EINVAL;

                a = unoctchar(p[0]);
                ਜੇ (a < 0)
                        ਵਾਪਸ -EINVAL;

                b = unoctchar(p[1]);
                ਜੇ (b < 0)
                        ਵਾਪਸ -EINVAL;

                c = unoctchar(p[2]);
                ਜੇ (c < 0)
                        ਵਾਪਸ -EINVAL;

                /* don't allow NUL bytes */
                ਜੇ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ਵਾਪਸ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ਜੇ (m > 255)
                        ਵਾਪਸ -EINVAL;

                *ret = m;
                *eight_ਦੋਆਧਾਰੀ_ਅੰਕ = true;
                r = 3;
                ਰੋਕੋ;
        }

        ਮੂਲ:
                ਵਾਪਸ -EINVAL;
        }

        ਵਾਪਸ r;
}

ssize_t cunescape_ਲੰਮਾਈ_with_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *s, size_t length, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, UnescapeFlags flags, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ans = ਨਿਰਾ;
        ਅੱਖਰ *t;
        ਅਚਲ ਅੱਖਰ *f;
        size_t pl;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ਜੇ (length == SIZE_MAX)
                length = ਲੜੀ_ਲੰਬਾਈ(s);

        pl = strlen_ptr(ਅਗੇਤਰ);

        ans = new(ਅੱਖਰ, pl+length+1);
        ਜੇ (!ans)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (ਅਗੇਤਰ)
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(ans, ਅਗੇਤਰ, pl);

        ਲਈ (f = s, t = ans + pl; f < s + length; f++) {
                size_t ਬਚਿਆ;
                bool eight_ਦੋਆਧਾਰੀ_ਅੰਕ = false;
                char32_t u;

                ਬਚਿਆ = s + length - f;
                ਪ੍ਰਤਿਜ੍ਞਾ(ਬਚਿਆ > 0);

                ਜੇ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        ਜਾਰੀ;
                }

                ਜੇ (ਬਚਿਆ == 1) {
                        ਜੇ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                ਜਾਰੀ;
                        }

                        ਵਾਪਸ -EINVAL;
                }

                r = cunescape_one(f + 1, ਬਚਿਆ - 1, &u, &eight_ਦੋਆਧਾਰੀ_ਅੰਕ, flags & UNESCAPE_ACCEPT_NUL);
                ਜੇ (r < 0) {
                        ਜੇ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                ਜਾਰੀ;
                        }

                        ਵਾਪਸ r;
                }

                f += r;
                ਜੇ (eight_ਦੋਆਧਾਰੀ_ਅੰਕ)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ਨਹੀਂਤਾਂ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ਵਾਪਸ t - *ret;
}

ਅੱਖਰ* xescape_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad, size_t console_ਚੌੜਾਈ, XEscapeFlags flags) {
        ਅੱਖਰ *ans, *t, *prev, *prev2;
        ਅਚਲ ਅੱਖਰ *f;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ਜੇ (console_ਚੌੜਾਈ == 0)
                ਵਾਪਸ strdup("");

        size_t len_forced_ellipsis = FLAGS_ਸੈੱਟ(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = ਲੜੀ_ਲੰਬਾਈ(s);

        size_t len_body = MIN(console_ਚੌੜਾਈ, SIZE_MAX - 1); /* We need room for the NUL byte */
        ਜੇ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(ਅੱਖਰ, len_body + 1);
        ਜੇ (!ans)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (f = s, t = prev = prev2 = ans; ; f++) {
                ਅੱਖਰ *tmp_t = t;

                ਜੇ (!*f) {
                        ਜੇ (len_forced_ellipsis != 0)
                                ਰੋਕੋ;

                        *t = 0;
                        ਵਾਪਸ ans;
                }

                ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) *f < ' ' ||
                    (!FLAGS_ਸੈੱਟ(flags, XESCAPE_8_ਦੋਆਧਾਰੀ_ਅੰਕ) && (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ਜੇ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                ਰੋਕੋ;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ਨਹੀਂਤਾਂ {
                        ਜੇ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                ਰੋਕੋ;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ਜੇ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ਨਹੀਂਤਾਂ ਜੇ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        ਨਹੀਂਤਾਂ ਜੇ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ਨਹੀਂਤਾਂ
                off = len_body - c;
        ਪ੍ਰਤਿਜ੍ਞਾ(off <= (size_t) (t - ans));

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ans + off, "...", c);
        ans[off + c] = '\0';
        ਵਾਪਸ ans;
}

ਅੱਖਰ* escape_non_printable_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *str, size_t console_ਚੌੜਾਈ, XEscapeFlags flags) {
        ਜੇ (FLAGS_ਸੈੱਟ(flags, XESCAPE_8_ਦੋਆਧਾਰੀ_ਅੰਕ))
                ਵਾਪਸ xescape_ਭਰਿਆ(str, /* bad= */ ਨਿਰਾ, console_ਚੌੜਾਈ, flags);
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ utf8_escape_non_printable_ਭਰਿਆ(str,
                                                      console_ਚੌੜਾਈ,
                                                      FLAGS_ਸੈੱਟ(flags, XESCAPE_FORCE_ELLIPSIS));
}

ਅੱਖਰ* octescape_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, size_t len, ਅਚਲ ਅੱਖਰ *bad) {
        ਅੱਖਰ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        ਪ੍ਰਤਿਜ੍ਞਾ(s || len == 0);

        ਜੇ (len == SIZE_MAX)
                len = ਲੜੀ_ਲੰਬਾਈ(s);

        ਜੇ (len > (SIZE_MAX - 1) / 4)
                ਵਾਪਸ ਨਿਰਾ;

        t = buf = new(ਅੱਖਰ, len * 4 + 1);
        ਜੇ (!buf)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ਜੇ (u < ' ' || u >= 127 || ਵਿੱਚ_ਸੈੱਟ(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ਨਹੀਂਤਾਂ
                        *(t++) = u;
        }

        *t = 0;
        ਵਾਪਸ buf;
}

ਅੱਖਰ* decescape(ਅਚਲ ਅੱਖਰ *s, size_t len, ਅਚਲ ਅੱਖਰ *bad) {
        ਅੱਖਰ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        ਪ੍ਰਤਿਜ੍ਞਾ(s || len == 0);

        ਜੇ (len == SIZE_MAX)
                len = ਲੜੀ_ਲੰਬਾਈ(s);

        ਜੇ (len > (SIZE_MAX - 1) / 4)
                ਵਾਪਸ ਨਿਰਾ;

        t = buf = new(ਅੱਖਰ, len * 4 + 1);
        ਜੇ (!buf)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ਜੇ (u < ' ' || u >= 127 || ਵਿੱਚ_ਸੈੱਟ(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ਨਹੀਂਤਾਂ
                        *(t++) = u;
        }

        *t = 0;
        ਵਾਪਸ buf;
}

ਸਥਿਰ ਅੱਖਰ* strcpy_backslash_escaped(ਅੱਖਰ *t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad) {
        ਪ੍ਰਤਿਜ੍ਞਾ(bad);
        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜਦੋਂ (*s) {
                ਪੂਰਨ l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ਜੇ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ਨਹੀਂਤਾਂ ਜੇ (l == 1) {
                        ਜੇ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ਨਹੀਂਤਾਂ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ਵਾਪਸ t;
}

ਅੱਖਰ* shell_escape(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad) {
        ਅੱਖਰ *buf, *t;

        buf = new(ਅੱਖਰ, ਲੜੀ_ਲੰਬਾਈ(s)*4+1);
        ਜੇ (!buf)
                ਵਾਪਸ ਨਿਰਾ;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ਵਾਪਸ buf;
}

ਅੱਖਰ* shell_maybe_quote(ਅਚਲ ਅੱਖਰ *s, ShellEscapeFlags flags) {
        ਅਚਲ ਅੱਖਰ *p;
        ਅੱਖਰ *buf, *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ਜੇ (FLAGS_ਸੈੱਟ(flags, SHELL_ESCAPE_ਖਾਲੀ_ਥਾਵਾਂ) && isempty(s))
                ਵਾਪਸ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        ਲਈ (p = s; *p; ) {
                ਪੂਰਨ l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ਜੇ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        ਰੋਕੋ;

                p += l;
        }

        ਜੇ (!*p)
                ਵਾਪਸ strdup(s);

        buf = new(ਅੱਖਰ, FLAGS_ਸੈੱਟ(flags, SHELL_ESCAPE_POSIX) + 1 + ਲੜੀ_ਲੰਬਾਈ(s)*4 + 1 + 1);
        ਜੇ (!buf)
                ਵਾਪਸ ਨਿਰਾ;

        t = buf;
        ਜੇ (FLAGS_ਸੈੱਟ(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ਨਹੀਂਤਾਂ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_ਸੈੱਟ(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ਨਹੀਂਤਾਂ
                *(t++) = '"';
        *t = 0;

        ਵਾਪਸ str_realloc(buf);
}

ਅੱਖਰ* quote_command_ਸਤਰ(ਅੱਖਰ * ਅਚਲ *argv, ShellEscapeFlags flags) {
        _cleanup_free_ ਅੱਖਰ *ਨਤੀਜਾ = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;

                t = shell_maybe_quote(*a, flags);
                ਜੇ (!t)
                        ਵਾਪਸ ਨਿਰਾ;

                ਜੇ (!strextend_with_separator(&ਨਤੀਜਾ, " ", t))
                        ਵਾਪਸ ਨਿਰਾ;
        }

        ਵਾਪਸ str_realloc(TAKE_PTR(ਨਤੀਜਾ));
}
