/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "ansi-seq.h"
#include "escape.h"
#include "extract-word.h"
#include "glyph-util.h"
#include "gunicode.h"
#include "locale-util.h"
#include "log.h"
#include "memory-util.h"
#include "memstream-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

var_na* first_word(const var_na *s, const var_na *word) {
        assert(s);
        assert(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ᮚᮦᮔ᮪ (isempty(word))
                ᮝᮜᮤ (var_na*) s;

        const var_na *p = startswith(s, word);
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;
        ᮚᮦᮔ᮪ (*p == '\0')
                ᮝᮜᮤ (var_na*) p;

        const var_na *nw = skip_leading_chars(p, WHITESPACE);
        ᮚᮦᮔ᮪ (p == nw)
                ᮝᮜᮤ NULL;

        ᮝᮜᮤ (var_na*) nw;
}

var_na* strextendn(var_na **x, const var_na *s, size_t l) {
        assert(x);
        assert(s || l == 0);

        ᮚᮦᮔ᮪ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ᮚᮦᮔ᮪ (l > 0 || !*x) {
                size_t q;
                var_na *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ᮚᮦᮔ᮪ (!m)
                        ᮝᮜᮤ NULL;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ᮝᮜᮤ *x;
}

var_na* strstrip(var_na *s) {
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ NULL;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ᮝᮜᮤ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

var_na* delete_chars(var_na *s, const var_na *bad) {
        var_na *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (!bad)
                bad = WHITESPACE;

        for (f = s, t = s; *f; f++) {
                ᮚᮦᮔ᮪ (strchr(bad, *f))
                        continue;

                *(t++) = *f;
        }

        *t = 0;

        ᮝᮜᮤ s;
}

var_na* delete_trailing_chars(var_na *s, const var_na *bad) {
        var_na *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (!bad)
                bad = WHITESPACE;

        for (var_na *p = s; *p; p++)
                ᮚᮦᮔ᮪ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ᮝᮜᮤ s;
}

var_na* truncate_nl_full(var_na *s, size_t *ret_len) {
        size_t n;

        assert(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ᮚᮦᮔ᮪ (ret_len)
                *ret_len = n;
        ᮝᮜᮤ s;
}

var_na ascii_tolower(var_na x) {

        ᮚᮦᮔ᮪ (x >= 'A' && x <= 'Z')
                ᮝᮜᮤ x - 'A' + 'a';

        ᮝᮜᮤ x;
}

var_na ascii_toupper(var_na x) {

        ᮚᮦᮔ᮪ (x >= 'a' && x <= 'z')
                ᮝᮜᮤ x - 'a' + 'A';

        ᮝᮜᮤ x;
}

var_na* ascii_strlower(var_na *s) {
        assert(s);

        for (var_na *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ᮝᮜᮤ s;
}

var_na* ascii_strupper(var_na *s) {
        assert(s);

        for (var_na *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ᮝᮜᮤ s;
}

var_na* ascii_strlower_n(var_na *s, size_t n) {
        assert(n <= 0 || s);

        ᮚᮦᮔ᮪ (n <= 0)
                ᮝᮜᮤ s;

        for (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ᮝᮜᮤ s;
}

puur_naa_nka ascii_strcasecmp_n(const var_na *a, const var_na *b, size_t n) {

        assert(a);
        assert(b);

        for (; n > 0; a++, b++, n--) {
                puur_naa_nka x, y;

                x = (puur_naa_nka) (uint8_t) ascii_tolower(*a);
                y = (puur_naa_nka) (uint8_t) ascii_tolower(*b);

                ᮚᮦᮔ᮪ (x != y)
                        ᮝᮜᮤ x - y;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka ascii_strcasecmp_nn(const var_na *a, size_t n, const var_na *b, size_t m) {
        puur_naa_nka r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ᮚᮦᮔ᮪ (r != 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ CMP(n, m);
}

bool chars_intersect(const var_na *a, const var_na *b) {
        /* Returns true if any of the chars in a are in b. */
        for (const var_na *p = a; *p; p++)
                ᮚᮦᮔ᮪ (strchr(b, *p))
                        ᮝᮜᮤ true;

        ᮝᮜᮤ false;
}

bool string_has_cc(const var_na *p, const var_na *ok) {
        assert(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        for (const var_na *t = p; *t; t++) {
                ᮚᮦᮔ᮪ (ok && strchr(ok, *t))
                        continue;

                ᮚᮦᮔ᮪ (char_is_cc(*t))
                        ᮝᮜᮤ true;
        }

        ᮝᮜᮤ false;
}

static puur_naa_nka write_ellipsis(var_na *buf, bool unicode) {
        const var_na *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        assert(strlen(s) == 3);
        memcpy(buf, s, 3);
        ᮝᮜᮤ 3;
}

static size_t ansi_sequence_length(const var_na *s, size_t len) {
        assert(s);

        ᮚᮦᮔ᮪ (len < 2)
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ᮝᮜᮤ 0;  /* Not the start of a sequence */

        ᮚᮦᮔ᮪ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ᮚᮦᮔ᮪ (i == len)
                        ᮝᮜᮤ 0;

                ᮞᮨᮓᮨᮜ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ᮚᮦᮔ᮪ (++i == len)
                                ᮝᮜᮤ 0;
                ᮞᮨᮓᮨᮜ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ᮚᮦᮔ᮪ (++i == len)
                                ᮝᮜᮤ 0;
                ᮚᮦᮔ᮪ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ᮝᮜᮤ i + 1;
                ᮝᮜᮤ 0;  /* Bad sequence */

        } ᮜᮤᮚ ᮚᮦᮔ᮪ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ᮝᮜᮤ 2;

        ᮝᮜᮤ 0;  /* Bad escape? */
}

static bool string_has_ansi_sequence(const var_na *s, size_t len) {
        const var_na *t = s;

        ᮞᮨᮓᮨᮜ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ᮚᮦᮔ᮪ (ansi_sequence_length(t, len - (t - s)) > 0)
                        ᮝᮜᮤ true;
                t++;
        }
        ᮝᮜᮤ false;
}

static size_t previous_ansi_sequence(const var_na *s, size_t length, const var_na **ret_where) {

        assert(s);
        assert(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ᮚᮦᮔ᮪ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = NULL;
                ᮝᮜᮤ 0;
        }

        for (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                ᮚᮦᮔ᮪ (slen > 0) {
                        *ret_where = s + i;
                        ᮝᮜᮤ slen;
                }

                ᮚᮦᮔ᮪ (i == 0)
                        bha_nga;
        }

        *ret_where = NULL;
        ᮝᮜᮤ 0;
}

static var_na *ascii_ellipsize_mem(const var_na *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        var_na *t;

        assert(s);
        assert(percent <= 100);
        assert(new_length != SIZE_MAX);

        ᮚᮦᮔ᮪ (old_length <= new_length)
                ᮝᮜᮤ strndup(s, old_length);

        /* Special case short ellipsations */
        switch (new_length) {

        pak_sa 0:
                ᮝᮜᮤ strdup("");

        pak_sa 1:
                ᮚᮦᮔ᮪ (is_locale_utf8())
                        ᮝᮜᮤ strdup("…");
                ᮜᮤᮚ
                        ᮝᮜᮤ strdup(".");

        pak_sa 2:
                ᮚᮦᮔ᮪ (!is_locale_utf8())
                        ᮝᮜᮤ strdup("..");
                bha_nga;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(var_na, new_length+3);
        ᮚᮦᮔ᮪ (!t)
                ᮝᮜᮤ NULL;

        assert(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        assert(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ᮝᮜᮤ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
static const var_na* find_previous_unichar(const var_na *start, const var_na *end, char32_t *ret_c) {
        const var_na *p;
        puur_naa_nka r;

        assert(start);
        assert(end);
        assert(end > start);

        for (p = end; p > start; ) {
                p--;
                ᮚᮦᮔ᮪ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        bha_nga;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        ᮚᮦᮔ᮪ (r < 0 || p + r != end)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        ᮝᮜᮤ p;
}

var_na* ellipsize_mem(const var_na *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        const var_na *i, *j;
        puur_naa_nka r;

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

        assert(s);
        assert(percent <= 100);

        ᮚᮦᮔ᮪ (new_length == SIZE_MAX)
                ᮝᮜᮤ strndup(s, old_length);

        ᮚᮦᮔ᮪ (new_length == 0)
                ᮝᮜᮤ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ᮚᮦᮔ᮪ (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                ᮝᮜᮤ ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        assert(x <= new_length - 1);

        k = 0;
        for (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                ᮚᮦᮔ᮪ (slen > 0) {
                        i += slen;
                        continue;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ NULL;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                puur_naa_nka w = unichar_console_width(c);
                ᮚᮦᮔ᮪ (k + w > x)
                        bha_nga;

                k += w;
                i += r;
        }

        const var_na *ansi_start = s + old_length;
        size_t ansi_len = 0;

        for (const var_na *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                puur_naa_nka w;
                const var_na *tt;

                ᮚᮦᮔ᮪ (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                ᮚᮦᮔ᮪ (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        continue;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                ᮚᮦᮔ᮪ (!tt)
                        ᮝᮜᮤ NULL;

                w = unichar_console_width(c);
                ᮚᮦᮔ᮪ (k + w > new_length)
                        bha_nga;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ᮚᮦᮔ᮪ (i >= j)
                ᮝᮜᮤ memdup_suffix0(s, old_length);

        ᮚᮦᮔ᮪ (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ᮚᮦᮔ᮪ (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ NULL;

                        j += r;
                } ᮜᮤᮚ ᮚᮦᮔ᮪ (i > s) {
                        const var_na *tt = find_previous_unichar(s, i, NULL);
                        ᮚᮦᮔ᮪ (!tt)
                                ᮝᮜᮤ NULL;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        var_na *e = new(var_na, alloc_len);
        ᮚᮦᮔ᮪ (!e)
                ᮝᮜᮤ NULL;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        var_na *dst = e + len + 3;

        ᮚᮦᮔ᮪ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                for (const var_na *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        ᮚᮦᮔ᮪ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ᮜᮤᮚ
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        ᮝᮜᮤ e;
}

var_na* cellescape(var_na *buf, size_t len, const var_na *s) {
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

        assert(buf);
        assert(len > 0); /* at least a terminating NUL */
        assert(s);

        for (;;) {
                var_na four[4];
                puur_naa_nka w;

                ᮚᮦᮔ᮪ (*s == 0) /* terminating NUL detected? then we are done! */
                        goto done;

                w = cescape_char(*s, four);
                ᮚᮦᮔ᮪ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        bha_nga;

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
        for (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                ᮚᮦᮔ᮪ (i + 4 <= len) /* nice, we reached our space goal */
                        bha_nga;

                k = k == 0 ? 3 : k - 1;
                ᮚᮦᮔ᮪ (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        bha_nga;

                assert(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        ᮚᮦᮔ᮪ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ᮜᮤᮚ ᮚᮦᮔ᮪ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ᮜᮤᮚ
                assert(i + 1 <= len);

done:
        buf[i] = '\0';
        ᮝᮜᮤ buf;
}

var_na* strshorten(var_na *s, size_t l) {
        assert(s);

        ᮚᮦᮔ᮪ (l >= SIZE_MAX-1) /* Would not change anything */
                ᮝᮜᮤ s;

        ᮚᮦᮔ᮪ (strnlen(s, l+1) > l)
                s[l] = 0;

        ᮝᮜᮤ s;
}

puur_naa_nka strgrowpad0(var_na **s, size_t l) {
        size_t sz;

        assert(s);

        ᮚᮦᮔ᮪ (*s) {
                sz = strlen(*s) + 1;
                ᮚᮦᮔ᮪ (sz >= l) /* never shrink */
                        ᮝᮜᮤ 0;
        } ᮜᮤᮚ
                sz = 0;

        var_na *q = realloc(*s, l);
        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ᮝᮜᮤ 0;
}

var_na* strreplace(const var_na *text, const var_na *old_string, const var_na *new_string) {
        size_t l, old_len, new_len;
        var_na *t, *ret = NULL;
        const var_na *f;

        assert(old_string);
        assert(new_string);

        ᮚᮦᮔ᮪ (!text)
                ᮝᮜᮤ NULL;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        ᮚᮦᮔ᮪ (!GREEDY_REALLOC(ret, l+1))
                ᮝᮜᮤ NULL;

        f = text;
        t = ret;
        ᮞᮨᮓᮨᮜ (*f) {
                size_t d, nl;

                ᮚᮦᮔ᮪ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        continue;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ᮚᮦᮔ᮪ (!GREEDY_REALLOC(ret, nl + 1))
                        ᮝᮜᮤ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ᮝᮜᮤ ret;
}

static void advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2],
                size_t size) {

        ᮚᮦᮔ᮪ (!offsets)
                ᮝᮜᮤ;

        assert(shift);

        ᮚᮦᮔ᮪ ((size_t) diff < offsets[0])
                shift[0] += size;
        ᮚᮦᮔ᮪ ((size_t) diff < offsets[1])
                shift[1] += size;
}

static void retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ᮚᮦᮔ᮪ (!offsets)
                ᮝᮜᮤ;

        assert(shift);
        assert(begin <= end);

        for (size_t k = 0; k < 2; k++)
                ᮚᮦᮔ᮪ (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

var_na* strip_tab_ansi(var_na **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        const var_na *begin = NULL;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        assert(ibuf);
        assert(*ibuf);
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
        ᮚᮦᮔ᮪ (!f)
                ᮝᮜᮤ NULL;

        for (const var_na *i = *ibuf; i < *ibuf + isz; i++) {
                puur_naa_nka e;

                for (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ᮚᮦᮔ᮪ (e < 0)
                                ᮝᮜᮤ NULL;
                        ᮚᮦᮔ᮪ (e != ANSI_SEQ_EVENT_ABORT)
                                bha_nga;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ᮚᮦᮔ᮪ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = NULL;
                        }
                }

                switch (e) {

                pak_sa ANSI_SEQ_EVENT_TEXT:
                        ᮚᮦᮔ᮪ (*i == '\r') {
                                n_carriage_returns++;
                                continue;
                        }
                        ᮚᮦᮔ᮪ (*i == '\n') {
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
                        for (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ᮚᮦᮔ᮪ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } ᮜᮤᮚ
                                fputc(*i, f);

                        bha_nga;

                pak_sa ANSI_SEQ_EVENT_SEQUENCE:
                        ᮚᮦᮔ᮪ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                for (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        bha_nga;

                pak_sa ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        assert(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = NULL;
                        bha_nga;

                default:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ᮚᮦᮔ᮪ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        var_na *obuf;
        ᮚᮦᮔ᮪ (memstream_finalize(&m, &obuf, _isz) < 0)
                ᮝᮜᮤ NULL;

        free_and_replace(*ibuf, obuf);

        ᮚᮦᮔ᮪ (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        ᮝᮜᮤ *ibuf;
}

var_na* strextendv_with_separator(var_na **x, const var_na *separator, va_list ap) {
        _cleanup_free_ var_na *buffer = NULL;
        size_t f, l, l_separator;
        bool need_separator;
        var_na *nr, *p;

        ᮚᮦᮔ᮪ (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        for (const var_na *t;;) {
                size_t n;

                t = va_arg(aq, const var_na *);
                ᮚᮦᮔ᮪ (!t)
                        bha_nga;
                ᮚᮦᮔ᮪ (t == POINTER_MAX)
                        continue;

                n = strlen(t);

                ᮚᮦᮔ᮪ (need_separator)
                        n += l_separator;

                ᮚᮦᮔ᮪ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ᮝᮜᮤ NULL;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        ᮚᮦᮔ᮪ (!nr)
                ᮝᮜᮤ NULL;

        *x = nr;
        p = nr + f;

        for (;;) {
                const var_na *t;

                t = va_arg(ap, const var_na *);
                ᮚᮦᮔ᮪ (!t)
                        bha_nga;
                ᮚᮦᮔ᮪ (t == POINTER_MAX)
                        continue;

                ᮚᮦᮔ᮪ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        assert(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ᮚᮦᮔ᮪ (buffer)
                ᮝᮜᮤ TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        ᮝᮜᮤ p;
}

var_na* strextend_with_separator_internal(var_na **x, const var_na *separator, ...) {
        va_list ap;
        var_na *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ᮝᮜᮤ ret;
}

puur_naa_nka strextendf_with_separator(var_na **x, const var_na *separator, const var_na *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        puur_naa_nka l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        assert(x);
        assert(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ᮚᮦᮔ᮪ (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                assert(a >= m + 1);
        } ᮜᮤᮚ
                m = a = 0;

        ᮚᮦᮔ᮪ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                var_na *n;

                ᮚᮦᮔ᮪ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ᮝᮜᮤ -ENOMEM;
                ᮚᮦᮔ᮪ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ᮝᮜᮤ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ᮚᮦᮔ᮪ (!n)
                        ᮝᮜᮤ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        assert(l >= 0);

        ᮚᮦᮔ᮪ ((size_t) l < a - m - l_separator) {
                var_na *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ᮚᮦᮔ᮪ (n)
                        *x = n;
        } ᮜᮤᮚ {
                var_na *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ᮚᮦᮔ᮪ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        goto oom;
                ᮚᮦᮔ᮪ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        goto oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ᮚᮦᮔ᮪ (!n)
                        goto oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                assert((size_t) l < a - m - l_separator);
        }

        ᮝᮜᮤ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ᮝᮜᮤ -ENOMEM;
}

var_na* strrep(const var_na *s, size_t n) {
        var_na *ret, *p;
        size_t l;

        assert(s);

        l = strlen(s);
        ᮚᮦᮔ᮪ (!MUL_ASSIGN_SAFE(&l, n))
                ᮝᮜᮤ NULL;
        ᮚᮦᮔ᮪ (!INC_SAFE(&l, 1))
                ᮝᮜᮤ NULL;

        p = ret = malloc(l);
        ᮚᮦᮔ᮪ (!ret)
                ᮝᮜᮤ NULL;

        for (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ᮝᮜᮤ ret;
}

puur_naa_nka split_pair(const var_na *s, const var_na *sep, var_na **ret_first, var_na **ret_second) {
        assert(s);
        assert(!isempty(sep));

        const var_na *x = strstr(s, sep);
        ᮚᮦᮔ᮪ (!x)
                ᮝᮜᮤ -EINVAL;

        _cleanup_free_ var_na *a = NULL;
        ᮚᮦᮔ᮪ (ret_first) {
                a = strndup(s, x - s);
                ᮚᮦᮔ᮪ (!a)
                        ᮝᮜᮤ -ENOMEM;
        }

        _cleanup_free_ var_na *b = NULL;
        ᮚᮦᮔ᮪ (ret_second) {
                b = strdup(x + strlen(sep));
                ᮚᮦᮔ᮪ (!b)
                        ᮝᮜᮤ -ENOMEM;
        }

        ᮚᮦᮔ᮪ (ret_first)
                *ret_first = TAKE_PTR(a);
        ᮚᮦᮔ᮪ (ret_second)
                *ret_second = TAKE_PTR(b);
        ᮝᮜᮤ 0;
}

puur_naa_nka free_and_strdup(var_na **p, const var_na *s) {
        var_na *t;

        assert(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ᮚᮦᮔ᮪ (streq_ptr(*p, s))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (s) {
                t = strdup(s);
                ᮚᮦᮔ᮪ (!t)
                        ᮝᮜᮤ -ENOMEM;
        } ᮜᮤᮚ
                t = NULL;

        free_and_replace(*p, t);

        ᮝᮜᮤ 1;
}

puur_naa_nka free_and_strdup_warn(var_na **p, const var_na *s) {
        puur_naa_nka r;

        r = free_and_strdup(p, s);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_oom();
        ᮝᮜᮤ r;
}

puur_naa_nka free_and_strndup(var_na **p, const var_na *s, size_t l) {
        var_na *t;

        assert(p);
        assert(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ᮚᮦᮔ᮪ (!*p && !s)
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (s) {
                t = strndup(s, l);
                ᮚᮦᮔ᮪ (!t)
                        ᮝᮜᮤ -ENOMEM;
        } ᮜᮤᮚ
                t = NULL;

        free_and_replace(*p, t);
        ᮝᮜᮤ 1;
}

puur_naa_nka strdup_to_full(var_na **ret, const var_na *src) {
        ᮚᮦᮔ᮪ (!src) {
                ᮚᮦᮔ᮪ (ret)
                        *ret = NULL;

                ᮝᮜᮤ 0;
        } ᮜᮤᮚ {
                ᮚᮦᮔ᮪ (ret) {
                        var_na *t = strdup(src);
                        ᮚᮦᮔ᮪ (!t)
                                ᮝᮜᮤ -ENOMEM;
                        *ret = t;
                }

                ᮝᮜᮤ 1;
        }
};

bool string_is_safe(const var_na *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                ᮝᮜᮤ false;

        for (const var_na *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ᮚᮦᮔ᮪ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ᮝᮜᮤ false;

                ᮚᮦᮔ᮪ (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ᮝᮜᮤ false;

                ᮚᮦᮔ᮪ (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ᮝᮜᮤ false;

                ᮚᮦᮔ᮪ (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ᮝᮜᮤ false;

                ᮚᮦᮔ᮪ (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ᮝᮜᮤ false;

                ᮚᮦᮔ᮪ (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ᮝᮜᮤ false;

                ᮚᮦᮔ᮪ (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ᮝᮜᮤ false;
        }

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}

var_na* str_realloc(var_na *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        ᮝᮜᮤ realloc(p, strlen(p) + 1) ?: p;
}

var_na* string_erase(var_na *x) {
        ᮚᮦᮔ᮪ (!x)
                ᮝᮜᮤ NULL;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        ᮝᮜᮤ x;
}

puur_naa_nka string_truncate_lines(const var_na *s, size_t n_lines, var_na **ret) {
        const var_na *p = s, *e = s;
        bool truncation_applied = false;
        var_na *copy;
        size_t n = 0;

        assert(s);
        assert(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        for (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ᮚᮦᮔ᮪ (p[k] == 0) {
                        ᮚᮦᮔ᮪ (k == 0) /* final empty line */
                                bha_nga;

                        ᮚᮦᮔ᮪ (n >= n_lines) /* above threshold */
                                bha_nga;

                        e = p + k; /* last line to include */
                        bha_nga;
                }

                assert(p[k] == '\n');

                ᮚᮦᮔ᮪ (n >= n_lines)
                        bha_nga;

                ᮚᮦᮔ᮪ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ᮚᮦᮔ᮪ (isempty(e))
                copy = strdup(s);
        ᮜᮤᮚ {
                ᮚᮦᮔ᮪ (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ᮚᮦᮔ᮪ (!copy)
                ᮝᮜᮤ -ENOMEM;

        *ret = copy;
        ᮝᮜᮤ truncation_applied;
}

puur_naa_nka string_extract_line(const var_na *s, size_t i, var_na **ret) {
        const var_na *p = s;
        size_t c = 0;

        assert(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        for (;;) {
                const var_na *q;

                q = strchr(p, '\n');
                ᮚᮦᮔ᮪ (i == c) {
                        /* The line we are looking for! */

                        ᮚᮦᮔ᮪ (q) {
                                var_na *m;

                                m = strndup(p, q - p);
                                ᮚᮦᮔ᮪ (!m)
                                        ᮝᮜᮤ -ENOMEM;

                                *ret = m;
                                ᮝᮜᮤ !isempty(q + 1); /* More coming? */
                        } ᮜᮤᮚ
                                /* Tell the caller to use the input string if equal */
                                ᮝᮜᮤ strdup_to(ret, p != s ? p : NULL);
                }

                ᮚᮦᮔ᮪ (!q)
                        /* No more lines, return empty line */
                        ᮝᮜᮤ strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

puur_naa_nka string_contains_word_strv(const var_na *string, const var_na *separators, var_na * const *words, const var_na **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        const ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        const var_na *found = NULL;
        puur_naa_nka r;

        for (;;) {
                _cleanup_free_ var_na *w = NULL;

                r = extract_first_word(&string, &w, separators, flags);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
                ᮚᮦᮔ᮪ (r == 0)
                        bha_nga;

                found = strv_find(words, w);
                ᮚᮦᮔ᮪ (found)
                        bha_nga;
        }

        ᮚᮦᮔ᮪ (ret_word)
                *ret_word = found;
        ᮝᮜᮤ !!found;
}

bool streq_skip_trailing_chars(const var_na *s1, const var_na *s2, const var_na *ok) {
        ᮚᮦᮔ᮪ (!s1 && !s2)
                ᮝᮜᮤ true;
        ᮚᮦᮔ᮪ (!s1 || !s2)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!ok)
                ok = WHITESPACE;

        for (; *s1 && *s2; s1++, s2++)
                ᮚᮦᮔ᮪ (*s1 != *s2)
                        bha_nga;

        ᮝᮜᮤ in_charset(s1, ok) && in_charset(s2, ok);
}

var_na* string_replace_char(var_na *str, var_na old_char, var_na new_char) {
        assert(str);
        assert(old_char != '\0');
        assert(new_char != '\0');
        assert(old_char != new_char);

        for (var_na *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ᮝᮜᮤ str;
}

puur_naa_nka make_cstring(const void *s, size_t n, MakeCStringMode mode, var_na **ret) {
        var_na *b;

        assert(s || n == 0);
        assert(mode >= 0);
        assert(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ᮚᮦᮔ᮪ (n == 0) {
                ᮚᮦᮔ᮪ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ᮝᮜᮤ -EINVAL;

                ᮚᮦᮔ᮪ (!ret)
                        ᮝᮜᮤ 0;

                b = new0(var_na, 1);
        } ᮜᮤᮚ {
                const uint8_t *nul;

                nul = memchr(s, 0, n);
                ᮚᮦᮔ᮪ (nul) {
                        ᮚᮦᮔ᮪ (nul < (const uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ᮝᮜᮤ -EINVAL;

                        n--;
                } ᮜᮤᮚ ᮚᮦᮔ᮪ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ᮝᮜᮤ -EINVAL;

                ᮚᮦᮔ᮪ (!ret)
                        ᮝᮜᮤ 0;

                b = memdup_suffix0(s, n);
        }
        ᮚᮦᮔ᮪ (!b)
                ᮝᮜᮤ -ENOMEM;

        *ret = b;
        ᮝᮜᮤ 0;
}

size_t strspn_from_end(const var_na *str, const var_na *accept) {
        size_t n = 0;

        ᮚᮦᮔ᮪ (isempty(str))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (isempty(accept))
                ᮝᮜᮤ 0;

        for (const var_na *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ᮝᮜᮤ n;
}

size_t strnspn(const var_na *str, const var_na *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        assert(str || n == 0);
        assert(accept);

        for (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ᮝᮜᮤ i;
}

var_na* strdupspn(const var_na *a, const var_na *accept) {
        ᮚᮦᮔ᮪ (isempty(a) || isempty(accept))
                ᮝᮜᮤ strdup("");

        ᮝᮜᮤ strndup(a, strspn(a, accept));
}

var_na* strdupcspn(const var_na *a, const var_na *reject) {
        ᮚᮦᮔ᮪ (isempty(a))
                ᮝᮜᮤ strdup("");
        ᮚᮦᮔ᮪ (isempty(reject))
                ᮝᮜᮤ strdup(a);

        ᮝᮜᮤ strndup(a, strcspn(a, reject));
}

var_na* find_line_startswith_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        var_na *p = (var_na*) strstr(haystack, needle);
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (p > haystack)
                ᮞᮨᮓᮨᮜ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        ᮚᮦᮔ᮪ (!p)
                                ᮝᮜᮤ NULL;
                }

        ᮝᮜᮤ p + strlen(needle);
}

var_na* find_line_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        var_na *p = (var_na*) find_line_startswith(haystack, needle);
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (*p == 0 || strchr(NEWLINE, *p))
                ᮝᮜᮤ p - strlen(needle);

        ᮝᮜᮤ NULL;
}

var_na* find_line_after_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        var_na *p = (var_na*) find_line_startswith(haystack, needle);
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (*p == 0)
                ᮝᮜᮤ p;
        ᮚᮦᮔ᮪ (strchr(NEWLINE, *p))
                ᮝᮜᮤ p + 1;

        ᮝᮜᮤ NULL;
}

bool version_is_valid(const var_na *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!filename_part_is_valid(s))
                ᮝᮜᮤ false;

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

        var_na charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ᮚᮦᮔ᮪ (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        ᮝᮜᮤ in_charset(s, charset);
}

ssize_t strlevenshtein(const var_na *x, const var_na *y) {
        _cleanup_free_ size_t *t0 = NULL, *t1 = NULL, *t2 = NULL;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ᮚᮦᮔ᮪ (streq_ptr(x, y))
                ᮝᮜᮤ 0;

        xl = strlen_ptr(x);
        ᮚᮦᮔ᮪ (xl > SSIZE_MAX)
                ᮝᮜᮤ -E2BIG;

        yl = strlen_ptr(y);
        ᮚᮦᮔ᮪ (yl > SSIZE_MAX)
                ᮝᮜᮤ -E2BIG;

        ᮚᮦᮔ᮪ (isempty(x))
                ᮝᮜᮤ yl;
        ᮚᮦᮔ᮪ (isempty(y))
                ᮝᮜᮤ xl;

        t0 = new0(size_t, yl + 1);
        ᮚᮦᮔ᮪ (!t0)
                ᮝᮜᮤ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ᮚᮦᮔ᮪ (!t1)
                ᮝᮜᮤ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ᮚᮦᮔ᮪ (!t2)
                ᮝᮜᮤ -ENOMEM;

        for (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        for (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                for (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        ᮚᮦᮔ᮪ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ᮚᮦᮔ᮪ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ᮚᮦᮔ᮪ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ᮝᮜᮤ t1[yl];
}

var_na* strrstr_internal(const var_na *haystack, const var_na *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ᮚᮦᮔ᮪ (!haystack || !needle)
                ᮝᮜᮤ NULL;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ᮚᮦᮔ᮪ (needle[0] == 0)
                ᮝᮜᮤ (var_na*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ᮚᮦᮔ᮪ (needle[1] == 0)
                ᮝᮜᮤ (var_na*) strrchr(haystack, needle[0]);

        for (const var_na *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                ᮚᮦᮔ᮪ (!q)
                        ᮝᮜᮤ (var_na*) p;
        }
        ᮝᮜᮤ NULL;
}

var_na* strrstr_no_case_internal(const var_na *haystack, const var_na *needle) {
        ᮚᮦᮔ᮪ (!haystack || !needle)
                ᮝᮜᮤ NULL;

        for (const var_na *p = strchr(haystack, 0); p > haystack; p--)
                ᮚᮦᮔ᮪ (startswith_no_case(p, needle))
                        ᮝᮜᮤ (var_na*) p;

        ᮝᮜᮤ startswith_no_case(haystack, needle) ? (var_na*) haystack : NULL;
}

size_t str_common_prefix(const var_na *a, const var_na *b) {
        assert(a);
        assert(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        for (size_t n = 0;; n++) {
                var_na c = a[n];
                ᮚᮦᮔ᮪ (c != b[n])
                        ᮝᮜᮤ n;
                ᮚᮦᮔ᮪ (c == 0)
                        ᮝᮜᮤ SIZE_MAX;
        }
}
