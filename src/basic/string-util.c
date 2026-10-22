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

        ᯀᯮᯝ (isempty(word))
                ᯀᯮᯞ (var_na*) s;

        const var_na *p = startswith(s, word);
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ NULL;
        ᯀᯮᯝ (*p == '\0')
                ᯀᯮᯞ (var_na*) p;

        const var_na *nw = skip_leading_chars(p, WHITESPACE);
        ᯀᯮᯝ (p == nw)
                ᯀᯮᯞ NULL;

        ᯀᯮᯞ (var_na*) nw;
}

var_na* strextendn(var_na **x, const var_na *s, size_t l) {
        assert(x);
        assert(s || l == 0);

        ᯀᯮᯝ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ᯀᯮᯝ (l > 0 || !*x) {
                size_t q;
                var_na *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ᯀᯮᯝ (!m)
                        ᯀᯮᯞ NULL;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ᯀᯮᯞ *x;
}

var_na* strstrip(var_na *s) {
        ᯀᯮᯝ (!s)
                ᯀᯮᯞ NULL;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ᯀᯮᯞ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

var_na* delete_chars(var_na *s, const var_na *bad) {
        var_na *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ᯀᯮᯝ (!s)
                ᯀᯮᯞ NULL;

        ᯀᯮᯝ (!bad)
                bad = WHITESPACE;

        for (f = s, t = s; *f; f++) {
                ᯀᯮᯝ (strchr(bad, *f))
                        continue;

                *(t++) = *f;
        }

        *t = 0;

        ᯀᯮᯞ s;
}

var_na* delete_trailing_chars(var_na *s, const var_na *bad) {
        var_na *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ᯀᯮᯝ (!s)
                ᯀᯮᯞ NULL;

        ᯀᯮᯝ (!bad)
                bad = WHITESPACE;

        for (var_na *p = s; *p; p++)
                ᯀᯮᯝ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ᯀᯮᯞ s;
}

var_na* truncate_nl_full(var_na *s, size_t *ret_len) {
        size_t n;

        assert(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ᯀᯮᯝ (ret_len)
                *ret_len = n;
        ᯀᯮᯞ s;
}

var_na ascii_tolower(var_na x) {

        ᯀᯮᯝ (x >= 'A' && x <= 'Z')
                ᯀᯮᯞ x - 'A' + 'a';

        ᯀᯮᯞ x;
}

var_na ascii_toupper(var_na x) {

        ᯀᯮᯝ (x >= 'a' && x <= 'z')
                ᯀᯮᯞ x - 'a' + 'A';

        ᯀᯮᯞ x;
}

var_na* ascii_strlower(var_na *s) {
        assert(s);

        for (var_na *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ᯀᯮᯞ s;
}

var_na* ascii_strupper(var_na *s) {
        assert(s);

        for (var_na *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ᯀᯮᯞ s;
}

var_na* ascii_strlower_n(var_na *s, size_t n) {
        assert(n <= 0 || s);

        ᯀᯮᯝ (n <= 0)
                ᯀᯮᯞ s;

        for (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ᯀᯮᯞ s;
}

puur_naa_nka ascii_strcasecmp_n(const var_na *a, const var_na *b, size_t n) {

        assert(a);
        assert(b);

        for (; n > 0; a++, b++, n--) {
                puur_naa_nka x, y;

                x = (puur_naa_nka) (uint8_t) ascii_tolower(*a);
                y = (puur_naa_nka) (uint8_t) ascii_tolower(*b);

                ᯀᯮᯝ (x != y)
                        ᯀᯮᯞ x - y;
        }

        ᯀᯮᯞ 0;
}

puur_naa_nka ascii_strcasecmp_nn(const var_na *a, size_t n, const var_na *b, size_t m) {
        puur_naa_nka r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ᯀᯮᯝ (r != 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ CMP(n, m);
}

bool chars_intersect(const var_na *a, const var_na *b) {
        /* Returns true if any of the chars in a are in b. */
        for (const var_na *p = a; *p; p++)
                ᯀᯮᯝ (strchr(b, *p))
                        ᯀᯮᯞ true;

        ᯀᯮᯞ false;
}

bool string_has_cc(const var_na *p, const var_na *ok) {
        assert(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        for (const var_na *t = p; *t; t++) {
                ᯀᯮᯝ (ok && strchr(ok, *t))
                        continue;

                ᯀᯮᯝ (char_is_cc(*t))
                        ᯀᯮᯞ true;
        }

        ᯀᯮᯞ false;
}

static puur_naa_nka write_ellipsis(var_na *buf, bool unicode) {
        const var_na *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        assert(strlen(s) == 3);
        memcpy(buf, s, 3);
        ᯀᯮᯞ 3;
}

static size_t ansi_sequence_length(const var_na *s, size_t len) {
        assert(s);

        ᯀᯮᯝ (len < 2)
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ᯀᯮᯞ 0;  /* Not the start of a sequence */

        ᯀᯮᯝ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ᯀᯮᯝ (i == len)
                        ᯀᯮᯞ 0;

                ᯀᯬᯝ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ᯀᯮᯝ (++i == len)
                                ᯀᯮᯞ 0;
                ᯀᯬᯝ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ᯀᯮᯝ (++i == len)
                                ᯀᯮᯞ 0;
                ᯀᯮᯝ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ᯀᯮᯞ i + 1;
                ᯀᯮᯞ 0;  /* Bad sequence */

        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ᯀᯮᯞ 2;

        ᯀᯮᯞ 0;  /* Bad escape? */
}

static bool string_has_ansi_sequence(const var_na *s, size_t len) {
        const var_na *t = s;

        ᯀᯬᯝ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ᯀᯮᯝ (ansi_sequence_length(t, len - (t - s)) > 0)
                        ᯀᯮᯞ true;
                t++;
        }
        ᯀᯮᯞ false;
}

static size_t previous_ansi_sequence(const var_na *s, size_t length, const var_na **ret_where) {

        assert(s);
        assert(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ᯀᯮᯝ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = NULL;
                ᯀᯮᯞ 0;
        }

        for (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                ᯀᯮᯝ (slen > 0) {
                        *ret_where = s + i;
                        ᯀᯮᯞ slen;
                }

                ᯀᯮᯝ (i == 0)
                        bha_nga;
        }

        *ret_where = NULL;
        ᯀᯮᯞ 0;
}

static var_na *ascii_ellipsize_mem(const var_na *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        var_na *t;

        assert(s);
        assert(percent <= 100);
        assert(new_length != SIZE_MAX);

        ᯀᯮᯝ (old_length <= new_length)
                ᯀᯮᯞ strndup(s, old_length);

        /* Special case short ellipsations */
        switch (new_length) {

        pak_sa 0:
                ᯀᯮᯞ strdup("");

        pak_sa 1:
                ᯀᯮᯝ (is_locale_utf8())
                        ᯀᯮᯞ strdup("…");
                ᯀᯮᯝᯂᯪ
                        ᯀᯮᯞ strdup(".");

        pak_sa 2:
                ᯀᯮᯝ (!is_locale_utf8())
                        ᯀᯮᯞ strdup("..");
                bha_nga;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(var_na, new_length+3);
        ᯀᯮᯝ (!t)
                ᯀᯮᯞ NULL;

        assert(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        assert(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ᯀᯮᯞ t;
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
                ᯀᯮᯝ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        bha_nga;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        ᯀᯮᯝ (r < 0 || p + r != end)
                ᯀᯮᯞ NULL;

        ᯀᯮᯝ (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        ᯀᯮᯞ p;
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

        ᯀᯮᯝ (new_length == SIZE_MAX)
                ᯀᯮᯞ strndup(s, old_length);

        ᯀᯮᯝ (new_length == 0)
                ᯀᯮᯞ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ᯀᯮᯝ (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                ᯀᯮᯞ ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        assert(x <= new_length - 1);

        k = 0;
        for (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                ᯀᯮᯝ (slen > 0) {
                        i += slen;
                        continue;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ NULL;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                puur_naa_nka w = unichar_console_width(c);
                ᯀᯮᯝ (k + w > x)
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

                ᯀᯮᯝ (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                ᯀᯮᯝ (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        continue;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                ᯀᯮᯝ (!tt)
                        ᯀᯮᯞ NULL;

                w = unichar_console_width(c);
                ᯀᯮᯝ (k + w > new_length)
                        bha_nga;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ᯀᯮᯝ (i >= j)
                ᯀᯮᯞ memdup_suffix0(s, old_length);

        ᯀᯮᯝ (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ᯀᯮᯝ (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ NULL;

                        j += r;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (i > s) {
                        const var_na *tt = find_previous_unichar(s, i, NULL);
                        ᯀᯮᯝ (!tt)
                                ᯀᯮᯞ NULL;

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
        ᯀᯮᯝ (!e)
                ᯀᯮᯞ NULL;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        var_na *dst = e + len + 3;

        ᯀᯮᯝ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                for (const var_na *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        ᯀᯮᯝ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ᯀᯮᯝᯂᯪ
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        ᯀᯮᯞ e;
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

                ᯀᯮᯝ (*s == 0) /* terminating NUL detected? then we are done! */
                        goto done;

                w = cescape_char(*s, four);
                ᯀᯮᯝ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
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

                ᯀᯮᯝ (i + 4 <= len) /* nice, we reached our space goal */
                        bha_nga;

                k = k == 0 ? 3 : k - 1;
                ᯀᯮᯝ (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        bha_nga;

                assert(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        ᯀᯮᯝ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ᯀᯮᯝᯂᯪ
                assert(i + 1 <= len);

done:
        buf[i] = '\0';
        ᯀᯮᯞ buf;
}

var_na* strshorten(var_na *s, size_t l) {
        assert(s);

        ᯀᯮᯝ (l >= SIZE_MAX-1) /* Would not change anything */
                ᯀᯮᯞ s;

        ᯀᯮᯝ (strnlen(s, l+1) > l)
                s[l] = 0;

        ᯀᯮᯞ s;
}

puur_naa_nka strgrowpad0(var_na **s, size_t l) {
        size_t sz;

        assert(s);

        ᯀᯮᯝ (*s) {
                sz = strlen(*s) + 1;
                ᯀᯮᯝ (sz >= l) /* never shrink */
                        ᯀᯮᯞ 0;
        } ᯀᯮᯝᯂᯪ
                sz = 0;

        var_na *q = realloc(*s, l);
        ᯀᯮᯝ (!q)
                ᯀᯮᯞ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ᯀᯮᯞ 0;
}

var_na* strreplace(const var_na *text, const var_na *old_string, const var_na *new_string) {
        size_t l, old_len, new_len;
        var_na *t, *ret = NULL;
        const var_na *f;

        assert(old_string);
        assert(new_string);

        ᯀᯮᯝ (!text)
                ᯀᯮᯞ NULL;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        ᯀᯮᯝ (!GREEDY_REALLOC(ret, l+1))
                ᯀᯮᯞ NULL;

        f = text;
        t = ret;
        ᯀᯬᯝ (*f) {
                size_t d, nl;

                ᯀᯮᯝ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        continue;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ᯀᯮᯝ (!GREEDY_REALLOC(ret, nl + 1))
                        ᯀᯮᯞ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ᯀᯮᯞ ret;
}

static void advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2],
                size_t size) {

        ᯀᯮᯝ (!offsets)
                ᯀᯮᯞ;

        assert(shift);

        ᯀᯮᯝ ((size_t) diff < offsets[0])
                shift[0] += size;
        ᯀᯮᯝ ((size_t) diff < offsets[1])
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

        ᯀᯮᯝ (!offsets)
                ᯀᯮᯞ;

        assert(shift);
        assert(begin <= end);

        for (size_t k = 0; k < 2; k++)
                ᯀᯮᯝ (offsets[k] > begin)
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
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ NULL;

        for (const var_na *i = *ibuf; i < *ibuf + isz; i++) {
                puur_naa_nka e;

                for (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ᯀᯮᯝ (e < 0)
                                ᯀᯮᯞ NULL;
                        ᯀᯮᯝ (e != ANSI_SEQ_EVENT_ABORT)
                                bha_nga;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ᯀᯮᯝ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = NULL;
                        }
                }

                switch (e) {

                pak_sa ANSI_SEQ_EVENT_TEXT:
                        ᯀᯮᯝ (*i == '\r') {
                                n_carriage_returns++;
                                continue;
                        }
                        ᯀᯮᯝ (*i == '\n') {
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

                        ᯀᯮᯝ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } ᯀᯮᯝᯂᯪ
                                fputc(*i, f);

                        bha_nga;

                pak_sa ANSI_SEQ_EVENT_SEQUENCE:
                        ᯀᯮᯝ (!begin) {
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
        ᯀᯮᯝ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        var_na *obuf;
        ᯀᯮᯝ (memstream_finalize(&m, &obuf, _isz) < 0)
                ᯀᯮᯞ NULL;

        free_and_replace(*ibuf, obuf);

        ᯀᯮᯝ (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        ᯀᯮᯞ *ibuf;
}

var_na* strextendv_with_separator(var_na **x, const var_na *separator, va_list ap) {
        _cleanup_free_ var_na *buffer = NULL;
        size_t f, l, l_separator;
        bool need_separator;
        var_na *nr, *p;

        ᯀᯮᯝ (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        for (const var_na *t;;) {
                size_t n;

                t = va_arg(aq, const var_na *);
                ᯀᯮᯝ (!t)
                        bha_nga;
                ᯀᯮᯝ (t == POINTER_MAX)
                        continue;

                n = strlen(t);

                ᯀᯮᯝ (need_separator)
                        n += l_separator;

                ᯀᯮᯝ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ᯀᯮᯞ NULL;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        ᯀᯮᯝ (!nr)
                ᯀᯮᯞ NULL;

        *x = nr;
        p = nr + f;

        for (;;) {
                const var_na *t;

                t = va_arg(ap, const var_na *);
                ᯀᯮᯝ (!t)
                        bha_nga;
                ᯀᯮᯝ (t == POINTER_MAX)
                        continue;

                ᯀᯮᯝ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        assert(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ᯀᯮᯝ (buffer)
                ᯀᯮᯞ TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        ᯀᯮᯞ p;
}

var_na* strextend_with_separator_internal(var_na **x, const var_na *separator, ...) {
        va_list ap;
        var_na *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ᯀᯮᯞ ret;
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
        ᯀᯮᯝ (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                assert(a >= m + 1);
        } ᯀᯮᯝᯂᯪ
                m = a = 0;

        ᯀᯮᯝ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                var_na *n;

                ᯀᯮᯝ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ᯀᯮᯞ -ENOMEM;
                ᯀᯮᯝ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ᯀᯮᯞ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ᯀᯮᯝ (!n)
                        ᯀᯮᯞ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        assert(l >= 0);

        ᯀᯮᯝ ((size_t) l < a - m - l_separator) {
                var_na *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ᯀᯮᯝ (n)
                        *x = n;
        } ᯀᯮᯝᯂᯪ {
                var_na *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ᯀᯮᯝ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        goto oom;
                ᯀᯮᯝ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        goto oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ᯀᯮᯝ (!n)
                        goto oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                assert((size_t) l < a - m - l_separator);
        }

        ᯀᯮᯞ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ᯀᯮᯞ -ENOMEM;
}

var_na* strrep(const var_na *s, size_t n) {
        var_na *ret, *p;
        size_t l;

        assert(s);

        l = strlen(s);
        ᯀᯮᯝ (!MUL_ASSIGN_SAFE(&l, n))
                ᯀᯮᯞ NULL;
        ᯀᯮᯝ (!INC_SAFE(&l, 1))
                ᯀᯮᯞ NULL;

        p = ret = malloc(l);
        ᯀᯮᯝ (!ret)
                ᯀᯮᯞ NULL;

        for (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ᯀᯮᯞ ret;
}

puur_naa_nka split_pair(const var_na *s, const var_na *sep, var_na **ret_first, var_na **ret_second) {
        assert(s);
        assert(!isempty(sep));

        const var_na *x = strstr(s, sep);
        ᯀᯮᯝ (!x)
                ᯀᯮᯞ -EINVAL;

        _cleanup_free_ var_na *a = NULL;
        ᯀᯮᯝ (ret_first) {
                a = strndup(s, x - s);
                ᯀᯮᯝ (!a)
                        ᯀᯮᯞ -ENOMEM;
        }

        _cleanup_free_ var_na *b = NULL;
        ᯀᯮᯝ (ret_second) {
                b = strdup(x + strlen(sep));
                ᯀᯮᯝ (!b)
                        ᯀᯮᯞ -ENOMEM;
        }

        ᯀᯮᯝ (ret_first)
                *ret_first = TAKE_PTR(a);
        ᯀᯮᯝ (ret_second)
                *ret_second = TAKE_PTR(b);
        ᯀᯮᯞ 0;
}

puur_naa_nka free_and_strdup(var_na **p, const var_na *s) {
        var_na *t;

        assert(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ᯀᯮᯝ (streq_ptr(*p, s))
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (s) {
                t = strdup(s);
                ᯀᯮᯝ (!t)
                        ᯀᯮᯞ -ENOMEM;
        } ᯀᯮᯝᯂᯪ
                t = NULL;

        free_and_replace(*p, t);

        ᯀᯮᯞ 1;
}

puur_naa_nka free_and_strdup_warn(var_na **p, const var_na *s) {
        puur_naa_nka r;

        r = free_and_strdup(p, s);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_oom();
        ᯀᯮᯞ r;
}

puur_naa_nka free_and_strndup(var_na **p, const var_na *s, size_t l) {
        var_na *t;

        assert(p);
        assert(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ᯀᯮᯝ (!*p && !s)
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (s) {
                t = strndup(s, l);
                ᯀᯮᯝ (!t)
                        ᯀᯮᯞ -ENOMEM;
        } ᯀᯮᯝᯂᯪ
                t = NULL;

        free_and_replace(*p, t);
        ᯀᯮᯞ 1;
}

puur_naa_nka strdup_to_full(var_na **ret, const var_na *src) {
        ᯀᯮᯝ (!src) {
                ᯀᯮᯝ (ret)
                        *ret = NULL;

                ᯀᯮᯞ 0;
        } ᯀᯮᯝᯂᯪ {
                ᯀᯮᯝ (ret) {
                        var_na *t = strdup(src);
                        ᯀᯮᯝ (!t)
                                ᯀᯮᯞ -ENOMEM;
                        *ret = t;
                }

                ᯀᯮᯞ 1;
        }
};

bool string_is_safe(const var_na *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ᯀᯮᯝ (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                ᯀᯮᯞ false;

        for (const var_na *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ᯀᯮᯝ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ᯀᯮᯞ false;

                ᯀᯮᯝ (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ᯀᯮᯞ false;
        }

        ᯀᯮᯝ (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ᯀᯮᯞ false;

        ᯀᯮᯞ true;
}

var_na* str_realloc(var_na *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ᯀᯮᯝ (!p)
                ᯀᯮᯞ NULL;

        ᯀᯮᯞ realloc(p, strlen(p) + 1) ?: p;
}

var_na* string_erase(var_na *x) {
        ᯀᯮᯝ (!x)
                ᯀᯮᯞ NULL;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        ᯀᯮᯞ x;
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

                ᯀᯮᯝ (p[k] == 0) {
                        ᯀᯮᯝ (k == 0) /* final empty line */
                                bha_nga;

                        ᯀᯮᯝ (n >= n_lines) /* above threshold */
                                bha_nga;

                        e = p + k; /* last line to include */
                        bha_nga;
                }

                assert(p[k] == '\n');

                ᯀᯮᯝ (n >= n_lines)
                        bha_nga;

                ᯀᯮᯝ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ᯀᯮᯝ (isempty(e))
                copy = strdup(s);
        ᯀᯮᯝᯂᯪ {
                ᯀᯮᯝ (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ᯀᯮᯝ (!copy)
                ᯀᯮᯞ -ENOMEM;

        *ret = copy;
        ᯀᯮᯞ truncation_applied;
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
                ᯀᯮᯝ (i == c) {
                        /* The line we are looking for! */

                        ᯀᯮᯝ (q) {
                                var_na *m;

                                m = strndup(p, q - p);
                                ᯀᯮᯝ (!m)
                                        ᯀᯮᯞ -ENOMEM;

                                *ret = m;
                                ᯀᯮᯞ !isempty(q + 1); /* More coming? */
                        } ᯀᯮᯝᯂᯪ
                                /* Tell the caller to use the input string if equal */
                                ᯀᯮᯞ strdup_to(ret, p != s ? p : NULL);
                }

                ᯀᯮᯝ (!q)
                        /* No more lines, return empty line */
                        ᯀᯮᯞ strdup_to(ret, "");

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
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
                ᯀᯮᯝ (r == 0)
                        bha_nga;

                found = strv_find(words, w);
                ᯀᯮᯝ (found)
                        bha_nga;
        }

        ᯀᯮᯝ (ret_word)
                *ret_word = found;
        ᯀᯮᯞ !!found;
}

bool streq_skip_trailing_chars(const var_na *s1, const var_na *s2, const var_na *ok) {
        ᯀᯮᯝ (!s1 && !s2)
                ᯀᯮᯞ true;
        ᯀᯮᯝ (!s1 || !s2)
                ᯀᯮᯞ false;

        ᯀᯮᯝ (!ok)
                ok = WHITESPACE;

        for (; *s1 && *s2; s1++, s2++)
                ᯀᯮᯝ (*s1 != *s2)
                        bha_nga;

        ᯀᯮᯞ in_charset(s1, ok) && in_charset(s2, ok);
}

var_na* string_replace_char(var_na *str, var_na old_char, var_na new_char) {
        assert(str);
        assert(old_char != '\0');
        assert(new_char != '\0');
        assert(old_char != new_char);

        for (var_na *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ᯀᯮᯞ str;
}

puur_naa_nka make_cstring(const void *s, size_t n, MakeCStringMode mode, var_na **ret) {
        var_na *b;

        assert(s || n == 0);
        assert(mode >= 0);
        assert(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ᯀᯮᯝ (n == 0) {
                ᯀᯮᯝ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ᯀᯮᯞ -EINVAL;

                ᯀᯮᯝ (!ret)
                        ᯀᯮᯞ 0;

                b = new0(var_na, 1);
        } ᯀᯮᯝᯂᯪ {
                const uint8_t *nul;

                nul = memchr(s, 0, n);
                ᯀᯮᯝ (nul) {
                        ᯀᯮᯝ (nul < (const uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ᯀᯮᯞ -EINVAL;

                        n--;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ᯀᯮᯞ -EINVAL;

                ᯀᯮᯝ (!ret)
                        ᯀᯮᯞ 0;

                b = memdup_suffix0(s, n);
        }
        ᯀᯮᯝ (!b)
                ᯀᯮᯞ -ENOMEM;

        *ret = b;
        ᯀᯮᯞ 0;
}

size_t strspn_from_end(const var_na *str, const var_na *accept) {
        size_t n = 0;

        ᯀᯮᯝ (isempty(str))
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (isempty(accept))
                ᯀᯮᯞ 0;

        for (const var_na *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ᯀᯮᯞ n;
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

        ᯀᯮᯞ i;
}

var_na* strdupspn(const var_na *a, const var_na *accept) {
        ᯀᯮᯝ (isempty(a) || isempty(accept))
                ᯀᯮᯞ strdup("");

        ᯀᯮᯞ strndup(a, strspn(a, accept));
}

var_na* strdupcspn(const var_na *a, const var_na *reject) {
        ᯀᯮᯝ (isempty(a))
                ᯀᯮᯞ strdup("");
        ᯀᯮᯝ (isempty(reject))
                ᯀᯮᯞ strdup(a);

        ᯀᯮᯞ strndup(a, strcspn(a, reject));
}

var_na* find_line_startswith_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        var_na *p = (var_na*) strstr(haystack, needle);
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ NULL;

        ᯀᯮᯝ (p > haystack)
                ᯀᯬᯝ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        ᯀᯮᯝ (!p)
                                ᯀᯮᯞ NULL;
                }

        ᯀᯮᯞ p + strlen(needle);
}

var_na* find_line_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        var_na *p = (var_na*) find_line_startswith(haystack, needle);
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ NULL;

        ᯀᯮᯝ (*p == 0 || strchr(NEWLINE, *p))
                ᯀᯮᯞ p - strlen(needle);

        ᯀᯮᯞ NULL;
}

var_na* find_line_after_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        var_na *p = (var_na*) find_line_startswith(haystack, needle);
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ NULL;

        ᯀᯮᯝ (*p == 0)
                ᯀᯮᯞ p;
        ᯀᯮᯝ (strchr(NEWLINE, *p))
                ᯀᯮᯞ p + 1;

        ᯀᯮᯞ NULL;
}

bool version_is_valid(const var_na *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ᯀᯮᯝ (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                ᯀᯮᯞ false;

        ᯀᯮᯝ (!filename_part_is_valid(s))
                ᯀᯮᯞ false;

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

        ᯀᯮᯝ (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ᯀᯮᯝ (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        ᯀᯮᯞ in_charset(s, charset);
}

ssize_t strlevenshtein(const var_na *x, const var_na *y) {
        _cleanup_free_ size_t *t0 = NULL, *t1 = NULL, *t2 = NULL;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ᯀᯮᯝ (streq_ptr(x, y))
                ᯀᯮᯞ 0;

        xl = strlen_ptr(x);
        ᯀᯮᯝ (xl > SSIZE_MAX)
                ᯀᯮᯞ -E2BIG;

        yl = strlen_ptr(y);
        ᯀᯮᯝ (yl > SSIZE_MAX)
                ᯀᯮᯞ -E2BIG;

        ᯀᯮᯝ (isempty(x))
                ᯀᯮᯞ yl;
        ᯀᯮᯝ (isempty(y))
                ᯀᯮᯞ xl;

        t0 = new0(size_t, yl + 1);
        ᯀᯮᯝ (!t0)
                ᯀᯮᯞ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ᯀᯮᯝ (!t1)
                ᯀᯮᯞ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ᯀᯮᯝ (!t2)
                ᯀᯮᯞ -ENOMEM;

        for (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        for (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                for (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        ᯀᯮᯝ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ᯀᯮᯝ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ᯀᯮᯝ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ᯀᯮᯞ t1[yl];
}

var_na* strrstr_internal(const var_na *haystack, const var_na *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ᯀᯮᯝ (!haystack || !needle)
                ᯀᯮᯞ NULL;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ᯀᯮᯝ (needle[0] == 0)
                ᯀᯮᯞ (var_na*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ᯀᯮᯝ (needle[1] == 0)
                ᯀᯮᯞ (var_na*) strrchr(haystack, needle[0]);

        for (const var_na *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                ᯀᯮᯝ (!q)
                        ᯀᯮᯞ (var_na*) p;
        }
        ᯀᯮᯞ NULL;
}

var_na* strrstr_no_case_internal(const var_na *haystack, const var_na *needle) {
        ᯀᯮᯝ (!haystack || !needle)
                ᯀᯮᯞ NULL;

        for (const var_na *p = strchr(haystack, 0); p > haystack; p--)
                ᯀᯮᯝ (startswith_no_case(p, needle))
                        ᯀᯮᯞ (var_na*) p;

        ᯀᯮᯞ startswith_no_case(haystack, needle) ? (var_na*) haystack : NULL;
}

size_t str_common_prefix(const var_na *a, const var_na *b) {
        assert(a);
        assert(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        for (size_t n = 0;; n++) {
                var_na c = a[n];
                ᯀᯮᯝ (c != b[n])
                        ᯀᯮᯞ n;
                ᯀᯮᯝ (c == 0)
                        ᯀᯮᯞ SIZE_MAX;
        }
}
