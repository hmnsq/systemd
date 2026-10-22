/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "ansi-seq.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "glyph-util.h"
#ଅନ୍ତର୍ଭାଵ "gunicode.h"
#ଅନ୍ତର୍ଭାଵ "locale-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "memstream-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ଅକ୍ଷର* first_word(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *word) {
        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ଯଦି (isempty(word))
                ଫେରନ୍ତୁ (ଅକ୍ଷର*) s;

        ଅଚଳ ଅକ୍ଷର *p = startswith(s, word);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;
        ଯଦି (*p == '\0')
                ଫେରନ୍ତୁ (ଅକ୍ଷର*) p;

        ଅଚଳ ଅକ୍ଷର *nw = skip_leading_chars(p, WHITESPACE);
        ଯଦି (p == nw)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) nw;
}

ଅକ୍ଷର* strextendn(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *s, size_t l) {
        ପ୍ରତିଜ୍ଞା(x);
        ପ୍ରତିଜ୍ଞା(s || l == 0);

        ଯଦି (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ଯଦି (l > 0 || !*x) {
                size_t q;
                ଅକ୍ଷର *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ଯଦି (!m)
                        ଫେରନ୍ତୁ ଖାଲି;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ଫେରନ୍ତୁ *x;
}

ଅକ୍ଷର* strstrip(ଅକ୍ଷର *s) {
        ଯଦି (!s)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ଫେରନ୍ତୁ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

ଅକ୍ଷର* delete_chars(ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad) {
        ଅକ୍ଷର *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ଯଦି (!s)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (!bad)
                bad = WHITESPACE;

        ପାଇଁ (f = s, t = s; *f; f++) {
                ଯଦି (strchr(bad, *f))
                        ଜାରି;

                *(t++) = *f;
        }

        *t = 0;

        ଫେରନ୍ତୁ s;
}

ଅକ୍ଷର* delete_trailing_chars(ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad) {
        ଅକ୍ଷର *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ଯଦି (!s)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (!bad)
                bad = WHITESPACE;

        ପାଇଁ (ଅକ୍ଷର *p = s; *p; p++)
                ଯଦି (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ଫେରନ୍ତୁ s;
}

ଅକ୍ଷର* truncate_nl_full(ଅକ୍ଷର *s, size_t *ret_len) {
        size_t n;

        ପ୍ରତିଜ୍ଞା(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ଯଦି (ret_len)
                *ret_len = n;
        ଫେରନ୍ତୁ s;
}

ଅକ୍ଷର ascii_tolower(ଅକ୍ଷର x) {

        ଯଦି (x >= 'A' && x <= 'Z')
                ଫେରନ୍ତୁ x - 'A' + 'a';

        ଫେରନ୍ତୁ x;
}

ଅକ୍ଷର ascii_toupper(ଅକ୍ଷର x) {

        ଯଦି (x >= 'a' && x <= 'z')
                ଫେରନ୍ତୁ x - 'a' + 'A';

        ଫେରନ୍ତୁ x;
}

ଅକ୍ଷର* ascii_strlower(ଅକ୍ଷର *s) {
        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (ଅକ୍ଷର *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ଫେରନ୍ତୁ s;
}

ଅକ୍ଷର* ascii_strupper(ଅକ୍ଷର *s) {
        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (ଅକ୍ଷର *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ଫେରନ୍ତୁ s;
}

ଅକ୍ଷର* ascii_strlower_n(ଅକ୍ଷର *s, size_t n) {
        ପ୍ରତିଜ୍ଞା(n <= 0 || s);

        ଯଦି (n <= 0)
                ଫେରନ୍ତୁ s;

        ପାଇଁ (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ଫେରନ୍ତୁ s;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ascii_strcasecmp_n(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b, size_t n) {

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ପାଇଁ (; n > 0; a++, b++, n--) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y;

                x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (uint8_t) ascii_tolower(*a);
                y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (uint8_t) ascii_tolower(*b);

                ଯଦି (x != y)
                        ଫେରନ୍ତୁ x - y;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ascii_strcasecmp_nn(ଅଚଳ ଅକ୍ଷର *a, size_t n, ଅଚଳ ଅକ୍ଷର *b, size_t m) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ CMP(n, m);
}

bool chars_intersect(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        /* Returns true if any of the chars in a are in b. */
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = a; *p; p++)
                ଯଦି (strchr(b, *p))
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

bool string_has_cc(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *ok) {
        ପ୍ରତିଜ୍ଞା(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *t = p; *t; t++) {
                ଯଦି (ok && strchr(ok, *t))
                        ଜାରି;

                ଯଦି (char_is_cc(*t))
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_ellipsis(ଅକ୍ଷର *buf, bool unicode) {
        ଅଚଳ ଅକ୍ଷର *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        ପ୍ରତିଜ୍ଞା(strlen(s) == 3);
        memcpy(buf, s, 3);
        ଫେରନ୍ତୁ 3;
}

ସ୍ଥିର size_t ansi_sequence_length(ଅଚଳ ଅକ୍ଷର *s, size_t len) {
        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (len < 2)
                ଫେରନ୍ତୁ 0;

        ଯଦି (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ଫେରନ୍ତୁ 0;  /* Not the start of a sequence */

        ଯଦି (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ଯଦି (i == len)
                        ଫେରନ୍ତୁ 0;

                ଯେତେବେଳେ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ଯଦି (++i == len)
                                ଫେରନ୍ତୁ 0;
                ଯେତେବେଳେ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ଯଦି (++i == len)
                                ଫେରନ୍ତୁ 0;
                ଯଦି (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ଫେରନ୍ତୁ i + 1;
                ଫେରନ୍ତୁ 0;  /* Bad sequence */

        } ଅନ୍ୟଥା ଯଦି (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ଫେରନ୍ତୁ 2;

        ଫେରନ୍ତୁ 0;  /* Bad escape? */
}

ସ୍ଥିର bool string_has_ansi_sequence(ଅଚଳ ଅକ୍ଷର *s, size_t len) {
        ଅଚଳ ଅକ୍ଷର *t = s;

        ଯେତେବେଳେ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ଯଦି (ansi_sequence_length(t, len - (t - s)) > 0)
                        ଫେରନ୍ତୁ true;
                t++;
        }
        ଫେରନ୍ତୁ false;
}

ସ୍ଥିର size_t previous_ansi_sequence(ଅଚଳ ଅକ୍ଷର *s, size_t length, ଅଚଳ ଅକ୍ଷର **ret_where) {

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ଯଦି (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ପାଇଁ (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                ଯଦି (slen > 0) {
                        *ret_where = s + i;
                        ଫେରନ୍ତୁ slen;
                }

                ଯଦି (i == 0)
                        ଭାଙ୍ଗ;
        }

        *ret_where = ଖାଲି;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅକ୍ଷର *ascii_ellipsize_mem(ଅଚଳ ଅକ୍ଷର *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(percent <= 100);
        ପ୍ରତିଜ୍ଞା(new_length != SIZE_MAX);

        ଯଦି (old_length <= new_length)
                ଫେରନ୍ତୁ strndup(s, old_length);

        /* Special case short ellipsations */
        ବାଛ (new_length) {

        ସ୍ଥିତି 0:
                ଫେରନ୍ତୁ strdup("");

        ସ୍ଥିତି 1:
                ଯଦି (is_locale_utf8())
                        ଫେରନ୍ତୁ strdup("…");
                ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ strdup(".");

        ସ୍ଥିତି 2:
                ଯଦି (!is_locale_utf8())
                        ଫେରନ୍ତୁ strdup("..");
                ଭାଙ୍ଗ;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(ଅକ୍ଷର, new_length+3);
        ଯଦି (!t)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        ପ୍ରତିଜ୍ଞା(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ଫେରନ୍ତୁ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* find_previous_unichar(ଅଚଳ ଅକ୍ଷର *start, ଅଚଳ ଅକ୍ଷର *end, char32_t *ret_c) {
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(start);
        ପ୍ରତିଜ୍ଞା(end);
        ପ୍ରତିଜ୍ଞା(end > start);

        ପାଇଁ (p = end; p > start; ) {
                p--;
                ଯଦି (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        ଭାଙ୍ଗ;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        ଯଦି (r < 0 || p + r != end)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        ଫେରନ୍ତୁ p;
}

ଅକ୍ଷର* ellipsize_mem(ଅଚଳ ଅକ୍ଷର *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        ଅଚଳ ଅକ୍ଷର *i, *j;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(percent <= 100);

        ଯଦି (new_length == SIZE_MAX)
                ଫେରନ୍ତୁ strndup(s, old_length);

        ଯଦି (new_length == 0)
                ଫେରନ୍ତୁ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ଯଦି (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                ଫେରନ୍ତୁ ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        ପ୍ରତିଜ୍ଞା(x <= new_length - 1);

        k = 0;
        ପାଇଁ (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                ଯଦି (slen > 0) {
                        i += slen;
                        ଜାରି;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ ଖାଲି;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = unichar_console_width(c);
                ଯଦି (k + w > x)
                        ଭାଙ୍ଗ;

                k += w;
                i += r;
        }

        ଅଚଳ ଅକ୍ଷର *ansi_start = s + old_length;
        size_t ansi_len = 0;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା w;
                ଅଚଳ ଅକ୍ଷର *tt;

                ଯଦି (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                ଯଦି (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        ଜାରି;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                ଯଦି (!tt)
                        ଫେରନ୍ତୁ ଖାଲି;

                w = unichar_console_width(c);
                ଯଦି (k + w > new_length)
                        ଭାଙ୍ଗ;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ଯଦି (i >= j)
                ଫେରନ୍ତୁ memdup_suffix0(s, old_length);

        ଯଦି (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ଯଦି (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ ଖାଲି;

                        j += r;
                } ଅନ୍ୟଥା ଯଦି (i > s) {
                        ଅଚଳ ଅକ୍ଷର *tt = find_previous_unichar(s, i, ଖାଲି);
                        ଯଦି (!tt)
                                ଫେରନ୍ତୁ ଖାଲି;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        ଅକ୍ଷର *e = new(ଅକ୍ଷର, alloc_len);
        ଯଦି (!e)
                ଫେରନ୍ତୁ ଖାଲି;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        ଅକ୍ଷର *dst = e + len + 3;

        ଯଦି (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        ଯଦି (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ଅନ୍ୟଥା
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        ଫେରନ୍ତୁ e;
}

ଅକ୍ଷର* cellescape(ଅକ୍ଷର *buf, size_t len, ଅଚଳ ଅକ୍ଷର *s) {
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

        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(len > 0); /* at least a terminating NUL */
        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (;;) {
                ଅକ୍ଷର four[4];
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା w;

                ଯଦି (*s == 0) /* terminating NUL detected? then we are done! */
                        ଯାଅ done;

                w = cescape_char(*s, four);
                ଯଦି (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        ଭାଙ୍ଗ;

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
        ପାଇଁ (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                ଯଦି (i + 4 <= len) /* nice, we reached our space goal */
                        ଭାଙ୍ଗ;

                k = k == 0 ? 3 : k - 1;
                ଯଦି (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        ଭାଙ୍ଗ;

                ପ୍ରତିଜ୍ଞା(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        ଯଦି (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ଅନ୍ୟଥା ଯଦି (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ଅନ୍ୟଥା ଯଦି (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ଅନ୍ୟଥା
                ପ୍ରତିଜ୍ଞା(i + 1 <= len);

done:
        buf[i] = '\0';
        ଫେରନ୍ତୁ buf;
}

ଅକ୍ଷର* strshorten(ଅକ୍ଷର *s, size_t l) {
        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (l >= SIZE_MAX-1) /* Would not change anything */
                ଫେରନ୍ତୁ s;

        ଯଦି (strnlen(s, l+1) > l)
                s[l] = 0;

        ଫେରନ୍ତୁ s;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strgrowpad0(ଅକ୍ଷର **s, size_t l) {
        size_t sz;

        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (*s) {
                sz = strlen(*s) + 1;
                ଯଦି (sz >= l) /* never shrink */
                        ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା
                sz = 0;

        ଅକ୍ଷର *q = realloc(*s, l);
        ଯଦି (!q)
                ଫେରନ୍ତୁ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* strreplace(ଅଚଳ ଅକ୍ଷର *text, ଅଚଳ ଅକ୍ଷର *old_string, ଅଚଳ ଅକ୍ଷର *new_string) {
        size_t l, old_len, new_len;
        ଅକ୍ଷର *t, *ret = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *f;

        ପ୍ରତିଜ୍ଞା(old_string);
        ପ୍ରତିଜ୍ଞା(new_string);

        ଯଦି (!text)
                ଫେରନ୍ତୁ ଖାଲି;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        ଯଦି (!GREEDY_REALLOC(ret, l+1))
                ଫେରନ୍ତୁ ଖାଲି;

        f = text;
        t = ret;
        ଯେତେବେଳେ (*f) {
                size_t d, nl;

                ଯଦି (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        ଜାରି;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ଯଦି (!GREEDY_REALLOC(ret, nl + 1))
                        ଫେରନ୍ତୁ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଶୂନ୍ୟ advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[ସ୍ଥିର 2],
                size_t size) {

        ଯଦି (!offsets)
                ଫେରନ୍ତୁ;

        ପ୍ରତିଜ୍ଞା(shift);

        ଯଦି ((size_t) diff < offsets[0])
                shift[0] += size;
        ଯଦି ((size_t) diff < offsets[1])
                shift[1] += size;
}

ସ୍ଥିର ଶୂନ୍ୟ retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[ସ୍ଥିର 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ଯଦି (!offsets)
                ଫେରନ୍ତୁ;

        ପ୍ରତିଜ୍ଞା(shift);
        ପ୍ରତିଜ୍ଞା(begin <= end);

        ପାଇଁ (size_t k = 0; k < 2; k++)
                ଯଦି (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

ଅକ୍ଷର* strip_tab_ansi(ଅକ୍ଷର **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ଅଚଳ ଅକ୍ଷର *begin = ଖାଲି;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        ପ୍ରତିଜ୍ଞା(ibuf);
        ପ୍ରତିଜ୍ଞା(*ibuf);
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
        ଯଦି (!f)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *i = *ibuf; i < *ibuf + isz; i++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା e;

                ପାଇଁ (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ଯଦି (e < 0)
                                ଫେରନ୍ତୁ ଖାଲି;
                        ଯଦି (e != ANSI_SEQ_EVENT_ABORT)
                                ଭାଙ୍ଗ;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ଯଦି (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = ଖାଲି;
                        }
                }

                ବାଛ (e) {

                ସ୍ଥିତି ANSI_SEQ_EVENT_TEXT:
                        ଯଦି (*i == '\r') {
                                n_carriage_returns++;
                                ଜାରି;
                        }
                        ଯଦି (*i == '\n') {
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
                        ପାଇଁ (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ଯଦି (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } ଅନ୍ୟଥା
                                fputc(*i, f);

                        ଭାଙ୍ଗ;

                ସ୍ଥିତି ANSI_SEQ_EVENT_SEQUENCE:
                        ଯଦି (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                ପାଇଁ (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        ପ୍ରତିଜ୍ଞା(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = ଖାଲି;
                        ଭାଙ୍ଗ;

                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ଯଦି (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        ଅକ୍ଷର *obuf;
        ଯଦି (memstream_finalize(&m, &obuf, _isz) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        free_and_replace(*ibuf, obuf);

        ଯଦି (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        ଫେରନ୍ତୁ *ibuf;
}

ଅକ୍ଷର* strextendv_with_separator(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *separator, va_list ap) {
        _cleanup_free_ ଅକ୍ଷର *buffer = ଖାଲି;
        size_t f, l, l_separator;
        bool need_separator;
        ଅକ୍ଷର *nr, *p;

        ଯଦି (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *t;;) {
                size_t n;

                t = va_arg(aq, ଅଚଳ ଅକ୍ଷର *);
                ଯଦି (!t)
                        ଭାଙ୍ଗ;
                ଯଦି (t == POINTER_MAX)
                        ଜାରି;

                n = strlen(t);

                ଯଦି (need_separator)
                        n += l_separator;

                ଯଦି (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ଫେରନ୍ତୁ ଖାଲି;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        ଯଦି (!nr)
                ଫେରନ୍ତୁ ଖାଲି;

        *x = nr;
        p = nr + f;

        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *t;

                t = va_arg(ap, ଅଚଳ ଅକ୍ଷର *);
                ଯଦି (!t)
                        ଭାଙ୍ଗ;
                ଯଦି (t == POINTER_MAX)
                        ଜାରି;

                ଯଦି (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        ପ୍ରତିଜ୍ଞା(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ଯଦି (buffer)
                ଫେରନ୍ତୁ TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        ଫେରନ୍ତୁ p;
}

ଅକ୍ଷର* strextend_with_separator_internal(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *separator, ...) {
        va_list ap;
        ଅକ୍ଷର *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strextendf_with_separator(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *separator, ଅଚଳ ଅକ୍ଷର *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        ପ୍ରତିଜ୍ଞା(x);
        ପ୍ରତିଜ୍ଞା(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ଯଦି (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                ପ୍ରତିଜ୍ଞା(a >= m + 1);
        } ଅନ୍ୟଥା
                m = a = 0;

        ଯଦି (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                ଅକ୍ଷର *n;

                ଯଦି (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ଫେରନ୍ତୁ -ENOMEM;
                ଯଦି (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ଫେରନ୍ତୁ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ଯଦି (!n)
                        ଫେରନ୍ତୁ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        ପ୍ରତିଜ୍ଞା(l >= 0);

        ଯଦି ((size_t) l < a - m - l_separator) {
                ଅକ୍ଷର *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ଯଦି (n)
                        *x = n;
        } ଅନ୍ୟଥା {
                ଅକ୍ଷର *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ଯଦି (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        ଯାଅ oom;
                ଯଦି (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        ଯାଅ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ଯଦି (!n)
                        ଯାଅ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                ପ୍ରତିଜ୍ଞା((size_t) l < a - m - l_separator);
        }

        ଫେରନ୍ତୁ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ଫେରନ୍ତୁ -ENOMEM;
}

ଅକ୍ଷର* strrep(ଅଚଳ ଅକ୍ଷର *s, size_t n) {
        ଅକ୍ଷର *ret, *p;
        size_t l;

        ପ୍ରତିଜ୍ଞା(s);

        l = strlen(s);
        ଯଦି (!MUL_ASSIGN_SAFE(&l, n))
                ଫେରନ୍ତୁ ଖାଲି;
        ଯଦି (!INC_SAFE(&l, 1))
                ଫେରନ୍ତୁ ଖାଲି;

        p = ret = malloc(l);
        ଯଦି (!ret)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା split_pair(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *sep, ଅକ୍ଷର **ret_first, ଅକ୍ଷର **ret_second) {
        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(!isempty(sep));

        ଅଚଳ ଅକ୍ଷର *x = strstr(s, sep);
        ଯଦି (!x)
                ଫେରନ୍ତୁ -EINVAL;

        _cleanup_free_ ଅକ୍ଷର *a = ଖାଲି;
        ଯଦି (ret_first) {
                a = strndup(s, x - s);
                ଯଦି (!a)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        _cleanup_free_ ଅକ୍ଷର *b = ଖାଲି;
        ଯଦି (ret_second) {
                b = strdup(x + strlen(sep));
                ଯଦି (!b)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        ଯଦି (ret_first)
                *ret_first = TAKE_PTR(a);
        ଯଦି (ret_second)
                *ret_second = TAKE_PTR(b);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_and_strdup(ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *s) {
        ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ଯଦି (streq_ptr(*p, s))
                ଫେରନ୍ତୁ 0;

        ଯଦି (s) {
                t = strdup(s);
                ଯଦି (!t)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା
                t = ଖାଲି;

        free_and_replace(*p, t);

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_and_strdup_warn(ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *s) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = free_and_strdup(p, s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_oom();
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_and_strndup(ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *s, size_t l) {
        ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ଯଦି (!*p && !s)
                ଫେରନ୍ତୁ 0;

        ଯଦି (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                ଫେରନ୍ତୁ 0;

        ଯଦି (s) {
                t = strndup(s, l);
                ଯଦି (!t)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା
                t = ଖାଲି;

        free_and_replace(*p, t);
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strdup_to_full(ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *src) {
        ଯଦି (!src) {
                ଯଦି (ret)
                        *ret = ଖାଲି;

                ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା {
                ଯଦି (ret) {
                        ଅକ୍ଷର *t = strdup(src);
                        ଯଦି (!t)
                                ଫେରନ୍ତୁ -ENOMEM;
                        *ret = t;
                }

                ଫେରନ୍ତୁ 1;
        }
};

bool string_is_safe(ଅଚଳ ଅକ୍ଷର *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ଯଦି (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                ଫେରନ୍ତୁ false;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ଯଦି ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ଫେରନ୍ତୁ false;

                ଯଦି (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ଫେରନ୍ତୁ false;

                ଯଦି (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ଫେରନ୍ତୁ false;

                ଯଦି (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ଫେରନ୍ତୁ false;

                ଯଦି (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ଫେରନ୍ତୁ false;

                ଯଦି (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ଫେରନ୍ତୁ false;

                ଯଦି (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ଫେରନ୍ତୁ false;
        }

        ଯଦି (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ଅକ୍ଷର* str_realloc(ଅକ୍ଷର *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ realloc(p, strlen(p) + 1) ?: p;
}

ଅକ୍ଷର* string_erase(ଅକ୍ଷର *x) {
        ଯଦି (!x)
                ଫେରନ୍ତୁ ଖାଲି;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        ଫେରନ୍ତୁ x;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_truncate_ଧାଡ଼ିଗୁଡ଼ିକ(ଅଚଳ ଅକ୍ଷର *s, size_t n_ଧାଡ଼ିଗୁଡ଼ିକ, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p = s, *e = s;
        bool truncation_applied = false;
        ଅକ୍ଷର *copy;
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        ପାଇଁ (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ଯଦି (p[k] == 0) {
                        ଯଦି (k == 0) /* final empty line */
                                ଭାଙ୍ଗ;

                        ଯଦି (n >= n_ଧାଡ଼ିଗୁଡ଼ିକ) /* above threshold */
                                ଭାଙ୍ଗ;

                        e = p + k; /* last line to include */
                        ଭାଙ୍ଗ;
                }

                ପ୍ରତିଜ୍ଞା(p[k] == '\n');

                ଯଦି (n >= n_ଧାଡ଼ିଗୁଡ଼ିକ)
                        ଭାଙ୍ଗ;

                ଯଦି (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ଯଦି (isempty(e))
                copy = strdup(s);
        ଅନ୍ୟଥା {
                ଯଦି (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ଯଦି (!copy)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = copy;
        ଫେରନ୍ତୁ truncation_applied;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_extract_ଧାଡ଼ି(ଅଚଳ ଅକ୍ଷର *s, size_t i, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p = s;
        size_t c = 0;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *q;

                q = strchr(p, '\n');
                ଯଦି (i == c) {
                        /* The line we are looking for! */

                        ଯଦି (q) {
                                ଅକ୍ଷର *m;

                                m = strndup(p, q - p);
                                ଯଦି (!m)
                                        ଫେରନ୍ତୁ -ENOMEM;

                                *ret = m;
                                ଫେରନ୍ତୁ !isempty(q + 1); /* More coming? */
                        } ଅନ୍ୟଥା
                                /* Tell the caller to use the input string if equal */
                                ଫେରନ୍ତୁ strdup_to(ret, p != s ? p : ଖାଲି);
                }

                ଯଦି (!q)
                        /* No more lines, return empty line */
                        ଫେରନ୍ତୁ strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_contains_word_strv(ଅଚଳ ଅକ୍ଷର *string, ଅଚଳ ଅକ୍ଷର *separators, ଅକ୍ଷର * ଅଚଳ *ଶବ୍ଦଗୁଡ଼ିକ, ଅଚଳ ଅକ୍ଷର **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ଅଚଳ ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ଅଚଳ ଅକ୍ଷର *found = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *w = ଖାଲି;

                r = extract_first_word(&string, &w, separators, flags);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                found = strv_find(ଶବ୍ଦଗୁଡ଼ିକ, w);
                ଯଦି (found)
                        ଭାଙ୍ଗ;
        }

        ଯଦି (ret_word)
                *ret_word = found;
        ଫେରନ୍ତୁ !!found;
}

bool streq_skip_trailing_chars(ଅଚଳ ଅକ୍ଷର *s1, ଅଚଳ ଅକ୍ଷର *s2, ଅଚଳ ଅକ୍ଷର *ok) {
        ଯଦି (!s1 && !s2)
                ଫେରନ୍ତୁ true;
        ଯଦି (!s1 || !s2)
                ଫେରନ୍ତୁ false;

        ଯଦି (!ok)
                ok = WHITESPACE;

        ପାଇଁ (; *s1 && *s2; s1++, s2++)
                ଯଦି (*s1 != *s2)
                        ଭାଙ୍ଗ;

        ଫେରନ୍ତୁ in_charset(s1, ok) && in_charset(s2, ok);
}

ଅକ୍ଷର* string_replace_char(ଅକ୍ଷର *str, ଅକ୍ଷର old_char, ଅକ୍ଷର new_char) {
        ପ୍ରତିଜ୍ଞା(str);
        ପ୍ରତିଜ୍ଞା(old_char != '\0');
        ପ୍ରତିଜ୍ଞା(new_char != '\0');
        ପ୍ରତିଜ୍ଞା(old_char != new_char);

        ପାଇଁ (ଅକ୍ଷର *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ଫେରନ୍ତୁ str;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_cstring(ଅଚଳ ଶୂନ୍ୟ *s, size_t n, MakeCStringMode mode, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *b;

        ପ୍ରତିଜ୍ଞା(s || n == 0);
        ପ୍ରତିଜ୍ଞା(mode >= 0);
        ପ୍ରତିଜ୍ଞା(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ଯଦି (n == 0) {
                ଯଦି (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (!ret)
                        ଫେରନ୍ତୁ 0;

                b = new0(ଅକ୍ଷର, 1);
        } ଅନ୍ୟଥା {
                ଅଚଳ uint8_t *nul;

                nul = memchr(s, 0, n);
                ଯଦି (nul) {
                        ଯଦି (nul < (ଅଚଳ uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ଫେରନ୍ତୁ -EINVAL;

                        n--;
                } ଅନ୍ୟଥା ଯଦି (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (!ret)
                        ଫେରନ୍ତୁ 0;

                b = memdup_suffix0(s, n);
        }
        ଯଦି (!b)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = b;
        ଫେରନ୍ତୁ 0;
}

size_t strspn_from_end(ଅଚଳ ଅକ୍ଷର *str, ଅଚଳ ଅକ୍ଷର *accept) {
        size_t n = 0;

        ଯଦି (isempty(str))
                ଫେରନ୍ତୁ 0;

        ଯଦି (isempty(accept))
                ଫେରନ୍ତୁ 0;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ଫେରନ୍ତୁ n;
}

size_t strnspn(ଅଚଳ ଅକ୍ଷର *str, ଅଚଳ ଅକ୍ଷର *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        ପ୍ରତିଜ୍ଞା(str || n == 0);
        ପ୍ରତିଜ୍ଞା(accept);

        ପାଇଁ (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ଫେରନ୍ତୁ i;
}

ଅକ୍ଷର* strdupspn(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *accept) {
        ଯଦି (isempty(a) || isempty(accept))
                ଫେରନ୍ତୁ strdup("");

        ଫେରନ୍ତୁ strndup(a, strspn(a, accept));
}

ଅକ୍ଷର* strdupcspn(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *reject) {
        ଯଦି (isempty(a))
                ଫେରନ୍ତୁ strdup("");
        ଯଦି (isempty(reject))
                ଫେରନ୍ତୁ strdup(a);

        ଫେରନ୍ତୁ strndup(a, strcspn(a, reject));
}

ଅକ୍ଷର* find_ଧାଡ଼ି_startswith_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        ପ୍ରତିଜ୍ଞା(haystack);
        ପ୍ରତିଜ୍ଞା(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        ଅକ୍ଷର *p = (ଅକ୍ଷର*) strstr(haystack, needle);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (p > haystack)
                ଯେତେବେଳେ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        ଯଦି (!p)
                                ଫେରନ୍ତୁ ଖାଲି;
                }

        ଫେରନ୍ତୁ p + strlen(needle);
}

ଅକ୍ଷର* find_ଧାଡ଼ି_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        ପ୍ରତିଜ୍ଞା(haystack);
        ପ୍ରତିଜ୍ଞା(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        ଅକ୍ଷର *p = (ଅକ୍ଷର*) find_ଧାଡ଼ି_startswith(haystack, needle);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (*p == 0 || strchr(NEWLINE, *p))
                ଫେରନ୍ତୁ p - strlen(needle);

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* find_ଧାଡ଼ି_after_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        ପ୍ରତିଜ୍ଞା(haystack);
        ପ୍ରତିଜ୍ଞା(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        ଅକ୍ଷର *p = (ଅକ୍ଷର*) find_ଧାଡ଼ି_startswith(haystack, needle);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (*p == 0)
                ଫେରନ୍ତୁ p;
        ଯଦି (strchr(NEWLINE, *p))
                ଫେରନ୍ତୁ p + 1;

        ଫେରନ୍ତୁ ଖାଲି;
}

bool version_is_valid(ଅଚଳ ଅକ୍ଷର *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ଯଦି (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                ଫେରନ୍ତୁ false;

        ଯଦି (!filename_part_is_valid(s))
                ଫେରନ୍ତୁ false;

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

        ଅକ୍ଷର charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        ଯଦି (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ଯଦି (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        ଫେରନ୍ତୁ in_charset(s, charset);
}

ssize_t strlevenshtein(ଅଚଳ ଅକ୍ଷର *x, ଅଚଳ ଅକ୍ଷର *y) {
        _cleanup_free_ size_t *t0 = ଖାଲି, *t1 = ଖାଲି, *t2 = ଖାଲି;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ଯଦି (streq_ptr(x, y))
                ଫେରନ୍ତୁ 0;

        xl = strlen_ptr(x);
        ଯଦି (xl > SSIZE_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        yl = strlen_ptr(y);
        ଯଦି (yl > SSIZE_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        ଯଦି (isempty(x))
                ଫେରନ୍ତୁ yl;
        ଯଦି (isempty(y))
                ଫେରନ୍ତୁ xl;

        t0 = new0(size_t, yl + 1);
        ଯଦି (!t0)
                ଫେରନ୍ତୁ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ଯଦି (!t1)
                ଫେରନ୍ତୁ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ଯଦି (!t2)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        ପାଇଁ (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                ପାଇଁ (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        ଯଦି (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ଯଦି (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ଯଦି (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ଫେରନ୍ତୁ t1[yl];
}

ଅକ୍ଷର* strrstr_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ଯଦି (!haystack || !needle)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ଯଦି (needle[0] == 0)
                ଫେରନ୍ତୁ (ଅକ୍ଷର*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ଯଦି (needle[1] == 0)
                ଫେରନ୍ତୁ (ଅକ୍ଷର*) strrchr(haystack, needle[0]);

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                ଯଦି (!q)
                        ଫେରନ୍ତୁ (ଅକ୍ଷର*) p;
        }
        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* strrstr_no_case_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        ଯଦି (!haystack || !needle)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = strchr(haystack, 0); p > haystack; p--)
                ଯଦି (startswith_no_case(p, needle))
                        ଫେରନ୍ତୁ (ଅକ୍ଷର*) p;

        ଫେରନ୍ତୁ startswith_no_case(haystack, needle) ? (ଅକ୍ଷର*) haystack : ଖାଲି;
}

size_t str_common_prefix(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        ପାଇଁ (size_t n = 0;; n++) {
                ଅକ୍ଷର c = a[n];
                ଯଦି (c != b[n])
                        ଫେରନ୍ତୁ n;
                ଯଦି (c == 0)
                        ଫେରନ୍ତୁ SIZE_MAX;
        }
}
