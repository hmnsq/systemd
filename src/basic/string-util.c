/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-seq.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "glyph-util.h"
#ಅನ್ತರ್ಭಾವ "gunicode.h"
#ಅನ್ತರ್ಭಾವ "locale-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "memstream-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಅಕ್ಷರ* ಮೊದಲ_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ಶಬ್ದ);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ಆದರೆ (isempty(ಶಬ್ದ))
                ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) s;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = startswith(s, ಶಬ್ದ);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಆದರೆ (*p == '\0')
                ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) p;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nw = skip_leading_chars(p, WHITESPACE);
        ಆದರೆ (p == nw)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) nw;
}

ಅಕ್ಷರ* strextendn(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l) {
        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(s || l == 0);

        ಆದರೆ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ಆದರೆ (l > 0 || !*x) {
                size_t q;
                ಅಕ್ಷರ *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ಆದರೆ (!m)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ಹಿಂತಿರುಗಿ *x;
}

ಅಕ್ಷರ* strstrip(ಅಕ್ಷರ *s) {
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ಹಿಂತಿರುಗಿ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

ಅಕ್ಷರ* delete_chars(ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಅಕ್ಷರ *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (!bad)
                bad = WHITESPACE;

        ಗಾಗಿ (f = s, t = s; *f; f++) {
                ಆದರೆ (strchr(bad, *f))
                        ಮುಂದುವರಿ;

                *(t++) = *f;
        }

        *t = 0;

        ಹಿಂತಿರುಗಿ s;
}

ಅಕ್ಷರ* delete_trailing_chars(ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಅಕ್ಷರ *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (!bad)
                bad = WHITESPACE;

        ಗಾಗಿ (ಅಕ್ಷರ *p = s; *p; p++)
                ಆದರೆ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ಹಿಂತಿರುಗಿ s;
}

ಅಕ್ಷರ* truncate_nl_ತುಂಬಿದ(ಅಕ್ಷರ *s, size_t *ret_len) {
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ಆದರೆ (ret_len)
                *ret_len = n;
        ಹಿಂತಿರುಗಿ s;
}

ಅಕ್ಷರ ascii_tolower(ಅಕ್ಷರ x) {

        ಆದರೆ (x >= 'A' && x <= 'Z')
                ಹಿಂತಿರುಗಿ x - 'A' + 'a';

        ಹಿಂತಿರುಗಿ x;
}

ಅಕ್ಷರ ascii_toupper(ಅಕ್ಷರ x) {

        ಆದರೆ (x >= 'a' && x <= 'z')
                ಹಿಂತಿರುಗಿ x - 'a' + 'A';

        ಹಿಂತಿರುಗಿ x;
}

ಅಕ್ಷರ* ascii_strlower(ಅಕ್ಷರ *s) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (ಅಕ್ಷರ *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ಹಿಂತಿರುಗಿ s;
}

ಅಕ್ಷರ* ascii_strupper(ಅಕ್ಷರ *s) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (ಅಕ್ಷರ *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ಹಿಂತಿರುಗಿ s;
}

ಅಕ್ಷರ* ascii_strlower_n(ಅಕ್ಷರ *s, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(n <= 0 || s);

        ಆದರೆ (n <= 0)
                ಹಿಂತಿರುಗಿ s;

        ಗಾಗಿ (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ಹಿಂತಿರುಗಿ s;
}

ಪೂರ್ಣಾಂಕ ascii_strcasecmp_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, size_t n) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಗಾಗಿ (; n > 0; a++, b++, n--) {
                ಪೂರ್ಣಾಂಕ x, y;

                x = (ಪೂರ್ಣಾಂಕ) (uint8_t) ascii_tolower(*a);
                y = (ಪೂರ್ಣಾಂಕ) (uint8_t) ascii_tolower(*b);

                ಆದರೆ (x != y)
                        ಹಿಂತಿರುಗಿ x - y;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ascii_strcasecmp_nn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, size_t n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, size_t m) {
        ಪೂರ್ಣಾಂಕ r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ CMP(n, m);
}

bool chars_intersect(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        /* Returns true if any of the chars in a are in b. */
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = a; *p; p++)
                ಆದರೆ (strchr(b, *p))
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

bool string_has_cc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ok) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = p; *t; t++) {
                ಆದರೆ (ok && strchr(ok, *t))
                        ಮುಂದುವರಿ;

                ಆದರೆ (char_is_cc(*t))
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_ellipsis(ಅಕ್ಷರ *buf, bool unicode) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = glyph_ತುಂಬಿದ(GLYPH_ELLIPSIS, unicode);
        ಪ್ರತಿಜ್ಞಾ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) == 3);
        ಸ್ಮರಣೆ_ನಕಲು(buf, s, 3);
        ಹಿಂತಿರುಗಿ 3;
}

ಸ್ಥಿರ size_t ansi_sequence_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (len < 2)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ಹಿಂತಿರುಗಿ 0;  /* Not the start of a sequence */

        ಆದರೆ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ಆದರೆ (i == len)
                        ಹಿಂತಿರುಗಿ 0;

                ಆಗ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ಆದರೆ (++i == len)
                                ಹಿಂತಿರುಗಿ 0;
                ಆಗ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ಆದರೆ (++i == len)
                                ಹಿಂತಿರುಗಿ 0;
                ಆದರೆ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ಹಿಂತಿರುಗಿ i + 1;
                ಹಿಂತಿರುಗಿ 0;  /* Bad sequence */

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ಹಿಂತಿರುಗಿ 2;

        ಹಿಂತಿರುಗಿ 0;  /* Bad escape? */
}

ಸ್ಥಿರ bool string_has_ansi_sequence(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = s;

        ಆಗ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ಆದರೆ (ansi_sequence_ಉದ್ದಳತೆ(t, len - (t - s)) > 0)
                        ಹಿಂತಿರುಗಿ true;
                t++;
        }
        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ size_t ಹಿಂದಿನ_ansi_sequence(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret_where) {

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ಆದರೆ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಗಾಗಿ (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_ಉದ್ದಳತೆ(s + i, length - i);
                ಆದರೆ (slen > 0) {
                        *ret_where = s + i;
                        ಹಿಂತಿರುಗಿ slen;
                }

                ಆದರೆ (i == 0)
                        ನಿಲ್ಲಿಸು;
        }

        *ret_where = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಅಕ್ಷರ *ascii_ellipsize_mem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t old_ಉದ್ದಳತೆ, size_t new_ಉದ್ದಳತೆ, ಚಿಹ್ನೆರಹಿತ percent) {
        size_t x, need_space, suffix_len;
        ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(percent <= 100);
        ಪ್ರತಿಜ್ಞಾ(new_ಉದ್ದಳತೆ != SIZE_MAX);

        ಆದರೆ (old_ಉದ್ದಳತೆ <= new_ಉದ್ದಳತೆ)
                ಹಿಂತಿರುಗಿ strndup(s, old_ಉದ್ದಳತೆ);

        /* Special case short ellipsations */
        ಆಯ್ಕೆ (new_ಉದ್ದಳತೆ) {

        ಸ್ಥಿತಿ 0:
                ಹಿಂತಿರುಗಿ strdup("");

        ಸ್ಥಿತಿ 1:
                ಆದರೆ (is_locale_utf8())
                        ಹಿಂತಿರುಗಿ strdup("…");
                ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ strdup(".");

        ಸ್ಥಿತಿ 2:
                ಆದರೆ (!is_locale_utf8())
                        ಹಿಂತಿರುಗಿ strdup("..");
                ನಿಲ್ಲಿಸು;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(ಅಕ್ಷರ, new_ಉದ್ದಳತೆ+3);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(new_ಉದ್ದಳತೆ >= need_space);

        x = ((new_ಉದ್ದಳತೆ - need_space) * percent + 50) / 100;
        ಪ್ರತಿಜ್ಞಾ(x <= new_ಉದ್ದಳತೆ - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_ಉದ್ದಳತೆ - x - need_space;
        ಸ್ಮರಣೆ_ನಕಲು(t + x + 3, s + old_ಉದ್ದಳತೆ - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ಹಿಂತಿರುಗಿ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_unichar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ, char32_t *ret_c) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಆರಂಭ);
        ಪ್ರತಿಜ್ಞಾ(ಕೊನೆ);
        ಪ್ರತಿಜ್ಞಾ(ಕೊನೆ > ಆರಂಭ);

        ಗಾಗಿ (p = ಕೊನೆ; p > ಆರಂಭ; ) {
                p--;
                ಆದರೆ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        ನಿಲ್ಲಿಸು;
        }

        r = utf8_encoded_valid_unichar(p, ಕೊನೆ - p);
        ಆದರೆ (r < 0 || p + r != ಕೊನೆ)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (ret_c)
                assert_se(utf8_encoded_ಗಮ್ಯ_unichar(p, ret_c) == r);

        ಹಿಂತಿರುಗಿ p;
}

ಅಕ್ಷರ* ellipsize_mem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t old_ಉದ್ದಳತೆ, size_t new_ಉದ್ದಳತೆ, ಚಿಹ್ನೆರಹಿತ percent) {
        size_t x, k, len, len2;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *i, *j;
        ಪೂರ್ಣಾಂಕ r;

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

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(percent <= 100);

        ಆದರೆ (new_ಉದ್ದಳತೆ == SIZE_MAX)
                ಹಿಂತಿರುಗಿ strndup(s, old_ಉದ್ದಳತೆ);

        ಆದರೆ (new_ಉದ್ದಳತೆ == 0)
                ಹಿಂತಿರುಗಿ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_ಉದ್ದಳತೆ);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ಆದರೆ (!has_ansi_seq && ascii_is_valid_n(s, old_ಉದ್ದಳತೆ) && !memchr(s, '\t', old_ಉದ್ದಳತೆ))
                ಹಿಂತಿರುಗಿ ascii_ellipsize_mem(s, old_ಉದ್ದಳತೆ, new_ಉದ್ದಳತೆ, percent);

        x = (new_ಉದ್ದಳತೆ - 1) * percent / 100;
        ಪ್ರತಿಜ್ಞಾ(x <= new_ಉದ್ದಳತೆ - 1);

        k = 0;
        ಗಾಗಿ (i = s; i < s + old_ಉದ್ದಳತೆ; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_ಉದ್ದಳತೆ(i, old_ಉದ್ದಳತೆ - (i - s)) : 0;
                ಆದರೆ (slen > 0) {
                        i += slen;
                        ಮುಂದುವರಿ;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_ಉದ್ದಳತೆ - i);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                char32_t c;
                assert_se(utf8_encoded_ಗಮ್ಯ_unichar(i, &c) == r);

                ಪೂರ್ಣಾಂಕ w = unichar_console_ಅಗಲ(c);
                ಆದರೆ (k + w > x)
                        ನಿಲ್ಲಿಸು;

                k += w;
                i += r;
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ansi_ಆರಂಭ = s + old_ಉದ್ದಳತೆ;
        size_t ansi_len = 0;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = j = s + old_ಉದ್ದಳತೆ; t > i && k < new_ಉದ್ದಳತೆ; ) {
                char32_t c;
                ಪೂರ್ಣಾಂಕ w;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tt;

                ಆದರೆ (has_ansi_seq && ansi_ಆರಂಭ >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = ಹಿಂದಿನ_ansi_sequence(s, t - s, &ansi_ಆರಂಭ);

                /* If the sequence extends all the way to the current position, skip it. */
                ಆದರೆ (has_ansi_seq && ansi_len > 0 && ansi_ಆರಂಭ + ansi_len == t) {
                        t = ansi_ಆರಂಭ;
                        ಮುಂದುವರಿ;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_unichar(i, t, &c);
                ಆದರೆ (!tt)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                w = unichar_console_ಅಗಲ(c);
                ಆದರೆ (k + w > new_ಉದ್ದಳತೆ)
                        ನಿಲ್ಲಿಸು;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ಆದರೆ (i >= j)
                ಹಿಂತಿರುಗಿ memdup_suffix0(s, old_ಉದ್ದಳತೆ);

        ಆದರೆ (k >= new_ಉದ್ದಳತೆ) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ಆದರೆ (j < s + old_ಉದ್ದಳತೆ) {
                        r = utf8_encoded_valid_unichar(j, s + old_ಉದ್ದಳತೆ - j);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                        j += r;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > s) {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tt = ಕಂಡುಹಿಡಿ_ಹಿಂದಿನ_unichar(s, i, ಖಾಲಿ);
                        ಆದರೆ (!tt)
                                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_ಉದ್ದಳತೆ - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_ಉದ್ದಳತೆ + 3 + 1 : len + 3 + len2 + 1;

        ಅಕ್ಷರ *e = new(ಅಕ್ಷರ, alloc_len);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        memcpy_ಸುರಕ್ಷಿತ(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        ಅಕ್ಷರ *dst = e + len + 3;

        ಆದರೆ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_ಉದ್ದಳತೆ(p, j - p);
                        ಆದರೆ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ಇಲ್ಲದಿದ್ದರೆ
                                p = utf8_ಮುಂದಿನ_char(p);
                }

        memcpy_ಸುರಕ್ಷಿತ(dst, j, len2);
        dst[len2] = '\0';

        ಹಿಂತಿರುಗಿ e;
}

ಅಕ್ಷರ* cellescape(ಅಕ್ಷರ *buf, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
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

        size_t i = 0, ಕೊನೆಯ_char_ಅಗಲ[4] = {}, k = 0;

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(len > 0); /* at least a terminating NUL */
        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ four[4];
                ಪೂರ್ಣಾಂಕ w;

                ಆದರೆ (*s == 0) /* terminating NUL detected? then we are done! */
                        ಹೋಗು ಮುಗಿದ;

                w = cescape_char(*s, four);
                ಆದರೆ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        ನಿಲ್ಲಿಸು;

                /* OK, there was space, let's add this escaped character to the buffer */
                ಸ್ಮರಣೆ_ನಕಲು(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                ಕೊನೆಯ_char_ಅಗಲ[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        ಗಾಗಿ (size_t j = 0; j < ELEMENTSOF(ಕೊನೆಯ_char_ಅಗಲ); j++) {

                ಆದರೆ (i + 4 <= len) /* nice, we reached our space goal */
                        ನಿಲ್ಲಿಸು;

                k = k == 0 ? 3 : k - 1;
                ಆದರೆ (ಕೊನೆಯ_char_ಅಗಲ[k] == 0) /* bummer, we reached the beginning of the strings */
                        ನಿಲ್ಲಿಸು;

                ಪ್ರತಿಜ್ಞಾ(i >= ಕೊನೆಯ_char_ಅಗಲ[k]);
                i -= ಕೊನೆಯ_char_ಅಗಲ[k];
        }

        ಆದರೆ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ಇಲ್ಲದಿದ್ದರೆ
                ಪ್ರತಿಜ್ಞಾ(i + 1 <= len);

ಮುಗಿದ:
        buf[i] = '\0';
        ಹಿಂತಿರುಗಿ buf;
}

ಅಕ್ಷರ* strshorten(ಅಕ್ಷರ *s, size_t l) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (l >= SIZE_MAX-1) /* Would not change anything */
                ಹಿಂತಿರುಗಿ s;

        ಆದರೆ (strnlen(s, l+1) > l)
                s[l] = 0;

        ಹಿಂತಿರುಗಿ s;
}

ಪೂರ್ಣಾಂಕ strgrowpad0(ಅಕ್ಷರ **s, size_t l) {
        size_t sz;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (*s) {
                sz = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*s) + 1;
                ಆದರೆ (sz >= l) /* never shrink */
                        ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ
                sz = 0;

        ಅಕ್ಷರ *q = realloc(*s, l);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* strreplace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *old_string, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *new_string) {
        size_t l, old_len, new_len;
        ಅಕ್ಷರ *t, *ret = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f;

        ಪ್ರತಿಜ್ಞಾ(old_string);
        ಪ್ರತಿಜ್ಞಾ(new_string);

        ಆದರೆ (!ಪಠ್ಯ)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        old_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(old_string);
        new_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(new_string);

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಪಠ್ಯ);
        ಆದರೆ (!GREEDY_REALLOC(ret, l+1))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        f = ಪಠ್ಯ;
        t = ret;
        ಆಗ (*f) {
                size_t d, nl;

                ಆದರೆ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        ಮುಂದುವರಿ;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ಆದರೆ (!GREEDY_REALLOC(ret, nl + 1))
                        ಹಿಂತಿರುಗಿ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t ಸರಿಸು[ಸ್ಥಿರ 2],
                size_t ಗಾತ್ರ_2) {

        ಆದರೆ (!offsets)
                ಹಿಂತಿರುಗಿ;

        ಪ್ರತಿಜ್ಞಾ(ಸರಿಸು);

        ಆದರೆ ((size_t) diff < offsets[0])
                ಸರಿಸು[0] += ಗಾತ್ರ_2;
        ಆದರೆ ((size_t) diff < offsets[1])
                ಸರಿಸು[1] += ಗಾತ್ರ_2;
}

ಸ್ಥಿರ ಶೂನ್ಯ retract_offsets(
                size_t begin,
                size_t ಕೊನೆ,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t ಸರಿಸು[ಸ್ಥಿರ 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ಆದರೆ (!offsets)
                ಹಿಂತಿರುಗಿ;

        ಪ್ರತಿಜ್ಞಾ(ಸರಿಸು);
        ಪ್ರತಿಜ್ಞಾ(begin <= ಕೊನೆ);

        ಗಾಗಿ (size_t k = 0; k < 2; k++)
                ಆದರೆ (offsets[k] > begin)
                        ಸರಿಸು[k] -= MIN(offsets[k], ಕೊನೆ) - begin;
}

ಅಕ್ಷರ* strip_tab_ansi(ಅಕ್ಷರ **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_ಮುಗಿದ) AnsiSeqParser parser = {};
        _cleanup_(memstream_ಮುಗಿದ) Mem_ಹರಿವು m = {};
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *begin = ಖಾಲಿ;
        size_t isz, ಸರಿಸು[2] = {}, n_carriage_returns = 0;
        FILE *f;

        ಪ್ರತಿಜ್ಞಾ(ibuf);
        ಪ್ರತಿಜ್ಞಾ(*ibuf);
        ಸೂಚಕ_MAY_BE_NULL(_isz);

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

        isz = _isz ? *_isz : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *i = *ibuf; i < *ibuf + isz; i++) {
                ಪೂರ್ಣಾಂಕ e;

                ಗಾಗಿ (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ಆದರೆ (e < 0)
                                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                        ಆದರೆ (e != ANSI_SEQ_EVENT_ABORT)
                                ನಿಲ್ಲಿಸು;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ಆದರೆ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, ಸರಿಸು);
                                begin = ಖಾಲಿ;
                        }
                }

                ಆಯ್ಕೆ (e) {

                ಸ್ಥಿತಿ ANSI_SEQ_EVENT_ಪಠ್ಯ:
                        ಆದರೆ (*i == '\r') {
                                n_carriage_returns++;
                                ಮುಂದುವರಿ;
                        }
                        ಆದರೆ (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (size_t) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                ಸರಿಸು);
                                n_carriage_returns = 0;
                        }
                        ಗಾಗಿ (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ಆದರೆ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, ಸರಿಸು, 7);
                        } ಇಲ್ಲದಿದ್ದರೆ
                                fputc(*i, f);

                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ ANSI_SEQ_EVENT_SEQUENCE:
                        ಆದರೆ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                ಗಾಗಿ (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ ANSI_SEQ_EVENT_ಕೊನೆ:
                        /* A complete sequence, drop it (the current character is its last one) */
                        ಪ್ರತಿಜ್ಞಾ(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, ಸರಿಸು);
                        begin = ಖಾಲಿ;
                        ನಿಲ್ಲಿಸು;

                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ಆದರೆ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, ಸರಿಸು);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, ಸರಿಸು);

        ಅಕ್ಷರ *obuf;
        ಆದರೆ (memstream_finalize(&m, &obuf, _isz) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        free_and_replace(*ibuf, obuf);

        ಆದರೆ (highlight) {
                highlight[0] += ಸರಿಸು[0];
                highlight[1] += ಸರಿಸು[1];
        }

        ಹಿಂತಿರುಗಿ *ibuf;
}

ಅಕ್ಷರ* strextendv_with_separator(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, va_list ap) {
        _cleanup_free_ ಅಕ್ಷರ *ಬಫರ್ = ಖಾಲಿ;
        size_t f, l, l_separator;
        bool need_separator;
        ಅಕ್ಷರ *nr, *p;

        ಆದರೆ (!x)
                x = &ಬಫರ್;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;;) {
                size_t n;

                t = va_arg(aq, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
                ಆದರೆ (!t)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (t == ಸೂಚಕ_MAX)
                        ಮುಂದುವರಿ;

                n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(t);

                ಆದರೆ (need_separator)
                        n += l_separator;

                ಆದರೆ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(l+1));
        ಆದರೆ (!nr)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *x = nr;
        p = nr + f;

        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;

                t = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
                ಆದರೆ (!t)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (t == ಸೂಚಕ_MAX)
                        ಮುಂದುವರಿ;

                ಆದರೆ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        ಪ್ರತಿಜ್ಞಾ(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ಆದರೆ (ಬಫರ್)
                ಹಿಂತಿರುಗಿ TAKE_PTR(ಬಫರ್);

        /* Otherwise we extended the buffer: return the end */
        ಹಿಂತಿರುಗಿ p;
}

ಅಕ್ಷರ* strextend_with_separator_ಆಂತರಿಕ(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, ...) {
        va_list ap;
        ಅಕ್ಷರ *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ strextendf_with_separator(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        ಪೂರ್ಣಾಂಕ l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ಆದರೆ (*x) {
                m = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*x);
                a = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*x);
                ಪ್ರತಿಜ್ಞಾ(a >= m + 1);
        } ಇಲ್ಲದಿದ್ದರೆ
                m = a = 0;

        ಆದರೆ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                ಅಕ್ಷರ *n;

                ಆದರೆ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                ಆದರೆ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ಆದರೆ (!n)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_ಸುರಕ್ಷಿತ(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        ಪ್ರತಿಜ್ಞಾ(l >= 0);

        ಆದರೆ ((size_t) l < a - m - l_separator) {
                ಅಕ್ಷರ *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ಆದರೆ (n)
                        *x = n;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಅಕ್ಷರ *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ಆದರೆ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        ಹೋಗು oom;
                ಆದರೆ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        ಹೋಗು oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ಆದರೆ (!n)
                        ಹೋಗು oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                ಪ್ರತಿಜ್ಞಾ((size_t) l < a - m - l_separator);
        }

        ಹಿಂತಿರುಗಿ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ಹಿಂತಿರುಗಿ -ENOMEM;
}

ಅಕ್ಷರ* strrep(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n) {
        ಅಕ್ಷರ *ret, *p;
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(s);

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
        ಆದರೆ (!MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&l, n))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಆದರೆ (!INC_ಸುರಕ್ಷಿತ(&l, 1))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        p = ret = ಸ್ಮರಣೆ_ಕೊಡು(l);
        ಆದರೆ (!ret)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು_pair(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep, ಅಕ್ಷರ **ret_ಮೊದಲ, ಅಕ್ಷರ **ret_ಎರಡನೇ) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(!isempty(sep));

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(s, sep);
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ -EINVAL;

        _cleanup_free_ ಅಕ್ಷರ *a = ಖಾಲಿ;
        ಆದರೆ (ret_ಮೊದಲ) {
                a = strndup(s, x - s);
                ಆದರೆ (!a)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        _cleanup_free_ ಅಕ್ಷರ *b = ಖಾಲಿ;
        ಆದರೆ (ret_ಎರಡನೇ) {
                b = strdup(x + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sep));
                ಆದರೆ (!b)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        ಆದರೆ (ret_ಮೊದಲ)
                *ret_ಮೊದಲ = TAKE_PTR(a);
        ಆದರೆ (ret_ಎರಡನೇ)
                *ret_ಎರಡನೇ = TAKE_PTR(b);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ free_and_strdup(ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ಆದರೆ (streq_ptr(*p, s))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (s) {
                t = strdup(s);
                ಆದರೆ (!t)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ
                t = ಖಾಲಿ;

        free_and_replace(*p, t);

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ free_and_strdup_warn(ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಪೂರ್ಣಾಂಕ r;

        r = free_and_strdup(p, s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_oom();
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ free_and_strndup(ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l) {
        ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ಆದರೆ (!*p && !s)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (*p && s && strneq(*p, s, l) && (l > ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*p) || (*p)[l] == '\0'))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (s) {
                t = strndup(s, l);
                ಆದರೆ (!t)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ
                t = ಖಾಲಿ;

        free_and_replace(*p, t);
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ strdup_ಗಮ್ಯ_ತುಂಬಿದ(ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src) {
        ಆದರೆ (!src) {
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;

                ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (ret) {
                        ಅಕ್ಷರ *t = strdup(src);
                        ಆದರೆ (!t)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                        *ret = t;
                }

                ಹಿಂತಿರುಗಿ 1;
        }
};

bool string_is_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, String_ಸುರಕ್ಷಿತ_Flags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ಆದರೆ (FLAGS_ಗಣ(flags, STRING_ALLOW_ಖಾಲಿಗಳು) ? !p : isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!FLAGS_ಗಣ(flags, STRING_ASCII) && !utf8_is_valid(p))
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ಆದರೆ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (!FLAGS_ಗಣ(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (!FLAGS_ಗಣ(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (!FLAGS_ಗಣ(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (!FLAGS_ಗಣ(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (FLAGS_ಗಣ(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (FLAGS_ಗಣ(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ಹಿಂತಿರುಗಿ false;
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, STRING_FILENAME) && !filename_is_valid(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (FLAGS_ಗಣ(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಅಕ್ಷರ* str_realloc(ಅಕ್ಷರ *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ realloc(p, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) + 1) ?: p;
}

ಅಕ್ಷರ* string_erase(ಅಕ್ಷರ *x) {
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_ಸುರಕ್ಷಿತ(x, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(x));
        ಹಿಂತಿರುಗಿ x;
}

ಪೂರ್ಣಾಂಕ string_truncate_ಸಾಲುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n_ಸಾಲುಗಳು, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s, *e = s;
        bool truncation_applied = false;
        ಅಕ್ಷರ *copy;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        ಗಾಗಿ (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ಆದರೆ (p[k] == 0) {
                        ಆದರೆ (k == 0) /* final empty line */
                                ನಿಲ್ಲಿಸು;

                        ಆದರೆ (n >= n_ಸಾಲುಗಳು) /* above threshold */
                                ನಿಲ್ಲಿಸು;

                        e = p + k; /* last line to include */
                        ನಿಲ್ಲಿಸು;
                }

                ಪ್ರತಿಜ್ಞಾ(p[k] == '\n');

                ಆದರೆ (n >= n_ಸಾಲುಗಳು)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ಆದರೆ (isempty(e))
                copy = strdup(s);
        ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (!ಒಳ_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ಆದರೆ (!copy)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = copy;
        ಹಿಂತಿರುಗಿ truncation_applied;
}

ಪೂರ್ಣಾಂಕ string_ಹೊರತೆಗೆ_ಸಾಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t i, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s;
        size_t c = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q;

                q = strchr(p, '\n');
                ಆದರೆ (i == c) {
                        /* The line we are looking for! */

                        ಆದರೆ (q) {
                                ಅಕ್ಷರ *m;

                                m = strndup(p, q - p);
                                ಆದರೆ (!m)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                *ret = m;
                                ಹಿಂತಿರುಗಿ !isempty(q + 1); /* More coming? */
                        } ಇಲ್ಲದಿದ್ದರೆ
                                /* Tell the caller to use the input string if equal */
                                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, p != s ? p : ಖಾಲಿ);
                }

                ಆದರೆ (!q)
                        /* No more lines, return empty line */
                        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, "");

                p = q + 1;
                c++;
        }
}

ಪೂರ್ಣಾಂಕ string_ಒಳಗೊಂಡಿದೆ_ಶಬ್ದ_strv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಪದಗಳು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret_ಶಬ್ದ) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ಸ್ಥಿರಾಂಕ ಹೊರತೆಗೆ_Flags flags = separators ? ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS : 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಿಕ್ಕಿತು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *w = ಖಾಲಿ;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&string, &w, separators, flags);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಸಿಕ್ಕಿತು = strv_ಕಂಡುಹಿಡಿ(ಪದಗಳು, w);
                ಆದರೆ (ಸಿಕ್ಕಿತು)
                        ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (ret_ಶಬ್ದ)
                *ret_ಶಬ್ದ = ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ !!ಸಿಕ್ಕಿತು;
}

bool streq_skip_trailing_chars(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ok) {
        ಆದರೆ (!s1 && !s2)
                ಹಿಂತಿರುಗಿ true;
        ಆದರೆ (!s1 || !s2)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!ok)
                ok = WHITESPACE;

        ಗಾಗಿ (; *s1 && *s2; s1++, s2++)
                ಆದರೆ (*s1 != *s2)
                        ನಿಲ್ಲಿಸು;

        ಹಿಂತಿರುಗಿ ಒಳ_charset(s1, ok) && ಒಳ_charset(s2, ok);
}

ಅಕ್ಷರ* string_replace_char(ಅಕ್ಷರ *str, ಅಕ್ಷರ old_char, ಅಕ್ಷರ new_char) {
        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(old_char != '\0');
        ಪ್ರತಿಜ್ಞಾ(new_char != '\0');
        ಪ್ರತಿಜ್ಞಾ(old_char != new_char);

        ಗಾಗಿ (ಅಕ್ಷರ *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ಹಿಂತಿರುಗಿ str;
}

ಪೂರ್ಣಾಂಕ make_cstring(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s, size_t n, Make_CString_ಬಹುಲಕ ಬಹುಲಕ, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *b;

        ಪ್ರತಿಜ್ಞಾ(s || n == 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ < MAKE_CSTRING_ಬಹುಲಕ_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ಆದರೆ (n == 0) {
                ಆದರೆ (ಬಹುಲಕ == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (!ret)
                        ಹಿಂತಿರುಗಿ 0;

                b = new0(ಅಕ್ಷರ, 1);
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ uint8_t *nul;

                nul = memchr(s, 0, n);
                ಆದರೆ (nul) {
                        ಆದರೆ (nul < (ಸ್ಥಿರಾಂಕ uint8_t*) s + n - 1 || /* embedded NUL? */
                            ಬಹುಲಕ == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        n--;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (!ret)
                        ಹಿಂತಿರುಗಿ 0;

                b = memdup_suffix0(s, n);
        }
        ಆದರೆ (!b)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = b;
        ಹಿಂತಿರುಗಿ 0;
}

size_t strspn_from_ಕೊನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept) {
        size_t n = 0;

        ಆದರೆ (isempty(str))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (isempty(accept))
                ಹಿಂತಿರುಗಿ 0;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = str + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ಹಿಂತಿರುಗಿ n;
}

size_t strnspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        ಪ್ರತಿಜ್ಞಾ(str || n == 0);
        ಪ್ರತಿಜ್ಞಾ(accept);

        ಗಾಗಿ (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ಹಿಂತಿರುಗಿ i;
}

ಅಕ್ಷರ* strdupspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept) {
        ಆದರೆ (isempty(a) || isempty(accept))
                ಹಿಂತಿರುಗಿ strdup("");

        ಹಿಂತಿರುಗಿ strndup(a, strspn(a, accept));
}

ಅಕ್ಷರ* strdupcspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *reject) {
        ಆದರೆ (isempty(a))
                ಹಿಂತಿರುಗಿ strdup("");
        ಆದರೆ (isempty(reject))
                ಹಿಂತಿರುಗಿ strdup(a);

        ಹಿಂತಿರುಗಿ strndup(a, strcspn(a, reject));
}

ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಸಾಲು_startswith_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        ಅಕ್ಷರ *p = (ಅಕ್ಷರ*) ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(haystack, needle);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (p > haystack)
                ಆಗ (p[-1] != '\n') {
                        p = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p + 1, needle);
                        ಆದರೆ (!p)
                                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }

        ಹಿಂತಿರುಗಿ p + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(needle);
}

ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಸಾಲು_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        ಅಕ್ಷರ *p = (ಅಕ್ಷರ*) ಕಂಡುಹಿಡಿ_ಸಾಲು_startswith(haystack, needle);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (*p == 0 || strchr(NEWLINE, *p))
                ಹಿಂತಿರುಗಿ p - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(needle);

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಸಾಲು_after_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        ಅಕ್ಷರ *p = (ಅಕ್ಷರ*) ಕಂಡುಹಿಡಿ_ಸಾಲು_startswith(haystack, needle);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (*p == 0)
                ಹಿಂತಿರುಗಿ p;
        ಆದರೆ (strchr(NEWLINE, *p))
                ಹಿಂತಿರುಗಿ p + 1;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

bool ಆವೃತ್ತಿ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಆವೃತ್ತಿ_Flags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ಆದರೆ (FLAGS_ಗಣ(flags, ಆವೃತ್ತಿ_ALLOW_ಖಾಲಿಗಳು) ? !s : isempty(s))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!filename_part_is_valid(s))
                ಹಿಂತಿರುಗಿ false;

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

        ಅಕ್ಷರ charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(charset);

        ಆದರೆ (FLAGS_ಗಣ(flags, ಆವೃತ್ತಿ_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ಆದರೆ (FLAGS_ಗಣ(flags, ಆವೃತ್ತಿ_ALLOW_PLUS))
                charset[l++] = '+';

        ಹಿಂತಿರುಗಿ ಒಳ_charset(s, charset);
}

ssize_t strlevenshtein(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *y) {
        _cleanup_free_ size_t *t0 = ಖಾಲಿ, *t1 = ಖಾಲಿ, *t2 = ಖಾಲಿ;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ಆದರೆ (streq_ptr(x, y))
                ಹಿಂತಿರುಗಿ 0;

        xl = strlen_ptr(x);
        ಆದರೆ (xl > SSIZE_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        yl = strlen_ptr(y);
        ಆದರೆ (yl > SSIZE_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        ಆದರೆ (isempty(x))
                ಹಿಂತಿರುಗಿ yl;
        ಆದರೆ (isempty(y))
                ಹಿಂತಿರುಗಿ xl;

        t0 = new0(size_t, yl + 1);
        ಆದರೆ (!t0)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ಆದರೆ (!t1)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ಆದರೆ (!t2)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        ಗಾಗಿ (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                ಗಾಗಿ (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /*ಅದಲುಬದಲು*/
                        ಆದರೆ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ಆದರೆ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ಆದರೆ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ಹಿಂತಿರುಗಿ t1[yl];
}

ಅಕ್ಷರ* strrstr_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ಆದರೆ (!haystack || !needle)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ಆದರೆ (needle[0] == 0)
                ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ಆದರೆ (needle[1] == 0)
                ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) strrchr(haystack, needle[0]);

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(haystack, needle), *q; p; p = q) {
                q = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(p + 1, needle);
                ಆದರೆ (!q)
                        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) p;
        }
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* strrstr_no_case_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಆದರೆ (!haystack || !needle)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strchr(haystack, 0); p > haystack; p--)
                ಆದರೆ (startswith_no_case(p, needle))
                        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) p;

        ಹಿಂತಿರುಗಿ startswith_no_case(haystack, needle) ? (ಅಕ್ಷರ*) haystack : ಖಾಲಿ;
}

size_t str_common_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        ಗಾಗಿ (size_t n = 0;; n++) {
                ಅಕ್ಷರ c = a[n];
                ಆದರೆ (c != b[n])
                        ಹಿಂತಿರುಗಿ n;
                ಆದರೆ (c == 0)
                        ಹಿಂತಿರುಗಿ SIZE_MAX;
        }
}
