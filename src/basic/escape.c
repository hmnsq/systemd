/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಪೂರ್ಣಾಂಕ cescape_char(ಅಕ್ಷರ c, ಅಕ್ಷರ *buf) {
        ಅಕ್ಷರ *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        ಆಯ್ಕೆ (c) {

                ಸ್ಥಿತಿ '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        ನಿಲ್ಲಿಸು;

                ಪೂರ್ವನಿಯೋಜಿತ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ಆದರೆ ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c >> 6);
                                *(buf++) = octchar((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c >> 3);
                                *(buf++) = octchar((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c);
                        } ಇಲ್ಲದಿದ್ದರೆ
                                *(buf++) = c;
                        ನಿಲ್ಲಿಸು;
        }

        ಹಿಂತಿರುಗಿ buf - buf_old;
}

ಅಕ್ಷರ* cescape_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;
        ಅಕ್ಷರ *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        ಪ್ರತಿಜ್ಞಾ(s || n == 0);

        ಆದರೆ (n == SIZE_MAX)
                n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        ಆದರೆ (n > (SIZE_MAX - 1) / 4)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        r = new(ಅಕ್ಷರ, n*4 + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ cunescape_one(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t length, char32_t *ret, bool *eight_ಬಿಟ್, bool accept_nul) {
        ಪೂರ್ಣಾಂಕ r = 1;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(eight_ಬಿಟ್);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ಆದರೆ (length != SIZE_MAX && length < 1)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆಯ್ಕೆ (p[0]) {

        ಸ್ಥಿತಿ 'a':
                *ret = '\a';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'b':
                *ret = '\b';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'f':
                *ret = '\f';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'n':
                *ret = '\n';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'r':
                *ret = '\r';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 't':
                *ret = '\t';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'v':
                *ret = '\v';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ '\\':
                *ret = '\\';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ '"':
                *ret = '"';
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ '\'':
                *ret = '\'';
                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'x': {
                /* hexadecimal encoding */
                ಪೂರ್ಣಾಂಕ a, b;

                ಆದರೆ (length != SIZE_MAX && length < 3)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                a = unhexchar(p[1]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unhexchar(p[2]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Don't allow NUL bytes */
                ಆದರೆ (a == 0 && b == 0 && !accept_nul)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_ಬಿಟ್ = true;
                r = 3;
                ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ 'u': {
                /* C++11 style 16-bit unicode */

                ಪೂರ್ಣಾಂಕ a[4];
                size_t i;
                uint32_t c;

                ಆದರೆ (length != SIZE_MAX && length < 5)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಗಾಗಿ (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ಆದರೆ (a[i] < 0)
                                ಹಿಂತಿರುಗಿ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ಆದರೆ (c == 0 && !accept_nul)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ಆದರೆ (utf16_is_surrogate(c))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *ret = c;
                r = 5;
                ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ 'U': {
                /* C++11 style 32-bit unicode */

                ಪೂರ್ಣಾಂಕ a[8];
                size_t i;
                char32_t c;

                ಆದರೆ (length != SIZE_MAX && length < 9)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಗಾಗಿ (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ಆದರೆ (a[i] < 0)
                                ಹಿಂತಿರುಗಿ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ಆದರೆ (c == 0 && !accept_nul)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Don't allow invalid code points */
                ಆದರೆ (!unichar_is_valid(c))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *ret = c;
                r = 9;
                ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ '0':
        ಸ್ಥಿತಿ '1':
        ಸ್ಥಿತಿ '2':
        ಸ್ಥಿತಿ '3':
        ಸ್ಥಿತಿ '4':
        ಸ್ಥಿತಿ '5':
        ಸ್ಥಿತಿ '6':
        ಸ್ಥಿತಿ '7': {
                /* octal encoding */
                ಪೂರ್ಣಾಂಕ a, b, c;
                char32_t m;

                ಆದರೆ (length != SIZE_MAX && length < 3)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                a = unoctchar(p[0]);
                ಆದರೆ (a < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                b = unoctchar(p[1]);
                ಆದರೆ (b < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                c = unoctchar(p[2]);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* don't allow NUL bytes */
                ಆದರೆ (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ಆದರೆ (m > 255)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *ret = m;
                *eight_ಬಿಟ್ = true;
                r = 3;
                ನಿಲ್ಲಿಸು;
        }

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಹಿಂತಿರುಗಿ r;
}

ssize_t cunescape_ಉದ್ದಳತೆ_with_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, UnescapeFlags flags, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ans = ಖಾಲಿ;
        ಅಕ್ಷರ *t;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;
        size_t pl;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ಆದರೆ (length == SIZE_MAX)
                length = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        pl = strlen_ptr(ಪೂರ್ವಪ್ರತ್ಯಯ);

        ans = new(ಅಕ್ಷರ, pl+length+1);
        ಆದರೆ (!ans)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ)
                ಸ್ಮರಣೆ_ನಕಲು(ans, ಪೂರ್ವಪ್ರತ್ಯಯ, pl);

        ಗಾಗಿ (f = s, t = ans + pl; f < s + length; f++) {
                size_t ಉಳಿದ;
                bool eight_ಬಿಟ್ = false;
                char32_t u;

                ಉಳಿದ = s + length - f;
                ಪ್ರತಿಜ್ಞಾ(ಉಳಿದ > 0);

                ಆದರೆ (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (ಉಳಿದ == 1) {
                        ಆದರೆ (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                ಮುಂದುವರಿ;
                        }

                        ಹಿಂತಿರುಗಿ -EINVAL;
                }

                r = cunescape_one(f + 1, ಉಳಿದ - 1, &u, &eight_ಬಿಟ್, flags & UNESCAPE_ACCEPT_NUL);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                ಮುಂದುವರಿ;
                        }

                        ಹಿಂತಿರುಗಿ r;
                }

                f += r;
                ಆದರೆ (eight_ಬಿಟ್)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ಇಲ್ಲದಿದ್ದರೆ
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        ಪ್ರತಿಜ್ಞಾ(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ಹಿಂತಿರುಗಿ t - *ret;
}

ಅಕ್ಷರ* xescape_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad, size_t console_ಅಗಲ, XEscapeFlags flags) {
        ಅಕ್ಷರ *ans, *t, *prev, *prev2;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ಆದರೆ (console_ಅಗಲ == 0)
                ಹಿಂತಿರುಗಿ strdup("");

        size_t len_forced_ellipsis = FLAGS_ಗಣ(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        size_t len_body = MIN(console_ಅಗಲ, SIZE_MAX - 1); /* We need room for the NUL byte */
        ಆದರೆ (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(ಅಕ್ಷರ, len_body + 1);
        ಆದರೆ (!ans)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (f = s, t = prev = prev2 = ans; ; f++) {
                ಅಕ್ಷರ *tmp_t = t;

                ಆದರೆ (!*f) {
                        ಆದರೆ (len_forced_ellipsis != 0)
                                ನಿಲ್ಲಿಸು;

                        *t = 0;
                        ಹಿಂತಿರುಗಿ ans;
                }

                ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) *f < ' ' ||
                    (!FLAGS_ಗಣ(flags, XESCAPE_8_ಬಿಟ್) && (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ಆದರೆ ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                ನಿಲ್ಲಿಸು;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                ನಿಲ್ಲಿಸು;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ಆದರೆ (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ಇಲ್ಲದಿದ್ದರೆ
                off = len_body - c;
        ಪ್ರತಿಜ್ಞಾ(off <= (size_t) (t - ans));

        ಸ್ಮರಣೆ_ನಕಲು(ans + off, "...", c);
        ans[off + c] = '\0';
        ಹಿಂತಿರುಗಿ ans;
}

ಅಕ್ಷರ* escape_non_printable_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t console_ಅಗಲ, XEscapeFlags flags) {
        ಆದರೆ (FLAGS_ಗಣ(flags, XESCAPE_8_ಬಿಟ್))
                ಹಿಂತಿರುಗಿ xescape_ತುಂಬಿದ(str, /* bad= */ ಖಾಲಿ, console_ಅಗಲ, flags);
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ utf8_escape_non_printable_ತುಂಬಿದ(str,
                                                      console_ಅಗಲ,
                                                      FLAGS_ಗಣ(flags, XESCAPE_FORCE_ELLIPSIS));
}

ಅಕ್ಷರ* octescape_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಅಕ್ಷರ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        ಪ್ರತಿಜ್ಞಾ(s || len == 0);

        ಆದರೆ (len == SIZE_MAX)
                len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        ಆದರೆ (len > (SIZE_MAX - 1) / 4)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        t = buf = new(ಅಕ್ಷರ, len * 4 + 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ಆದರೆ (u < ' ' || u >= 127 || ಒಳ_ಗಣ(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = u;
        }

        *t = 0;
        ಹಿಂತಿರುಗಿ buf;
}

ಅಕ್ಷರ* decescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಅಕ್ಷರ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        ಪ್ರತಿಜ್ಞಾ(s || len == 0);

        ಆದರೆ (len == SIZE_MAX)
                len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        ಆದರೆ (len > (SIZE_MAX - 1) / 4)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        t = buf = new(ಅಕ್ಷರ, len * 4 + 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ಆದರೆ (u < ' ' || u >= 127 || ಒಳ_ಗಣ(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = u;
        }

        *t = 0;
        ಹಿಂತಿರುಗಿ buf;
}

ಸ್ಥಿರ ಅಕ್ಷರ* strcpy_backslash_escaped(ಅಕ್ಷರ *t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಪ್ರತಿಜ್ಞಾ(bad);
        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆಗ (*s) {
                ಪೂರ್ಣಾಂಕ l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ಆದರೆ (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l == 1) {
                        ಆದರೆ (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ಇಲ್ಲದಿದ್ದರೆ {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ಹಿಂತಿರುಗಿ t;
}

ಅಕ್ಷರ* shell_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಅಕ್ಷರ *buf, *t;

        buf = new(ಅಕ್ಷರ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s)*4+1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ಹಿಂತಿರುಗಿ buf;
}

ಅಕ್ಷರ* shell_maybe_quote(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ShellEscapeFlags flags) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಅಕ್ಷರ *buf, *t;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ಆದರೆ (FLAGS_ಗಣ(flags, SHELL_ESCAPE_ಖಾಲಿಗಳು) && isempty(s))
                ಹಿಂತಿರುಗಿ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        ಗಾಗಿ (p = s; *p; ) {
                ಪೂರ್ಣಾಂಕ l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ಆದರೆ (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        ನಿಲ್ಲಿಸು;

                p += l;
        }

        ಆದರೆ (!*p)
                ಹಿಂತಿರುಗಿ strdup(s);

        buf = new(ಅಕ್ಷರ, FLAGS_ಗಣ(flags, SHELL_ESCAPE_POSIX) + 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s)*4 + 1 + 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        t = buf;
        ಆದರೆ (FLAGS_ಗಣ(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ಇಲ್ಲದಿದ್ದರೆ
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_ಗಣ(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ಆದರೆ (FLAGS_ಗಣ(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ಇಲ್ಲದಿದ್ದರೆ
                *(t++) = '"';
        *t = 0;

        ಹಿಂತಿರುಗಿ str_realloc(buf);
}

ಅಕ್ಷರ* quote_command_ಸಾಲು(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *argv, ShellEscapeFlags flags) {
        _cleanup_free_ ಅಕ್ಷರ *ಫಲಿತಾಂಶ = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;

                t = shell_maybe_quote(*a, flags);
                ಆದರೆ (!t)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಆದರೆ (!strextend_with_separator(&ಫಲಿತಾಂಶ, " ", t))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }

        ಹಿಂತಿರುಗಿ str_realloc(TAKE_PTR(ಫಲಿತಾಂಶ));
}
