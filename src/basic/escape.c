/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಪೂರ್ಣಾಙ್ಕ cescape_char(ವರ್ಣ c, ವರ್ಣ *buf) {
        ವರ್ಣ *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        ವಿಕಲ್ಪ (c) {

                ಪಕ್ಷ '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        ಭಙ್ಗ;
                ಪಕ್ಷ '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        ಭಙ್ಗ;

                ಸ್ವಭಾವ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ಯದಿ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned ವರ್ಣ) c >> 6);
                                *(buf++) = octchar((unsigned ವರ್ಣ) c >> 3);
                                *(buf++) = octchar((unsigned ವರ್ಣ) c);
                        } ಅನ್ಯಥಾ
                                *(buf++) = c;
                        ಭಙ್ಗ;
        }

        ಪ್ರತ್ಯಾಗಮ buf - buf_old;
}

ವರ್ಣ* cescape_length(ಧ್ರುವ ವರ್ಣ *s, size_t n) {
        ಧ್ರುವ ವರ್ಣ *f;
        ವರ್ಣ *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        ಪ್ರತಿಜ್ಞಾ(s || n == 0);

        ಯದಿ (n == SIZE_MAX)
                n = strlen(s);

        ಯದಿ (n > (SIZE_MAX - 1) / 4)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        r = new(ವರ್ಣ, n*4 + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ cunescape_one(ಧ್ರುವ ವರ್ಣ *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        ಪೂರ್ಣಾಙ್ಕ r = 1;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ಯದಿ (length != SIZE_MAX && length < 1)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ವಿಕಲ್ಪ (p[0]) {

        ಪಕ್ಷ 'a':
                *ret = '\a';
                ಭಙ್ಗ;
        ಪಕ್ಷ 'b':
                *ret = '\b';
                ಭಙ್ಗ;
        ಪಕ್ಷ 'f':
                *ret = '\f';
                ಭಙ್ಗ;
        ಪಕ್ಷ 'n':
                *ret = '\n';
                ಭಙ್ಗ;
        ಪಕ್ಷ 'r':
                *ret = '\r';
                ಭಙ್ಗ;
        ಪಕ್ಷ 't':
                *ret = '\t';
                ಭಙ್ಗ;
        ಪಕ್ಷ 'v':
                *ret = '\v';
                ಭಙ್ಗ;
        ಪಕ್ಷ '\\':
                *ret = '\\';
                ಭಙ್ಗ;
        ಪಕ್ಷ '"':
                *ret = '"';
                ಭಙ್ಗ;
        ಪಕ್ಷ '\'':
                *ret = '\'';
                ಭಙ್ಗ;

        ಪಕ್ಷ 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                ಭಙ್ಗ;

        ಪಕ್ಷ 'x': {
                /* hexadecimal encoding */
                ಪೂರ್ಣಾಙ್ಕ a, b;

                ಯದಿ (length != SIZE_MAX && length < 3)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                a = unhexchar(p[1]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unhexchar(p[2]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Don't allow NUL bytes */
                ಯದಿ (a == 0 && b == 0 && !accept_nul)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                ಭಙ್ಗ;
        }

        ಪಕ್ಷ 'u': {
                /* C++11 style 16-bit unicode */

                ಪೂರ್ಣಾಙ್ಕ a[4];
                size_t i;
                uint32_t c;

                ಯದಿ (length != SIZE_MAX && length < 5)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತಿ (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ಯದಿ (a[i] < 0)
                                ಪ್ರತ್ಯಾಗಮ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ಯದಿ (c == 0 && !accept_nul)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ಯದಿ (utf16_is_surrogate(c))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *ret = c;
                r = 5;
                ಭಙ್ಗ;
        }

        ಪಕ್ಷ 'U': {
                /* C++11 style 32-bit unicode */

                ಪೂರ್ಣಾಙ್ಕ a[8];
                size_t i;
                char32_t c;

                ಯದಿ (length != SIZE_MAX && length < 9)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತಿ (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ಯದಿ (a[i] < 0)
                                ಪ್ರತ್ಯಾಗಮ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ಯದಿ (c == 0 && !accept_nul)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Don't allow invalid code points */
                ಯದಿ (!unichar_is_valid(c))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *ret = c;
                r = 9;
                ಭಙ್ಗ;
        }

        ಪಕ್ಷ '0':
        ಪಕ್ಷ '1':
        ಪಕ್ಷ '2':
        ಪಕ್ಷ '3':
        ಪಕ್ಷ '4':
        ಪಕ್ಷ '5':
        ಪಕ್ಷ '6':
        ಪಕ್ಷ '7': {
                /* octal encoding */
                ಪೂರ್ಣಾಙ್ಕ a, b, c;
                char32_t m;

                ಯದಿ (length != SIZE_MAX && length < 3)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                a = unoctchar(p[0]);
                ಯದಿ (a < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                b = unoctchar(p[1]);
                ಯದಿ (b < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                c = unoctchar(p[2]);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* don't allow NUL bytes */
                ಯದಿ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ಯದಿ (m > 255)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                ಭಙ್ಗ;
        }

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ssize_t cunescape_length_with_prefix(ಧ್ರುವ ವರ್ಣ *s, size_t length, ಧ್ರುವ ವರ್ಣ *prefix, UnescapeFlags flags, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *ans = ರಿಕ್ತ;
        ವರ್ಣ *t;
        ಧ್ರುವ ವರ್ಣ *f;
        size_t pl;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ಯದಿ (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(ವರ್ಣ, pl+length+1);
        ಯದಿ (!ans)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (prefix)
                memcpy(ans, prefix, pl);

        ಪ್ರತಿ (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                ಪ್ರತಿಜ್ಞಾ(remaining > 0);

                ಯದಿ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        ಅನುವರ್ತ;
                }

                ಯದಿ (remaining == 1) {
                        ಯದಿ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                ಅನುವರ್ತ;
                        }

                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                ಯದಿ (r < 0) {
                        ಯದಿ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                ಅನುವರ್ತ;
                        }

                        ಪ್ರತ್ಯಾಗಮ r;
                }

                f += r;
                ಯದಿ (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ಅನ್ಯಥಾ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        ಪ್ರತಿಜ್ಞಾ(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ಪ್ರತ್ಯಾಗಮ t - *ret;
}

ವರ್ಣ* xescape_full(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad, size_t console_width, XEscapeFlags flags) {
        ವರ್ಣ *ans, *t, *prev, *prev2;
        ಧ್ರುವ ವರ್ಣ *f;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ಯದಿ (console_width == 0)
                ಪ್ರತ್ಯಾಗಮ strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        ಯದಿ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(ವರ್ಣ, len_body + 1);
        ಯದಿ (!ans)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (f = s, t = prev = prev2 = ans; ; f++) {
                ವರ್ಣ *tmp_t = t;

                ಯದಿ (!*f) {
                        ಯದಿ (len_forced_ellipsis != 0)
                                ಭಙ್ಗ;

                        *t = 0;
                        ಪ್ರತ್ಯಾಗಮ ans;
                }

                ಯದಿ ((unsigned ವರ್ಣ) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned ವರ್ಣ) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ಯದಿ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                ಭಙ್ಗ;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ಅನ್ಯಥಾ {
                        ಯದಿ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                ಭಙ್ಗ;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ಯದಿ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ಅನ್ಯಥಾ ಯದಿ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        ಅನ್ಯಥಾ ಯದಿ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ಅನ್ಯಥಾ
                off = len_body - c;
        ಪ್ರತಿಜ್ಞಾ(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        ಪ್ರತ್ಯಾಗಮ ans;
}

ವರ್ಣ* escape_non_printable_full(ಧ್ರುವ ವರ್ಣ *str, size_t console_width, XEscapeFlags flags) {
        ಯದಿ (FLAGS_SET(flags, XESCAPE_8_BIT))
                ಪ್ರತ್ಯಾಗಮ xescape_full(str, /* bad= */ ರಿಕ್ತ, console_width, flags);
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

ವರ್ಣ* octescape_full(ಧ್ರುವ ವರ್ಣ *s, size_t len, ಧ್ರುವ ವರ್ಣ *bad) {
        ವರ್ಣ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        ಪ್ರತಿಜ್ಞಾ(s || len == 0);

        ಯದಿ (len == SIZE_MAX)
                len = strlen(s);

        ಯದಿ (len > (SIZE_MAX - 1) / 4)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        t = buf = new(ವರ್ಣ, len * 4 + 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ಯದಿ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ಅನ್ಯಥಾ
                        *(t++) = u;
        }

        *t = 0;
        ಪ್ರತ್ಯಾಗಮ buf;
}

ವರ್ಣ* decescape(ಧ್ರುವ ವರ್ಣ *s, size_t len, ಧ್ರುವ ವರ್ಣ *bad) {
        ವರ್ಣ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        ಪ್ರತಿಜ್ಞಾ(s || len == 0);

        ಯದಿ (len == SIZE_MAX)
                len = strlen(s);

        ಯದಿ (len > (SIZE_MAX - 1) / 4)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        t = buf = new(ವರ್ಣ, len * 4 + 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ಯದಿ (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ಅನ್ಯಥಾ
                        *(t++) = u;
        }

        *t = 0;
        ಪ್ರತ್ಯಾಗಮ buf;
}

ಸ್ಥಿರ ವರ್ಣ* strcpy_backslash_escaped(ವರ್ಣ *t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad) {
        ಪ್ರತಿಜ್ಞಾ(bad);
        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯಾವತ್ (*s) {
                ಪೂರ್ಣಾಙ್ಕ l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ಯದಿ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ಅನ್ಯಥಾ ಯದಿ (l == 1) {
                        ಯದಿ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ಅನ್ಯಥಾ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ಪ್ರತ್ಯಾಗಮ t;
}

ವರ್ಣ* shell_escape(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad) {
        ವರ್ಣ *buf, *t;

        buf = new(ವರ್ಣ, strlen(s)*4+1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ಪ್ರತ್ಯಾಗಮ buf;
}

ವರ್ಣ* shell_maybe_quote(ಧ್ರುವ ವರ್ಣ *s, ShellEscapeFlags flags) {
        ಧ್ರುವ ವರ್ಣ *p;
        ವರ್ಣ *buf, *t;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ಯದಿ (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                ಪ್ರತ್ಯಾಗಮ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        ಪ್ರತಿ (p = s; *p; ) {
                ಪೂರ್ಣಾಙ್ಕ l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ಯದಿ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        ಭಙ್ಗ;

                p += l;
        }

        ಯದಿ (!*p)
                ಪ್ರತ್ಯಾಗಮ strdup(s);

        buf = new(ವರ್ಣ, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        t = buf;
        ಯದಿ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ಅನ್ಯಥಾ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ಯದಿ (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ಅನ್ಯಥಾ
                *(t++) = '"';
        *t = 0;

        ಪ್ರತ್ಯಾಗಮ str_realloc(buf);
}

ವರ್ಣ* quote_command_ಸಾಲು(ವರ್ಣ * ಧ್ರುವ *argv, ShellEscapeFlags flags) {
        _cleanup_free_ ವರ್ಣ *result = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;

                t = shell_maybe_quote(*a, flags);
                ಯದಿ (!t)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಯದಿ (!strextend_with_separator(&result, " ", t))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }

        ಪ್ರತ್ಯಾಗಮ str_realloc(TAKE_PTR(result));
}
