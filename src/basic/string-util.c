/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ansi-seq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glyph-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gunicode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locale-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memstream-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਅੱਖਰ* ਪਹਿਲਾ_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ਬਦ);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ਜੇ (isempty(ਸ਼ਬਦ))
                ਵਾਪਸ (ਅੱਖਰ*) s;

        ਅਚਲ ਅੱਖਰ *p = startswith(s, ਸ਼ਬਦ);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;
        ਜੇ (*p == '\0')
                ਵਾਪਸ (ਅੱਖਰ*) p;

        ਅਚਲ ਅੱਖਰ *nw = skip_leading_chars(p, WHITESPACE);
        ਜੇ (p == nw)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ (ਅੱਖਰ*) nw;
}

ਅੱਖਰ* strextendn(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *s, size_t l) {
        ਪ੍ਰਤਿਜ੍ਞਾ(x);
        ਪ੍ਰਤਿਜ੍ਞਾ(s || l == 0);

        ਜੇ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ਜੇ (l > 0 || !*x) {
                size_t q;
                ਅੱਖਰ *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ਜੇ (!m)
                        ਵਾਪਸ ਨਿਰਾ;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ਵਾਪਸ *x;
}

ਅੱਖਰ* strstrip(ਅੱਖਰ *s) {
        ਜੇ (!s)
                ਵਾਪਸ ਨਿਰਾ;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ਵਾਪਸ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

ਅੱਖਰ* delete_chars(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad) {
        ਅੱਖਰ *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ਜੇ (!s)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (!bad)
                bad = WHITESPACE;

        ਲਈ (f = s, t = s; *f; f++) {
                ਜੇ (strchr(bad, *f))
                        ਜਾਰੀ;

                *(t++) = *f;
        }

        *t = 0;

        ਵਾਪਸ s;
}

ਅੱਖਰ* delete_trailing_chars(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad) {
        ਅੱਖਰ *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ਜੇ (!s)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (!bad)
                bad = WHITESPACE;

        ਲਈ (ਅੱਖਰ *p = s; *p; p++)
                ਜੇ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ਵਾਪਸ s;
}

ਅੱਖਰ* truncate_nl_ਭਰਿਆ(ਅੱਖਰ *s, size_t *ret_len) {
        size_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ਜੇ (ret_len)
                *ret_len = n;
        ਵਾਪਸ s;
}

ਅੱਖਰ ascii_tolower(ਅੱਖਰ x) {

        ਜੇ (x >= 'A' && x <= 'Z')
                ਵਾਪਸ x - 'A' + 'a';

        ਵਾਪਸ x;
}

ਅੱਖਰ ascii_toupper(ਅੱਖਰ x) {

        ਜੇ (x >= 'a' && x <= 'z')
                ਵਾਪਸ x - 'a' + 'A';

        ਵਾਪਸ x;
}

ਅੱਖਰ* ascii_strlower(ਅੱਖਰ *s) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (ਅੱਖਰ *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ਵਾਪਸ s;
}

ਅੱਖਰ* ascii_strupper(ਅੱਖਰ *s) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (ਅੱਖਰ *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ਵਾਪਸ s;
}

ਅੱਖਰ* ascii_strlower_n(ਅੱਖਰ *s, size_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(n <= 0 || s);

        ਜੇ (n <= 0)
                ਵਾਪਸ s;

        ਲਈ (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ਵਾਪਸ s;
}

ਪੂਰਨ ascii_strcasecmp_n(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b, size_t n) {

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਲਈ (; n > 0; a++, b++, n--) {
                ਪੂਰਨ x, y;

                x = (ਪੂਰਨ) (uint8_t) ascii_tolower(*a);
                y = (ਪੂਰਨ) (uint8_t) ascii_tolower(*b);

                ਜੇ (x != y)
                        ਵਾਪਸ x - y;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ ascii_strcasecmp_nn(ਅਚਲ ਅੱਖਰ *a, size_t n, ਅਚਲ ਅੱਖਰ *b, size_t m) {
        ਪੂਰਨ r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਵਾਪਸ CMP(n, m);
}

bool chars_intersect(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        /* Returns true if any of the chars in a are in b. */
        ਲਈ (ਅਚਲ ਅੱਖਰ *p = a; *p; p++)
                ਜੇ (strchr(b, *p))
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

bool string_has_cc(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ok) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        ਲਈ (ਅਚਲ ਅੱਖਰ *t = p; *t; t++) {
                ਜੇ (ok && strchr(ok, *t))
                        ਜਾਰੀ;

                ਜੇ (char_is_cc(*t))
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ write_ellipsis(ਅੱਖਰ *buf, bool unicode) {
        ਅਚਲ ਅੱਖਰ *s = glyph_ਭਰਿਆ(GLYPH_ELLIPSIS, unicode);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਲੜੀ_ਲੰਬਾਈ(s) == 3);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, s, 3);
        ਵਾਪਸ 3;
}

ਸਥਿਰ size_t ansi_sequence_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *s, size_t len) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (len < 2)
                ਵਾਪਸ 0;

        ਜੇ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ਵਾਪਸ 0;  /* Not the start of a sequence */

        ਜੇ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ਜੇ (i == len)
                        ਵਾਪਸ 0;

                ਜਦੋਂ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ਜੇ (++i == len)
                                ਵਾਪਸ 0;
                ਜਦੋਂ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ਜੇ (++i == len)
                                ਵਾਪਸ 0;
                ਜੇ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ਵਾਪਸ i + 1;
                ਵਾਪਸ 0;  /* Bad sequence */

        } ਨਹੀਂਤਾਂ ਜੇ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ਵਾਪਸ 2;

        ਵਾਪਸ 0;  /* Bad escape? */
}

ਸਥਿਰ bool string_has_ansi_sequence(ਅਚਲ ਅੱਖਰ *s, size_t len) {
        ਅਚਲ ਅੱਖਰ *t = s;

        ਜਦੋਂ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ਜੇ (ansi_sequence_ਲੰਮਾਈ(t, len - (t - s)) > 0)
                        ਵਾਪਸ true;
                t++;
        }
        ਵਾਪਸ false;
}

ਸਥਿਰ size_t ਪਿਛਲਾ_ansi_sequence(ਅਚਲ ਅੱਖਰ *s, size_t length, ਅਚਲ ਅੱਖਰ **ret_where) {

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ਜੇ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਲਈ (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_ਲੰਮਾਈ(s + i, length - i);
                ਜੇ (slen > 0) {
                        *ret_where = s + i;
                        ਵਾਪਸ slen;
                }

                ਜੇ (i == 0)
                        ਰੋਕੋ;
        }

        *ret_where = ਨਿਰਾ;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅੱਖਰ *ascii_ellipsize_mem(ਅਚਲ ਅੱਖਰ *s, size_t old_ਲੰਮਾਈ, size_t new_ਲੰਮਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ percent) {
        size_t x, need_space, suffix_len;
        ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(percent <= 100);
        ਪ੍ਰਤਿਜ੍ਞਾ(new_ਲੰਮਾਈ != SIZE_MAX);

        ਜੇ (old_ਲੰਮਾਈ <= new_ਲੰਮਾਈ)
                ਵਾਪਸ strndup(s, old_ਲੰਮਾਈ);

        /* Special case short ellipsations */
        ਚੋਣ (new_ਲੰਮਾਈ) {

        ਹਾਲਤ 0:
                ਵਾਪਸ strdup("");

        ਹਾਲਤ 1:
                ਜੇ (is_locale_utf8())
                        ਵਾਪਸ strdup("…");
                ਨਹੀਂਤਾਂ
                        ਵਾਪਸ strdup(".");

        ਹਾਲਤ 2:
                ਜੇ (!is_locale_utf8())
                        ਵਾਪਸ strdup("..");
                ਰੋਕੋ;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(ਅੱਖਰ, new_ਲੰਮਾਈ+3);
        ਜੇ (!t)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(new_ਲੰਮਾਈ >= need_space);

        x = ((new_ਲੰਮਾਈ - need_space) * percent + 50) / 100;
        ਪ੍ਰਤਿਜ੍ਞਾ(x <= new_ਲੰਮਾਈ - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_ਲੰਮਾਈ - x - need_space;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(t + x + 3, s + old_ਲੰਮਾਈ - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ਵਾਪਸ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਲੱਭੋ_ਪਿਛਲਾ_unichar(ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ, ਅਚਲ ਅੱਖਰ *ਸਿਰਾ, char32_t *ret_c) {
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ੁਰੂ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਿਰਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਿਰਾ > ਸ਼ੁਰੂ);

        ਲਈ (p = ਸਿਰਾ; p > ਸ਼ੁਰੂ; ) {
                p--;
                ਜੇ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        ਰੋਕੋ;
        }

        r = utf8_encoded_valid_unichar(p, ਸਿਰਾ - p);
        ਜੇ (r < 0 || p + r != ਸਿਰਾ)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (ret_c)
                assert_se(utf8_encoded_ਮੰਜ਼ਿਲ_unichar(p, ret_c) == r);

        ਵਾਪਸ p;
}

ਅੱਖਰ* ellipsize_mem(ਅਚਲ ਅੱਖਰ *s, size_t old_ਲੰਮਾਈ, size_t new_ਲੰਮਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ percent) {
        size_t x, k, len, len2;
        ਅਚਲ ਅੱਖਰ *i, *j;
        ਪੂਰਨ r;

        /* Note that 'old_length' refers to bytes in the string, while 'new_length' refers to character cells taken up
         * on screen. This distinction doesn't matter for ASCII strings, but it does matter for non-ASCII UTF-8
         * strings.
         *
         * Ellipsation is done in a locale-dependent way:
         * 1. If the string passed in is fully ASCII and the current locale is not UTF-8, three dots are used ("...")
         * 2. Otherwise, a unicode ellipsis is used ("…")
         *
         * In other words: you'll get a unicode ellipsis as soon as either the string contains non-ASCII characters or
         * the current locale is UTF-8.
         */

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(percent <= 100);

        ਜੇ (new_ਲੰਮਾਈ == SIZE_MAX)
                ਵਾਪਸ strndup(s, old_ਲੰਮਾਈ);

        ਜੇ (new_ਲੰਮਾਈ == 0)
                ਵਾਪਸ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_ਲੰਮਾਈ);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ਜੇ (!has_ansi_seq && ascii_is_valid_n(s, old_ਲੰਮਾਈ) && !memchr(s, '\t', old_ਲੰਮਾਈ))
                ਵਾਪਸ ascii_ellipsize_mem(s, old_ਲੰਮਾਈ, new_ਲੰਮਾਈ, percent);

        x = (new_ਲੰਮਾਈ - 1) * percent / 100;
        ਪ੍ਰਤਿਜ੍ਞਾ(x <= new_ਲੰਮਾਈ - 1);

        k = 0;
        ਲਈ (i = s; i < s + old_ਲੰਮਾਈ; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_ਲੰਮਾਈ(i, old_ਲੰਮਾਈ - (i - s)) : 0;
                ਜੇ (slen > 0) {
                        i += slen;
                        ਜਾਰੀ;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_ਲੰਮਾਈ - i);
                ਜੇ (r < 0)
                        ਵਾਪਸ ਨਿਰਾ;

                char32_t c;
                assert_se(utf8_encoded_ਮੰਜ਼ਿਲ_unichar(i, &c) == r);

                ਪੂਰਨ w = unichar_console_ਚੌੜਾਈ(c);
                ਜੇ (k + w > x)
                        ਰੋਕੋ;

                k += w;
                i += r;
        }

        ਅਚਲ ਅੱਖਰ *ansi_ਸ਼ੁਰੂ = s + old_ਲੰਮਾਈ;
        size_t ansi_len = 0;

        ਲਈ (ਅਚਲ ਅੱਖਰ *t = j = s + old_ਲੰਮਾਈ; t > i && k < new_ਲੰਮਾਈ; ) {
                char32_t c;
                ਪੂਰਨ w;
                ਅਚਲ ਅੱਖਰ *tt;

                ਜੇ (has_ansi_seq && ansi_ਸ਼ੁਰੂ >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = ਪਿਛਲਾ_ansi_sequence(s, t - s, &ansi_ਸ਼ੁਰੂ);

                /* If the sequence extends all the way to the current position, skip it. */
                ਜੇ (has_ansi_seq && ansi_len > 0 && ansi_ਸ਼ੁਰੂ + ansi_len == t) {
                        t = ansi_ਸ਼ੁਰੂ;
                        ਜਾਰੀ;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = ਲੱਭੋ_ਪਿਛਲਾ_unichar(i, t, &c);
                ਜੇ (!tt)
                        ਵਾਪਸ ਨਿਰਾ;

                w = unichar_console_ਚੌੜਾਈ(c);
                ਜੇ (k + w > new_ਲੰਮਾਈ)
                        ਰੋਕੋ;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ਜੇ (i >= j)
                ਵਾਪਸ memdup_suffix0(s, old_ਲੰਮਾਈ);

        ਜੇ (k >= new_ਲੰਮਾਈ) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ਜੇ (j < s + old_ਲੰਮਾਈ) {
                        r = utf8_encoded_valid_unichar(j, s + old_ਲੰਮਾਈ - j);
                        ਜੇ (r < 0)
                                ਵਾਪਸ ਨਿਰਾ;

                        j += r;
                } ਨਹੀਂਤਾਂ ਜੇ (i > s) {
                        ਅਚਲ ਅੱਖਰ *tt = ਲੱਭੋ_ਪਿਛਲਾ_unichar(s, i, ਨਿਰਾ);
                        ਜੇ (!tt)
                                ਵਾਪਸ ਨਿਰਾ;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_ਲੰਮਾਈ - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_ਲੰਮਾਈ + 3 + 1 : len + 3 + len2 + 1;

        ਅੱਖਰ *e = new(ਅੱਖਰ, alloc_len);
        ਜੇ (!e)
                ਵਾਪਸ ਨਿਰਾ;

        memcpy_ਸੁਰੱਖਿਅਤ(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        ਅੱਖਰ *dst = e + len + 3;

        ਜੇ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                ਲਈ (ਅਚਲ ਅੱਖਰ *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_ਲੰਮਾਈ(p, j - p);
                        ਜੇ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ਨਹੀਂਤਾਂ
                                p = utf8_ਅਗਲਾ_char(p);
                }

        memcpy_ਸੁਰੱਖਿਅਤ(dst, j, len2);
        dst[len2] = '\0';

        ਵਾਪਸ e;
}

ਅੱਖਰ* cellescape(ਅੱਖਰ *buf, size_t len, ਅਚਲ ਅੱਖਰ *s) {
        /* Escape and ellipsize s into buffer buf of size len. Only non-control ASCII
         * characters are copied as they are, everything else is escaped. The result
         * is different then if escaping and ellipsization was performed in two
         * separate steps, because each sequence is either stored in full or skipped.
         *
         * This function should be used for logging about strings which expected to
         * be plain ASCII in a safe way.
         *
         * An ellipsis will be used if s is too long. It was always placed at the
         * very end.
         */

        size_t i = 0, ਆਖਰੀ_char_ਚੌੜਾਈ[4] = {}, k = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(len > 0); /* at least a terminating NUL */
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (;;) {
                ਅੱਖਰ four[4];
                ਪੂਰਨ w;

                ਜੇ (*s == 0) /* terminating NUL detected? then we are done! */
                        ਜਾਓ ਪੂਰਾ;

                w = cescape_char(*s, four);
                ਜੇ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        ਰੋਕੋ;

                /* OK, there was space, let's add this escaped character to the buffer */
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                ਆਖਰੀ_char_ਚੌੜਾਈ[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        ਲਈ (size_t j = 0; j < ELEMENTSOF(ਆਖਰੀ_char_ਚੌੜਾਈ); j++) {

                ਜੇ (i + 4 <= len) /* nice, we reached our space goal */
                        ਰੋਕੋ;

                k = k == 0 ? 3 : k - 1;
                ਜੇ (ਆਖਰੀ_char_ਚੌੜਾਈ[k] == 0) /* bummer, we reached the beginning of the strings */
                        ਰੋਕੋ;

                ਪ੍ਰਤਿਜ੍ਞਾ(i >= ਆਖਰੀ_char_ਚੌੜਾਈ[k]);
                i -= ਆਖਰੀ_char_ਚੌੜਾਈ[k];
        }

        ਜੇ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ਨਹੀਂਤਾਂ ਜੇ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ਨਹੀਂਤਾਂ ਜੇ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ਨਹੀਂਤਾਂ
                ਪ੍ਰਤਿਜ੍ਞਾ(i + 1 <= len);

ਪੂਰਾ:
        buf[i] = '\0';
        ਵਾਪਸ buf;
}

ਅੱਖਰ* strshorten(ਅੱਖਰ *s, size_t l) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (l >= SIZE_MAX-1) /* Would not change anything */
                ਵਾਪਸ s;

        ਜੇ (strnlen(s, l+1) > l)
                s[l] = 0;

        ਵਾਪਸ s;
}

ਪੂਰਨ strgrowpad0(ਅੱਖਰ **s, size_t l) {
        size_t sz;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (*s) {
                sz = ਲੜੀ_ਲੰਬਾਈ(*s) + 1;
                ਜੇ (sz >= l) /* never shrink */
                        ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ
                sz = 0;

        ਅੱਖਰ *q = realloc(*s, l);
        ਜੇ (!q)
                ਵਾਪਸ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ਵਾਪਸ 0;
}

ਅੱਖਰ* strreplace(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਅਚਲ ਅੱਖਰ *old_string, ਅਚਲ ਅੱਖਰ *new_string) {
        size_t l, old_len, new_len;
        ਅੱਖਰ *t, *ret = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *f;

        ਪ੍ਰਤਿਜ੍ਞਾ(old_string);
        ਪ੍ਰਤਿਜ੍ਞਾ(new_string);

        ਜੇ (!ਪਾਠ)
                ਵਾਪਸ ਨਿਰਾ;

        old_len = ਲੜੀ_ਲੰਬਾਈ(old_string);
        new_len = ਲੜੀ_ਲੰਬਾਈ(new_string);

        l = ਲੜੀ_ਲੰਬਾਈ(ਪਾਠ);
        ਜੇ (!GREEDY_REALLOC(ret, l+1))
                ਵਾਪਸ ਨਿਰਾ;

        f = ਪਾਠ;
        t = ret;
        ਜਦੋਂ (*f) {
                size_t d, nl;

                ਜੇ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        ਜਾਰੀ;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ਜੇ (!GREEDY_REALLOC(ret, nl + 1))
                        ਵਾਪਸ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t ਖਿਸਕਾਓ[ਸਥਿਰ 2],
                size_t ਮਾਪ) {

        ਜੇ (!offsets)
                ਵਾਪਸ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਖਿਸਕਾਓ);

        ਜੇ ((size_t) diff < offsets[0])
                ਖਿਸਕਾਓ[0] += ਮਾਪ;
        ਜੇ ((size_t) diff < offsets[1])
                ਖਿਸਕਾਓ[1] += ਮਾਪ;
}

ਸਥਿਰ ਖਾਲੀ retract_offsets(
                size_t begin,
                size_t ਸਿਰਾ,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t ਖਿਸਕਾਓ[ਸਥਿਰ 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ਜੇ (!offsets)
                ਵਾਪਸ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਖਿਸਕਾਓ);
        ਪ੍ਰਤਿਜ੍ਞਾ(begin <= ਸਿਰਾ);

        ਲਈ (size_t k = 0; k < 2; k++)
                ਜੇ (offsets[k] > begin)
                        ਖਿਸਕਾਓ[k] -= MIN(offsets[k], ਸਿਰਾ) - begin;
}

ਅੱਖਰ* strip_tab_ansi(ਅੱਖਰ **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_ਪੂਰਾ) AnsiSeqParser parser = {};
        _cleanup_(memstream_ਪੂਰਾ) Mem_ਧਾਰਾ m = {};
        ਅਚਲ ਅੱਖਰ *begin = ਨਿਰਾ;
        size_t isz, ਖਿਸਕਾਓ[2] = {}, n_carriage_returns = 0;
        FILE *f;

        ਪ੍ਰਤਿਜ੍ਞਾ(ibuf);
        ਪ੍ਰਤਿਜ੍ਞਾ(*ibuf);
        ਸੂਚਕ_MAY_BE_NULL(_isz);

        /* This does three things:
         *
         * 1. Replaces TABs by 8 spaces
         * 2. Strips ANSI sequences (CSI, OSC and all other escape sequences)
         * 3. Strips trailing \r characters (since they would "move the cursor", but have no
         *    other effect).
         *
         * Any other special characters are left as they are. Invalid or truncated ANSI sequences are
         * dropped, too: when stripping ANSI sequences it's better to strip too much than too little. This
         * call is supposed to suppress formatting noise (including what terminal_urlify() and friends
         * generate), but nothing else. */

        isz = _isz ? *_isz : ਲੜੀ_ਲੰਬਾਈ(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        ਜੇ (!f)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (ਅਚਲ ਅੱਖਰ *i = *ibuf; i < *ibuf + isz; i++) {
                ਪੂਰਨ e;

                ਲਈ (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ਜੇ (e < 0)
                                ਵਾਪਸ ਨਿਰਾ;
                        ਜੇ (e != ANSI_SEQ_EVENT_ABORT)
                                ਰੋਕੋ;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ਜੇ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, ਖਿਸਕਾਓ);
                                begin = ਨਿਰਾ;
                        }
                }

                ਚੋਣ (e) {

                ਹਾਲਤ ANSI_SEQ_EVENT_ਪਾਠ:
                        ਜੇ (*i == '\r') {
                                n_carriage_returns++;
                                ਜਾਰੀ;
                        }
                        ਜੇ (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (size_t) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                ਖਿਸਕਾਓ);
                                n_carriage_returns = 0;
                        }
                        ਲਈ (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ਜੇ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, ਖਿਸਕਾਓ, 7);
                        } ਨਹੀਂਤਾਂ
                                fputc(*i, f);

                        ਰੋਕੋ;

                ਹਾਲਤ ANSI_SEQ_EVENT_SEQUENCE:
                        ਜੇ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                ਲਈ (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ ANSI_SEQ_EVENT_ਸਿਰਾ:
                        /* A complete sequence, drop it (the current character is its last one) */
                        ਪ੍ਰਤਿਜ੍ਞਾ(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, ਖਿਸਕਾਓ);
                        begin = ਨਿਰਾ;
                        ਰੋਕੋ;

                ਮੂਲ:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ਜੇ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, ਖਿਸਕਾਓ);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, ਖਿਸਕਾਓ);

        ਅੱਖਰ *obuf;
        ਜੇ (memstream_finalize(&m, &obuf, _isz) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        free_and_replace(*ibuf, obuf);

        ਜੇ (highlight) {
                highlight[0] += ਖਿਸਕਾਓ[0];
                highlight[1] += ਖਿਸਕਾਓ[1];
        }

        ਵਾਪਸ *ibuf;
}

ਅੱਖਰ* strextendv_with_separator(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *separator, va_list ap) {
        _cleanup_free_ ਅੱਖਰ *ਬਫਰ = ਨਿਰਾ;
        size_t f, l, l_separator;
        bool need_separator;
        ਅੱਖਰ *nr, *p;

        ਜੇ (!x)
                x = &ਬਫਰ;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        ਲਈ (ਅਚਲ ਅੱਖਰ *t;;) {
                size_t n;

                t = va_arg(aq, ਅਚਲ ਅੱਖਰ *);
                ਜੇ (!t)
                        ਰੋਕੋ;
                ਜੇ (t == ਸੂਚਕ_MAX)
                        ਜਾਰੀ;

                n = ਲੜੀ_ਲੰਬਾਈ(t);

                ਜੇ (need_separator)
                        n += l_separator;

                ਜੇ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ਵਾਪਸ ਨਿਰਾ;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(l+1));
        ਜੇ (!nr)
                ਵਾਪਸ ਨਿਰਾ;

        *x = nr;
        p = nr + f;

        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *t;

                t = va_arg(ap, ਅਚਲ ਅੱਖਰ *);
                ਜੇ (!t)
                        ਰੋਕੋ;
                ਜੇ (t == ਸੂਚਕ_MAX)
                        ਜਾਰੀ;

                ਜੇ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ਜੇ (ਬਫਰ)
                ਵਾਪਸ TAKE_PTR(ਬਫਰ);

        /* Otherwise we extended the buffer: return the end */
        ਵਾਪਸ p;
}

ਅੱਖਰ* strextend_with_separator_ਅੰਦਰੂਨੀ(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *separator, ...) {
        va_list ap;
        ਅੱਖਰ *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ਵਾਪਸ ret;
}

ਪੂਰਨ strextendf_with_separator(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *separator, ਅਚਲ ਅੱਖਰ *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        ਪੂਰਨ l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        ਪ੍ਰਤਿਜ੍ਞਾ(x);
        ਪ੍ਰਤਿਜ੍ਞਾ(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ਜੇ (*x) {
                m = ਲੜੀ_ਲੰਬਾਈ(*x);
                a = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*x);
                ਪ੍ਰਤਿਜ੍ਞਾ(a >= m + 1);
        } ਨਹੀਂਤਾਂ
                m = a = 0;

        ਜੇ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                ਅੱਖਰ *n;

                ਜੇ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ਵਾਪਸ -ENOMEM;
                ਜੇ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ਵਾਪਸ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ਜੇ (!n)
                        ਵਾਪਸ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_ਸੁਰੱਖਿਅਤ(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        ਪ੍ਰਤਿਜ੍ਞਾ(l >= 0);

        ਜੇ ((size_t) l < a - m - l_separator) {
                ਅੱਖਰ *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ਜੇ (n)
                        *x = n;
        } ਨਹੀਂਤਾਂ {
                ਅੱਖਰ *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ਜੇ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        ਜਾਓ oom;
                ਜੇ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        ਜਾਓ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ਜੇ (!n)
                        ਜਾਓ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l < a - m - l_separator);
        }

        ਵਾਪਸ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ਵਾਪਸ -ENOMEM;
}

ਅੱਖਰ* strrep(ਅਚਲ ਅੱਖਰ *s, size_t n) {
        ਅੱਖਰ *ret, *p;
        size_t l;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        l = ਲੜੀ_ਲੰਬਾਈ(s);
        ਜੇ (!MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&l, n))
                ਵਾਪਸ ਨਿਰਾ;
        ਜੇ (!INC_ਸੁਰੱਖਿਅਤ(&l, 1))
                ਵਾਪਸ ਨਿਰਾ;

        p = ret = ਸਮ੍ਰਿਤੀ_ਦਿਓ(l);
        ਜੇ (!ret)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ਵਾਪਸ ret;
}

ਪੂਰਨ ਵੰਡੋ_pair(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *sep, ਅੱਖਰ **ret_ਪਹਿਲਾ, ਅੱਖਰ **ret_ਦੂਜਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(sep));

        ਅਚਲ ਅੱਖਰ *x = ਲੜੀ_ਖੋਜ(s, sep);
        ਜੇ (!x)
                ਵਾਪਸ -EINVAL;

        _cleanup_free_ ਅੱਖਰ *a = ਨਿਰਾ;
        ਜੇ (ret_ਪਹਿਲਾ) {
                a = strndup(s, x - s);
                ਜੇ (!a)
                        ਵਾਪਸ -ENOMEM;
        }

        _cleanup_free_ ਅੱਖਰ *b = ਨਿਰਾ;
        ਜੇ (ret_ਦੂਜਾ) {
                b = strdup(x + ਲੜੀ_ਲੰਬਾਈ(sep));
                ਜੇ (!b)
                        ਵਾਪਸ -ENOMEM;
        }

        ਜੇ (ret_ਪਹਿਲਾ)
                *ret_ਪਹਿਲਾ = TAKE_PTR(a);
        ਜੇ (ret_ਦੂਜਾ)
                *ret_ਦੂਜਾ = TAKE_PTR(b);
        ਵਾਪਸ 0;
}

ਪੂਰਨ free_and_strdup(ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *s) {
        ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ਜੇ (streq_ptr(*p, s))
                ਵਾਪਸ 0;

        ਜੇ (s) {
                t = strdup(s);
                ਜੇ (!t)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ
                t = ਨਿਰਾ;

        free_and_replace(*p, t);

        ਵਾਪਸ 1;
}

ਪੂਰਨ free_and_strdup_warn(ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *s) {
        ਪੂਰਨ r;

        r = free_and_strdup(p, s);
        ਜੇ (r < 0)
                ਵਾਪਸ log_oom();
        ਵਾਪਸ r;
}

ਪੂਰਨ free_and_strndup(ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *s, size_t l) {
        ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ਜੇ (!*p && !s)
                ਵਾਪਸ 0;

        ਜੇ (*p && s && strneq(*p, s, l) && (l > ਲੜੀ_ਲੰਬਾਈ(*p) || (*p)[l] == '\0'))
                ਵਾਪਸ 0;

        ਜੇ (s) {
                t = strndup(s, l);
                ਜੇ (!t)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ
                t = ਨਿਰਾ;

        free_and_replace(*p, t);
        ਵਾਪਸ 1;
}

ਪੂਰਨ strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *src) {
        ਜੇ (!src) {
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;

                ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
                ਜੇ (ret) {
                        ਅੱਖਰ *t = strdup(src);
                        ਜੇ (!t)
                                ਵਾਪਸ -ENOMEM;
                        *ret = t;
                }

                ਵਾਪਸ 1;
        }
};

bool string_is_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *p, String_ਸੁਰੱਖਿਅਤ_Flags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ਜੇ (FLAGS_ਸੈੱਟ(flags, STRING_ALLOW_ਖਾਲੀ_ਥਾਵਾਂ) ? !p : isempty(p))
                ਵਾਪਸ false;

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, STRING_ASCII) && !utf8_is_valid(p))
                ਵਾਪਸ false;

        ਲਈ (ਅਚਲ ਅੱਖਰ *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ਜੇ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ਵਾਪਸ false;

                ਜੇ (!FLAGS_ਸੈੱਟ(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ਵਾਪਸ false;

                ਜੇ (!FLAGS_ਸੈੱਟ(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ਵਾਪਸ false;

                ਜੇ (!FLAGS_ਸੈੱਟ(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ਵਾਪਸ false;

                ਜੇ (!FLAGS_ਸੈੱਟ(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ਵਾਪਸ false;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ਵਾਪਸ false;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ਵਾਪਸ false;
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, STRING_FILENAME) && !filename_is_valid(p))
                ਵਾਪਸ false;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਅੱਖਰ* str_realloc(ਅੱਖਰ *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ realloc(p, ਲੜੀ_ਲੰਬਾਈ(p) + 1) ?: p;
}

ਅੱਖਰ* string_erase(ਅੱਖਰ *x) {
        ਜੇ (!x)
                ਵਾਪਸ ਨਿਰਾ;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_ਸੁਰੱਖਿਅਤ(x, ਲੜੀ_ਲੰਬਾਈ(x));
        ਵਾਪਸ x;
}

ਪੂਰਨ string_truncate_ਸਤਰਾਂ(ਅਚਲ ਅੱਖਰ *s, size_t n_ਸਤਰਾਂ, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p = s, *e = s;
        bool truncation_applied = false;
        ਅੱਖਰ *copy;
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        ਲਈ (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ਜੇ (p[k] == 0) {
                        ਜੇ (k == 0) /* final empty line */
                                ਰੋਕੋ;

                        ਜੇ (n >= n_ਸਤਰਾਂ) /* above threshold */
                                ਰੋਕੋ;

                        e = p + k; /* last line to include */
                        ਰੋਕੋ;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ(p[k] == '\n');

                ਜੇ (n >= n_ਸਤਰਾਂ)
                        ਰੋਕੋ;

                ਜੇ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ਜੇ (isempty(e))
                copy = strdup(s);
        ਨਹੀਂਤਾਂ {
                ਜੇ (!ਵਿੱਚ_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ਜੇ (!copy)
                ਵਾਪਸ -ENOMEM;

        *ret = copy;
        ਵਾਪਸ truncation_applied;
}

ਪੂਰਨ string_ਕੱਢੋ_ਸਤਰ(ਅਚਲ ਅੱਖਰ *s, size_t i, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p = s;
        size_t c = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *q;

                q = strchr(p, '\n');
                ਜੇ (i == c) {
                        /* The line we are looking for! */

                        ਜੇ (q) {
                                ਅੱਖਰ *m;

                                m = strndup(p, q - p);
                                ਜੇ (!m)
                                        ਵਾਪਸ -ENOMEM;

                                *ret = m;
                                ਵਾਪਸ !isempty(q + 1); /* More coming? */
                        } ਨਹੀਂਤਾਂ
                                /* Tell the caller to use the input string if equal */
                                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, p != s ? p : ਨਿਰਾ);
                }

                ਜੇ (!q)
                        /* No more lines, return empty line */
                        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, "");

                p = q + 1;
                c++;
        }
}

ਪੂਰਨ string_ਸ਼ਾਮਲ_ਹੈ_ਸ਼ਬਦ_strv(ਅਚਲ ਅੱਖਰ *string, ਅਚਲ ਅੱਖਰ *separators, ਅੱਖਰ * ਅਚਲ *ਸ਼ਬਦ_3, ਅਚਲ ਅੱਖਰ **ret_ਸ਼ਬਦ) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ਅਚਲ ਕੱਢੋ_Flags flags = separators ? ਕੱਢੋ_DONT_COALESCE_SEPARATORS : 0;
        ਅਚਲ ਅੱਖਰ *ਮਿਲਿਆ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *w = ਨਿਰਾ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&string, &w, separators, flags);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਮਿਲਿਆ = strv_ਲੱਭੋ(ਸ਼ਬਦ_3, w);
                ਜੇ (ਮਿਲਿਆ)
                        ਰੋਕੋ;
        }

        ਜੇ (ret_ਸ਼ਬਦ)
                *ret_ਸ਼ਬਦ = ਮਿਲਿਆ;
        ਵਾਪਸ !!ਮਿਲਿਆ;
}

bool streq_skip_trailing_chars(ਅਚਲ ਅੱਖਰ *s1, ਅਚਲ ਅੱਖਰ *s2, ਅਚਲ ਅੱਖਰ *ok) {
        ਜੇ (!s1 && !s2)
                ਵਾਪਸ true;
        ਜੇ (!s1 || !s2)
                ਵਾਪਸ false;

        ਜੇ (!ok)
                ok = WHITESPACE;

        ਲਈ (; *s1 && *s2; s1++, s2++)
                ਜੇ (*s1 != *s2)
                        ਰੋਕੋ;

        ਵਾਪਸ ਵਿੱਚ_charset(s1, ok) && ਵਿੱਚ_charset(s2, ok);
}

ਅੱਖਰ* string_replace_char(ਅੱਖਰ *str, ਅੱਖਰ old_char, ਅੱਖਰ new_char) {
        ਪ੍ਰਤਿਜ੍ਞਾ(str);
        ਪ੍ਰਤਿਜ੍ਞਾ(old_char != '\0');
        ਪ੍ਰਤਿਜ੍ਞਾ(new_char != '\0');
        ਪ੍ਰਤਿਜ੍ਞਾ(old_char != new_char);

        ਲਈ (ਅੱਖਰ *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ਵਾਪਸ str;
}

ਪੂਰਨ make_cstring(ਅਚਲ ਖਾਲੀ *s, size_t n, Make_CString_ਬਹੁਲਕ ਬਹੁਲਕ, ਅੱਖਰ **ret) {
        ਅੱਖਰ *b;

        ਪ੍ਰਤਿਜ੍ਞਾ(s || n == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ < MAKE_CSTRING_ਬਹੁਲਕ_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ਜੇ (n == 0) {
                ਜੇ (ਬਹੁਲਕ == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ਵਾਪਸ -EINVAL;

                ਜੇ (!ret)
                        ਵਾਪਸ 0;

                b = new0(ਅੱਖਰ, 1);
        } ਨਹੀਂਤਾਂ {
                ਅਚਲ uint8_t *nul;

                nul = memchr(s, 0, n);
                ਜੇ (nul) {
                        ਜੇ (nul < (ਅਚਲ uint8_t*) s + n - 1 || /* embedded NUL? */
                            ਬਹੁਲਕ == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ਵਾਪਸ -EINVAL;

                        n--;
                } ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ਵਾਪਸ -EINVAL;

                ਜੇ (!ret)
                        ਵਾਪਸ 0;

                b = memdup_suffix0(s, n);
        }
        ਜੇ (!b)
                ਵਾਪਸ -ENOMEM;

        *ret = b;
        ਵਾਪਸ 0;
}

size_t strspn_from_ਸਿਰਾ(ਅਚਲ ਅੱਖਰ *str, ਅਚਲ ਅੱਖਰ *accept) {
        size_t n = 0;

        ਜੇ (isempty(str))
                ਵਾਪਸ 0;

        ਜੇ (isempty(accept))
                ਵਾਪਸ 0;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = str + ਲੜੀ_ਲੰਬਾਈ(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ਵਾਪਸ n;
}

size_t strnspn(ਅਚਲ ਅੱਖਰ *str, ਅਚਲ ਅੱਖਰ *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        ਪ੍ਰਤਿਜ੍ਞਾ(str || n == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(accept);

        ਲਈ (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ਵਾਪਸ i;
}

ਅੱਖਰ* strdupspn(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *accept) {
        ਜੇ (isempty(a) || isempty(accept))
                ਵਾਪਸ strdup("");

        ਵਾਪਸ strndup(a, strspn(a, accept));
}

ਅੱਖਰ* strdupcspn(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *reject) {
        ਜੇ (isempty(a))
                ਵਾਪਸ strdup("");
        ਜੇ (isempty(reject))
                ਵਾਪਸ strdup(a);

        ਵਾਪਸ strndup(a, strcspn(a, reject));
}

ਅੱਖਰ* ਲੱਭੋ_ਸਤਰ_startswith_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        ਪ੍ਰਤਿਜ੍ਞਾ(haystack);
        ਪ੍ਰਤਿਜ੍ਞਾ(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        ਅੱਖਰ *p = (ਅੱਖਰ*) ਲੜੀ_ਖੋਜ(haystack, needle);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (p > haystack)
                ਜਦੋਂ (p[-1] != '\n') {
                        p = ਲੜੀ_ਖੋਜ(p + 1, needle);
                        ਜੇ (!p)
                                ਵਾਪਸ ਨਿਰਾ;
                }

        ਵਾਪਸ p + ਲੜੀ_ਲੰਬਾਈ(needle);
}

ਅੱਖਰ* ਲੱਭੋ_ਸਤਰ_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        ਪ੍ਰਤਿਜ੍ਞਾ(haystack);
        ਪ੍ਰਤਿਜ੍ਞਾ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        ਅੱਖਰ *p = (ਅੱਖਰ*) ਲੱਭੋ_ਸਤਰ_startswith(haystack, needle);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (*p == 0 || strchr(NEWLINE, *p))
                ਵਾਪਸ p - ਲੜੀ_ਲੰਬਾਈ(needle);

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* ਲੱਭੋ_ਸਤਰ_after_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        ਪ੍ਰਤਿਜ੍ਞਾ(haystack);
        ਪ੍ਰਤਿਜ੍ਞਾ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        ਅੱਖਰ *p = (ਅੱਖਰ*) ਲੱਭੋ_ਸਤਰ_startswith(haystack, needle);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (*p == 0)
                ਵਾਪਸ p;
        ਜੇ (strchr(NEWLINE, *p))
                ਵਾਪਸ p + 1;

        ਵਾਪਸ ਨਿਰਾ;
}

bool ਸੰਸਕਰਣ_is_valid(ਅਚਲ ਅੱਖਰ *s, ਸੰਸਕਰਣ_Flags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ਜੇ (FLAGS_ਸੈੱਟ(flags, ਸੰਸਕਰਣ_ALLOW_ਖਾਲੀ_ਥਾਵਾਂ) ? !s : isempty(s))
                ਵਾਪਸ false;

        ਜੇ (!filename_part_is_valid(s))
                ਵਾਪਸ false;

        /* We always allow all characters specified by the UAPI.10 Version Specification, i.e. 0-9, a-z, A-Z,
         * ".", "-", "~", "^".
         *
         * If the relevant flags are set we'll also allow "+" and "_" separators.
         *
         * Note that with SemVer allows 0-9, a-z, A-Z, "+", "-", ".", hence with VERSION_ALLOW_PLUS we
         * implement a superset of it.
         *
         * If you wonder when to use which flags: when validating foreign versions (e.g. distribution
         * versions in /etc/os-release or so) validate liberally, i.e. add
         * VERSION_ALLOW_UNDERSCORE|VERSION_ALLOW_PLUS. When validating our own versioned objects (e.g. vpick
         * or so) validate more strictly, and in particular refuse characters such as "_" and "+" that may be
         * used for separating component names or boot attempt counters. Also: first – if appropriate – split
         * the string into individual components. For example, if the string consists of a name and a
         * version, separated by some character, only pass the version part to this function. The name part
         * may pass verification, but it's cleaner to not rely on that.
         *
         * For details about UAPI.10 see:
         *
         * → https://uapi-group.org/specifications/specs/version_format_specification/ */

        ਅੱਖਰ charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = ਲੜੀ_ਲੰਬਾਈ(charset);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, ਸੰਸਕਰਣ_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ਜੇ (FLAGS_ਸੈੱਟ(flags, ਸੰਸਕਰਣ_ALLOW_PLUS))
                charset[l++] = '+';

        ਵਾਪਸ ਵਿੱਚ_charset(s, charset);
}

ssize_t strlevenshtein(ਅਚਲ ਅੱਖਰ *x, ਅਚਲ ਅੱਖਰ *y) {
        _cleanup_free_ size_t *t0 = ਨਿਰਾ, *t1 = ਨਿਰਾ, *t2 = ਨਿਰਾ;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ਜੇ (streq_ptr(x, y))
                ਵਾਪਸ 0;

        xl = strlen_ptr(x);
        ਜੇ (xl > SSIZE_MAX)
                ਵਾਪਸ -E2BIG;

        yl = strlen_ptr(y);
        ਜੇ (yl > SSIZE_MAX)
                ਵਾਪਸ -E2BIG;

        ਜੇ (isempty(x))
                ਵਾਪਸ yl;
        ਜੇ (isempty(y))
                ਵਾਪਸ xl;

        t0 = new0(size_t, yl + 1);
        ਜੇ (!t0)
                ਵਾਪਸ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ਜੇ (!t1)
                ਵਾਪਸ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ਜੇ (!t2)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        ਲਈ (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                ਲਈ (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /*ਅਦਲਾ_ਬਦਲੀ*/
                        ਜੇ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ਜੇ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ਜੇ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ਵਾਪਸ t1[yl];
}

ਅੱਖਰ* strrstr_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ਜੇ (!haystack || !needle)
                ਵਾਪਸ ਨਿਰਾ;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ਜੇ (needle[0] == 0)
                ਵਾਪਸ (ਅੱਖਰ*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ਜੇ (needle[1] == 0)
                ਵਾਪਸ (ਅੱਖਰ*) strrchr(haystack, needle[0]);

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = ਲੜੀ_ਖੋਜ(haystack, needle), *q; p; p = q) {
                q = ਲੜੀ_ਖੋਜ(p + 1, needle);
                ਜੇ (!q)
                        ਵਾਪਸ (ਅੱਖਰ*) p;
        }
        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* strrstr_no_case_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        ਜੇ (!haystack || !needle)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = strchr(haystack, 0); p > haystack; p--)
                ਜੇ (startswith_no_case(p, needle))
                        ਵਾਪਸ (ਅੱਖਰ*) p;

        ਵਾਪਸ startswith_no_case(haystack, needle) ? (ਅੱਖਰ*) haystack : ਨਿਰਾ;
}

size_t str_common_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        ਲਈ (size_t n = 0;; n++) {
                ਅੱਖਰ c = a[n];
                ਜੇ (c != b[n])
                        ਵਾਪਸ n;
                ਜੇ (c == 0)
                        ਵਾਪਸ SIZE_MAX;
        }
}
