/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "utf8.h"

முழு cescape_char(எழுத்து c, எழுத்து *buf) {
        எழுத்து *buf_old = buf;

        /* Needs space for 4 characters in the buffer */

        தேர்வு (c) {

                நிலை '\a':
                        *(buf++) = '\\';
                        *(buf++) = 'a';
                        நிறுத்து;
                நிலை '\b':
                        *(buf++) = '\\';
                        *(buf++) = 'b';
                        நிறுத்து;
                நிலை '\f':
                        *(buf++) = '\\';
                        *(buf++) = 'f';
                        நிறுத்து;
                நிலை '\n':
                        *(buf++) = '\\';
                        *(buf++) = 'n';
                        நிறுத்து;
                நிலை '\r':
                        *(buf++) = '\\';
                        *(buf++) = 'r';
                        நிறுத்து;
                நிலை '\t':
                        *(buf++) = '\\';
                        *(buf++) = 't';
                        நிறுத்து;
                நிலை '\v':
                        *(buf++) = '\\';
                        *(buf++) = 'v';
                        நிறுத்து;
                நிலை '\\':
                        *(buf++) = '\\';
                        *(buf++) = '\\';
                        நிறுத்து;
                நிலை '"':
                        *(buf++) = '\\';
                        *(buf++) = '"';
                        நிறுத்து;
                நிலை '\'':
                        *(buf++) = '\\';
                        *(buf++) = '\'';
                        நிறுத்து;

                இயல்பு:
                        /* For special chars we prefer octal over
                         * hexadecimal encoding, simply because glib's
                         * g_strescape() does the same */
                        எனில் ((c < ' ') || (c >= 127)) {
                                *(buf++) = '\\';
                                *(buf++) = octchar((unsigned எழுத்து) c >> 6);
                                *(buf++) = octchar((unsigned எழுத்து) c >> 3);
                                *(buf++) = octchar((unsigned எழுத்து) c);
                        } இல்லையெனில்
                                *(buf++) = c;
                        நிறுத்து;
        }

        திரும்பு buf - buf_old;
}

எழுத்து* cescape_length(மாறிலி எழுத்து *s, size_t n) {
        மாறிலி எழுத்து *f;
        எழுத்து *r, *t;

        /* Does C style string escaping. May be reversed with cunescape(). */

        ப்ரதிஜ்ஞா(s || n == 0);

        எனில் (n == SIZE_MAX)
                n = strlen(s);

        எனில் (n > (SIZE_MAX - 1) / 4)
                திரும்பு வெற்று;

        r = new(எழுத்து, n*4 + 1);
        எனில் (!r)
                திரும்பு வெற்று;

        க்கு (f = s, t = r; f < s + n; f++)
                t += cescape_char(*f, t);

        *t = 0;

        திரும்பு r;
}

முழு cunescape_one(மாறிலி எழுத்து *p, size_t length, char32_t *ret, bool *eight_bit, bool accept_nul) {
        முழு r = 1;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(eight_bit);

        /* Unescapes C style. Returns the unescaped character in ret.
         * Sets *eight_bit to true if the escaped sequence either fits in
         * one byte in UTF-8 or is a non-unicode literal byte and should
         * instead be copied directly.
         */

        எனில் (length != SIZE_MAX && length < 1)
                திரும்பு -EINVAL;

        தேர்வு (p[0]) {

        நிலை 'a':
                *ret = '\a';
                நிறுத்து;
        நிலை 'b':
                *ret = '\b';
                நிறுத்து;
        நிலை 'f':
                *ret = '\f';
                நிறுத்து;
        நிலை 'n':
                *ret = '\n';
                நிறுத்து;
        நிலை 'r':
                *ret = '\r';
                நிறுத்து;
        நிலை 't':
                *ret = '\t';
                நிறுத்து;
        நிலை 'v':
                *ret = '\v';
                நிறுத்து;
        நிலை '\\':
                *ret = '\\';
                நிறுத்து;
        நிலை '"':
                *ret = '"';
                நிறுத்து;
        நிலை '\'':
                *ret = '\'';
                நிறுத்து;

        நிலை 's':
                /* This is an extension of the XDG syntax files */
                *ret = ' ';
                நிறுத்து;

        நிலை 'x': {
                /* hexadecimal encoding */
                முழு a, b;

                எனில் (length != SIZE_MAX && length < 3)
                        திரும்பு -EINVAL;

                a = unhexchar(p[1]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unhexchar(p[2]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                /* Don't allow NUL bytes */
                எனில் (a == 0 && b == 0 && !accept_nul)
                        திரும்பு -EINVAL;

                *ret = (a << 4U) | b;
                *eight_bit = true;
                r = 3;
                நிறுத்து;
        }

        நிலை 'u': {
                /* C++11 style 16-bit unicode */

                முழு a[4];
                size_t i;
                uint32_t c;

                எனில் (length != SIZE_MAX && length < 5)
                        திரும்பு -EINVAL;

                க்கு (i = 0; i < 4; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        எனில் (a[i] < 0)
                                திரும்பு a[i];
                }

                c = ((uint32_t) a[0] << 12U) | ((uint32_t) a[1] << 8U) | ((uint32_t) a[2] << 4U) | (uint32_t) a[3];

                /* Don't allow 0 chars */
                எனில் (c == 0 && !accept_nul)
                        திரும்பு -EINVAL;

                /* Don't allow UTF-16 surrogates, they cannot be encoded as valid UTF-8. Note we
                 * deliberately do *not* use unichar_is_valid() here (unlike the \U case below):
                 * it also rejects noncharacters such as U+FFFE, which callers legitimately round-trip
                 * through \u (e.g. systemd.mount-extra= parsing, see test-fstab-generator). */
                எனில் (utf16_is_surrogate(c))
                        திரும்பு -EINVAL;

                *ret = c;
                r = 5;
                நிறுத்து;
        }

        நிலை 'U': {
                /* C++11 style 32-bit unicode */

                முழு a[8];
                size_t i;
                char32_t c;

                எனில் (length != SIZE_MAX && length < 9)
                        திரும்பு -EINVAL;

                க்கு (i = 0; i < 8; i++) {
                        a[i] = unhexchar(p[1 + i]);
                        எனில் (a[i] < 0)
                                திரும்பு a[i];
                }

                c = ((uint32_t) a[0] << 28U) | ((uint32_t) a[1] << 24U) | ((uint32_t) a[2] << 20U) | ((uint32_t) a[3] << 16U) |
                    ((uint32_t) a[4] << 12U) | ((uint32_t) a[5] <<  8U) | ((uint32_t) a[6] <<  4U) |  (uint32_t) a[7];

                /* Don't allow 0 chars */
                எனில் (c == 0 && !accept_nul)
                        திரும்பு -EINVAL;

                /* Don't allow invalid code points */
                எனில் (!unichar_is_valid(c))
                        திரும்பு -EINVAL;

                *ret = c;
                r = 9;
                நிறுத்து;
        }

        நிலை '0':
        நிலை '1':
        நிலை '2':
        நிலை '3':
        நிலை '4':
        நிலை '5':
        நிலை '6':
        நிலை '7': {
                /* octal encoding */
                முழு a, b, c;
                char32_t m;

                எனில் (length != SIZE_MAX && length < 3)
                        திரும்பு -EINVAL;

                a = unoctchar(p[0]);
                எனில் (a < 0)
                        திரும்பு -EINVAL;

                b = unoctchar(p[1]);
                எனில் (b < 0)
                        திரும்பு -EINVAL;

                c = unoctchar(p[2]);
                எனில் (c < 0)
                        திரும்பு -EINVAL;

                /* don't allow NUL bytes */
                எனில் (a == 0 && b == 0 && c == 0 && !accept_nul)
                        திரும்பு -EINVAL;

                /* Don't allow bytes above 255 */
                m = ((uint32_t) a << 6U) | ((uint32_t) b << 3U) | (uint32_t) c;
                எனில் (m > 255)
                        திரும்பு -EINVAL;

                *ret = m;
                *eight_bit = true;
                r = 3;
                நிறுத்து;
        }

        இயல்பு:
                திரும்பு -EINVAL;
        }

        திரும்பு r;
}

ssize_t cunescape_length_with_prefix(மாறிலி எழுத்து *s, size_t length, மாறிலி எழுத்து *prefix, UnescapeFlags flags, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *ans = வெற்று;
        எழுத்து *t;
        மாறிலி எழுத்து *f;
        size_t pl;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        /* Undoes C style string escaping, and optionally prefixes it. */

        எனில் (length == SIZE_MAX)
                length = strlen(s);

        pl = strlen_ptr(prefix);

        ans = new(எழுத்து, pl+length+1);
        எனில் (!ans)
                திரும்பு -ENOMEM;

        எனில் (prefix)
                memcpy(ans, prefix, pl);

        க்கு (f = s, t = ans + pl; f < s + length; f++) {
                size_t remaining;
                bool eight_bit = false;
                char32_t u;

                remaining = s + length - f;
                ப்ரதிஜ்ஞா(remaining > 0);

                எனில் (*f != '\\') {
                        /* A literal, copy verbatim */
                        *(t++) = *f;
                        தொடர்;
                }

                எனில் (remaining == 1) {
                        எனில் (flags & UNESCAPE_RELAX) {
                                /* A trailing backslash, copy verbatim */
                                *(t++) = *f;
                                தொடர்;
                        }

                        திரும்பு -EINVAL;
                }

                r = cunescape_one(f + 1, remaining - 1, &u, &eight_bit, flags & UNESCAPE_ACCEPT_NUL);
                எனில் (r < 0) {
                        எனில் (flags & UNESCAPE_RELAX) {
                                /* Invalid escape code, let's take it literal then */
                                *(t++) = '\\';
                                தொடர்;
                        }

                        திரும்பு r;
                }

                f += r;
                எனில் (eight_bit)
                        /* One byte? Set directly as specified */
                        *(t++) = u;
                இல்லையெனில்
                        /* Otherwise encode as multi-byte UTF-8 */
                        t += utf8_encode_unichar(t, u);
        }

        *t = 0;

        ப்ரதிஜ்ஞா(t >= ans); /* Let static analyzers know that the answer is non-negative. */
        *ret = TAKE_PTR(ans);
        திரும்பு t - *ret;
}

எழுத்து* xescape_full(மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad, size_t console_width, XEscapeFlags flags) {
        எழுத்து *ans, *t, *prev, *prev2;
        மாறிலி எழுத்து *f;

        ப்ரதிஜ்ஞா(s);

        /* Escapes all chars in bad, in addition to \ and all special chars, in \xFF style escaping. May be
         * reversed with cunescape(). If XESCAPE_8_BIT is specified, characters >= 127 are let through
         * unchanged. This corresponds to non-ASCII printable characters in pre-unicode encodings.
         *
         * If console_width is reached, or XESCAPE_FORCE_ELLIPSIS is set, output is truncated and "..." is
         * appended. */

        எனில் (console_width == 0)
                திரும்பு strdup("");

        size_t len_forced_ellipsis = FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS) ? STRLEN("...") : 0;
        size_t len_s = strlen(s);

        size_t len_body = MIN(console_width, SIZE_MAX - 1); /* We need room for the NUL byte */
        எனில் (len_body > len_forced_ellipsis && len_s <= (len_body - len_forced_ellipsis) / 4)
                len_body = len_s * 4 + len_forced_ellipsis;

        ans = new(எழுத்து, len_body + 1);
        எனில் (!ans)
                திரும்பு வெற்று;

        க்கு (f = s, t = prev = prev2 = ans; ; f++) {
                எழுத்து *tmp_t = t;

                எனில் (!*f) {
                        எனில் (len_forced_ellipsis != 0)
                                நிறுத்து;

                        *t = 0;
                        திரும்பு ans;
                }

                எனில் ((unsigned எழுத்து) *f < ' ' ||
                    (!FLAGS_SET(flags, XESCAPE_8_BIT) && (unsigned எழுத்து) *f >= 127) ||
                    *f == '\\' || (bad && strchr(bad, *f))) {
                        எனில் ((size_t) (t - ans) + 4 + len_forced_ellipsis > len_body)
                                நிறுத்து;

                        *(t++) = '\\';
                        *(t++) = 'x';
                        *(t++) = hexchar(*f >> 4);
                        *(t++) = hexchar(*f);
                } இல்லையெனில் {
                        எனில் ((size_t) (t - ans) + 1 + len_forced_ellipsis > len_body)
                                நிறுத்து;

                        *(t++) = *f;
                }

                /* We might need to go back two cycles to fit three dots, so remember two positions */
                prev2 = prev;
                prev = tmp_t;
        }

        /* We can just write where we want, since chars are one-byte */
        size_t c = MIN(len_body, STRLEN("...")); /* If the console is too narrow, write fewer dots */
        size_t off;
        எனில் (len_body - c >= (size_t) (t - ans))
                off = (size_t) (t - ans);
        இல்லையெனில் எனில் (len_body - c >= (size_t) (prev - ans))
                off = (size_t) (prev - ans);
        இல்லையெனில் எனில் (len_body - c >= (size_t) (prev2 - ans))
                off = (size_t) (prev2 - ans);
        இல்லையெனில்
                off = len_body - c;
        ப்ரதிஜ்ஞா(off <= (size_t) (t - ans));

        memcpy(ans + off, "...", c);
        ans[off + c] = '\0';
        திரும்பு ans;
}

எழுத்து* escape_non_printable_full(மாறிலி எழுத்து *str, size_t console_width, XEscapeFlags flags) {
        எனில் (FLAGS_SET(flags, XESCAPE_8_BIT))
                திரும்பு xescape_full(str, /* bad= */ வெற்று, console_width, flags);
        இல்லையெனில்
                திரும்பு utf8_escape_non_printable_full(str,
                                                      console_width,
                                                      FLAGS_SET(flags, XESCAPE_FORCE_ELLIPSIS));
}

எழுத்து* octescape_full(மாறிலி எழுத்து *s, size_t len, மாறிலி எழுத்து *bad) {
        எழுத்து *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn octal style escaping. May be
         * reversed with cunescape(). */

        ப்ரதிஜ்ஞா(s || len == 0);

        எனில் (len == SIZE_MAX)
                len = strlen(s);

        எனில் (len > (SIZE_MAX - 1) / 4)
                திரும்பு வெற்று;

        t = buf = new(எழுத்து, len * 4 + 1);
        எனில் (!buf)
                திரும்பு வெற்று;

        க்கு (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                எனில் (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || (bad && strchr(bad, u))) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u >> 6);
                        *(t++) = '0' + ((u >> 3) & 7);
                        *(t++) = '0' + (u & 7);
                } இல்லையெனில்
                        *(t++) = u;
        }

        *t = 0;
        திரும்பு buf;
}

எழுத்து* decescape(மாறிலி எழுத்து *s, size_t len, மாறிலி எழுத்து *bad) {
        எழுத்து *buf, *t;

        /* Escapes all chars in bad, in addition to \ and " chars, in \nnn decimal style escaping. */

        ப்ரதிஜ்ஞா(s || len == 0);

        எனில் (len == SIZE_MAX)
                len = strlen(s);

        எனில் (len > (SIZE_MAX - 1) / 4)
                திரும்பு வெற்று;

        t = buf = new(எழுத்து, len * 4 + 1);
        எனில் (!buf)
                திரும்பு வெற்று;

        க்கு (size_t i = 0; i < len; i++) {
                uint8_t u = (uint8_t) s[i];

                எனில் (u < ' ' || u >= 127 || IN_SET(u, '\\', '"') || strchr(bad, u)) {
                        *(t++) = '\\';
                        *(t++) = '0' + (u / 100);
                        *(t++) = '0' + ((u / 10) % 10);
                        *(t++) = '0' + (u % 10);
                } இல்லையெனில்
                        *(t++) = u;
        }

        *t = 0;
        திரும்பு buf;
}

நிலையான எழுத்து* strcpy_backslash_escaped(எழுத்து *t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad) {
        ப்ரதிஜ்ஞா(bad);
        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(s);

        போது (*s) {
                முழு l = utf8_encoded_valid_unichar(s, SIZE_MAX);

                எனில் (char_is_cc(*s) || l < 0)
                        t += cescape_char(*(s++), t);
                இல்லையெனில் எனில் (l == 1) {
                        எனில் (*s == '\\' || strchr(bad, *s))
                                *(t++) = '\\';
                        *(t++) = *(s++);
                } இல்லையெனில் {
                        t = mempcpy(t, s, l);
                        s += l;
                }
        }

        திரும்பு t;
}

எழுத்து* shell_escape(மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad) {
        எழுத்து *buf, *t;

        buf = new(எழுத்து, strlen(s)*4+1);
        எனில் (!buf)
                திரும்பு வெற்று;

        t = strcpy_backslash_escaped(buf, s, bad);
        *t = 0;

        திரும்பு buf;
}

எழுத்து* shell_maybe_quote(மாறிலி எழுத்து *s, ShellEscapeFlags flags) {
        மாறிலி எழுத்து *p;
        எழுத்து *buf, *t;

        ப்ரதிஜ்ஞா(s);

        /* Encloses a string in quotes if necessary to make it OK as a shell string. */

        எனில் (FLAGS_SET(flags, SHELL_ESCAPE_EMPTY) && isempty(s))
                திரும்பு strdup("\"\""); /* We don't use $'' here in the POSIX mode. "" is fine too. */

        க்கு (p = s; *p; ) {
                முழு l = utf8_encoded_valid_unichar(p, SIZE_MAX);

                எனில் (char_is_cc(*p) || l < 0 ||
                    strchr(WHITESPACE SHELL_NEED_QUOTES, *p))
                        நிறுத்து;

                p += l;
        }

        எனில் (!*p)
                திரும்பு strdup(s);

        buf = new(எழுத்து, FLAGS_SET(flags, SHELL_ESCAPE_POSIX) + 1 + strlen(s)*4 + 1 + 1);
        எனில் (!buf)
                திரும்பு வெற்று;

        t = buf;
        எனில் (FLAGS_SET(flags, SHELL_ESCAPE_POSIX)) {
                *(t++) = '$';
                *(t++) = '\'';
        } இல்லையெனில்
                *(t++) = '"';

        t = mempcpy(t, s, p - s);

        t = strcpy_backslash_escaped(t, p,
                                     FLAGS_SET(flags, SHELL_ESCAPE_POSIX) ? SHELL_NEED_ESCAPE_POSIX : SHELL_NEED_ESCAPE);

        எனில் (FLAGS_SET(flags, SHELL_ESCAPE_POSIX))
                *(t++) = '\'';
        இல்லையெனில்
                *(t++) = '"';
        *t = 0;

        திரும்பு str_realloc(buf);
}

எழுத்து* quote_command_வரி(எழுத்து * மாறிலி *argv, ShellEscapeFlags flags) {
        _cleanup_free_ எழுத்து *result = வெற்று;

        ப்ரதிஜ்ஞா(argv);

        STRV_FOREACH(a, argv) {
                _cleanup_free_ எழுத்து *t = வெற்று;

                t = shell_maybe_quote(*a, flags);
                எனில் (!t)
                        திரும்பு வெற்று;

                எனில் (!strextend_with_separator(&result, " ", t))
                        திரும்பு வெற்று;
        }

        திரும்பு str_realloc(TAKE_PTR(result));
}
