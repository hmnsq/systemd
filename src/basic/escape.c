/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "utf8.h"

পূর্ণাঙ্ক cescape_char(ৱর্ণ c, ৱর্ণ *buf) {
        ৱর্ণ *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        ৱিকল্প (c) {

                পক্ষ '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        ভঙ্গ;
                পক্ষ '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        ভঙ্গ;
                পক্ষ '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        ভঙ্গ;
                পক্ষ '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        ভঙ্গ;
                পক্ষ '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        ভঙ্গ;
                পক্ষ '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        ভঙ্গ;
                পক্ষ '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        ভঙ্গ;
                পক্ষ '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        ভঙ্গ;
                পক্ষ '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        ভঙ্গ;
                পক্ষ '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        ভঙ্গ;

                স্ৱভাৱ:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        যদি ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned ৱর্ণ) c >> 6);
                                *(buf++) = octchar((unsigned ৱর্ণ) c >> 3);
                                *(buf++) = octchar((unsigned ৱর্ণ) c);
                        } অন্যথা
                                *(buf++) = c;
                        ভঙ্গ;
        }

        ঘূৰাই buf - buf_old;
}

ৱর্ণ* cescape_length(ধ্রুৱ ৱর্ণ *s, size_t n) {
        ধ্রুৱ ৱর্ণ *f;
        ৱর্ণ *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        প্রতিজ্ঞা(s || n == 0);

        যদি (n == SIZE_MAX)
                n = strlen(s);

        যদি (n > (SIZE_MAX - 1) / 4)
                ঘূৰাই রিক্ত;

        r = new(ৱর্ণ, n*4 + 1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        বাবে (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        ঘূৰাই r;
}

পূর্ণাঙ্ক cunescape_one(ধ্রুৱ ৱর্ণ *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        পূর্ণাঙ্ক r = 1;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        যদি (length != SIZE_MAX && length < 1)
                ঘূৰাই -EINVAL;

        ৱিকল্প (p[0]) {

        পক্ষ 'a':
                *ret = '\a';
                ভঙ্গ;
        পক্ষ 'b':
                *ret = '\b';
                ভঙ্গ;
        পক্ষ 'f':
                *ret = '\f';
                ভঙ্গ;
        পক্ষ 'n':
                *ret = '\n';
                ভঙ্গ;
        পক্ষ 'r':
                *ret = '\r';
                ভঙ্গ;
        পক্ষ 't':
                *ret = '\t';
                ভঙ্গ;
        পক্ষ 'v':
                *ret = '\v';
                ভঙ্গ;
        পক্ষ '\\':
                *ret = '\\';
                ভঙ্গ;
        পক্ষ '"':
                *ret = '"';
                ভঙ্গ;
        পক্ষ '\'':
                *ret = '\'';
                ভঙ্গ;

        পক্ষ 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                ভঙ্গ;

        পক্ষ 'x': {
                /* hexadecimal encoding */
                পূর্ণাঙ্ক a, b;

                যদি (length != SIZE_MAX && length < 3)
                        ঘূৰাই -EINVAL;

                a = unhexchar(p[1]);
                যদি (a < 0)
                        ঘূৰাই -EINVAL;

                b = unhexchar(p[2]);
                যদি (b < 0)
                        ঘূৰাই -EINVAL;

                /* Don't allow NUL bytes */
                যদি (a == 0 && b == 0 && !accept_nul)
                        ঘূৰাই -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                ভঙ্গ;
        }

        পক্ষ 'u': {
                /* C++11 style 16-bit unicode */

                পূর্ণাঙ্ক a[4];
                size_t i;
                uint32_t c;

                যদি (length != SIZE_MAX && length < 5)
                        ঘূৰাই -EINVAL;

                বাবে (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        যদি (a[i] < 0)
                                ঘূৰাই a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                যদি (c == 0 && !accept_nul)
                        ঘূৰাই -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                যদি (utf16_is_surrogate(c))
                        ঘূৰাই -EINVAL;

                *ret = c;
                r = 5;
                ভঙ্গ;
        }

        পক্ষ 'U': {
                /* C++11 style 32-bit unicode */

                পূর্ণাঙ্ক a[8];
                size_t i;
                char32_t c;

                যদি (length != SIZE_MAX && length < 9)
                        ঘূৰাই -EINVAL;

                বাবে (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        যদি (a[i] < 0)
                                ঘূৰাই a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                যদি (c == 0 && !accept_nul)
                        ঘূৰাই -EINVAL;

                /* Don't allow invalid code points */
                যদি (!unichar_is_valid(c))
                        ঘূৰাই -EINVAL;

                *ret = c;
                r = 9;
                ভঙ্গ;
        }

        পক্ষ '0':
        পক্ষ '1':
        পক্ষ '2':
        পক্ষ '3':
        পক্ষ '4':
        পক্ষ '5':
        পক্ষ '6':
        পক্ষ '7': {
                /* octal encoding */
                পূর্ণাঙ্ক a, b, c;
                char32_t m;

                যদি (length != SIZE_MAX && length < 3)
                        ঘূৰাই -EINVAL;

                a = unoctchar(p[0]);
                যদি (a < 0)
                        ঘূৰাই -EINVAL;

                b = unoctchar(p[1]);
                যদি (b < 0)
                        ঘূৰাই -EINVAL;

                c = unoctchar(p[2]);
                যদি (c < 0)
                        ঘূৰাই -EINVAL;

                /* don't allow NUL bytes */
                যদি (a == 0 && b == 0 && c == 0 && !accept_nul)
                        ঘূৰাই -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                যদি (m > 255)
                        ঘূৰাই -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                ভঙ্গ;
        }

        স্ৱভাৱ:
                ঘূৰাই -EINVAL;
        }

        ঘূৰাই r;
}

ssize_t cunescape_length_with_prefix(ধ্রুৱ ৱর্ণ *s, size_t length, ধ্রুৱ ৱর্ণ *prefix, UnescapeFlags flags, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *ans = রিক্ত;
        ৱর্ণ *t;
        ধ্রুৱ ৱর্ণ *f;
        size_t pl;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        যদি (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(ৱর্ণ, pl+length+1);
        যদি (!ans)
                ঘূৰাই -ENOMEM;

        যদি (prefix)
                memcpy(ans, prefix, pl);

        বাবে (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                প্রতিজ্ঞা(remaining > 0);

                যদি (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        অনুৱর্ত;
                }

                যদি (remaining == 1) {
                        যদি (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                অনুৱর্ত;
                        }

                        ঘূৰাই -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                যদি (r < 0) {
                        যদি (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                অনুৱর্ত;
                        }

                        ঘূৰাই r;
                }

                f += r;
                যদি (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                অন্যথা
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        প্রতিজ্ঞা(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        ঘূৰাই t - *ret;
}

ৱর্ণ* xescape_full(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad, size_t console_width, XEscapeFlags flags) {
        ৱর্ণ *ans, *t, *prev, *prev2;
        ধ্রুৱ ৱর্ণ *f;

        প্রতিজ্ঞা(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        যদি (console_width == 0)
                ঘূৰাই strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        যদি (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(ৱর্ণ, len_body + 1);
        যদি (!ans)
                ঘূৰাই রিক্ত;

        বাবে (f = s, t = prev = prev2 = ans; ; f++) {
                ৱর্ণ *tmp_t = t;

                যদি (!*f) {
                        যদি (len_forced_ellipsis != 0)
                                ভঙ্গ;

                        *t = 0;
                        ঘূৰাই ans;
                }

                যদি ((unsigned ৱর্ণ) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned ৱর্ণ) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        যদি ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                ভঙ্গ;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } অন্যথা {
                        যদি ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                ভঙ্গ;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        যদি (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        অন্যথা যদি (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        অন্যথা যদি (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        অন্যথা
                off = len_body - c;
        প্রতিজ্ঞা(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        ঘূৰাই ans;
}

ৱর্ণ* escape_non_printable_full(ধ্রুৱ ৱর্ণ *str, size_t console_width, XEscapeFlags flags) {
        যদি (FLAGS_SET(flags, XESCAPE_8_BIT))
                ঘূৰাই xescape_full(str, /* bad= */ রিক্ত, console_width, flags);
        অন্যথা
                ঘূৰাই utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

ৱর্ণ* octescape_full(ধ্রুৱ ৱর্ণ *s, size_t len, ধ্রুৱ ৱর্ণ *bad) {
        ৱর্ণ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        প্রতিজ্ঞা(s || len == 0);

        যদি (len == SIZE_MAX)
                len = strlen(s);

        যদি (len > (SIZE_MAX - 1) / 4)
                ঘূৰাই রিক্ত;

        t = buf = new(ৱর্ণ, len * 4 + 1);
        যদি (!buf)
                ঘূৰাই রিক্ত;

        বাবে (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                যদি (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } অন্যথা
                        *(t++) = u;
        }

        *t = 0;
        ঘূৰাই buf;
}

ৱর্ণ* decescape(ধ্রুৱ ৱর্ণ *s, size_t len, ধ্রুৱ ৱর্ণ *bad) {
        ৱর্ণ *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        প্রতিজ্ঞা(s || len == 0);

        যদি (len == SIZE_MAX)
                len = strlen(s);

        যদি (len > (SIZE_MAX - 1) / 4)
                ঘূৰাই রিক্ত;

        t = buf = new(ৱর্ণ, len * 4 + 1);
        যদি (!buf)
                ঘূৰাই রিক্ত;

        বাবে (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                যদি (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } অন্যথা
                        *(t++) = u;
        }

        *t = 0;
        ঘূৰাই buf;
}

স্থির ৱর্ণ* strcpy_backslash_escaped(ৱর্ণ *t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad) {
        প্রতিজ্ঞা(bad);
        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        যেতিয়ালৈ (*s) {
                পূর্ণাঙ্ক l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                যদি (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                অন্যথা যদি (l == 1) {
                        যদি (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } অন্যথা {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        ঘূৰাই t;
}

ৱর্ণ* shell_escape(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad) {
        ৱর্ণ *buf, *t;

        buf = new(ৱর্ণ, strlen(s)*4+1);
        যদি (!buf)
                ঘূৰাই রিক্ত;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        ঘূৰাই buf;
}

ৱর্ণ* shell_maybe_quote(ধ্রুৱ ৱর্ণ *s, ShellEscapeFlags flags) {
        ধ্রুৱ ৱর্ণ *p;
        ৱর্ণ *buf, *t;

        প্রতিজ্ঞা(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        যদি (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                ঘূৰাই strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        বাবে (p = s; *p; ) {
                পূর্ণাঙ্ক l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                যদি (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        ভঙ্গ;

                p += l;
        }

        যদি (!*p)
                ঘূৰাই strdup(s);

        buf = new(ৱর্ণ, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        যদি (!buf)
                ঘূৰাই রিক্ত;

        t = buf;
        যদি (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } অন্যথা
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        যদি (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        অন্যথা
                *(t++) = '"';
        *t = 0;

        ঘূৰাই str_realloc(buf);
}

ৱর্ণ* quote_command_সাৰি(ৱর্ণ * ধ্রুৱ *argv, ShellEscapeFlags flags) {
        _cleanup_free_ ৱর্ণ *result = রিক্ত;

        প্রতিজ্ঞা(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ ৱর্ণ *t = রিক্ত;

                t = shell_maybe_quote(*a, flags);
                যদি (!t)
                        ঘূৰাই রিক্ত;

                যদি (!strextend_with_separator(&result, " ", t))
                        ঘূৰাই রিক্ত;
        }

        ঘূৰাই str_realloc(TAKE_PTR(result));
}
