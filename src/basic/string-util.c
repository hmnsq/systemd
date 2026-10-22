/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "ansi-seq.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "glyph-util.h"
#અન્તર્ભાવ "gunicode.h"
#અન્તર્ભાવ "locale-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "memstream-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "utf8.h"

અક્ષર* first_word(અચલ અક્ષર *s, અચલ અક્ષર *word) {
        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        જો (isempty(word))
                પાછા (અક્ષર*) s;

        અચલ અક્ષર *p = startswith(s, word);
        જો (!p)
                પાછા ખાલી;
        જો (*p == '\0')
                પાછા (અક્ષર*) p;

        અચલ અક્ષર *nw = skip_leading_chars(p, WHITESPACE);
        જો (p == nw)
                પાછા ખાલી;

        પાછા (અક્ષર*) nw;
}

અક્ષર* strextendn(અક્ષર **x, અચલ અક્ષર *s, size_t l) {
        પ્રતિજ્ઞા(x);
        પ્રતિજ્ઞા(s || l == 0);

        જો (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        જો (l > 0 || !*x) {
                size_t q;
                અક્ષર *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                જો (!m)
                        પાછા ખાલી;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        પાછા *x;
}

અક્ષર* strstrip(અક્ષર *s) {
        જો (!s)
                પાછા ખાલી;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        પાછા delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

અક્ષર* delete_chars(અક્ષર *s, અચલ અક્ષર *bad) {
        અક્ષર *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        જો (!s)
                પાછા ખાલી;

        જો (!bad)
                bad = WHITESPACE;

        માટે (f = s, t = s; *f; f++) {
                જો (strchr(bad, *f))
                        ચાલુ;

                *(t++) = *f;
        }

        *t = 0;

        પાછા s;
}

અક્ષર* delete_trailing_chars(અક્ષર *s, અચલ અક્ષર *bad) {
        અક્ષર *c = s;

        /* Drops all specified bad characters, at the end of the string */

        જો (!s)
                પાછા ખાલી;

        જો (!bad)
                bad = WHITESPACE;

        માટે (અક્ષર *p = s; *p; p++)
                જો (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        પાછા s;
}

અક્ષર* truncate_nl_full(અક્ષર *s, size_t *ret_len) {
        size_t n;

        પ્રતિજ્ઞા(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        જો (ret_len)
                *ret_len = n;
        પાછા s;
}

અક્ષર ascii_tolower(અક્ષર x) {

        જો (x >= 'A' && x <= 'Z')
                પાછા x - 'A' + 'a';

        પાછા x;
}

અક્ષર ascii_toupper(અક્ષર x) {

        જો (x >= 'a' && x <= 'z')
                પાછા x - 'a' + 'A';

        પાછા x;
}

અક્ષર* ascii_strlower(અક્ષર *s) {
        પ્રતિજ્ઞા(s);

        માટે (અક્ષર *p = s; *p; p++)
                *p = ascii_tolower(*p);

        પાછા s;
}

અક્ષર* ascii_strupper(અક્ષર *s) {
        પ્રતિજ્ઞા(s);

        માટે (અક્ષર *p = s; *p; p++)
                *p = ascii_toupper(*p);

        પાછા s;
}

અક્ષર* ascii_strlower_n(અક્ષર *s, size_t n) {
        પ્રતિજ્ઞા(n <= 0 || s);

        જો (n <= 0)
                પાછા s;

        માટે (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        પાછા s;
}

પૂર્ણાંક ascii_strcasecmp_n(અચલ અક્ષર *a, અચલ અક્ષર *b, size_t n) {

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        માટે (; n > 0; a++, b++, n--) {
                પૂર્ણાંક x, y;

                x = (પૂર્ણાંક) (uint8_t) ascii_tolower(*a);
                y = (પૂર્ણાંક) (uint8_t) ascii_tolower(*b);

                જો (x != y)
                        પાછા x - y;
        }

        પાછા 0;
}

પૂર્ણાંક ascii_strcasecmp_nn(અચલ અક્ષર *a, size_t n, અચલ અક્ષર *b, size_t m) {
        પૂર્ણાંક r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        જો (r != 0)
                પાછા r;

        પાછા CMP(n, m);
}

bool chars_intersect(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        /* Returns true if any of the chars in a are in b. */
        માટે (અચલ અક્ષર *p = a; *p; p++)
                જો (strchr(b, *p))
                        પાછા true;

        પાછા false;
}

bool string_has_cc(અચલ અક્ષર *p, અચલ અક્ષર *ok) {
        પ્રતિજ્ઞા(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        માટે (અચલ અક્ષર *t = p; *t; t++) {
                જો (ok && strchr(ok, *t))
                        ચાલુ;

                જો (char_is_cc(*t))
                        પાછા true;
        }

        પાછા false;
}

સ્થિર પૂર્ણાંક write_ellipsis(અક્ષર *buf, bool unicode) {
        અચલ અક્ષર *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        પ્રતિજ્ઞા(strlen(s) == 3);
        memcpy(buf, s, 3);
        પાછા 3;
}

સ્થિર size_t ansi_sequence_length(અચલ અક્ષર *s, size_t len) {
        પ્રતિજ્ઞા(s);

        જો (len < 2)
                પાછા 0;

        જો (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                પાછા 0;  /* Not the start of a sequence */

        જો (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                જો (i == len)
                        પાછા 0;

                જ્યારે (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        જો (++i == len)
                                પાછા 0;
                જ્યારે (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        જો (++i == len)
                                પાછા 0;
                જો (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        પાછા i + 1;
                પાછા 0;  /* Bad sequence */

        } નહીંતર જો (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                પાછા 2;

        પાછા 0;  /* Bad escape? */
}

સ્થિર bool string_has_ansi_sequence(અચલ અક્ષર *s, size_t len) {
        અચલ અક્ષર *t = s;

        જ્યારે ((t = memchr(t, 0x1B, len - (t - s)))) {
                જો (ansi_sequence_length(t, len - (t - s)) > 0)
                        પાછા true;
                t++;
        }
        પાછા false;
}

સ્થિર size_t previous_ansi_sequence(અચલ અક્ષર *s, size_t length, અચલ અક્ષર **ret_where) {

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        જો (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ખાલી;
                પાછા 0;
        }

        માટે (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                જો (slen > 0) {
                        *ret_where = s + i;
                        પાછા slen;
                }

                જો (i == 0)
                        તોડો;
        }

        *ret_where = ખાલી;
        પાછા 0;
}

સ્થિર અક્ષર *ascii_ellipsize_mem(અચલ અક્ષર *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        અક્ષર *t;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(percent <= 100);
        પ્રતિજ્ઞા(new_length != SIZE_MAX);

        જો (old_length <= new_length)
                પાછા strndup(s, old_length);

        /* Special case short ellipsations */
        પસંદગી (new_length) {

        સ્થિતિ 0:
                પાછા strdup("");

        સ્થિતિ 1:
                જો (is_locale_utf8())
                        પાછા strdup("…");
                નહીંતર
                        પાછા strdup(".");

        સ્થિતિ 2:
                જો (!is_locale_utf8())
                        પાછા strdup("..");
                તોડો;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(અક્ષર, new_length+3);
        જો (!t)
                પાછા ખાલી;

        પ્રતિજ્ઞા(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        પ્રતિજ્ઞા(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        પાછા t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
સ્થિર અચલ અક્ષર* find_previous_unichar(અચલ અક્ષર *start, અચલ અક્ષર *end, char32_t *ret_c) {
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(start);
        પ્રતિજ્ઞા(end);
        પ્રતિજ્ઞા(end > start);

        માટે (p = end; p > start; ) {
                p--;
                જો (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        તોડો;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        જો (r < 0 || p + r != end)
                પાછા ખાલી;

        જો (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        પાછા p;
}

અક્ષર* ellipsize_mem(અચલ અક્ષર *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        અચલ અક્ષર *i, *j;
        પૂર્ણાંક r;

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

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(percent <= 100);

        જો (new_length == SIZE_MAX)
                પાછા strndup(s, old_length);

        જો (new_length == 0)
                પાછા strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        જો (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                પાછા ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        પ્રતિજ્ઞા(x <= new_length - 1);

        k = 0;
        માટે (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                જો (slen > 0) {
                        i += slen;
                        ચાલુ;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                જો (r < 0)
                        પાછા ખાલી;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                પૂર્ણાંક w = unichar_console_width(c);
                જો (k + w > x)
                        તોડો;

                k += w;
                i += r;
        }

        અચલ અક્ષર *ansi_start = s + old_length;
        size_t ansi_len = 0;

        માટે (અચલ અક્ષર *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                પૂર્ણાંક w;
                અચલ અક્ષર *tt;

                જો (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                જો (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        ચાલુ;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                જો (!tt)
                        પાછા ખાલી;

                w = unichar_console_width(c);
                જો (k + w > new_length)
                        તોડો;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        જો (i >= j)
                પાછા memdup_suffix0(s, old_length);

        જો (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                જો (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        જો (r < 0)
                                પાછા ખાલી;

                        j += r;
                } નહીંતર જો (i > s) {
                        અચલ અક્ષર *tt = find_previous_unichar(s, i, ખાલી);
                        જો (!tt)
                                પાછા ખાલી;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        અક્ષર *e = new(અક્ષર, alloc_len);
        જો (!e)
                પાછા ખાલી;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        અક્ષર *dst = e + len + 3;

        જો (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                માટે (અચલ અક્ષર *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        જો (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } નહીંતર
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        પાછા e;
}

અક્ષર* cellescape(અક્ષર *buf, size_t len, અચલ અક્ષર *s) {
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

        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(len > 0); /* at least a terminating NUL */
        પ્રતિજ્ઞા(s);

        માટે (;;) {
                અક્ષર four[4];
                પૂર્ણાંક w;

                જો (*s == 0) /* terminating NUL detected? then we are done! */
                        જાઓ done;

                w = cescape_char(*s, four);
                જો (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        તોડો;

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
        માટે (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                જો (i + 4 <= len) /* nice, we reached our space goal */
                        તોડો;

                k = k == 0 ? 3 : k - 1;
                જો (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        તોડો;

                પ્રતિજ્ઞા(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        જો (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        નહીંતર જો (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } નહીંતર જો (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        નહીંતર
                પ્રતિજ્ઞા(i + 1 <= len);

done:
        buf[i] = '\0';
        પાછા buf;
}

અક્ષર* strshorten(અક્ષર *s, size_t l) {
        પ્રતિજ્ઞા(s);

        જો (l >= SIZE_MAX-1) /* Would not change anything */
                પાછા s;

        જો (strnlen(s, l+1) > l)
                s[l] = 0;

        પાછા s;
}

પૂર્ણાંક strgrowpad0(અક્ષર **s, size_t l) {
        size_t sz;

        પ્રતિજ્ઞા(s);

        જો (*s) {
                sz = strlen(*s) + 1;
                જો (sz >= l) /* never shrink */
                        પાછા 0;
        } નહીંતર
                sz = 0;

        અક્ષર *q = realloc(*s, l);
        જો (!q)
                પાછા -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        પાછા 0;
}

અક્ષર* strreplace(અચલ અક્ષર *text, અચલ અક્ષર *old_string, અચલ અક્ષર *new_string) {
        size_t l, old_len, new_len;
        અક્ષર *t, *ret = ખાલી;
        અચલ અક્ષર *f;

        પ્રતિજ્ઞા(old_string);
        પ્રતિજ્ઞા(new_string);

        જો (!text)
                પાછા ખાલી;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        જો (!GREEDY_REALLOC(ret, l+1))
                પાછા ખાલી;

        f = text;
        t = ret;
        જ્યારે (*f) {
                size_t d, nl;

                જો (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        ચાલુ;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                જો (!GREEDY_REALLOC(ret, nl + 1))
                        પાછા mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        પાછા ret;
}

સ્થિર શૂન્ય advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[સ્થિર 2],
                size_t size) {

        જો (!offsets)
                પાછા;

        પ્રતિજ્ઞા(shift);

        જો ((size_t) diff < offsets[0])
                shift[0] += size;
        જો ((size_t) diff < offsets[1])
                shift[1] += size;
}

સ્થિર શૂન્ય retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[સ્થિર 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        જો (!offsets)
                પાછા;

        પ્રતિજ્ઞા(shift);
        પ્રતિજ્ઞા(begin <= end);

        માટે (size_t k = 0; k < 2; k++)
                જો (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

અક્ષર* strip_tab_ansi(અક્ષર **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        અચલ અક્ષર *begin = ખાલી;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        પ્રતિજ્ઞા(ibuf);
        પ્રતિજ્ઞા(*ibuf);
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
        જો (!f)
                પાછા ખાલી;

        માટે (અચલ અક્ષર *i = *ibuf; i < *ibuf + isz; i++) {
                પૂર્ણાંક e;

                માટે (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        જો (e < 0)
                                પાછા ખાલી;
                        જો (e != ANSI_SEQ_EVENT_ABORT)
                                તોડો;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        જો (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = ખાલી;
                        }
                }

                પસંદગી (e) {

                સ્થિતિ ANSI_SEQ_EVENT_TEXT:
                        જો (*i == '\r') {
                                n_carriage_returns++;
                                ચાલુ;
                        }
                        જો (*i == '\n') {
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
                        માટે (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        જો (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } નહીંતર
                                fputc(*i, f);

                        તોડો;

                સ્થિતિ ANSI_SEQ_EVENT_SEQUENCE:
                        જો (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                માટે (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        તોડો;

                સ્થિતિ ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        પ્રતિજ્ઞા(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = ખાલી;
                        તોડો;

                મૂળભૂત:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        જો (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        અક્ષર *obuf;
        જો (memstream_finalize(&m, &obuf, _isz) < 0)
                પાછા ખાલી;

        free_and_replace(*ibuf, obuf);

        જો (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        પાછા *ibuf;
}

અક્ષર* strextendv_with_separator(અક્ષર **x, અચલ અક્ષર *separator, va_list ap) {
        _cleanup_free_ અક્ષર *buffer = ખાલી;
        size_t f, l, l_separator;
        bool need_separator;
        અક્ષર *nr, *p;

        જો (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        માટે (અચલ અક્ષર *t;;) {
                size_t n;

                t = va_arg(aq, અચલ અક્ષર *);
                જો (!t)
                        તોડો;
                જો (t == POINTER_MAX)
                        ચાલુ;

                n = strlen(t);

                જો (need_separator)
                        n += l_separator;

                જો (n >= SIZE_MAX - l) {
                        va_end(aq);
                        પાછા ખાલી;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        જો (!nr)
                પાછા ખાલી;

        *x = nr;
        p = nr + f;

        માટે (;;) {
                અચલ અક્ષર *t;

                t = va_arg(ap, અચલ અક્ષર *);
                જો (!t)
                        તોડો;
                જો (t == POINTER_MAX)
                        ચાલુ;

                જો (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        પ્રતિજ્ઞા(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        જો (buffer)
                પાછા TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        પાછા p;
}

અક્ષર* strextend_with_separator_internal(અક્ષર **x, અચલ અક્ષર *separator, ...) {
        va_list ap;
        અક્ષર *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        પાછા ret;
}

પૂર્ણાંક strextendf_with_separator(અક્ષર **x, અચલ અક્ષર *separator, અચલ અક્ષર *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        પૂર્ણાંક l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        પ્રતિજ્ઞા(x);
        પ્રતિજ્ઞા(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        જો (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                પ્રતિજ્ઞા(a >= m + 1);
        } નહીંતર
                m = a = 0;

        જો (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                અક્ષર *n;

                જો (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        પાછા -ENOMEM;
                જો (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        પાછા -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                જો (!n)
                        પાછા -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        પ્રતિજ્ઞા(l >= 0);

        જો ((size_t) l < a - m - l_separator) {
                અક્ષર *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                જો (n)
                        *x = n;
        } નહીંતર {
                અક્ષર *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                જો (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        જાઓ oom;
                જો (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        જાઓ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                જો (!n)
                        જાઓ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                પ્રતિજ્ઞા((size_t) l < a - m - l_separator);
        }

        પાછા 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        પાછા -ENOMEM;
}

અક્ષર* strrep(અચલ અક્ષર *s, size_t n) {
        અક્ષર *ret, *p;
        size_t l;

        પ્રતિજ્ઞા(s);

        l = strlen(s);
        જો (!MUL_ASSIGN_SAFE(&l, n))
                પાછા ખાલી;
        જો (!INC_SAFE(&l, 1))
                પાછા ખાલી;

        p = ret = malloc(l);
        જો (!ret)
                પાછા ખાલી;

        માટે (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        પાછા ret;
}

પૂર્ણાંક split_pair(અચલ અક્ષર *s, અચલ અક્ષર *sep, અક્ષર **ret_first, અક્ષર **ret_second) {
        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(!isempty(sep));

        અચલ અક્ષર *x = strstr(s, sep);
        જો (!x)
                પાછા -EINVAL;

        _cleanup_free_ અક્ષર *a = ખાલી;
        જો (ret_first) {
                a = strndup(s, x - s);
                જો (!a)
                        પાછા -ENOMEM;
        }

        _cleanup_free_ અક્ષર *b = ખાલી;
        જો (ret_second) {
                b = strdup(x + strlen(sep));
                જો (!b)
                        પાછા -ENOMEM;
        }

        જો (ret_first)
                *ret_first = TAKE_PTR(a);
        જો (ret_second)
                *ret_second = TAKE_PTR(b);
        પાછા 0;
}

પૂર્ણાંક free_and_strdup(અક્ષર **p, અચલ અક્ષર *s) {
        અક્ષર *t;

        પ્રતિજ્ઞા(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        જો (streq_ptr(*p, s))
                પાછા 0;

        જો (s) {
                t = strdup(s);
                જો (!t)
                        પાછા -ENOMEM;
        } નહીંતર
                t = ખાલી;

        free_and_replace(*p, t);

        પાછા 1;
}

પૂર્ણાંક free_and_strdup_warn(અક્ષર **p, અચલ અક્ષર *s) {
        પૂર્ણાંક r;

        r = free_and_strdup(p, s);
        જો (r < 0)
                પાછા log_oom();
        પાછા r;
}

પૂર્ણાંક free_and_strndup(અક્ષર **p, અચલ અક્ષર *s, size_t l) {
        અક્ષર *t;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        જો (!*p && !s)
                પાછા 0;

        જો (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                પાછા 0;

        જો (s) {
                t = strndup(s, l);
                જો (!t)
                        પાછા -ENOMEM;
        } નહીંતર
                t = ખાલી;

        free_and_replace(*p, t);
        પાછા 1;
}

પૂર્ણાંક strdup_to_full(અક્ષર **ret, અચલ અક્ષર *src) {
        જો (!src) {
                જો (ret)
                        *ret = ખાલી;

                પાછા 0;
        } નહીંતર {
                જો (ret) {
                        અક્ષર *t = strdup(src);
                        જો (!t)
                                પાછા -ENOMEM;
                        *ret = t;
                }

                પાછા 1;
        }
};

bool string_is_safe(અચલ અક્ષર *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        જો (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                પાછા false;

        જો (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                પાછા false;

        માટે (અચલ અક્ષર *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                જો ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        પાછા false;

                જો (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        પાછા false;

                જો (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        પાછા false;

                જો (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        પાછા false;

                જો (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        પાછા false;

                જો (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        પાછા false;

                જો (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        પાછા false;
        }

        જો (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                પાછા false;

        જો (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                પાછા false;

        પાછા true;
}

અક્ષર* str_realloc(અક્ષર *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        જો (!p)
                પાછા ખાલી;

        પાછા realloc(p, strlen(p) + 1) ?: p;
}

અક્ષર* string_erase(અક્ષર *x) {
        જો (!x)
                પાછા ખાલી;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        પાછા x;
}

પૂર્ણાંક string_truncate_લીટીઓ(અચલ અક્ષર *s, size_t n_લીટીઓ, અક્ષર **ret) {
        અચલ અક્ષર *p = s, *e = s;
        bool truncation_applied = false;
        અક્ષર *copy;
        size_t n = 0;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        માટે (;;) {
                size_t k;

                k = strcspn(p, "\n");

                જો (p[k] == 0) {
                        જો (k == 0) /* final empty line */
                                તોડો;

                        જો (n >= n_લીટીઓ) /* above threshold */
                                તોડો;

                        e = p + k; /* last line to include */
                        તોડો;
                }

                પ્રતિજ્ઞા(p[k] == '\n');

                જો (n >= n_લીટીઓ)
                        તોડો;

                જો (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        જો (isempty(e))
                copy = strdup(s);
        નહીંતર {
                જો (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        જો (!copy)
                પાછા -ENOMEM;

        *ret = copy;
        પાછા truncation_applied;
}

પૂર્ણાંક string_extract_લીટી(અચલ અક્ષર *s, size_t i, અક્ષર **ret) {
        અચલ અક્ષર *p = s;
        size_t c = 0;

        પ્રતિજ્ઞા(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        માટે (;;) {
                અચલ અક્ષર *q;

                q = strchr(p, '\n');
                જો (i == c) {
                        /* The line we are looking for! */

                        જો (q) {
                                અક્ષર *m;

                                m = strndup(p, q - p);
                                જો (!m)
                                        પાછા -ENOMEM;

                                *ret = m;
                                પાછા !isempty(q + 1); /* More coming? */
                        } નહીંતર
                                /* Tell the caller to use the input string if equal */
                                પાછા strdup_to(ret, p != s ? p : ખાલી);
                }

                જો (!q)
                        /* No more lines, return empty line */
                        પાછા strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

પૂર્ણાંક string_contains_word_strv(અચલ અક્ષર *string, અચલ અક્ષર *separators, અક્ષર * અચલ *શબ્દો, અચલ અક્ષર **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        અચલ ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        અચલ અક્ષર *found = ખાલી;
        પૂર્ણાંક r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *w = ખાલી;

                r = extract_first_word(&string, &w, separators, flags);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                found = strv_find(શબ્દો, w);
                જો (found)
                        તોડો;
        }

        જો (ret_word)
                *ret_word = found;
        પાછા !!found;
}

bool streq_skip_trailing_chars(અચલ અક્ષર *s1, અચલ અક્ષર *s2, અચલ અક્ષર *ok) {
        જો (!s1 && !s2)
                પાછા true;
        જો (!s1 || !s2)
                પાછા false;

        જો (!ok)
                ok = WHITESPACE;

        માટે (; *s1 && *s2; s1++, s2++)
                જો (*s1 != *s2)
                        તોડો;

        પાછા in_charset(s1, ok) && in_charset(s2, ok);
}

અક્ષર* string_replace_char(અક્ષર *str, અક્ષર old_char, અક્ષર new_char) {
        પ્રતિજ્ઞા(str);
        પ્રતિજ્ઞા(old_char != '\0');
        પ્રતિજ્ઞા(new_char != '\0');
        પ્રતિજ્ઞા(old_char != new_char);

        માટે (અક્ષર *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        પાછા str;
}

પૂર્ણાંક make_cstring(અચલ શૂન્ય *s, size_t n, MakeCStringMode mode, અક્ષર **ret) {
        અક્ષર *b;

        પ્રતિજ્ઞા(s || n == 0);
        પ્રતિજ્ઞા(mode >= 0);
        પ્રતિજ્ઞા(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        જો (n == 0) {
                જો (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        પાછા -EINVAL;

                જો (!ret)
                        પાછા 0;

                b = new0(અક્ષર, 1);
        } નહીંતર {
                અચલ uint8_t *nul;

                nul = memchr(s, 0, n);
                જો (nul) {
                        જો (nul < (અચલ uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                પાછા -EINVAL;

                        n--;
                } નહીંતર જો (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        પાછા -EINVAL;

                જો (!ret)
                        પાછા 0;

                b = memdup_suffix0(s, n);
        }
        જો (!b)
                પાછા -ENOMEM;

        *ret = b;
        પાછા 0;
}

size_t strspn_from_end(અચલ અક્ષર *str, અચલ અક્ષર *accept) {
        size_t n = 0;

        જો (isempty(str))
                પાછા 0;

        જો (isempty(accept))
                પાછા 0;

        માટે (અચલ અક્ષર *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        પાછા n;
}

size_t strnspn(અચલ અક્ષર *str, અચલ અક્ષર *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        પ્રતિજ્ઞા(str || n == 0);
        પ્રતિજ્ઞા(accept);

        માટે (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        પાછા i;
}

અક્ષર* strdupspn(અચલ અક્ષર *a, અચલ અક્ષર *accept) {
        જો (isempty(a) || isempty(accept))
                પાછા strdup("");

        પાછા strndup(a, strspn(a, accept));
}

અક્ષર* strdupcspn(અચલ અક્ષર *a, અચલ અક્ષર *reject) {
        જો (isempty(a))
                પાછા strdup("");
        જો (isempty(reject))
                પાછા strdup(a);

        પાછા strndup(a, strcspn(a, reject));
}

અક્ષર* find_લીટી_startswith_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        પ્રતિજ્ઞા(haystack);
        પ્રતિજ્ઞા(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        અક્ષર *p = (અક્ષર*) strstr(haystack, needle);
        જો (!p)
                પાછા ખાલી;

        જો (p > haystack)
                જ્યારે (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        જો (!p)
                                પાછા ખાલી;
                }

        પાછા p + strlen(needle);
}

અક્ષર* find_લીટી_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        પ્રતિજ્ઞા(haystack);
        પ્રતિજ્ઞા(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        અક્ષર *p = (અક્ષર*) find_લીટી_startswith(haystack, needle);
        જો (!p)
                પાછા ખાલી;

        જો (*p == 0 || strchr(NEWLINE, *p))
                પાછા p - strlen(needle);

        પાછા ખાલી;
}

અક્ષર* find_લીટી_after_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        પ્રતિજ્ઞા(haystack);
        પ્રતિજ્ઞા(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        અક્ષર *p = (અક્ષર*) find_લીટી_startswith(haystack, needle);
        જો (!p)
                પાછા ખાલી;

        જો (*p == 0)
                પાછા p;
        જો (strchr(NEWLINE, *p))
                પાછા p + 1;

        પાછા ખાલી;
}

bool version_is_valid(અચલ અક્ષર *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        જો (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                પાછા false;

        જો (!filename_part_is_valid(s))
                પાછા false;

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

        અક્ષર charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        જો (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        જો (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        પાછા in_charset(s, charset);
}

ssize_t strlevenshtein(અચલ અક્ષર *x, અચલ અક્ષર *y) {
        _cleanup_free_ size_t *t0 = ખાલી, *t1 = ખાલી, *t2 = ખાલી;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        જો (streq_ptr(x, y))
                પાછા 0;

        xl = strlen_ptr(x);
        જો (xl > SSIZE_MAX)
                પાછા -E2BIG;

        yl = strlen_ptr(y);
        જો (yl > SSIZE_MAX)
                પાછા -E2BIG;

        જો (isempty(x))
                પાછા yl;
        જો (isempty(y))
                પાછા xl;

        t0 = new0(size_t, yl + 1);
        જો (!t0)
                પાછા -ENOMEM;
        t1 = new0(size_t, yl + 1);
        જો (!t1)
                પાછા -ENOMEM;
        t2 = new0(size_t, yl + 1);
        જો (!t2)
                પાછા -ENOMEM;

        માટે (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        માટે (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                માટે (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        જો (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        જો (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        જો (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        પાછા t1[yl];
}

અક્ષર* strrstr_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        જો (!haystack || !needle)
                પાછા ખાલી;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        જો (needle[0] == 0)
                પાછા (અક્ષર*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        જો (needle[1] == 0)
                પાછા (અક્ષર*) strrchr(haystack, needle[0]);

        માટે (અચલ અક્ષર *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                જો (!q)
                        પાછા (અક્ષર*) p;
        }
        પાછા ખાલી;
}

અક્ષર* strrstr_no_case_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        જો (!haystack || !needle)
                પાછા ખાલી;

        માટે (અચલ અક્ષર *p = strchr(haystack, 0); p > haystack; p--)
                જો (startswith_no_case(p, needle))
                        પાછા (અક્ષર*) p;

        પાછા startswith_no_case(haystack, needle) ? (અક્ષર*) haystack : ખાલી;
}

size_t str_common_prefix(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        માટે (size_t n = 0;; n++) {
                અક્ષર c = a[n];
                જો (c != b[n])
                        પાછા n;
                જો (c == 0)
                        પાછા SIZE_MAX;
        }
}
