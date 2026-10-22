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

ವರ್ಣ* first_word(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *word) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ಯದಿ (isempty(word))
                ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) s;

        ಧ್ರುವ ವರ್ಣ *p = startswith(s, word);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಯದಿ (*p == '\0')
                ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) p;

        ಧ್ರುವ ವರ್ಣ *nw = skip_leading_chars(p, WHITESPACE);
        ಯದಿ (p == nw)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) nw;
}

ವರ್ಣ* strextendn(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *s, size_t l) {
        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(s || l == 0);

        ಯದಿ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ಯದಿ (l > 0 || !*x) {
                size_t q;
                ವರ್ಣ *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ಯದಿ (!m)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ಪ್ರತ್ಯಾಗಮ *x;
}

ವರ್ಣ* strstrip(ವರ್ಣ *s) {
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ಪ್ರತ್ಯಾಗಮ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

ವರ್ಣ* delete_chars(ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad) {
        ವರ್ಣ *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (!bad)
                bad = WHITESPACE;

        ಪ್ರತಿ (f = s, t = s; *f; f++) {
                ಯದಿ (strchr(bad, *f))
                        ಅನುವರ್ತ;

                *(t++) = *f;
        }

        *t = 0;

        ಪ್ರತ್ಯಾಗಮ s;
}

ವರ್ಣ* delete_trailing_chars(ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad) {
        ವರ್ಣ *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (!bad)
                bad = WHITESPACE;

        ಪ್ರತಿ (ವರ್ಣ *p = s; *p; p++)
                ಯದಿ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ಪ್ರತ್ಯಾಗಮ s;
}

ವರ್ಣ* truncate_nl_full(ವರ್ಣ *s, size_t *ret_len) {
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ಯದಿ (ret_len)
                *ret_len = n;
        ಪ್ರತ್ಯಾಗಮ s;
}

ವರ್ಣ ascii_tolower(ವರ್ಣ x) {

        ಯದಿ (x >= 'A' && x <= 'Z')
                ಪ್ರತ್ಯಾಗಮ x - 'A' + 'a';

        ಪ್ರತ್ಯಾಗಮ x;
}

ವರ್ಣ ascii_toupper(ವರ್ಣ x) {

        ಯದಿ (x >= 'a' && x <= 'z')
                ಪ್ರತ್ಯಾಗಮ x - 'a' + 'A';

        ಪ್ರತ್ಯಾಗಮ x;
}

ವರ್ಣ* ascii_strlower(ವರ್ಣ *s) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (ವರ್ಣ *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ಪ್ರತ್ಯಾಗಮ s;
}

ವರ್ಣ* ascii_strupper(ವರ್ಣ *s) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (ವರ್ಣ *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ಪ್ರತ್ಯಾಗಮ s;
}

ವರ್ಣ* ascii_strlower_n(ವರ್ಣ *s, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(n <= 0 || s);

        ಯದಿ (n <= 0)
                ಪ್ರತ್ಯಾಗಮ s;

        ಪ್ರತಿ (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ಪ್ರತ್ಯಾಗಮ s;
}

ಪೂರ್ಣಾಙ್ಕ ascii_strcasecmp_n(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b, size_t n) {

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತಿ (; n > 0; a++, b++, n--) {
                ಪೂರ್ಣಾಙ್ಕ x, y;

                x = (ಪೂರ್ಣಾಙ್ಕ) (uint8_t) ascii_tolower(*a);
                y = (ಪೂರ್ಣಾಙ್ಕ) (uint8_t) ascii_tolower(*b);

                ಯದಿ (x != y)
                        ಪ್ರತ್ಯಾಗಮ x - y;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ ascii_strcasecmp_nn(ಧ್ರುವ ವರ್ಣ *a, size_t n, ಧ್ರುವ ವರ್ಣ *b, size_t m) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ CMP(n, m);
}

bool chars_intersect(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        /* Returns true if any of the chars in a are in b. */
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = a; *p; p++)
                ಯದಿ (strchr(b, *p))
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

bool string_has_cc(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *ok) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *t = p; *t; t++) {
                ಯದಿ (ok && strchr(ok, *t))
                        ಅನುವರ್ತ;

                ಯದಿ (char_is_cc(*t))
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_ellipsis(ವರ್ಣ *buf, bool unicode) {
        ಧ್ರುವ ವರ್ಣ *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        ಪ್ರತಿಜ್ಞಾ(strlen(s) == 3);
        memcpy(buf, s, 3);
        ಪ್ರತ್ಯಾಗಮ 3;
}

ಸ್ಥಿರ size_t ansi_sequence_length(ಧ್ರುವ ವರ್ಣ *s, size_t len) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (len < 2)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ಪ್ರತ್ಯಾಗಮ 0;  /* Not the start of a sequence */

        ಯದಿ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ಯದಿ (i == len)
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯಾವತ್ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ಯದಿ (++i == len)
                                ಪ್ರತ್ಯಾಗಮ 0;
                ಯಾವತ್ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ಯದಿ (++i == len)
                                ಪ್ರತ್ಯಾಗಮ 0;
                ಯದಿ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ಪ್ರತ್ಯಾಗಮ i + 1;
                ಪ್ರತ್ಯಾಗಮ 0;  /* Bad sequence */

        } ಅನ್ಯಥಾ ಯದಿ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ಪ್ರತ್ಯಾಗಮ 2;

        ಪ್ರತ್ಯಾಗಮ 0;  /* Bad escape? */
}

ಸ್ಥಿರ bool string_has_ansi_sequence(ಧ್ರುವ ವರ್ಣ *s, size_t len) {
        ಧ್ರುವ ವರ್ಣ *t = s;

        ಯಾವತ್ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ಯದಿ (ansi_sequence_length(t, len - (t - s)) > 0)
                        ಪ್ರತ್ಯಾಗಮ true;
                t++;
        }
        ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ size_t previous_ansi_sequence(ಧ್ರುವ ವರ್ಣ *s, size_t length, ಧ್ರುವ ವರ್ಣ **ret_where) {

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ಯದಿ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತಿ (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                ಯದಿ (slen > 0) {
                        *ret_where = s + i;
                        ಪ್ರತ್ಯಾಗಮ slen;
                }

                ಯದಿ (i == 0)
                        ಭಙ್ಗ;
        }

        *ret_where = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ವರ್ಣ *ascii_ellipsize_mem(ಧ್ರುವ ವರ್ಣ *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(percent <= 100);
        ಪ್ರತಿಜ್ಞಾ(new_length != SIZE_MAX);

        ಯದಿ (old_length <= new_length)
                ಪ್ರತ್ಯಾಗಮ strndup(s, old_length);

        /* Special case short ellipsations */
        ವಿಕಲ್ಪ (new_length) {

        ಪಕ್ಷ 0:
                ಪ್ರತ್ಯಾಗಮ strdup("");

        ಪಕ್ಷ 1:
                ಯದಿ (is_locale_utf8())
                        ಪ್ರತ್ಯಾಗಮ strdup("…");
                ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ strdup(".");

        ಪಕ್ಷ 2:
                ಯದಿ (!is_locale_utf8())
                        ಪ್ರತ್ಯಾಗಮ strdup("..");
                ಭಙ್ಗ;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(ವರ್ಣ, new_length+3);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        ಪ್ರತಿಜ್ಞಾ(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ಪ್ರತ್ಯಾಗಮ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* find_previous_unichar(ಧ್ರುವ ವರ್ಣ *start, ಧ್ರುವ ವರ್ಣ *end, char32_t *ret_c) {
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(start);
        ಪ್ರತಿಜ್ಞಾ(end);
        ಪ್ರತಿಜ್ಞಾ(end > start);

        ಪ್ರತಿ (p = end; p > start; ) {
                p--;
                ಯದಿ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        ಭಙ್ಗ;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        ಯದಿ (r < 0 || p + r != end)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        ಪ್ರತ್ಯಾಗಮ p;
}

ವರ್ಣ* ellipsize_mem(ಧ್ರುವ ವರ್ಣ *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        ಧ್ರುವ ವರ್ಣ *i, *j;
        ಪೂರ್ಣಾಙ್ಕ r;

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

        ಯದಿ (new_length == SIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ strndup(s, old_length);

        ಯದಿ (new_length == 0)
                ಪ್ರತ್ಯಾಗಮ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ಯದಿ (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                ಪ್ರತ್ಯಾಗಮ ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        ಪ್ರತಿಜ್ಞಾ(x <= new_length - 1);

        k = 0;
        ಪ್ರತಿ (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                ಯದಿ (slen > 0) {
                        i += slen;
                        ಅನುವರ್ತ;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                ಪೂರ್ಣಾಙ್ಕ w = unichar_console_width(c);
                ಯದಿ (k + w > x)
                        ಭಙ್ಗ;

                k += w;
                i += r;
        }

        ಧ್ರುವ ವರ್ಣ *ansi_start = s + old_length;
        size_t ansi_len = 0;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                ಪೂರ್ಣಾಙ್ಕ w;
                ಧ್ರುವ ವರ್ಣ *tt;

                ಯದಿ (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                ಯದಿ (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        ಅನುವರ್ತ;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                ಯದಿ (!tt)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                w = unichar_console_width(c);
                ಯದಿ (k + w > new_length)
                        ಭಙ್ಗ;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ಯದಿ (i >= j)
                ಪ್ರತ್ಯಾಗಮ memdup_suffix0(s, old_length);

        ಯದಿ (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ಯದಿ (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                        j += r;
                } ಅನ್ಯಥಾ ಯದಿ (i > s) {
                        ಧ್ರುವ ವರ್ಣ *tt = find_previous_unichar(s, i, ರಿಕ್ತ);
                        ಯದಿ (!tt)
                                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        ವರ್ಣ *e = new(ವರ್ಣ, alloc_len);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        ವರ್ಣ *dst = e + len + 3;

        ಯದಿ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        ಯದಿ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ಅನ್ಯಥಾ
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        ಪ್ರತ್ಯಾಗಮ e;
}

ವರ್ಣ* cellescape(ವರ್ಣ *buf, size_t len, ಧ್ರುವ ವರ್ಣ *s) {
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

        size_t i = 0, last_char_width[4] = {}, k = 0;

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(len > 0); /* at least a terminating NUL */
        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (;;) {
                ವರ್ಣ four[4];
                ಪೂರ್ಣಾಙ್ಕ w;

                ಯದಿ (*s == 0) /* terminating NUL detected? then we are done! */
                        ಗಚ್ಛ done;

                w = cescape_char(*s, four);
                ಯದಿ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        ಭಙ್ಗ;

                /* OK, there was space, let's add this escaped character to the buffer */
                memcpy(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                last_char_width[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        ಪ್ರತಿ (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                ಯದಿ (i + 4 <= len) /* nice, we reached our space goal */
                        ಭಙ್ಗ;

                k = k == 0 ? 3 : k - 1;
                ಯದಿ (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        ಭಙ್ಗ;

                ಪ್ರತಿಜ್ಞಾ(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        ಯದಿ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ಅನ್ಯಥಾ ಯದಿ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ಅನ್ಯಥಾ ಯದಿ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ಅನ್ಯಥಾ
                ಪ್ರತಿಜ್ಞಾ(i + 1 <= len);

done:
        buf[i] = '\0';
        ಪ್ರತ್ಯಾಗಮ buf;
}

ವರ್ಣ* strshorten(ವರ್ಣ *s, size_t l) {
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (l >= SIZE_MAX-1) /* Would not change anything */
                ಪ್ರತ್ಯಾಗಮ s;

        ಯದಿ (strnlen(s, l+1) > l)
                s[l] = 0;

        ಪ್ರತ್ಯಾಗಮ s;
}

ಪೂರ್ಣಾಙ್ಕ strgrowpad0(ವರ್ಣ **s, size_t l) {
        size_t sz;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (*s) {
                sz = strlen(*s) + 1;
                ಯದಿ (sz >= l) /* never shrink */
                        ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ
                sz = 0;

        ವರ್ಣ *q = realloc(*s, l);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* strreplace(ಧ್ರುವ ವರ್ಣ *text, ಧ್ರುವ ವರ್ಣ *old_string, ಧ್ರುವ ವರ್ಣ *new_string) {
        size_t l, old_len, new_len;
        ವರ್ಣ *t, *ret = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *f;

        ಪ್ರತಿಜ್ಞಾ(old_string);
        ಪ್ರತಿಜ್ಞಾ(new_string);

        ಯದಿ (!text)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        ಯದಿ (!GREEDY_REALLOC(ret, l+1))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        f = text;
        t = ret;
        ಯಾವತ್ (*f) {
                size_t d, nl;

                ಯದಿ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        ಅನುವರ್ತ;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ಯದಿ (!GREEDY_REALLOC(ret, nl + 1))
                        ಪ್ರತ್ಯಾಗಮ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[ಸ್ಥಿರ 2],
                size_t size) {

        ಯದಿ (!offsets)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿಜ್ಞಾ(shift);

        ಯದಿ ((size_t) diff < offsets[0])
                shift[0] += size;
        ಯದಿ ((size_t) diff < offsets[1])
                shift[1] += size;
}

ಸ್ಥಿರ ಶೂನ್ಯ retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[ಸ್ಥಿರ 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ಯದಿ (!offsets)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿಜ್ಞಾ(shift);
        ಪ್ರತಿಜ್ಞಾ(begin <= end);

        ಪ್ರತಿ (size_t k = 0; k < 2; k++)
                ಯದಿ (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

ವರ್ಣ* strip_tab_ansi(ವರ್ಣ **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ಧ್ರುವ ವರ್ಣ *begin = ರಿಕ್ತ;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        ಪ್ರತಿಜ್ಞಾ(ibuf);
        ಪ್ರತಿಜ್ಞಾ(*ibuf);
        POINTER_MAY_BE_NULL(_isz);

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

        isz = _isz ? *_isz : strlen(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *i = *ibuf; i < *ibuf + isz; i++) {
                ಪೂರ್ಣಾಙ್ಕ e;

                ಪ್ರತಿ (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ಯದಿ (e < 0)
                                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                        ಯದಿ (e != ANSI_SEQ_EVENT_ABORT)
                                ಭಙ್ಗ;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ಯದಿ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = ರಿಕ್ತ;
                        }
                }

                ವಿಕಲ್ಪ (e) {

                ಪಕ್ಷ ANSI_SEQ_EVENT_TEXT:
                        ಯದಿ (*i == '\r') {
                                n_carriage_returns++;
                                ಅನುವರ್ತ;
                        }
                        ಯದಿ (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (size_t) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                shift);
                                n_carriage_returns = 0;
                        }
                        ಪ್ರತಿ (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ಯದಿ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } ಅನ್ಯಥಾ
                                fputc(*i, f);

                        ಭಙ್ಗ;

                ಪಕ್ಷ ANSI_SEQ_EVENT_SEQUENCE:
                        ಯದಿ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                ಪ್ರತಿ (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        ಪ್ರತಿಜ್ಞಾ(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = ರಿಕ್ತ;
                        ಭಙ್ಗ;

                ಸ್ವಭಾವ:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ಯದಿ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        ವರ್ಣ *obuf;
        ಯದಿ (memstream_finalize(&m, &obuf, _isz) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        free_and_replace(*ibuf, obuf);

        ಯದಿ (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        ಪ್ರತ್ಯಾಗಮ *ibuf;
}

ವರ್ಣ* strextendv_with_separator(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *separator, va_list ap) {
        _cleanup_free_ ವರ್ಣ *buffer = ರಿಕ್ತ;
        size_t f, l, l_separator;
        bool need_separator;
        ವರ್ಣ *nr, *p;

        ಯದಿ (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *t;;) {
                size_t n;

                t = va_arg(aq, ಧ್ರುವ ವರ್ಣ *);
                ಯದಿ (!t)
                        ಭಙ್ಗ;
                ಯದಿ (t == POINTER_MAX)
                        ಅನುವರ್ತ;

                n = strlen(t);

                ಯದಿ (need_separator)
                        n += l_separator;

                ಯದಿ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        ಯದಿ (!nr)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *x = nr;
        p = nr + f;

        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *t;

                t = va_arg(ap, ಧ್ರುವ ವರ್ಣ *);
                ಯದಿ (!t)
                        ಭಙ್ಗ;
                ಯದಿ (t == POINTER_MAX)
                        ಅನುವರ್ತ;

                ಯದಿ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        ಪ್ರತಿಜ್ಞಾ(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ಯದಿ (buffer)
                ಪ್ರತ್ಯಾಗಮ TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        ಪ್ರತ್ಯಾಗಮ p;
}

ವರ್ಣ* strextend_with_separator_internal(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *separator, ...) {
        va_list ap;
        ವರ್ಣ *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ strextendf_with_separator(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *separator, ಧ್ರುವ ವರ್ಣ *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ಯದಿ (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                ಪ್ರತಿಜ್ಞಾ(a >= m + 1);
        } ಅನ್ಯಥಾ
                m = a = 0;

        ಯದಿ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                ವರ್ಣ *n;

                ಯದಿ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                ಯದಿ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ಯದಿ (!n)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        ಪ್ರತಿಜ್ಞಾ(l >= 0);

        ಯದಿ ((size_t) l < a - m - l_separator) {
                ವರ್ಣ *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ಯದಿ (n)
                        *x = n;
        } ಅನ್ಯಥಾ {
                ವರ್ಣ *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ಯದಿ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        ಗಚ್ಛ oom;
                ಯದಿ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        ಗಚ್ಛ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ಯದಿ (!n)
                        ಗಚ್ಛ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                ಪ್ರತಿಜ್ಞಾ((size_t) l < a - m - l_separator);
        }

        ಪ್ರತ್ಯಾಗಮ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
}

ವರ್ಣ* strrep(ಧ್ರುವ ವರ್ಣ *s, size_t n) {
        ವರ್ಣ *ret, *p;
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(s);

        l = strlen(s);
        ಯದಿ (!MUL_ASSIGN_SAFE(&l, n))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಯದಿ (!INC_SAFE(&l, 1))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        p = ret = malloc(l);
        ಯದಿ (!ret)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ split_pair(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *sep, ವರ್ಣ **ret_first, ವರ್ಣ **ret_second) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(!isempty(sep));

        ಧ್ರುವ ವರ್ಣ *x = strstr(s, sep);
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        _cleanup_free_ ವರ್ಣ *a = ರಿಕ್ತ;
        ಯದಿ (ret_first) {
                a = strndup(s, x - s);
                ಯದಿ (!a)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        _cleanup_free_ ವರ್ಣ *b = ರಿಕ್ತ;
        ಯದಿ (ret_second) {
                b = strdup(x + strlen(sep));
                ಯದಿ (!b)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        ಯದಿ (ret_first)
                *ret_first = TAKE_PTR(a);
        ಯದಿ (ret_second)
                *ret_second = TAKE_PTR(b);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ free_and_strdup(ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *s) {
        ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ಯದಿ (streq_ptr(*p, s))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (s) {
                t = strdup(s);
                ಯದಿ (!t)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ
                t = ರಿಕ್ತ;

        free_and_replace(*p, t);

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ free_and_strdup_warn(ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *s) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = free_and_strdup(p, s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_oom();
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ free_and_strndup(ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *s, size_t l) {
        ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ಯದಿ (!*p && !s)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (s) {
                t = strndup(s, l);
                ಯದಿ (!t)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ
                t = ರಿಕ್ತ;

        free_and_replace(*p, t);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ strdup_to_full(ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *src) {
        ಯದಿ (!src) {
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;

                ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ {
                ಯದಿ (ret) {
                        ವರ್ಣ *t = strdup(src);
                        ಯದಿ (!t)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        *ret = t;
                }

                ಪ್ರತ್ಯಾಗಮ 1;
        }
};

bool string_is_safe(ಧ್ರುವ ವರ್ಣ *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ಯದಿ (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ಯದಿ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಯದಿ (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ವರ್ಣ* str_realloc(ವರ್ಣ *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ realloc(p, strlen(p) + 1) ?: p;
}

ವರ್ಣ* string_erase(ವರ್ಣ *x) {
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        ಪ್ರತ್ಯಾಗಮ x;
}

ಪೂರ್ಣಾಙ್ಕ string_truncate_ಸಾಲುಗಳು(ಧ್ರುವ ವರ್ಣ *s, size_t n_ಸಾಲುಗಳು, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p = s, *e = s;
        bool truncation_applied = false;
        ವರ್ಣ *copy;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        ಪ್ರತಿ (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ಯದಿ (p[k] == 0) {
                        ಯದಿ (k == 0) /* final empty line */
                                ಭಙ್ಗ;

                        ಯದಿ (n >= n_ಸಾಲುಗಳು) /* above threshold */
                                ಭಙ್ಗ;

                        e = p + k; /* last line to include */
                        ಭಙ್ಗ;
                }

                ಪ್ರತಿಜ್ಞಾ(p[k] == '\n');

                ಯದಿ (n >= n_ಸಾಲುಗಳು)
                        ಭಙ್ಗ;

                ಯದಿ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ಯದಿ (isempty(e))
                copy = strdup(s);
        ಅನ್ಯಥಾ {
                ಯದಿ (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ಯದಿ (!copy)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = copy;
        ಪ್ರತ್ಯಾಗಮ truncation_applied;
}

ಪೂರ್ಣಾಙ್ಕ string_extract_ಸಾಲು(ಧ್ರುವ ವರ್ಣ *s, size_t i, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p = s;
        size_t c = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *q;

                q = strchr(p, '\n');
                ಯದಿ (i == c) {
                        /* The line we are looking for! */

                        ಯದಿ (q) {
                                ವರ್ಣ *m;

                                m = strndup(p, q - p);
                                ಯದಿ (!m)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                *ret = m;
                                ಪ್ರತ್ಯಾಗಮ !isempty(q + 1); /* More coming? */
                        } ಅನ್ಯಥಾ
                                /* Tell the caller to use the input string if equal */
                                ಪ್ರತ್ಯಾಗಮ strdup_to(ret, p != s ? p : ರಿಕ್ತ);
                }

                ಯದಿ (!q)
                        /* No more lines, return empty line */
                        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

ಪೂರ್ಣಾಙ್ಕ string_contains_word_strv(ಧ್ರುವ ವರ್ಣ *string, ಧ್ರುವ ವರ್ಣ *separators, ವರ್ಣ * ಧ್ರುವ *ಪದಗಳು, ಧ್ರುವ ವರ್ಣ **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ಧ್ರುವ ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ಧ್ರುವ ವರ್ಣ *found = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *w = ರಿಕ್ತ;

                r = extract_first_word(&string, &w, separators, flags);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                found = strv_find(ಪದಗಳು, w);
                ಯದಿ (found)
                        ಭಙ್ಗ;
        }

        ಯದಿ (ret_word)
                *ret_word = found;
        ಪ್ರತ್ಯಾಗಮ !!found;
}

bool streq_skip_trailing_chars(ಧ್ರುವ ವರ್ಣ *s1, ಧ್ರುವ ವರ್ಣ *s2, ಧ್ರುವ ವರ್ಣ *ok) {
        ಯದಿ (!s1 && !s2)
                ಪ್ರತ್ಯಾಗಮ true;
        ಯದಿ (!s1 || !s2)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!ok)
                ok = WHITESPACE;

        ಪ್ರತಿ (; *s1 && *s2; s1++, s2++)
                ಯದಿ (*s1 != *s2)
                        ಭಙ್ಗ;

        ಪ್ರತ್ಯಾಗಮ in_charset(s1, ok) && in_charset(s2, ok);
}

ವರ್ಣ* string_replace_char(ವರ್ಣ *str, ವರ್ಣ old_char, ವರ್ಣ new_char) {
        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(old_char != '\0');
        ಪ್ರತಿಜ್ಞಾ(new_char != '\0');
        ಪ್ರತಿಜ್ಞಾ(old_char != new_char);

        ಪ್ರತಿ (ವರ್ಣ *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ಪ್ರತ್ಯಾಗಮ str;
}

ಪೂರ್ಣಾಙ್ಕ make_cstring(ಧ್ರುವ ಶೂನ್ಯ *s, size_t n, MakeCStringMode mode, ವರ್ಣ **ret) {
        ವರ್ಣ *b;

        ಪ್ರತಿಜ್ಞಾ(s || n == 0);
        ಪ್ರತಿಜ್ಞಾ(mode >= 0);
        ಪ್ರತಿಜ್ಞಾ(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ಯದಿ (n == 0) {
                ಯದಿ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (!ret)
                        ಪ್ರತ್ಯಾಗಮ 0;

                b = new0(ವರ್ಣ, 1);
        } ಅನ್ಯಥಾ {
                ಧ್ರುವ uint8_t *nul;

                nul = memchr(s, 0, n);
                ಯದಿ (nul) {
                        ಯದಿ (nul < (ಧ್ರುವ uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        n--;
                } ಅನ್ಯಥಾ ಯದಿ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (!ret)
                        ಪ್ರತ್ಯಾಗಮ 0;

                b = memdup_suffix0(s, n);
        }
        ಯದಿ (!b)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = b;
        ಪ್ರತ್ಯಾಗಮ 0;
}

size_t strspn_from_end(ಧ್ರುವ ವರ್ಣ *str, ಧ್ರುವ ವರ್ಣ *accept) {
        size_t n = 0;

        ಯದಿ (isempty(str))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (isempty(accept))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ಪ್ರತ್ಯಾಗಮ n;
}

size_t strnspn(ಧ್ರುವ ವರ್ಣ *str, ಧ್ರುವ ವರ್ಣ *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        ಪ್ರತಿಜ್ಞಾ(str || n == 0);
        ಪ್ರತಿಜ್ಞಾ(accept);

        ಪ್ರತಿ (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ಪ್ರತ್ಯಾಗಮ i;
}

ವರ್ಣ* strdupspn(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *accept) {
        ಯದಿ (isempty(a) || isempty(accept))
                ಪ್ರತ್ಯಾಗಮ strdup("");

        ಪ್ರತ್ಯಾಗಮ strndup(a, strspn(a, accept));
}

ವರ್ಣ* strdupcspn(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *reject) {
        ಯದಿ (isempty(a))
                ಪ್ರತ್ಯಾಗಮ strdup("");
        ಯದಿ (isempty(reject))
                ಪ್ರತ್ಯಾಗಮ strdup(a);

        ಪ್ರತ್ಯಾಗಮ strndup(a, strcspn(a, reject));
}

ವರ್ಣ* find_ಸಾಲು_startswith_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        ವರ್ಣ *p = (ವರ್ಣ*) strstr(haystack, needle);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (p > haystack)
                ಯಾವತ್ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        ಯದಿ (!p)
                                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }

        ಪ್ರತ್ಯಾಗಮ p + strlen(needle);
}

ವರ್ಣ* find_ಸಾಲು_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        ವರ್ಣ *p = (ವರ್ಣ*) find_ಸಾಲು_startswith(haystack, needle);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (*p == 0 || strchr(NEWLINE, *p))
                ಪ್ರತ್ಯಾಗಮ p - strlen(needle);

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* find_ಸಾಲು_after_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        ವರ್ಣ *p = (ವರ್ಣ*) find_ಸಾಲು_startswith(haystack, needle);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (*p == 0)
                ಪ್ರತ್ಯಾಗಮ p;
        ಯದಿ (strchr(NEWLINE, *p))
                ಪ್ರತ್ಯಾಗಮ p + 1;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

bool version_is_valid(ಧ್ರುವ ವರ್ಣ *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ಯದಿ (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!filename_part_is_valid(s))
                ಪ್ರತ್ಯಾಗಮ false;

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

        ವರ್ಣ charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        ಯದಿ (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ಯದಿ (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        ಪ್ರತ್ಯಾಗಮ in_charset(s, charset);
}

ssize_t strlevenshtein(ಧ್ರುವ ವರ್ಣ *x, ಧ್ರುವ ವರ್ಣ *y) {
        _cleanup_free_ size_t *t0 = ರಿಕ್ತ, *t1 = ರಿಕ್ತ, *t2 = ರಿಕ್ತ;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ಯದಿ (streq_ptr(x, y))
                ಪ್ರತ್ಯಾಗಮ 0;

        xl = strlen_ptr(x);
        ಯದಿ (xl > SSIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        yl = strlen_ptr(y);
        ಯದಿ (yl > SSIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        ಯದಿ (isempty(x))
                ಪ್ರತ್ಯಾಗಮ yl;
        ಯದಿ (isempty(y))
                ಪ್ರತ್ಯಾಗಮ xl;

        t0 = new0(size_t, yl + 1);
        ಯದಿ (!t0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ಯದಿ (!t1)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ಯದಿ (!t2)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        ಪ್ರತಿ (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                ಪ್ರತಿ (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        ಯದಿ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ಯದಿ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ಯದಿ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ಪ್ರತ್ಯಾಗಮ t1[yl];
}

ವರ್ಣ* strrstr_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ಯದಿ (!haystack || !needle)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ಯದಿ (needle[0] == 0)
                ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ಯದಿ (needle[1] == 0)
                ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) strrchr(haystack, needle[0]);

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                ಯದಿ (!q)
                        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) p;
        }
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* strrstr_no_case_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        ಯದಿ (!haystack || !needle)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = strchr(haystack, 0); p > haystack; p--)
                ಯದಿ (startswith_no_case(p, needle))
                        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) p;

        ಪ್ರತ್ಯಾಗಮ startswith_no_case(haystack, needle) ? (ವರ್ಣ*) haystack : ರಿಕ್ತ;
}

size_t str_common_prefix(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        ಪ್ರತಿ (size_t n = 0;; n++) {
                ವರ್ಣ c = a[n];
                ಯದಿ (c != b[n])
                        ಪ್ರತ್ಯಾಗಮ n;
                ಯದಿ (c == 0)
                        ಪ್ರತ್ಯಾಗಮ SIZE_MAX;
        }
}
