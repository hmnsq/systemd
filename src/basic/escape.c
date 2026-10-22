/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "utf8.h"

પૂર્ણાંક cescape_char(અક્ષર c, અક્ષર *buf) {
        અક્ષર *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        પસંદગી (c) {

                સ્થિતિ '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        તોડો;
                સ્થિતિ '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        તોડો;
                સ્થિતિ '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        તોડો;
                સ્થિતિ '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        તોડો;
                સ્થિતિ '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        તોડો;
                સ્થિતિ '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        તોડો;
                સ્થિતિ '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        તોડો;
                સ્થિતિ '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        તોડો;
                સ્થિતિ '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        તોડો;
                સ્થિતિ '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        તોડો;

                મૂળભૂત:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        જો ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned અક્ષર) c >> 6);
                                *(buf++) = octchar((unsigned અક્ષર) c >> 3);
                                *(buf++) = octchar((unsigned અક્ષર) c);
                        } નહીંતર
                                *(buf++) = c;
                        તોડો;
        }

        પાછા buf - buf_old;
}

અક્ષર* cescape_length(અચલ અક્ષર *s, size_t n) {
        અચલ અક્ષર *f;
        અક્ષર *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        પ્રતિજ્ઞા(s || n == 0);

        જો (n == SIZE_MAX)
                n = strlen(s);

        જો (n > (SIZE_MAX - 1) / 4)
                પાછા ખાલી;

        r = new(અક્ષર, n*4 + 1);
        જો (!r)
                પાછા ખાલી;

        માટે (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        પાછા r;
}

પૂર્ણાંક cunescape_one(અચલ અક્ષર *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        પૂર્ણાંક r = 1;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        જો (length != SIZE_MAX && length < 1)
                પાછા -EINVAL;

        પસંદગી (p[0]) {

        સ્થિતિ 'a':
                *ret = '\a';
                તોડો;
        સ્થિતિ 'b':
                *ret = '\b';
                તોડો;
        સ્થિતિ 'f':
                *ret = '\f';
                તોડો;
        સ્થિતિ 'n':
                *ret = '\n';
                તોડો;
        સ્થિતિ 'r':
                *ret = '\r';
                તોડો;
        સ્થિતિ 't':
                *ret = '\t';
                તોડો;
        સ્થિતિ 'v':
                *ret = '\v';
                તોડો;
        સ્થિતિ '\\':
                *ret = '\\';
                તોડો;
        સ્થિતિ '"':
                *ret = '"';
                તોડો;
        સ્થિતિ '\'':
                *ret = '\'';
                તોડો;

        સ્થિતિ 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                તોડો;

        સ્થિતિ 'x': {
                /* hexadecimal encoding */
                પૂર્ણાંક a, b;

                જો (length != SIZE_MAX && length < 3)
                        પાછા -EINVAL;

                a = unhexchar(p[1]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unhexchar(p[2]);
                જો (b < 0)
                        પાછા -EINVAL;

                /* Don't allow NUL bytes */
                જો (a == 0 && b == 0 && !accept_nul)
                        પાછા -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                તોડો;
        }

        સ્થિતિ 'u': {
                /* C++11 style 16-bit unicode */

                પૂર્ણાંક a[4];
                size_t i;
                uint32_t c;

                જો (length != SIZE_MAX && length < 5)
                        પાછા -EINVAL;

                માટે (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        જો (a[i] < 0)
                                પાછા a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                જો (c == 0 && !accept_nul)
                        પાછા -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                જો (utf16_is_surrogate(c))
                        પાછા -EINVAL;

                *ret = c;
                r = 5;
                તોડો;
        }

        સ્થિતિ 'U': {
                /* C++11 style 32-bit unicode */

                પૂર્ણાંક a[8];
                size_t i;
                char32_t c;

                જો (length != SIZE_MAX && length < 9)
                        પાછા -EINVAL;

                માટે (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        જો (a[i] < 0)
                                પાછા a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                જો (c == 0 && !accept_nul)
                        પાછા -EINVAL;

                /* Don't allow invalid code points */
                જો (!unichar_is_valid(c))
                        પાછા -EINVAL;

                *ret = c;
                r = 9;
                તોડો;
        }

        સ્થિતિ '0':
        સ્થિતિ '1':
        સ્થિતિ '2':
        સ્થિતિ '3':
        સ્થિતિ '4':
        સ્થિતિ '5':
        સ્થિતિ '6':
        સ્થિતિ '7': {
                /* octal encoding */
                પૂર્ણાંક a, b, c;
                char32_t m;

                જો (length != SIZE_MAX && length < 3)
                        પાછા -EINVAL;

                a = unoctchar(p[0]);
                જો (a < 0)
                        પાછા -EINVAL;

                b = unoctchar(p[1]);
                જો (b < 0)
                        પાછા -EINVAL;

                c = unoctchar(p[2]);
                જો (c < 0)
                        પાછા -EINVAL;

                /* don't allow NUL bytes */
                જો (a == 0 && b == 0 && c == 0 && !accept_nul)
                        પાછા -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                જો (m > 255)
                        પાછા -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                તોડો;
        }

        મૂળભૂત:
                પાછા -EINVAL;
        }

        પાછા r;
}

ssize_t cunescape_length_with_prefix(અચલ અક્ષર *s, size_t length, અચલ અક્ષર *prefix, UnescapeFlags flags, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *ans = ખાલી;
        અક્ષર *t;
        અચલ અક્ષર *f;
        size_t pl;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        જો (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(અક્ષર, pl+length+1);
        જો (!ans)
                પાછા -ENOMEM;

        જો (prefix)
                memcpy(ans, prefix, pl);

        માટે (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                પ્રતિજ્ઞા(remaining > 0);

                જો (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        ચાલુ;
                }

                જો (remaining == 1) {
                        જો (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                ચાલુ;
                        }

                        પાછા -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                જો (r < 0) {
                        જો (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                ચાલુ;
                        }

                        પાછા r;
                }

                f += r;
                જો (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                નહીંતર
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        પ્રતિજ્ઞા(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        પાછા t - *ret;
}

અક્ષર* xescape_full(અચલ અક્ષર *s, અચલ અક્ષર *bad, size_t console_width, XEscapeFlags flags) {
        અક્ષર *ans, *t, *prev, *prev2;
        અચલ અક્ષર *f;

        પ્રતિજ્ઞા(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        જો (console_width == 0)
                પાછા strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        જો (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(અક્ષર, len_body + 1);
        જો (!ans)
                પાછા ખાલી;

        માટે (f = s, t = prev = prev2 = ans; ; f++) {
                અક્ષર *tmp_t = t;

                જો (!*f) {
                        જો (len_forced_ellipsis != 0)
                                તોડો;

                        *t = 0;
                        પાછા ans;
                }

                જો ((unsigned અક્ષર) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned અક્ષર) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        જો ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                તોડો;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } નહીંતર {
                        જો ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                તોડો;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        જો (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        નહીંતર જો (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        નહીંતર જો (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        નહીંતર
                off = len_body - c;
        પ્રતિજ્ઞા(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        પાછા ans;
}

અક્ષર* escape_non_printable_full(અચલ અક્ષર *str, size_t console_width, XEscapeFlags flags) {
        જો (FLAGS_SET(flags, XESCAPE_8_BIT))
                પાછા xescape_full(str, /* bad= */ ખાલી, console_width, flags);
        નહીંતર
                પાછા utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

અક્ષર* octescape_full(અચલ અક્ષર *s, size_t len, અચલ અક્ષર *bad) {
        અક્ષર *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        પ્રતિજ્ઞા(s || len == 0);

        જો (len == SIZE_MAX)
                len = strlen(s);

        જો (len > (SIZE_MAX - 1) / 4)
                પાછા ખાલી;

        t = buf = new(અક્ષર, len * 4 + 1);
        જો (!buf)
                પાછા ખાલી;

        માટે (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                જો (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } નહીંતર
                        *(t++) = u;
        }

        *t = 0;
        પાછા buf;
}

અક્ષર* decescape(અચલ અક્ષર *s, size_t len, અચલ અક્ષર *bad) {
        અક્ષર *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        પ્રતિજ્ઞા(s || len == 0);

        જો (len == SIZE_MAX)
                len = strlen(s);

        જો (len > (SIZE_MAX - 1) / 4)
                પાછા ખાલી;

        t = buf = new(અક્ષર, len * 4 + 1);
        જો (!buf)
                પાછા ખાલી;

        માટે (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                જો (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } નહીંતર
                        *(t++) = u;
        }

        *t = 0;
        પાછા buf;
}

સ્થિર અક્ષર* strcpy_backslash_escaped(અક્ષર *t, અચલ અક્ષર *s, અચલ અક્ષર *bad) {
        પ્રતિજ્ઞા(bad);
        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(s);

        જ્યારે (*s) {
                પૂર્ણાંક l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                જો (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                નહીંતર જો (l == 1) {
                        જો (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } નહીંતર {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        પાછા t;
}

અક્ષર* shell_escape(અચલ અક્ષર *s, અચલ અક્ષર *bad) {
        અક્ષર *buf, *t;

        buf = new(અક્ષર, strlen(s)*4+1);
        જો (!buf)
                પાછા ખાલી;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        પાછા buf;
}

અક્ષર* shell_maybe_quote(અચલ અક્ષર *s, ShellEscapeFlags flags) {
        અચલ અક્ષર *p;
        અક્ષર *buf, *t;

        પ્રતિજ્ઞા(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        જો (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                પાછા strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        માટે (p = s; *p; ) {
                પૂર્ણાંક l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                જો (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        તોડો;

                p += l;
        }

        જો (!*p)
                પાછા strdup(s);

        buf = new(અક્ષર, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        જો (!buf)
                પાછા ખાલી;

        t = buf;
        જો (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } નહીંતર
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        જો (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        નહીંતર
                *(t++) = '"';
        *t = 0;

        પાછા str_realloc(buf);
}

અક્ષર* quote_command_લીટી(અક્ષર * અચલ *argv, ShellEscapeFlags flags) {
        _cleanup_free_ અક્ષર *result = ખાલી;

        પ્રતિજ્ઞા(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ અક્ષર *t = ખાલી;

                t = shell_maybe_quote(*a, flags);
                જો (!t)
                        પાછા ખાલી;

                જો (!strextend_with_separator(&result, " ", t))
                        પાછા ખાલી;
        }

        પાછા str_realloc(TAKE_PTR(result));
}
