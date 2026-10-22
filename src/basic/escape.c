/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cescape_char(ଅକ୍ଷର c, ଅକ୍ଷର *buf) {
        ଅକ୍ଷର *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        ବାଛ (c) {

                ସ୍ଥିତି '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        ଭାଙ୍ଗ;

                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        ଯଦି ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned ଅକ୍ଷର) c >> 6);
                                *(buf++) = octchar((unsigned ଅକ୍ଷର) c >> 3);
                                *(buf++) = octchar((unsigned ଅକ୍ଷର) c);
                        } ଅନ୍ୟଥା
                                *(buf++) = c;
                        ଭାଙ୍ଗ;
        }

        ଫେରନ୍ତୁ buf - buf_old;
}

ଅକ୍ଷର* cescape_length(ଅଚଳ ଅକ୍ଷର *s, size_t n) {
        ଅଚଳ ଅକ୍ଷର *f;
        ଅକ୍ଷର *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        ପ୍ରତିଜ୍ଞା(s || n == 0);

        ଯଦି (n == SIZE_MAX)
                n = strlen(s);

        ଯଦି (n > (SIZE_MAX - 1) / 4)
                ଫେରନ୍ତୁ ଖାଲି;

        r = new(ଅକ୍ଷର, n*4 + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cunescape_one(ଅଚଳ ଅକ୍ଷର *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 1;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        ଯଦି (length != SIZE_MAX && length < 1)
                ଫେରନ୍ତୁ -EINVAL;

        ବାଛ (p[0]) {

        ସ୍ଥିତି 'a':
                *ret = '\a';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 'b':
                *ret = '\b';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 'f':
                *ret = '\f';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 'n':
                *ret = '\n';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 'r':
                *ret = '\r';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 't':
                *ret = '\t';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 'v':
                *ret = '\v';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି '\\':
                *ret = '\\';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି '"':
                *ret = '"';
                ଭାଙ୍ଗ;
        ସ୍ଥିତି '\'':
                *ret = '\'';
                ଭାଙ୍ଗ;

        ସ୍ଥିତି 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                ଭାଙ୍ଗ;

        ସ୍ଥିତି 'x': {
                /* hexadecimal encoding */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b;

                ଯଦି (length != SIZE_MAX && length < 3)
                        ଫେରନ୍ତୁ -EINVAL;

                a = unhexchar(p[1]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unhexchar(p[2]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Don't allow NUL bytes */
                ଯଦି (a == 0 && b == 0 && !accept_nul)
                        ଫେରନ୍ତୁ -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି 'u': {
                /* C++11 style 16-bit unicode */

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା a[4];
                size_t i;
                uint32_t c;

                ଯଦି (length != SIZE_MAX && length < 5)
                        ଫେରନ୍ତୁ -EINVAL;

                ପାଇଁ (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ଯଦି (a[i] < 0)
                                ଫେରନ୍ତୁ a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                ଯଦି (c == 0 && !accept_nul)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                ଯଦି (utf16_is_surrogate(c))
                        ଫେରନ୍ତୁ -EINVAL;

                *ret = c;
                r = 5;
                ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି 'U': {
                /* C++11 style 32-bit unicode */

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା a[8];
                size_t i;
                char32_t c;

                ଯଦି (length != SIZE_MAX && length < 9)
                        ଫେରନ୍ତୁ -EINVAL;

                ପାଇଁ (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        ଯଦି (a[i] < 0)
                                ଫେରନ୍ତୁ a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                ଯଦି (c == 0 && !accept_nul)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Don't allow invalid code points */
                ଯଦି (!unichar_is_valid(c))
                        ଫେରନ୍ତୁ -EINVAL;

                *ret = c;
                r = 9;
                ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି '0':
        ସ୍ଥିତି '1':
        ସ୍ଥିତି '2':
        ସ୍ଥିତି '3':
        ସ୍ଥିତି '4':
        ସ୍ଥିତି '5':
        ସ୍ଥିତି '6':
        ସ୍ଥିତି '7': {
                /* octal encoding */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b, c;
                char32_t m;

                ଯଦି (length != SIZE_MAX && length < 3)
                        ଫେରନ୍ତୁ -EINVAL;

                a = unoctchar(p[0]);
                ଯଦି (a < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                b = unoctchar(p[1]);
                ଯଦି (b < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                c = unoctchar(p[2]);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* don't allow NUL bytes */
                ଯଦି (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                ଯଦି (m > 255)
                        ଫେରନ୍ତୁ -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                ଭାଙ୍ଗ;
        }

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EINVAL;
        }

        ଫେରନ୍ତୁ r;
}

ssize_t cunescape_length_with_prefix(ଅଚଳ ଅକ୍ଷର *s, size_t length, ଅଚଳ ଅକ୍ଷର *prefix, UnescapeFlags flags, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *ans = ଖାଲି;
        ଅକ୍ଷର *t;
        ଅଚଳ ଅକ୍ଷର *f;
        size_t pl;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        ଯଦି (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(ଅକ୍ଷର, pl+length+1);
        ଯଦି (!ans)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (prefix)
                memcpy(ans, prefix, pl);

        ପାଇଁ (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                ପ୍ରତିଜ୍ଞା(remaining > 0);

                ଯଦି (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        ଜାରି;
                }

                ଯଦି (remaining == 1) {
                        ଯଦି (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                ଜାରି;
                        }

                        ଫେରନ୍ତୁ -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                ଯଦି (r < 0) {
                        ଯଦି (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                ଜାରି;
                        }

                        ଫେରନ୍ତୁ r;
                }

                f += r;
                ଯଦି (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                ଅନ୍ୟଥା
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        ପ୍ରତିଜ୍ଞା(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ଫେରନ୍ତୁ t - *ret;
}

ଅକ୍ଷର* xescape_full(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad, size_t console_width, XEscapeFlags flags) {
        ଅକ୍ଷର *ans, *t, *prev, *prev2;
        ଅଚଳ ଅକ୍ଷର *f;

        ପ୍ରତିଜ୍ଞା(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        ଯଦି (console_width == 0)
                ଫେରନ୍ତୁ strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        ଯଦି (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(ଅକ୍ଷର, len_body + 1);
        ଯଦି (!ans)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (f = s, t = prev = prev2 = ans; ; f++) {
                ଅକ୍ଷର *tmp_t = t;

                ଯଦି (!*f) {
                        ଯଦି (len_forced_ellipsis != 0)
                                ଭାଙ୍ଗ;

                        *t = 0;
                        ଫେରନ୍ତୁ ans;
                }

                ଯଦି ((unsigned ଅକ୍ଷର) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned ଅକ୍ଷର) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        ଯଦି ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                ଭାଙ୍ଗ;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } ଅନ୍ୟଥା {
                        ଯଦି ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                ଭାଙ୍ଗ;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        ଯଦି (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        ଅନ୍ୟଥା ଯଦି (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        ଅନ୍ୟଥା ଯଦି (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        ଅନ୍ୟଥା
                off = len_body - c;
        ପ୍ରତିଜ୍ଞା(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        ଫେରନ୍ତୁ ans;
}

ଅକ୍ଷର* escape_non_printable_full(ଅଚଳ ଅକ୍ଷର *str, size_t console_width, XEscapeFlags flags) {
        ଯଦି (FLAGS_SET(flags, XESCAPE_8_BIT))
                ଫେରନ୍ତୁ xescape_full(str, /* bad= */ ଖାଲି, console_width, flags);
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

ଅକ୍ଷର* octescape_full(ଅଚଳ ଅକ୍ଷର *s, size_t len, ଅଚଳ ଅକ୍ଷର *bad) {
        ଅକ୍ଷର *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        ପ୍ରତିଜ୍ଞା(s || len == 0);

        ଯଦି (len == SIZE_MAX)
                len = strlen(s);

        ଯଦି (len > (SIZE_MAX - 1) / 4)
                ଫେରନ୍ତୁ ଖାଲି;

        t = buf = new(ଅକ୍ଷର, len * 4 + 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ଯଦି (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } ଅନ୍ୟଥା
                        *(t++) = u;
        }

        *t = 0;
        ଫେରନ୍ତୁ buf;
}

ଅକ୍ଷର* decescape(ଅଚଳ ଅକ୍ଷର *s, size_t len, ଅଚଳ ଅକ୍ଷର *bad) {
        ଅକ୍ଷର *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        ପ୍ରତିଜ୍ଞା(s || len == 0);

        ଯଦି (len == SIZE_MAX)
                len = strlen(s);

        ଯଦି (len > (SIZE_MAX - 1) / 4)
                ଫେରନ୍ତୁ ଖାଲି;

        t = buf = new(ଅକ୍ଷର, len * 4 + 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                ଯଦି (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } ଅନ୍ୟଥା
                        *(t++) = u;
        }

        *t = 0;
        ଫେରନ୍ତୁ buf;
}

ସ୍ଥିର ଅକ୍ଷର* strcpy_backslash_escaped(ଅକ୍ଷର *t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad) {
        ପ୍ରତିଜ୍ଞା(bad);
        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(s);

        ଯେତେବେଳେ (*s) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                ଯଦି (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                ଅନ୍ୟଥା ଯଦି (l == 1) {
                        ଯଦି (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } ଅନ୍ୟଥା {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ଫେରନ୍ତୁ t;
}

ଅକ୍ଷର* shell_escape(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad) {
        ଅକ୍ଷର *buf, *t;

        buf = new(ଅକ୍ଷର, strlen(s)*4+1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ ଖାଲି;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ଫେରନ୍ତୁ buf;
}

ଅକ୍ଷର* shell_maybe_quote(ଅଚଳ ଅକ୍ଷର *s, ShellEscapeFlags flags) {
        ଅଚଳ ଅକ୍ଷର *p;
        ଅକ୍ଷର *buf, *t;

        ପ୍ରତିଜ୍ଞା(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        ଯଦି (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                ଫେରନ୍ତୁ strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        ପାଇଁ (p = s; *p; ) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                ଯଦି (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        ଭାଙ୍ଗ;

                p += l;
        }

        ଯଦି (!*p)
                ଫେରନ୍ତୁ strdup(s);

        buf = new(ଅକ୍ଷର, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ ଖାଲି;

        t = buf;
        ଯଦି (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } ଅନ୍ୟଥା
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        ଯଦି (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        ଅନ୍ୟଥା
                *(t++) = '"';
        *t = 0;

        ଫେରନ୍ତୁ str_realloc(buf);
}

ଅକ୍ଷର* quote_command_ଧାଡ଼ି(ଅକ୍ଷର * ଅଚଳ *argv, ShellEscapeFlags flags) {
        _cleanup_free_ ଅକ୍ଷର *result = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;

                t = shell_maybe_quote(*a, flags);
                ଯଦି (!t)
                        ଫେରନ୍ତୁ ଖାଲି;

                ଯଦି (!strextend_with_separator(&result, " ", t))
                        ଫେରନ୍ତୁ ଖାଲି;
        }

        ଫେରନ୍ତୁ str_realloc(TAKE_PTR(result));
}
