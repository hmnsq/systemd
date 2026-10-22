/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "ansi-seq.h"
#ضمن "escape.h"
#ضمن "extract-word.h"
#ضمن "glyph-util.h"
#ضمن "gunicode.h"
#ضمن "locale-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "memstream-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"

حرف* first_word(ثابت حرف *s, ثابت حرف *word) {
        أكد(s);
        أكد(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ܐܢ (isempty(word))
                ܦܢܐ (حرف*) s;

        ثابت حرف *p = startswith(s, word);
        ܐܢ (!p)
                ܦܢܐ عدم;
        ܐܢ (*p == '\0')
                ܦܢܐ (حرف*) p;

        ثابت حرف *nw = skip_leading_chars(p, WHITESPACE);
        ܐܢ (p == nw)
                ܦܢܐ عدم;

        ܦܢܐ (حرف*) nw;
}

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) {
        أكد(x);
        أكد(s || l == 0);

        ܐܢ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ܐܢ (l > 0 || !*x) {
                size_t q;
                حرف *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ܐܢ (!m)
                        ܦܢܐ عدم;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ܦܢܐ *x;
}

حرف* strstrip(حرف *s) {
        ܐܢ (!s)
                ܦܢܐ عدم;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ܦܢܐ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

حرف* delete_chars(حرف *s, ثابت حرف *bad) {
        حرف *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ܐܢ (!s)
                ܦܢܐ عدم;

        ܐܢ (!bad)
                bad = WHITESPACE;

        لكل (f = s, t = s; *f; f++) {
                ܐܢ (strchr(bad, *f))
                        استمر;

                *(t++) = *f;
        }

        *t = 0;

        ܦܢܐ s;
}

حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad) {
        حرف *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ܐܢ (!s)
                ܦܢܐ عدم;

        ܐܢ (!bad)
                bad = WHITESPACE;

        لكل (حرف *p = s; *p; p++)
                ܐܢ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ܦܢܐ s;
}

حرف* truncate_nl_full(حرف *s, size_t *ret_len) {
        size_t n;

        أكد(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ܐܢ (ret_len)
                *ret_len = n;
        ܦܢܐ s;
}

حرف ascii_tolower(حرف x) {

        ܐܢ (x >= 'A' && x <= 'Z')
                ܦܢܐ x - 'A' + 'a';

        ܦܢܐ x;
}

حرف ascii_toupper(حرف x) {

        ܐܢ (x >= 'a' && x <= 'z')
                ܦܢܐ x - 'a' + 'A';

        ܦܢܐ x;
}

حرف* ascii_strlower(حرف *s) {
        أكد(s);

        لكل (حرف *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ܦܢܐ s;
}

حرف* ascii_strupper(حرف *s) {
        أكد(s);

        لكل (حرف *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ܦܢܐ s;
}

حرف* ascii_strlower_n(حرف *s, size_t n) {
        أكد(n <= 0 || s);

        ܐܢ (n <= 0)
                ܦܢܐ s;

        لكل (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ܦܢܐ s;
}

صحيح ascii_strcasecmp_n(ثابت حرف *a, ثابت حرف *b, size_t n) {

        أكد(a);
        أكد(b);

        لكل (; n > 0; a++, b++, n--) {
                صحيح x, y;

                x = (صحيح) (uint8_t) ascii_tolower(*a);
                y = (صحيح) (uint8_t) ascii_tolower(*b);

                ܐܢ (x != y)
                        ܦܢܐ x - y;
        }

        ܦܢܐ 0;
}

صحيح ascii_strcasecmp_nn(ثابت حرف *a, size_t n, ثابت حرف *b, size_t m) {
        صحيح r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ܐܢ (r != 0)
                ܦܢܐ r;

        ܦܢܐ CMP(n, m);
}

bool chars_intersect(ثابت حرف *a, ثابت حرف *b) {
        /* Returns true if any of the chars in a are in b. */
        لكل (ثابت حرف *p = a; *p; p++)
                ܐܢ (strchr(b, *p))
                        ܦܢܐ true;

        ܦܢܐ false;
}

bool string_has_cc(ثابت حرف *p, ثابت حرف *ok) {
        أكد(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        لكل (ثابت حرف *t = p; *t; t++) {
                ܐܢ (ok && strchr(ok, *t))
                        استمر;

                ܐܢ (char_is_cc(*t))
                        ܦܢܐ true;
        }

        ܦܢܐ false;
}

ساكن صحيح write_ellipsis(حرف *buf, bool unicode) {
        ثابت حرف *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        أكد(strlen(s) == 3);
        memcpy(buf, s, 3);
        ܦܢܐ 3;
}

ساكن size_t ansi_sequence_length(ثابت حرف *s, size_t len) {
        أكد(s);

        ܐܢ (len < 2)
                ܦܢܐ 0;

        ܐܢ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ܦܢܐ 0;  /* Not the start of a sequence */

        ܐܢ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ܐܢ (i == len)
                        ܦܢܐ 0;

                ܟܡܐ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ܐܢ (++i == len)
                                ܦܢܐ 0;
                ܟܡܐ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ܐܢ (++i == len)
                                ܦܢܐ 0;
                ܐܢ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ܦܢܐ i + 1;
                ܦܢܐ 0;  /* Bad sequence */

        } ܐܚܪܢܐ ܐܢ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ܦܢܐ 2;

        ܦܢܐ 0;  /* Bad escape? */
}

ساكن bool string_has_ansi_sequence(ثابت حرف *s, size_t len) {
        ثابت حرف *t = s;

        ܟܡܐ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ܐܢ (ansi_sequence_length(t, len - (t - s)) > 0)
                        ܦܢܐ true;
                t++;
        }
        ܦܢܐ false;
}

ساكن size_t previous_ansi_sequence(ثابت حرف *s, size_t length, ثابت حرف **ret_where) {

        أكد(s);
        أكد(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ܐܢ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = عدم;
                ܦܢܐ 0;
        }

        لكل (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                ܐܢ (slen > 0) {
                        *ret_where = s + i;
                        ܦܢܐ slen;
                }

                ܐܢ (i == 0)
                        اكسر;
        }

        *ret_where = عدم;
        ܦܢܐ 0;
}

ساكن حرف *ascii_ellipsize_mem(ثابت حرف *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        حرف *t;

        أكد(s);
        أكد(percent <= 100);
        أكد(new_length != SIZE_MAX);

        ܐܢ (old_length <= new_length)
                ܦܢܐ strndup(s, old_length);

        /* Special case short ellipsations */
        بدل (new_length) {

        حالة 0:
                ܦܢܐ strdup("");

        حالة 1:
                ܐܢ (is_locale_utf8())
                        ܦܢܐ strdup("…");
                ܐܚܪܢܐ
                        ܦܢܐ strdup(".");

        حالة 2:
                ܐܢ (!is_locale_utf8())
                        ܦܢܐ strdup("..");
                اكسر;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(حرف, new_length+3);
        ܐܢ (!t)
                ܦܢܐ عدم;

        أكد(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        أكد(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ܦܢܐ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ساكن ثابت حرف* find_previous_unichar(ثابت حرف *start, ثابت حرف *end, char32_t *ret_c) {
        ثابت حرف *p;
        صحيح r;

        أكد(start);
        أكد(end);
        أكد(end > start);

        لكل (p = end; p > start; ) {
                p--;
                ܐܢ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        اكسر;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        ܐܢ (r < 0 || p + r != end)
                ܦܢܐ عدم;

        ܐܢ (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        ܦܢܐ p;
}

حرف* ellipsize_mem(ثابت حرف *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        ثابت حرف *i, *j;
        صحيح r;

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

        أكد(s);
        أكد(percent <= 100);

        ܐܢ (new_length == SIZE_MAX)
                ܦܢܐ strndup(s, old_length);

        ܐܢ (new_length == 0)
                ܦܢܐ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ܐܢ (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                ܦܢܐ ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        أكد(x <= new_length - 1);

        k = 0;
        لكل (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                ܐܢ (slen > 0) {
                        i += slen;
                        استمر;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                ܐܢ (r < 0)
                        ܦܢܐ عدم;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                صحيح w = unichar_console_width(c);
                ܐܢ (k + w > x)
                        اكسر;

                k += w;
                i += r;
        }

        ثابت حرف *ansi_start = s + old_length;
        size_t ansi_len = 0;

        لكل (ثابت حرف *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                صحيح w;
                ثابت حرف *tt;

                ܐܢ (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                ܐܢ (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        استمر;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                ܐܢ (!tt)
                        ܦܢܐ عدم;

                w = unichar_console_width(c);
                ܐܢ (k + w > new_length)
                        اكسر;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ܐܢ (i >= j)
                ܦܢܐ memdup_suffix0(s, old_length);

        ܐܢ (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ܐܢ (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        ܐܢ (r < 0)
                                ܦܢܐ عدم;

                        j += r;
                } ܐܚܪܢܐ ܐܢ (i > s) {
                        ثابت حرف *tt = find_previous_unichar(s, i, عدم);
                        ܐܢ (!tt)
                                ܦܢܐ عدم;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        حرف *e = new(حرف, alloc_len);
        ܐܢ (!e)
                ܦܢܐ عدم;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        حرف *dst = e + len + 3;

        ܐܢ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                لكل (ثابت حرف *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        ܐܢ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ܐܚܪܢܐ
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        ܦܢܐ e;
}

حرف* cellescape(حرف *buf, size_t len, ثابت حرف *s) {
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

        أكد(buf);
        أكد(len > 0); /* at least a terminating NUL */
        أكد(s);

        لكل (;;) {
                حرف four[4];
                صحيح w;

                ܐܢ (*s == 0) /* terminating NUL detected? then we are done! */
                        اذهب done;

                w = cescape_char(*s, four);
                ܐܢ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        اكسر;

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
        لكل (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                ܐܢ (i + 4 <= len) /* nice, we reached our space goal */
                        اكسر;

                k = k == 0 ? 3 : k - 1;
                ܐܢ (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        اكسر;

                أكد(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        ܐܢ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ܐܚܪܢܐ ܐܢ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ܐܚܪܢܐ ܐܢ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ܐܚܪܢܐ
                أكد(i + 1 <= len);

done:
        buf[i] = '\0';
        ܦܢܐ buf;
}

حرف* strshorten(حرف *s, size_t l) {
        أكد(s);

        ܐܢ (l >= SIZE_MAX-1) /* Would not change anything */
                ܦܢܐ s;

        ܐܢ (strnlen(s, l+1) > l)
                s[l] = 0;

        ܦܢܐ s;
}

صحيح strgrowpad0(حرف **s, size_t l) {
        size_t sz;

        أكد(s);

        ܐܢ (*s) {
                sz = strlen(*s) + 1;
                ܐܢ (sz >= l) /* never shrink */
                        ܦܢܐ 0;
        } ܐܚܪܢܐ
                sz = 0;

        حرف *q = realloc(*s, l);
        ܐܢ (!q)
                ܦܢܐ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ܦܢܐ 0;
}

حرف* strreplace(ثابت حرف *text, ثابت حرف *old_string, ثابت حرف *new_string) {
        size_t l, old_len, new_len;
        حرف *t, *ret = عدم;
        ثابت حرف *f;

        أكد(old_string);
        أكد(new_string);

        ܐܢ (!text)
                ܦܢܐ عدم;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        ܐܢ (!GREEDY_REALLOC(ret, l+1))
                ܦܢܐ عدم;

        f = text;
        t = ret;
        ܟܡܐ (*f) {
                size_t d, nl;

                ܐܢ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        استمر;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ܐܢ (!GREEDY_REALLOC(ret, nl + 1))
                        ܦܢܐ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ܦܢܐ ret;
}

ساكن فراغ advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[ساكن 2],
                size_t size) {

        ܐܢ (!offsets)
                ܦܢܐ;

        أكد(shift);

        ܐܢ ((size_t) diff < offsets[0])
                shift[0] += size;
        ܐܢ ((size_t) diff < offsets[1])
                shift[1] += size;
}

ساكن فراغ retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[ساكن 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ܐܢ (!offsets)
                ܦܢܐ;

        أكد(shift);
        أكد(begin <= end);

        لكل (size_t k = 0; k < 2; k++)
                ܐܢ (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ثابت حرف *begin = عدم;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        أكد(ibuf);
        أكد(*ibuf);
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
        ܐܢ (!f)
                ܦܢܐ عدم;

        لكل (ثابت حرف *i = *ibuf; i < *ibuf + isz; i++) {
                صحيح e;

                لكل (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ܐܢ (e < 0)
                                ܦܢܐ عدم;
                        ܐܢ (e != ANSI_SEQ_EVENT_ABORT)
                                اكسر;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ܐܢ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = عدم;
                        }
                }

                بدل (e) {

                حالة ANSI_SEQ_EVENT_TEXT:
                        ܐܢ (*i == '\r') {
                                n_carriage_returns++;
                                استمر;
                        }
                        ܐܢ (*i == '\n') {
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
                        لكل (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ܐܢ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } ܐܚܪܢܐ
                                fputc(*i, f);

                        اكسر;

                حالة ANSI_SEQ_EVENT_SEQUENCE:
                        ܐܢ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                لكل (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        اكسر;

                حالة ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        أكد(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = عدم;
                        اكسر;

                افتراضي:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ܐܢ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        حرف *obuf;
        ܐܢ (memstream_finalize(&m, &obuf, _isz) < 0)
                ܦܢܐ عدم;

        free_and_replace(*ibuf, obuf);

        ܐܢ (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        ܦܢܐ *ibuf;
}

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap) {
        _cleanup_free_ حرف *buffer = عدم;
        size_t f, l, l_separator;
        bool need_separator;
        حرف *nr, *p;

        ܐܢ (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        لكل (ثابت حرف *t;;) {
                size_t n;

                t = va_arg(aq, ثابت حرف *);
                ܐܢ (!t)
                        اكسر;
                ܐܢ (t == POINTER_MAX)
                        استمر;

                n = strlen(t);

                ܐܢ (need_separator)
                        n += l_separator;

                ܐܢ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ܦܢܐ عدم;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        ܐܢ (!nr)
                ܦܢܐ عدم;

        *x = nr;
        p = nr + f;

        لكل (;;) {
                ثابت حرف *t;

                t = va_arg(ap, ثابت حرف *);
                ܐܢ (!t)
                        اكسر;
                ܐܢ (t == POINTER_MAX)
                        استمر;

                ܐܢ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        أكد(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ܐܢ (buffer)
                ܦܢܐ TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        ܦܢܐ p;
}

حرف* strextend_with_separator_internal(حرف **x, ثابت حرف *separator, ...) {
        va_list ap;
        حرف *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ܦܢܐ ret;
}

صحيح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        صحيح l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        أكد(x);
        أكد(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ܐܢ (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                أكد(a >= m + 1);
        } ܐܚܪܢܐ
                m = a = 0;

        ܐܢ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                حرف *n;

                ܐܢ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ܦܢܐ -ENOMEM;
                ܐܢ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ܦܢܐ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ܐܢ (!n)
                        ܦܢܐ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        أكد(l >= 0);

        ܐܢ ((size_t) l < a - m - l_separator) {
                حرف *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ܐܢ (n)
                        *x = n;
        } ܐܚܪܢܐ {
                حرف *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ܐܢ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        اذهب oom;
                ܐܢ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        اذهب oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ܐܢ (!n)
                        اذهب oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                أكد((size_t) l < a - m - l_separator);
        }

        ܦܢܐ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ܦܢܐ -ENOMEM;
}

حرف* strrep(ثابت حرف *s, size_t n) {
        حرف *ret, *p;
        size_t l;

        أكد(s);

        l = strlen(s);
        ܐܢ (!MUL_ASSIGN_SAFE(&l, n))
                ܦܢܐ عدم;
        ܐܢ (!INC_SAFE(&l, 1))
                ܦܢܐ عدم;

        p = ret = malloc(l);
        ܐܢ (!ret)
                ܦܢܐ عدم;

        لكل (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ܦܢܐ ret;
}

صحيح split_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_first, حرف **ret_second) {
        أكد(s);
        أكد(!isempty(sep));

        ثابت حرف *x = strstr(s, sep);
        ܐܢ (!x)
                ܦܢܐ -EINVAL;

        _cleanup_free_ حرف *a = عدم;
        ܐܢ (ret_first) {
                a = strndup(s, x - s);
                ܐܢ (!a)
                        ܦܢܐ -ENOMEM;
        }

        _cleanup_free_ حرف *b = عدم;
        ܐܢ (ret_second) {
                b = strdup(x + strlen(sep));
                ܐܢ (!b)
                        ܦܢܐ -ENOMEM;
        }

        ܐܢ (ret_first)
                *ret_first = TAKE_PTR(a);
        ܐܢ (ret_second)
                *ret_second = TAKE_PTR(b);
        ܦܢܐ 0;
}

صحيح free_and_strdup(حرف **p, ثابت حرف *s) {
        حرف *t;

        أكد(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ܐܢ (streq_ptr(*p, s))
                ܦܢܐ 0;

        ܐܢ (s) {
                t = strdup(s);
                ܐܢ (!t)
                        ܦܢܐ -ENOMEM;
        } ܐܚܪܢܐ
                t = عدم;

        free_and_replace(*p, t);

        ܦܢܐ 1;
}

صحيح free_and_strdup_warn(حرف **p, ثابت حرف *s) {
        صحيح r;

        r = free_and_strdup(p, s);
        ܐܢ (r < 0)
                ܦܢܐ log_oom();
        ܦܢܐ r;
}

صحيح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) {
        حرف *t;

        أكد(p);
        أكد(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ܐܢ (!*p && !s)
                ܦܢܐ 0;

        ܐܢ (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                ܦܢܐ 0;

        ܐܢ (s) {
                t = strndup(s, l);
                ܐܢ (!t)
                        ܦܢܐ -ENOMEM;
        } ܐܚܪܢܐ
                t = عدم;

        free_and_replace(*p, t);
        ܦܢܐ 1;
}

صحيح strdup_to_full(حرف **ret, ثابت حرف *src) {
        ܐܢ (!src) {
                ܐܢ (ret)
                        *ret = عدم;

                ܦܢܐ 0;
        } ܐܚܪܢܐ {
                ܐܢ (ret) {
                        حرف *t = strdup(src);
                        ܐܢ (!t)
                                ܦܢܐ -ENOMEM;
                        *ret = t;
                }

                ܦܢܐ 1;
        }
};

bool string_is_safe(ثابت حرف *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ܐܢ (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                ܦܢܐ false;

        ܐܢ (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                ܦܢܐ false;

        لكل (ثابت حرف *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ܐܢ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ܦܢܐ false;

                ܐܢ (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ܦܢܐ false;

                ܐܢ (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ܦܢܐ false;

                ܐܢ (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ܦܢܐ false;

                ܐܢ (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ܦܢܐ false;

                ܐܢ (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ܦܢܐ false;

                ܐܢ (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ܦܢܐ false;
        }

        ܐܢ (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                ܦܢܐ false;

        ܐܢ (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ܦܢܐ false;

        ܦܢܐ true;
}

حرف* str_realloc(حرف *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ܐܢ (!p)
                ܦܢܐ عدم;

        ܦܢܐ realloc(p, strlen(p) + 1) ?: p;
}

حرف* string_erase(حرف *x) {
        ܐܢ (!x)
                ܦܢܐ عدم;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        ܦܢܐ x;
}

صحيح string_truncate_lines(ثابت حرف *s, size_t n_lines, حرف **ret) {
        ثابت حرف *p = s, *e = s;
        bool truncation_applied = false;
        حرف *copy;
        size_t n = 0;

        أكد(s);
        أكد(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        لكل (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ܐܢ (p[k] == 0) {
                        ܐܢ (k == 0) /* final empty line */
                                اكسر;

                        ܐܢ (n >= n_lines) /* above threshold */
                                اكسر;

                        e = p + k; /* last line to include */
                        اكسر;
                }

                أكد(p[k] == '\n');

                ܐܢ (n >= n_lines)
                        اكسر;

                ܐܢ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ܐܢ (isempty(e))
                copy = strdup(s);
        ܐܚܪܢܐ {
                ܐܢ (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ܐܢ (!copy)
                ܦܢܐ -ENOMEM;

        *ret = copy;
        ܦܢܐ truncation_applied;
}

صحيح string_extract_line(ثابت حرف *s, size_t i, حرف **ret) {
        ثابت حرف *p = s;
        size_t c = 0;

        أكد(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        لكل (;;) {
                ثابت حرف *q;

                q = strchr(p, '\n');
                ܐܢ (i == c) {
                        /* The line we are looking for! */

                        ܐܢ (q) {
                                حرف *m;

                                m = strndup(p, q - p);
                                ܐܢ (!m)
                                        ܦܢܐ -ENOMEM;

                                *ret = m;
                                ܦܢܐ !isempty(q + 1); /* More coming? */
                        } ܐܚܪܢܐ
                                /* Tell the caller to use the input string if equal */
                                ܦܢܐ strdup_to(ret, p != s ? p : عدم);
                }

                ܐܢ (!q)
                        /* No more lines, return empty line */
                        ܦܢܐ strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

صحيح string_contains_word_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *words, ثابت حرف **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ثابت ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ثابت حرف *found = عدم;
        صحيح r;

        لكل (;;) {
                _cleanup_free_ حرف *w = عدم;

                r = extract_first_word(&string, &w, separators, flags);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;

                found = strv_find(words, w);
                ܐܢ (found)
                        اكسر;
        }

        ܐܢ (ret_word)
                *ret_word = found;
        ܦܢܐ !!found;
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok) {
        ܐܢ (!s1 && !s2)
                ܦܢܐ true;
        ܐܢ (!s1 || !s2)
                ܦܢܐ false;

        ܐܢ (!ok)
                ok = WHITESPACE;

        لكل (; *s1 && *s2; s1++, s2++)
                ܐܢ (*s1 != *s2)
                        اكسر;

        ܦܢܐ in_charset(s1, ok) && in_charset(s2, ok);
}

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char) {
        أكد(str);
        أكد(old_char != '\0');
        أكد(new_char != '\0');
        أكد(old_char != new_char);

        لكل (حرف *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ܦܢܐ str;
}

صحيح make_cstring(ثابت فراغ *s, size_t n, MakeCStringMode mode, حرف **ret) {
        حرف *b;

        أكد(s || n == 0);
        أكد(mode >= 0);
        أكد(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ܐܢ (n == 0) {
                ܐܢ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ܦܢܐ -EINVAL;

                ܐܢ (!ret)
                        ܦܢܐ 0;

                b = new0(حرف, 1);
        } ܐܚܪܢܐ {
                ثابت uint8_t *nul;

                nul = memchr(s, 0, n);
                ܐܢ (nul) {
                        ܐܢ (nul < (ثابت uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ܦܢܐ -EINVAL;

                        n--;
                } ܐܚܪܢܐ ܐܢ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ܦܢܐ -EINVAL;

                ܐܢ (!ret)
                        ܦܢܐ 0;

                b = memdup_suffix0(s, n);
        }
        ܐܢ (!b)
                ܦܢܐ -ENOMEM;

        *ret = b;
        ܦܢܐ 0;
}

size_t strspn_from_end(ثابت حرف *str, ثابت حرف *accept) {
        size_t n = 0;

        ܐܢ (isempty(str))
                ܦܢܐ 0;

        ܐܢ (isempty(accept))
                ܦܢܐ 0;

        لكل (ثابت حرف *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ܦܢܐ n;
}

size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        أكد(str || n == 0);
        أكد(accept);

        لكل (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ܦܢܐ i;
}

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept) {
        ܐܢ (isempty(a) || isempty(accept))
                ܦܢܐ strdup("");

        ܦܢܐ strndup(a, strspn(a, accept));
}

حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject) {
        ܐܢ (isempty(a))
                ܦܢܐ strdup("");
        ܐܢ (isempty(reject))
                ܦܢܐ strdup(a);

        ܦܢܐ strndup(a, strcspn(a, reject));
}

حرف* find_line_startswith_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        حرف *p = (حرف*) strstr(haystack, needle);
        ܐܢ (!p)
                ܦܢܐ عدم;

        ܐܢ (p > haystack)
                ܟܡܐ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        ܐܢ (!p)
                                ܦܢܐ عدم;
                }

        ܦܢܐ p + strlen(needle);
}

حرف* find_line_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        حرف *p = (حرف*) find_line_startswith(haystack, needle);
        ܐܢ (!p)
                ܦܢܐ عدم;

        ܐܢ (*p == 0 || strchr(NEWLINE, *p))
                ܦܢܐ p - strlen(needle);

        ܦܢܐ عدم;
}

حرف* find_line_after_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        حرف *p = (حرف*) find_line_startswith(haystack, needle);
        ܐܢ (!p)
                ܦܢܐ عدم;

        ܐܢ (*p == 0)
                ܦܢܐ p;
        ܐܢ (strchr(NEWLINE, *p))
                ܦܢܐ p + 1;

        ܦܢܐ عدم;
}

bool version_is_valid(ثابت حرف *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ܐܢ (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                ܦܢܐ false;

        ܐܢ (!filename_part_is_valid(s))
                ܦܢܐ false;

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

        حرف charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        ܐܢ (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ܐܢ (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        ܦܢܐ in_charset(s, charset);
}

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y) {
        _cleanup_free_ size_t *t0 = عدم, *t1 = عدم, *t2 = عدم;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ܐܢ (streq_ptr(x, y))
                ܦܢܐ 0;

        xl = strlen_ptr(x);
        ܐܢ (xl > SSIZE_MAX)
                ܦܢܐ -E2BIG;

        yl = strlen_ptr(y);
        ܐܢ (yl > SSIZE_MAX)
                ܦܢܐ -E2BIG;

        ܐܢ (isempty(x))
                ܦܢܐ yl;
        ܐܢ (isempty(y))
                ܦܢܐ xl;

        t0 = new0(size_t, yl + 1);
        ܐܢ (!t0)
                ܦܢܐ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ܐܢ (!t1)
                ܦܢܐ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ܐܢ (!t2)
                ܦܢܐ -ENOMEM;

        لكل (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        لكل (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                لكل (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        ܐܢ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ܐܢ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ܐܢ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ܦܢܐ t1[yl];
}

حرف* strrstr_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ܐܢ (!haystack || !needle)
                ܦܢܐ عدم;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ܐܢ (needle[0] == 0)
                ܦܢܐ (حرف*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ܐܢ (needle[1] == 0)
                ܦܢܐ (حرف*) strrchr(haystack, needle[0]);

        لكل (ثابت حرف *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                ܐܢ (!q)
                        ܦܢܐ (حرف*) p;
        }
        ܦܢܐ عدم;
}

حرف* strrstr_no_case_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        ܐܢ (!haystack || !needle)
                ܦܢܐ عدم;

        لكل (ثابت حرف *p = strchr(haystack, 0); p > haystack; p--)
                ܐܢ (startswith_no_case(p, needle))
                        ܦܢܐ (حرف*) p;

        ܦܢܐ startswith_no_case(haystack, needle) ? (حرف*) haystack : عدم;
}

size_t str_common_prefix(ثابت حرف *a, ثابت حرف *b) {
        أكد(a);
        أكد(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        لكل (size_t n = 0;; n++) {
                حرف c = a[n];
                ܐܢ (c != b[n])
                        ܦܢܐ n;
                ܐܢ (c == 0)
                        ܦܢܐ SIZE_MAX;
        }
}
