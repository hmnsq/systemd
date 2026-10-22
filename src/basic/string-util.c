/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "ansi-seq.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "glyph-util.h"
#অন্তর্ভাব "gunicode.h"
#অন্তর্ভাব "locale-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "memstream-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "utf8.h"

অক্ষর* first_word(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *word) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        যদি (isempty(word))
                ফেরত (অক্ষর*) s;

        ধ্রুবক অক্ষর *p = startswith(s, word);
        যদি (!p)
                ফেরত ফাঁকা;
        যদি (*p == '\0')
                ফেরত (অক্ষর*) p;

        ধ্রুবক অক্ষর *nw = skip_leading_chars(p, WHITESPACE);
        যদি (p == nw)
                ফেরত ফাঁকা;

        ফেরত (অক্ষর*) nw;
}

অক্ষর* strextendn(অক্ষর **x, ধ্রুবক অক্ষর *s, size_t l) {
        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(s || l == 0);

        যদি (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        যদি (l > 0 || !*x) {
                size_t q;
                অক্ষর *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                যদি (!m)
                        ফেরত ফাঁকা;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        ফেরত *x;
}

অক্ষর* strstrip(অক্ষর *s) {
        যদি (!s)
                ফেরত ফাঁকা;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        ফেরত delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

অক্ষর* delete_chars(অক্ষর *s, ধ্রুবক অক্ষর *bad) {
        অক্ষর *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        যদি (!s)
                ফেরত ফাঁকা;

        যদি (!bad)
                bad = WHITESPACE;

        জন্য (f = s, t = s; *f; f++) {
                যদি (strchr(bad, *f))
                        চালু;

                *(t++) = *f;
        }

        *t = 0;

        ফেরত s;
}

অক্ষর* delete_trailing_chars(অক্ষর *s, ধ্রুবক অক্ষর *bad) {
        অক্ষর *c = s;

        /* Drops all specified bad characters, at the end of the string */

        যদি (!s)
                ফেরত ফাঁকা;

        যদি (!bad)
                bad = WHITESPACE;

        জন্য (অক্ষর *p = s; *p; p++)
                যদি (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        ফেরত s;
}

অক্ষর* truncate_nl_full(অক্ষর *s, size_t *ret_len) {
        size_t n;

        প্রতিজ্ঞা(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        যদি (ret_len)
                *ret_len = n;
        ফেরত s;
}

অক্ষর ascii_tolower(অক্ষর x) {

        যদি (x >= 'A' && x <= 'Z')
                ফেরত x - 'A' + 'a';

        ফেরত x;
}

অক্ষর ascii_toupper(অক্ষর x) {

        যদি (x >= 'a' && x <= 'z')
                ফেরত x - 'a' + 'A';

        ফেরত x;
}

অক্ষর* ascii_strlower(অক্ষর *s) {
        প্রতিজ্ঞা(s);

        জন্য (অক্ষর *p = s; *p; p++)
                *p = ascii_tolower(*p);

        ফেরত s;
}

অক্ষর* ascii_strupper(অক্ষর *s) {
        প্রতিজ্ঞা(s);

        জন্য (অক্ষর *p = s; *p; p++)
                *p = ascii_toupper(*p);

        ফেরত s;
}

অক্ষর* ascii_strlower_n(অক্ষর *s, size_t n) {
        প্রতিজ্ঞা(n <= 0 || s);

        যদি (n <= 0)
                ফেরত s;

        জন্য (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        ফেরত s;
}

পূর্ণসংখ্যা ascii_strcasecmp_n(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b, size_t n) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        জন্য (; n > 0; a++, b++, n--) {
                পূর্ণসংখ্যা x, y;

                x = (পূর্ণসংখ্যা) (uint8_t) ascii_tolower(*a);
                y = (পূর্ণসংখ্যা) (uint8_t) ascii_tolower(*b);

                যদি (x != y)
                        ফেরত x - y;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা ascii_strcasecmp_nn(ধ্রুবক অক্ষর *a, size_t n, ধ্রুবক অক্ষর *b, size_t m) {
        পূর্ণসংখ্যা r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        যদি (r != 0)
                ফেরত r;

        ফেরত CMP(n, m);
}

bool chars_intersect(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        /* Returns true if any of the chars in a are in b. */
        জন্য (ধ্রুবক অক্ষর *p = a; *p; p++)
                যদি (strchr(b, *p))
                        ফেরত true;

        ফেরত false;
}

bool string_has_cc(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *ok) {
        প্রতিজ্ঞা(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        জন্য (ধ্রুবক অক্ষর *t = p; *t; t++) {
                যদি (ok && strchr(ok, *t))
                        চালু;

                যদি (char_is_cc(*t))
                        ফেরত true;
        }

        ফেরত false;
}

স্থির পূর্ণসংখ্যা write_ellipsis(অক্ষর *buf, bool unicode) {
        ধ্রুবক অক্ষর *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        প্রতিজ্ঞা(strlen(s) == 3);
        memcpy(buf, s, 3);
        ফেরত 3;
}

স্থির size_t ansi_sequence_দৈর্ঘ্য(ধ্রুবক অক্ষর *s, size_t len) {
        প্রতিজ্ঞা(s);

        যদি (len < 2)
                ফেরত 0;

        যদি (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                ফেরত 0;  /* Not the start of a sequence */

        যদি (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                যদি (i == len)
                        ফেরত 0;

                যখন (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        যদি (++i == len)
                                ফেরত 0;
                যখন (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        যদি (++i == len)
                                ফেরত 0;
                যদি (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        ফেরত i + 1;
                ফেরত 0;  /* Bad sequence */

        } অন্যথা যদি (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                ফেরত 2;

        ফেরত 0;  /* Bad escape? */
}

স্থির bool string_has_ansi_sequence(ধ্রুবক অক্ষর *s, size_t len) {
        ধ্রুবক অক্ষর *t = s;

        যখন ((t = memchr(t, 0x1B, len - (t - s)))) {
                যদি (ansi_sequence_দৈর্ঘ্য(t, len - (t - s)) > 0)
                        ফেরত true;
                t++;
        }
        ফেরত false;
}

স্থির size_t previous_ansi_sequence(ধ্রুবক অক্ষর *s, size_t length, ধ্রুবক অক্ষর **ret_where) {

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        যদি (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ফাঁকা;
                ফেরত 0;
        }

        জন্য (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_দৈর্ঘ্য(s + i, length - i);
                যদি (slen > 0) {
                        *ret_where = s + i;
                        ফেরত slen;
                }

                যদি (i == 0)
                        বিরতি;
        }

        *ret_where = ফাঁকা;
        ফেরত 0;
}

স্থির অক্ষর *ascii_ellipsize_mem(ধ্রুবক অক্ষর *s, size_t old_দৈর্ঘ্য, size_t new_দৈর্ঘ্য, unsigned percent) {
        size_t x, need_space, suffix_len;
        অক্ষর *t;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(percent <= 100);
        প্রতিজ্ঞা(new_দৈর্ঘ্য != SIZE_MAX);

        যদি (old_দৈর্ঘ্য <= new_দৈর্ঘ্য)
                ফেরত strndup(s, old_দৈর্ঘ্য);

        /* Special case short ellipsations */
        নির্বাচন (new_দৈর্ঘ্য) {

        ক্ষেত্র 0:
                ফেরত strdup("");

        ক্ষেত্র 1:
                যদি (is_locale_utf8())
                        ফেরত strdup("…");
                অন্যথা
                        ফেরত strdup(".");

        ক্ষেত্র 2:
                যদি (!is_locale_utf8())
                        ফেরত strdup("..");
                বিরতি;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(অক্ষর, new_দৈর্ঘ্য+3);
        যদি (!t)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(new_দৈর্ঘ্য >= need_space);

        x = ((new_দৈর্ঘ্য - need_space) * percent + 50) / 100;
        প্রতিজ্ঞা(x <= new_দৈর্ঘ্য - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_দৈর্ঘ্য - x - need_space;
        memcpy(t + x + 3, s + old_দৈর্ঘ্য - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        ফেরত t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
স্থির ধ্রুবক অক্ষর* find_previous_unichar(ধ্রুবক অক্ষর *start, ধ্রুবক অক্ষর *end, char32_t *ret_c) {
        ধ্রুবক অক্ষর *p;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(end);
        প্রতিজ্ঞা(end > start);

        জন্য (p = end; p > start; ) {
                p--;
                যদি (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        বিরতি;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        যদি (r < 0 || p + r != end)
                ফেরত ফাঁকা;

        যদি (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        ফেরত p;
}

অক্ষর* ellipsize_mem(ধ্রুবক অক্ষর *s, size_t old_দৈর্ঘ্য, size_t new_দৈর্ঘ্য, unsigned percent) {
        size_t x, k, len, len2;
        ধ্রুবক অক্ষর *i, *j;
        পূর্ণসংখ্যা r;

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

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(percent <= 100);

        যদি (new_দৈর্ঘ্য == SIZE_MAX)
                ফেরত strndup(s, old_দৈর্ঘ্য);

        যদি (new_দৈর্ঘ্য == 0)
                ফেরত strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_দৈর্ঘ্য);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        যদি (!has_ansi_seq && ascii_is_valid_n(s, old_দৈর্ঘ্য) && !memchr(s, '\t', old_দৈর্ঘ্য))
                ফেরত ascii_ellipsize_mem(s, old_দৈর্ঘ্য, new_দৈর্ঘ্য, percent);

        x = (new_দৈর্ঘ্য - 1) * percent / 100;
        প্রতিজ্ঞা(x <= new_দৈর্ঘ্য - 1);

        k = 0;
        জন্য (i = s; i < s + old_দৈর্ঘ্য; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_দৈর্ঘ্য(i, old_দৈর্ঘ্য - (i - s)) : 0;
                যদি (slen > 0) {
                        i += slen;
                        চালু;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_দৈর্ঘ্য - i);
                যদি (r < 0)
                        ফেরত ফাঁকা;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                পূর্ণসংখ্যা w = unichar_console_width(c);
                যদি (k + w > x)
                        বিরতি;

                k += w;
                i += r;
        }

        ধ্রুবক অক্ষর *ansi_start = s + old_দৈর্ঘ্য;
        size_t ansi_len = 0;

        জন্য (ধ্রুবক অক্ষর *t = j = s + old_দৈর্ঘ্য; t > i && k < new_দৈর্ঘ্য; ) {
                char32_t c;
                পূর্ণসংখ্যা w;
                ধ্রুবক অক্ষর *tt;

                যদি (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                যদি (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        চালু;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                যদি (!tt)
                        ফেরত ফাঁকা;

                w = unichar_console_width(c);
                যদি (k + w > new_দৈর্ঘ্য)
                        বিরতি;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        যদি (i >= j)
                ফেরত memdup_suffix0(s, old_দৈর্ঘ্য);

        যদি (k >= new_দৈর্ঘ্য) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                যদি (j < s + old_দৈর্ঘ্য) {
                        r = utf8_encoded_valid_unichar(j, s + old_দৈর্ঘ্য - j);
                        যদি (r < 0)
                                ফেরত ফাঁকা;

                        j += r;
                } অন্যথা যদি (i > s) {
                        ধ্রুবক অক্ষর *tt = find_previous_unichar(s, i, ফাঁকা);
                        যদি (!tt)
                                ফেরত ফাঁকা;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_দৈর্ঘ্য - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_দৈর্ঘ্য + 3 + 1 : len + 3 + len2 + 1;

        অক্ষর *e = new(অক্ষর, alloc_len);
        যদি (!e)
                ফেরত ফাঁকা;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        অক্ষর *dst = e + len + 3;

        যদি (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                জন্য (ধ্রুবক অক্ষর *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_দৈর্ঘ্য(p, j - p);
                        যদি (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } অন্যথা
                                p = utf8_পরবর্তী_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        ফেরত e;
}

অক্ষর* cellescape(অক্ষর *buf, size_t len, ধ্রুবক অক্ষর *s) {
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

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(len > 0); /* at least a terminating NUL */
        প্রতিজ্ঞা(s);

        জন্য (;;) {
                অক্ষর four[4];
                পূর্ণসংখ্যা w;

                যদি (*s == 0) /* terminating NUL detected? then we are done! */
                        যাও done;

                w = cescape_char(*s, four);
                যদি (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        বিরতি;

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
        জন্য (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                যদি (i + 4 <= len) /* nice, we reached our space goal */
                        বিরতি;

                k = k == 0 ? 3 : k - 1;
                যদি (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        বিরতি;

                প্রতিজ্ঞা(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        যদি (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        অন্যথা যদি (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } অন্যথা যদি (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        অন্যথা
                প্রতিজ্ঞা(i + 1 <= len);

done:
        buf[i] = '\0';
        ফেরত buf;
}

অক্ষর* strshorten(অক্ষর *s, size_t l) {
        প্রতিজ্ঞা(s);

        যদি (l >= SIZE_MAX-1) /* Would not change anything */
                ফেরত s;

        যদি (strnlen(s, l+1) > l)
                s[l] = 0;

        ফেরত s;
}

পূর্ণসংখ্যা strgrowpad0(অক্ষর **s, size_t l) {
        size_t sz;

        প্রতিজ্ঞা(s);

        যদি (*s) {
                sz = strlen(*s) + 1;
                যদি (sz >= l) /* never shrink */
                        ফেরত 0;
        } অন্যথা
                sz = 0;

        অক্ষর *q = realloc(*s, l);
        যদি (!q)
                ফেরত -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        ফেরত 0;
}

অক্ষর* strreplace(ধ্রুবক অক্ষর *text, ধ্রুবক অক্ষর *old_string, ধ্রুবক অক্ষর *new_string) {
        size_t l, old_len, new_len;
        অক্ষর *t, *ret = ফাঁকা;
        ধ্রুবক অক্ষর *f;

        প্রতিজ্ঞা(old_string);
        প্রতিজ্ঞা(new_string);

        যদি (!text)
                ফেরত ফাঁকা;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        যদি (!GREEDY_REALLOC(ret, l+1))
                ফেরত ফাঁকা;

        f = text;
        t = ret;
        যখন (*f) {
                size_t d, nl;

                যদি (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        চালু;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                যদি (!GREEDY_REALLOC(ret, nl + 1))
                        ফেরত mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        ফেরত ret;
}

স্থির শূন্য advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[স্থির 2],
                size_t আকার_2) {

        যদি (!offsets)
                ফেরত;

        প্রতিজ্ঞা(shift);

        যদি ((size_t) diff < offsets[0])
                shift[0] += আকার_2;
        যদি ((size_t) diff < offsets[1])
                shift[1] += আকার_2;
}

স্থির শূন্য retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[স্থির 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        যদি (!offsets)
                ফেরত;

        প্রতিজ্ঞা(shift);
        প্রতিজ্ঞা(begin <= end);

        জন্য (size_t k = 0; k < 2; k++)
                যদি (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

অক্ষর* strip_tab_ansi(অক্ষর **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ধ্রুবক অক্ষর *begin = ফাঁকা;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        প্রতিজ্ঞা(ibuf);
        প্রতিজ্ঞা(*ibuf);
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
        যদি (!f)
                ফেরত ফাঁকা;

        জন্য (ধ্রুবক অক্ষর *i = *ibuf; i < *ibuf + isz; i++) {
                পূর্ণসংখ্যা e;

                জন্য (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        যদি (e < 0)
                                ফেরত ফাঁকা;
                        যদি (e != ANSI_SEQ_EVENT_ABORT)
                                বিরতি;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        যদি (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = ফাঁকা;
                        }
                }

                নির্বাচন (e) {

                ক্ষেত্র ANSI_SEQ_EVENT_TEXT:
                        যদি (*i == '\r') {
                                n_carriage_returns++;
                                চালু;
                        }
                        যদি (*i == '\n') {
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
                        জন্য (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        যদি (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } অন্যথা
                                fputc(*i, f);

                        বিরতি;

                ক্ষেত্র ANSI_SEQ_EVENT_SEQUENCE:
                        যদি (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                জন্য (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        বিরতি;

                ক্ষেত্র ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        প্রতিজ্ঞা(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = ফাঁকা;
                        বিরতি;

                পূর্বনির্ধারিত:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        যদি (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        অক্ষর *obuf;
        যদি (memstream_finalize(&m, &obuf, _isz) < 0)
                ফেরত ফাঁকা;

        free_and_replace(*ibuf, obuf);

        যদি (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        ফেরত *ibuf;
}

অক্ষর* strextendv_with_separator(অক্ষর **x, ধ্রুবক অক্ষর *separator, va_list ap) {
        _cleanup_free_ অক্ষর *বাফার = ফাঁকা;
        size_t f, l, l_separator;
        bool need_separator;
        অক্ষর *nr, *p;

        যদি (!x)
                x = &বাফার;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        জন্য (ধ্রুবক অক্ষর *t;;) {
                size_t n;

                t = va_arg(aq, ধ্রুবক অক্ষর *);
                যদি (!t)
                        বিরতি;
                যদি (t == POINTER_MAX)
                        চালু;

                n = strlen(t);

                যদি (need_separator)
                        n += l_separator;

                যদি (n >= SIZE_MAX - l) {
                        va_end(aq);
                        ফেরত ফাঁকা;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        যদি (!nr)
                ফেরত ফাঁকা;

        *x = nr;
        p = nr + f;

        জন্য (;;) {
                ধ্রুবক অক্ষর *t;

                t = va_arg(ap, ধ্রুবক অক্ষর *);
                যদি (!t)
                        বিরতি;
                যদি (t == POINTER_MAX)
                        চালু;

                যদি (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        প্রতিজ্ঞা(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        যদি (বাফার)
                ফেরত TAKE_PTR(বাফার);

        /* Otherwise we extended the buffer: return the end */
        ফেরত p;
}

অক্ষর* strextend_with_separator_internal(অক্ষর **x, ধ্রুবক অক্ষর *separator, ...) {
        va_list ap;
        অক্ষর *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        ফেরত ret;
}

পূর্ণসংখ্যা strextendf_with_separator(অক্ষর **x, ধ্রুবক অক্ষর *separator, ধ্রুবক অক্ষর *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        পূর্ণসংখ্যা l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        যদি (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                প্রতিজ্ঞা(a >= m + 1);
        } অন্যথা
                m = a = 0;

        যদি (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                অক্ষর *n;

                যদি (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        ফেরত -ENOMEM;
                যদি (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        ফেরত -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                যদি (!n)
                        ফেরত -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        প্রতিজ্ঞা(l >= 0);

        যদি ((size_t) l < a - m - l_separator) {
                অক্ষর *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                যদি (n)
                        *x = n;
        } অন্যথা {
                অক্ষর *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                যদি (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        যাও oom;
                যদি (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        যাও oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                যদি (!n)
                        যাও oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                প্রতিজ্ঞা((size_t) l < a - m - l_separator);
        }

        ফেরত 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        ফেরত -ENOMEM;
}

অক্ষর* strrep(ধ্রুবক অক্ষর *s, size_t n) {
        অক্ষর *ret, *p;
        size_t l;

        প্রতিজ্ঞা(s);

        l = strlen(s);
        যদি (!MUL_ASSIGN_SAFE(&l, n))
                ফেরত ফাঁকা;
        যদি (!INC_SAFE(&l, 1))
                ফেরত ফাঁকা;

        p = ret = malloc(l);
        যদি (!ret)
                ফেরত ফাঁকা;

        জন্য (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        ফেরত ret;
}

পূর্ণসংখ্যা split_pair(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *sep, অক্ষর **ret_first, অক্ষর **ret_second) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(!isempty(sep));

        ধ্রুবক অক্ষর *x = strstr(s, sep);
        যদি (!x)
                ফেরত -EINVAL;

        _cleanup_free_ অক্ষর *a = ফাঁকা;
        যদি (ret_first) {
                a = strndup(s, x - s);
                যদি (!a)
                        ফেরত -ENOMEM;
        }

        _cleanup_free_ অক্ষর *b = ফাঁকা;
        যদি (ret_second) {
                b = strdup(x + strlen(sep));
                যদি (!b)
                        ফেরত -ENOMEM;
        }

        যদি (ret_first)
                *ret_first = TAKE_PTR(a);
        যদি (ret_second)
                *ret_second = TAKE_PTR(b);
        ফেরত 0;
}

পূর্ণসংখ্যা free_and_strdup(অক্ষর **p, ধ্রুবক অক্ষর *s) {
        অক্ষর *t;

        প্রতিজ্ঞা(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        যদি (streq_ptr(*p, s))
                ফেরত 0;

        যদি (s) {
                t = strdup(s);
                যদি (!t)
                        ফেরত -ENOMEM;
        } অন্যথা
                t = ফাঁকা;

        free_and_replace(*p, t);

        ফেরত 1;
}

পূর্ণসংখ্যা free_and_strdup_warn(অক্ষর **p, ধ্রুবক অক্ষর *s) {
        পূর্ণসংখ্যা r;

        r = free_and_strdup(p, s);
        যদি (r < 0)
                ফেরত log_oom();
        ফেরত r;
}

পূর্ণসংখ্যা free_and_strndup(অক্ষর **p, ধ্রুবক অক্ষর *s, size_t l) {
        অক্ষর *t;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        যদি (!*p && !s)
                ফেরত 0;

        যদি (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                ফেরত 0;

        যদি (s) {
                t = strndup(s, l);
                যদি (!t)
                        ফেরত -ENOMEM;
        } অন্যথা
                t = ফাঁকা;

        free_and_replace(*p, t);
        ফেরত 1;
}

পূর্ণসংখ্যা strdup_to_full(অক্ষর **ret, ধ্রুবক অক্ষর *src) {
        যদি (!src) {
                যদি (ret)
                        *ret = ফাঁকা;

                ফেরত 0;
        } অন্যথা {
                যদি (ret) {
                        অক্ষর *t = strdup(src);
                        যদি (!t)
                                ফেরত -ENOMEM;
                        *ret = t;
                }

                ফেরত 1;
        }
};

bool string_is_safe(ধ্রুবক অক্ষর *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        যদি (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                ফেরত false;

        যদি (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                ফেরত false;

        জন্য (ধ্রুবক অক্ষর *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                যদি ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        ফেরত false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        ফেরত false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        ফেরত false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        ফেরত false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        ফেরত false;

                যদি (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        ফেরত false;

                যদি (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        ফেরত false;
        }

        যদি (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                ফেরত false;

        যদি (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                ফেরত false;

        ফেরত true;
}

অক্ষর* str_realloc(অক্ষর *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        যদি (!p)
                ফেরত ফাঁকা;

        ফেরত realloc(p, strlen(p) + 1) ?: p;
}

অক্ষর* string_erase(অক্ষর *x) {
        যদি (!x)
                ফেরত ফাঁকা;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        ফেরত x;
}

পূর্ণসংখ্যা string_truncate_সারিগুলি(ধ্রুবক অক্ষর *s, size_t n_সারিগুলি, অক্ষর **ret) {
        ধ্রুবক অক্ষর *p = s, *e = s;
        bool truncation_applied = false;
        অক্ষর *copy;
        size_t n = 0;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        জন্য (;;) {
                size_t k;

                k = strcspn(p, "\n");

                যদি (p[k] == 0) {
                        যদি (k == 0) /* final empty line */
                                বিরতি;

                        যদি (n >= n_সারিগুলি) /* above threshold */
                                বিরতি;

                        e = p + k; /* last line to include */
                        বিরতি;
                }

                প্রতিজ্ঞা(p[k] == '\n');

                যদি (n >= n_সারিগুলি)
                        বিরতি;

                যদি (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        যদি (isempty(e))
                copy = strdup(s);
        অন্যথা {
                যদি (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        যদি (!copy)
                ফেরত -ENOMEM;

        *ret = copy;
        ফেরত truncation_applied;
}

পূর্ণসংখ্যা string_extract_সারি(ধ্রুবক অক্ষর *s, size_t i, অক্ষর **ret) {
        ধ্রুবক অক্ষর *p = s;
        size_t c = 0;

        প্রতিজ্ঞা(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        জন্য (;;) {
                ধ্রুবক অক্ষর *q;

                q = strchr(p, '\n');
                যদি (i == c) {
                        /* The line we are looking for! */

                        যদি (q) {
                                অক্ষর *m;

                                m = strndup(p, q - p);
                                যদি (!m)
                                        ফেরত -ENOMEM;

                                *ret = m;
                                ফেরত !isempty(q + 1); /* More coming? */
                        } অন্যথা
                                /* Tell the caller to use the input string if equal */
                                ফেরত strdup_to(ret, p != s ? p : ফাঁকা);
                }

                যদি (!q)
                        /* No more lines, return empty line */
                        ফেরত strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

পূর্ণসংখ্যা string_contains_word_strv(ধ্রুবক অক্ষর *string, ধ্রুবক অক্ষর *separators, অক্ষর * ধ্রুবক *শব্দগুলি, ধ্রুবক অক্ষর **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ধ্রুবক ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ধ্রুবক অক্ষর *found = ফাঁকা;
        পূর্ণসংখ্যা r;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *w = ফাঁকা;

                r = extract_first_word(&string, &w, separators, flags);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                found = strv_find(শব্দগুলি, w);
                যদি (found)
                        বিরতি;
        }

        যদি (ret_word)
                *ret_word = found;
        ফেরত !!found;
}

bool streq_skip_trailing_chars(ধ্রুবক অক্ষর *s1, ধ্রুবক অক্ষর *s2, ধ্রুবক অক্ষর *ok) {
        যদি (!s1 && !s2)
                ফেরত true;
        যদি (!s1 || !s2)
                ফেরত false;

        যদি (!ok)
                ok = WHITESPACE;

        জন্য (; *s1 && *s2; s1++, s2++)
                যদি (*s1 != *s2)
                        বিরতি;

        ফেরত in_charset(s1, ok) && in_charset(s2, ok);
}

অক্ষর* string_replace_char(অক্ষর *str, অক্ষর old_char, অক্ষর new_char) {
        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(old_char != '\0');
        প্রতিজ্ঞা(new_char != '\0');
        প্রতিজ্ঞা(old_char != new_char);

        জন্য (অক্ষর *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        ফেরত str;
}

পূর্ণসংখ্যা make_cstring(ধ্রুবক শূন্য *s, size_t n, MakeCStringMode mode, অক্ষর **ret) {
        অক্ষর *b;

        প্রতিজ্ঞা(s || n == 0);
        প্রতিজ্ঞা(mode >= 0);
        প্রতিজ্ঞা(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        যদি (n == 0) {
                যদি (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ফেরত -EINVAL;

                যদি (!ret)
                        ফেরত 0;

                b = new0(অক্ষর, 1);
        } অন্যথা {
                ধ্রুবক uint8_t *nul;

                nul = memchr(s, 0, n);
                যদি (nul) {
                        যদি (nul < (ধ্রুবক uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                ফেরত -EINVAL;

                        n--;
                } অন্যথা যদি (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        ফেরত -EINVAL;

                যদি (!ret)
                        ফেরত 0;

                b = memdup_suffix0(s, n);
        }
        যদি (!b)
                ফেরত -ENOMEM;

        *ret = b;
        ফেরত 0;
}

size_t strspn_from_end(ধ্রুবক অক্ষর *str, ধ্রুবক অক্ষর *accept) {
        size_t n = 0;

        যদি (isempty(str))
                ফেরত 0;

        যদি (isempty(accept))
                ফেরত 0;

        জন্য (ধ্রুবক অক্ষর *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        ফেরত n;
}

size_t strnspn(ধ্রুবক অক্ষর *str, ধ্রুবক অক্ষর *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        প্রতিজ্ঞা(str || n == 0);
        প্রতিজ্ঞা(accept);

        জন্য (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        ফেরত i;
}

অক্ষর* strdupspn(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *accept) {
        যদি (isempty(a) || isempty(accept))
                ফেরত strdup("");

        ফেরত strndup(a, strspn(a, accept));
}

অক্ষর* strdupcspn(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *reject) {
        যদি (isempty(a))
                ফেরত strdup("");
        যদি (isempty(reject))
                ফেরত strdup(a);

        ফেরত strndup(a, strcspn(a, reject));
}

অক্ষর* find_সারি_startswith_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        অক্ষর *p = (অক্ষর*) strstr(haystack, needle);
        যদি (!p)
                ফেরত ফাঁকা;

        যদি (p > haystack)
                যখন (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        যদি (!p)
                                ফেরত ফাঁকা;
                }

        ফেরত p + strlen(needle);
}

অক্ষর* find_সারি_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        অক্ষর *p = (অক্ষর*) find_সারি_startswith(haystack, needle);
        যদি (!p)
                ফেরত ফাঁকা;

        যদি (*p == 0 || strchr(NEWLINE, *p))
                ফেরত p - strlen(needle);

        ফেরত ফাঁকা;
}

অক্ষর* find_সারি_after_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        অক্ষর *p = (অক্ষর*) find_সারি_startswith(haystack, needle);
        যদি (!p)
                ফেরত ফাঁকা;

        যদি (*p == 0)
                ফেরত p;
        যদি (strchr(NEWLINE, *p))
                ফেরত p + 1;

        ফেরত ফাঁকা;
}

bool version_is_valid(ধ্রুবক অক্ষর *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        যদি (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                ফেরত false;

        যদি (!filename_part_is_valid(s))
                ফেরত false;

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

        অক্ষর charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        যদি (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        যদি (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        ফেরত in_charset(s, charset);
}

ssize_t strlevenshtein(ধ্রুবক অক্ষর *x, ধ্রুবক অক্ষর *y) {
        _cleanup_free_ size_t *t0 = ফাঁকা, *t1 = ফাঁকা, *t2 = ফাঁকা;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        যদি (streq_ptr(x, y))
                ফেরত 0;

        xl = strlen_ptr(x);
        যদি (xl > SSIZE_MAX)
                ফেরত -E2BIG;

        yl = strlen_ptr(y);
        যদি (yl > SSIZE_MAX)
                ফেরত -E2BIG;

        যদি (isempty(x))
                ফেরত yl;
        যদি (isempty(y))
                ফেরত xl;

        t0 = new0(size_t, yl + 1);
        যদি (!t0)
                ফেরত -ENOMEM;
        t1 = new0(size_t, yl + 1);
        যদি (!t1)
                ফেরত -ENOMEM;
        t2 = new0(size_t, yl + 1);
        যদি (!t2)
                ফেরত -ENOMEM;

        জন্য (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        জন্য (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                জন্য (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        যদি (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        যদি (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        যদি (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        ফেরত t1[yl];
}

অক্ষর* strrstr_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        যদি (!haystack || !needle)
                ফেরত ফাঁকা;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        যদি (needle[0] == 0)
                ফেরত (অক্ষর*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        যদি (needle[1] == 0)
                ফেরত (অক্ষর*) strrchr(haystack, needle[0]);

        জন্য (ধ্রুবক অক্ষর *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                যদি (!q)
                        ফেরত (অক্ষর*) p;
        }
        ফেরত ফাঁকা;
}

অক্ষর* strrstr_no_case_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        যদি (!haystack || !needle)
                ফেরত ফাঁকা;

        জন্য (ধ্রুবক অক্ষর *p = strchr(haystack, 0); p > haystack; p--)
                যদি (startswith_no_case(p, needle))
                        ফেরত (অক্ষর*) p;

        ফেরত startswith_no_case(haystack, needle) ? (অক্ষর*) haystack : ফাঁকা;
}

size_t str_common_prefix(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        জন্য (size_t n = 0;; n++) {
                অক্ষর c = a[n];
                যদি (c != b[n])
                        ফেরত n;
                যদি (c == 0)
                        ফেরত SIZE_MAX;
        }
}
