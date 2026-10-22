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

বর্ণ* first_word(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *word) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        যদি (isempty(word))
                প্রত্যাগম (বর্ণ*) s;

        ধ্রুব বর্ণ *p = startswith(s, word);
        যদি (!p)
                প্রত্যাগম রিক্ত;
        যদি (*p == '\0')
                প্রত্যাগম (বর্ণ*) p;

        ধ্রুব বর্ণ *nw = skip_leading_chars(p, WHITESPACE);
        যদি (p == nw)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম (বর্ণ*) nw;
}

বর্ণ* strextendn(বর্ণ **x, ধ্রুব বর্ণ *s, size_t l) {
        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(s || l == 0);

        যদি (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        যদি (l > 0 || !*x) {
                size_t q;
                বর্ণ *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                যদি (!m)
                        প্রত্যাগম রিক্ত;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        প্রত্যাগম *x;
}

বর্ণ* strstrip(বর্ণ *s) {
        যদি (!s)
                প্রত্যাগম রিক্ত;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        প্রত্যাগম delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

বর্ণ* delete_chars(বর্ণ *s, ধ্রুব বর্ণ *bad) {
        বর্ণ *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        যদি (!s)
                প্রত্যাগম রিক্ত;

        যদি (!bad)
                bad = WHITESPACE;

        প্রতি (f = s, t = s; *f; f++) {
                যদি (strchr(bad, *f))
                        অনুবর্ত;

                *(t++) = *f;
        }

        *t = 0;

        প্রত্যাগম s;
}

বর্ণ* delete_trailing_chars(বর্ণ *s, ধ্রুব বর্ণ *bad) {
        বর্ণ *c = s;

        /* Drops all specified bad characters, at the end of the string */

        যদি (!s)
                প্রত্যাগম রিক্ত;

        যদি (!bad)
                bad = WHITESPACE;

        প্রতি (বর্ণ *p = s; *p; p++)
                যদি (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        প্রত্যাগম s;
}

বর্ণ* truncate_nl_full(বর্ণ *s, size_t *ret_len) {
        size_t n;

        প্রতিজ্ঞা(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        যদি (ret_len)
                *ret_len = n;
        প্রত্যাগম s;
}

বর্ণ ascii_tolower(বর্ণ x) {

        যদি (x >= 'A' && x <= 'Z')
                প্রত্যাগম x - 'A' + 'a';

        প্রত্যাগম x;
}

বর্ণ ascii_toupper(বর্ণ x) {

        যদি (x >= 'a' && x <= 'z')
                প্রত্যাগম x - 'a' + 'A';

        প্রত্যাগম x;
}

বর্ণ* ascii_strlower(বর্ণ *s) {
        প্রতিজ্ঞা(s);

        প্রতি (বর্ণ *p = s; *p; p++)
                *p = ascii_tolower(*p);

        প্রত্যাগম s;
}

বর্ণ* ascii_strupper(বর্ণ *s) {
        প্রতিজ্ঞা(s);

        প্রতি (বর্ণ *p = s; *p; p++)
                *p = ascii_toupper(*p);

        প্রত্যাগম s;
}

বর্ণ* ascii_strlower_n(বর্ণ *s, size_t n) {
        প্রতিজ্ঞা(n <= 0 || s);

        যদি (n <= 0)
                প্রত্যাগম s;

        প্রতি (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        প্রত্যাগম s;
}

পূর্ণাঙ্ক ascii_strcasecmp_n(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b, size_t n) {

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        প্রতি (; n > 0; a++, b++, n--) {
                পূর্ণাঙ্ক x, y;

                x = (পূর্ণাঙ্ক) (uint8_t) ascii_tolower(*a);
                y = (পূর্ণাঙ্ক) (uint8_t) ascii_tolower(*b);

                যদি (x != y)
                        প্রত্যাগম x - y;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক ascii_strcasecmp_nn(ধ্রুব বর্ণ *a, size_t n, ধ্রুব বর্ণ *b, size_t m) {
        পূর্ণাঙ্ক r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        যদি (r != 0)
                প্রত্যাগম r;

        প্রত্যাগম CMP(n, m);
}

bool chars_intersect(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        /* Returns true if any of the chars in a are in b. */
        প্রতি (ধ্রুব বর্ণ *p = a; *p; p++)
                যদি (strchr(b, *p))
                        প্রত্যাগম true;

        প্রত্যাগম false;
}

bool string_has_cc(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *ok) {
        প্রতিজ্ঞা(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        প্রতি (ধ্রুব বর্ণ *t = p; *t; t++) {
                যদি (ok && strchr(ok, *t))
                        অনুবর্ত;

                যদি (char_is_cc(*t))
                        প্রত্যাগম true;
        }

        প্রত্যাগম false;
}

স্থির পূর্ণাঙ্ক write_ellipsis(বর্ণ *buf, bool unicode) {
        ধ্রুব বর্ণ *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        প্রতিজ্ঞা(strlen(s) == 3);
        memcpy(buf, s, 3);
        প্রত্যাগম 3;
}

স্থির size_t ansi_sequence_length(ধ্রুব বর্ণ *s, size_t len) {
        প্রতিজ্ঞা(s);

        যদি (len < 2)
                প্রত্যাগম 0;

        যদি (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                প্রত্যাগম 0;  /* Not the start of a sequence */

        যদি (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                যদি (i == len)
                        প্রত্যাগম 0;

                যাবত্ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        যদি (++i == len)
                                প্রত্যাগম 0;
                যাবত্ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        যদি (++i == len)
                                প্রত্যাগম 0;
                যদি (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        প্রত্যাগম i + 1;
                প্রত্যাগম 0;  /* Bad sequence */

        } অন্যথা যদি (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                প্রত্যাগম 2;

        প্রত্যাগম 0;  /* Bad escape? */
}

স্থির bool string_has_ansi_sequence(ধ্রুব বর্ণ *s, size_t len) {
        ধ্রুব বর্ণ *t = s;

        যাবত্ ((t = memchr(t, 0x1B, len - (t - s)))) {
                যদি (ansi_sequence_length(t, len - (t - s)) > 0)
                        প্রত্যাগম true;
                t++;
        }
        প্রত্যাগম false;
}

স্থির size_t previous_ansi_sequence(ধ্রুব বর্ণ *s, size_t length, ধ্রুব বর্ণ **ret_where) {

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        যদি (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = রিক্ত;
                প্রত্যাগম 0;
        }

        প্রতি (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                যদি (slen > 0) {
                        *ret_where = s + i;
                        প্রত্যাগম slen;
                }

                যদি (i == 0)
                        ভঙ্গ;
        }

        *ret_where = রিক্ত;
        প্রত্যাগম 0;
}

স্থির বর্ণ *ascii_ellipsize_mem(ধ্রুব বর্ণ *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        বর্ণ *t;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(percent <= 100);
        প্রতিজ্ঞা(new_length != SIZE_MAX);

        যদি (old_length <= new_length)
                প্রত্যাগম strndup(s, old_length);

        /* Special case short ellipsations */
        বিকল্প (new_length) {

        পক্ষ 0:
                প্রত্যাগম strdup("");

        পক্ষ 1:
                যদি (is_locale_utf8())
                        প্রত্যাগম strdup("…");
                অন্যথা
                        প্রত্যাগম strdup(".");

        পক্ষ 2:
                যদি (!is_locale_utf8())
                        প্রত্যাগম strdup("..");
                ভঙ্গ;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(বর্ণ, new_length+3);
        যদি (!t)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        প্রতিজ্ঞা(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        প্রত্যাগম t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
স্থির ধ্রুব বর্ণ* find_previous_unichar(ধ্রুব বর্ণ *start, ধ্রুব বর্ণ *end, char32_t *ret_c) {
        ধ্রুব বর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(end);
        প্রতিজ্ঞা(end > start);

        প্রতি (p = end; p > start; ) {
                p--;
                যদি (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        ভঙ্গ;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        যদি (r < 0 || p + r != end)
                প্রত্যাগম রিক্ত;

        যদি (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        প্রত্যাগম p;
}

বর্ণ* ellipsize_mem(ধ্রুব বর্ণ *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        ধ্রুব বর্ণ *i, *j;
        পূর্ণাঙ্ক r;

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

        যদি (new_length == SIZE_MAX)
                প্রত্যাগম strndup(s, old_length);

        যদি (new_length == 0)
                প্রত্যাগম strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        যদি (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                প্রত্যাগম ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        প্রতিজ্ঞা(x <= new_length - 1);

        k = 0;
        প্রতি (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                যদি (slen > 0) {
                        i += slen;
                        অনুবর্ত;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                যদি (r < 0)
                        প্রত্যাগম রিক্ত;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                পূর্ণাঙ্ক w = unichar_console_width(c);
                যদি (k + w > x)
                        ভঙ্গ;

                k += w;
                i += r;
        }

        ধ্রুব বর্ণ *ansi_start = s + old_length;
        size_t ansi_len = 0;

        প্রতি (ধ্রুব বর্ণ *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                পূর্ণাঙ্ক w;
                ধ্রুব বর্ণ *tt;

                যদি (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                যদি (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        অনুবর্ত;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                যদি (!tt)
                        প্রত্যাগম রিক্ত;

                w = unichar_console_width(c);
                যদি (k + w > new_length)
                        ভঙ্গ;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        যদি (i >= j)
                প্রত্যাগম memdup_suffix0(s, old_length);

        যদি (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                যদি (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        যদি (r < 0)
                                প্রত্যাগম রিক্ত;

                        j += r;
                } অন্যথা যদি (i > s) {
                        ধ্রুব বর্ণ *tt = find_previous_unichar(s, i, রিক্ত);
                        যদি (!tt)
                                প্রত্যাগম রিক্ত;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        বর্ণ *e = new(বর্ণ, alloc_len);
        যদি (!e)
                প্রত্যাগম রিক্ত;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        বর্ণ *dst = e + len + 3;

        যদি (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                প্রতি (ধ্রুব বর্ণ *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        যদি (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } অন্যথা
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        প্রত্যাগম e;
}

বর্ণ* cellescape(বর্ণ *buf, size_t len, ধ্রুব বর্ণ *s) {
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

        প্রতি (;;) {
                বর্ণ four[4];
                পূর্ণাঙ্ক w;

                যদি (*s == 0) /* terminating NUL detected? then we are done! */
                        গচ্ছ done;

                w = cescape_char(*s, four);
                যদি (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        ভঙ্গ;

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
        প্রতি (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                যদি (i + 4 <= len) /* nice, we reached our space goal */
                        ভঙ্গ;

                k = k == 0 ? 3 : k - 1;
                যদি (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        ভঙ্গ;

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
        প্রত্যাগম buf;
}

বর্ণ* strshorten(বর্ণ *s, size_t l) {
        প্রতিজ্ঞা(s);

        যদি (l >= SIZE_MAX-1) /* Would not change anything */
                প্রত্যাগম s;

        যদি (strnlen(s, l+1) > l)
                s[l] = 0;

        প্রত্যাগম s;
}

পূর্ণাঙ্ক strgrowpad0(বর্ণ **s, size_t l) {
        size_t sz;

        প্রতিজ্ঞা(s);

        যদি (*s) {
                sz = strlen(*s) + 1;
                যদি (sz >= l) /* never shrink */
                        প্রত্যাগম 0;
        } অন্যথা
                sz = 0;

        বর্ণ *q = realloc(*s, l);
        যদি (!q)
                প্রত্যাগম -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        প্রত্যাগম 0;
}

বর্ণ* strreplace(ধ্রুব বর্ণ *text, ধ্রুব বর্ণ *old_string, ধ্রুব বর্ণ *new_string) {
        size_t l, old_len, new_len;
        বর্ণ *t, *ret = রিক্ত;
        ধ্রুব বর্ণ *f;

        প্রতিজ্ঞা(old_string);
        প্রতিজ্ঞা(new_string);

        যদি (!text)
                প্রত্যাগম রিক্ত;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        যদি (!GREEDY_REALLOC(ret, l+1))
                প্রত্যাগম রিক্ত;

        f = text;
        t = ret;
        যাবত্ (*f) {
                size_t d, nl;

                যদি (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        অনুবর্ত;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                যদি (!GREEDY_REALLOC(ret, nl + 1))
                        প্রত্যাগম mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        প্রত্যাগম ret;
}

স্থির শূন্য advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[স্থির 2],
                size_t size) {

        যদি (!offsets)
                প্রত্যাগম;

        প্রতিজ্ঞা(shift);

        যদি ((size_t) diff < offsets[0])
                shift[0] += size;
        যদি ((size_t) diff < offsets[1])
                shift[1] += size;
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
                প্রত্যাগম;

        প্রতিজ্ঞা(shift);
        প্রতিজ্ঞা(begin <= end);

        প্রতি (size_t k = 0; k < 2; k++)
                যদি (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

বর্ণ* strip_tab_ansi(বর্ণ **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ধ্রুব বর্ণ *begin = রিক্ত;
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
                প্রত্যাগম রিক্ত;

        প্রতি (ধ্রুব বর্ণ *i = *ibuf; i < *ibuf + isz; i++) {
                পূর্ণাঙ্ক e;

                প্রতি (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        যদি (e < 0)
                                প্রত্যাগম রিক্ত;
                        যদি (e != ANSI_SEQ_EVENT_ABORT)
                                ভঙ্গ;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        যদি (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = রিক্ত;
                        }
                }

                বিকল্প (e) {

                পক্ষ ANSI_SEQ_EVENT_TEXT:
                        যদি (*i == '\r') {
                                n_carriage_returns++;
                                অনুবর্ত;
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
                        প্রতি (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        যদি (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } অন্যথা
                                fputc(*i, f);

                        ভঙ্গ;

                পক্ষ ANSI_SEQ_EVENT_SEQUENCE:
                        যদি (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                প্রতি (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        ভঙ্গ;

                পক্ষ ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        প্রতিজ্ঞা(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = রিক্ত;
                        ভঙ্গ;

                স্বভাব:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        যদি (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        বর্ণ *obuf;
        যদি (memstream_finalize(&m, &obuf, _isz) < 0)
                প্রত্যাগম রিক্ত;

        free_and_replace(*ibuf, obuf);

        যদি (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        প্রত্যাগম *ibuf;
}

বর্ণ* strextendv_with_separator(বর্ণ **x, ধ্রুব বর্ণ *separator, va_list ap) {
        _cleanup_free_ বর্ণ *buffer = রিক্ত;
        size_t f, l, l_separator;
        bool need_separator;
        বর্ণ *nr, *p;

        যদি (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        প্রতি (ধ্রুব বর্ণ *t;;) {
                size_t n;

                t = va_arg(aq, ধ্রুব বর্ণ *);
                যদি (!t)
                        ভঙ্গ;
                যদি (t == POINTER_MAX)
                        অনুবর্ত;

                n = strlen(t);

                যদি (need_separator)
                        n += l_separator;

                যদি (n >= SIZE_MAX - l) {
                        va_end(aq);
                        প্রত্যাগম রিক্ত;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        যদি (!nr)
                প্রত্যাগম রিক্ত;

        *x = nr;
        p = nr + f;

        প্রতি (;;) {
                ধ্রুব বর্ণ *t;

                t = va_arg(ap, ধ্রুব বর্ণ *);
                যদি (!t)
                        ভঙ্গ;
                যদি (t == POINTER_MAX)
                        অনুবর্ত;

                যদি (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        প্রতিজ্ঞা(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        যদি (buffer)
                প্রত্যাগম TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        প্রত্যাগম p;
}

বর্ণ* strextend_with_separator_internal(বর্ণ **x, ধ্রুব বর্ণ *separator, ...) {
        va_list ap;
        বর্ণ *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক strextendf_with_separator(বর্ণ **x, ধ্রুব বর্ণ *separator, ধ্রুব বর্ণ *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        পূর্ণাঙ্ক l;

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
                বর্ণ *n;

                যদি (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        প্রত্যাগম -ENOMEM;
                যদি (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        প্রত্যাগম -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                যদি (!n)
                        প্রত্যাগম -ENOMEM;

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
                বর্ণ *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                যদি (n)
                        *x = n;
        } অন্যথা {
                বর্ণ *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                যদি (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        গচ্ছ oom;
                যদি (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        গচ্ছ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                যদি (!n)
                        গচ্ছ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                প্রতিজ্ঞা((size_t) l < a - m - l_separator);
        }

        প্রত্যাগম 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        প্রত্যাগম -ENOMEM;
}

বর্ণ* strrep(ধ্রুব বর্ণ *s, size_t n) {
        বর্ণ *ret, *p;
        size_t l;

        প্রতিজ্ঞা(s);

        l = strlen(s);
        যদি (!MUL_ASSIGN_SAFE(&l, n))
                প্রত্যাগম রিক্ত;
        যদি (!INC_SAFE(&l, 1))
                প্রত্যাগম রিক্ত;

        p = ret = malloc(l);
        যদি (!ret)
                প্রত্যাগম রিক্ত;

        প্রতি (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        প্রত্যাগম ret;
}

পূর্ণাঙ্ক split_pair(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *sep, বর্ণ **ret_first, বর্ণ **ret_second) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(!isempty(sep));

        ধ্রুব বর্ণ *x = strstr(s, sep);
        যদি (!x)
                প্রত্যাগম -EINVAL;

        _cleanup_free_ বর্ণ *a = রিক্ত;
        যদি (ret_first) {
                a = strndup(s, x - s);
                যদি (!a)
                        প্রত্যাগম -ENOMEM;
        }

        _cleanup_free_ বর্ণ *b = রিক্ত;
        যদি (ret_second) {
                b = strdup(x + strlen(sep));
                যদি (!b)
                        প্রত্যাগম -ENOMEM;
        }

        যদি (ret_first)
                *ret_first = TAKE_PTR(a);
        যদি (ret_second)
                *ret_second = TAKE_PTR(b);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক free_and_strdup(বর্ণ **p, ধ্রুব বর্ণ *s) {
        বর্ণ *t;

        প্রতিজ্ঞা(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        যদি (streq_ptr(*p, s))
                প্রত্যাগম 0;

        যদি (s) {
                t = strdup(s);
                যদি (!t)
                        প্রত্যাগম -ENOMEM;
        } অন্যথা
                t = রিক্ত;

        free_and_replace(*p, t);

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক free_and_strdup_warn(বর্ণ **p, ধ্রুব বর্ণ *s) {
        পূর্ণাঙ্ক r;

        r = free_and_strdup(p, s);
        যদি (r < 0)
                প্রত্যাগম log_oom();
        প্রত্যাগম r;
}

পূর্ণাঙ্ক free_and_strndup(বর্ণ **p, ধ্রুব বর্ণ *s, size_t l) {
        বর্ণ *t;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        যদি (!*p && !s)
                প্রত্যাগম 0;

        যদি (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                প্রত্যাগম 0;

        যদি (s) {
                t = strndup(s, l);
                যদি (!t)
                        প্রত্যাগম -ENOMEM;
        } অন্যথা
                t = রিক্ত;

        free_and_replace(*p, t);
        প্রত্যাগম 1;
}

পূর্ণাঙ্ক strdup_to_full(বর্ণ **ret, ধ্রুব বর্ণ *src) {
        যদি (!src) {
                যদি (ret)
                        *ret = রিক্ত;

                প্রত্যাগম 0;
        } অন্যথা {
                যদি (ret) {
                        বর্ণ *t = strdup(src);
                        যদি (!t)
                                প্রত্যাগম -ENOMEM;
                        *ret = t;
                }

                প্রত্যাগম 1;
        }
};

bool string_is_safe(ধ্রুব বর্ণ *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        যদি (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                প্রত্যাগম false;

        যদি (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                প্রত্যাগম false;

        প্রতি (ধ্রুব বর্ণ *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                যদি ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        প্রত্যাগম false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        প্রত্যাগম false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        প্রত্যাগম false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        প্রত্যাগম false;

                যদি (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        প্রত্যাগম false;

                যদি (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        প্রত্যাগম false;

                যদি (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        প্রত্যাগম false;
        }

        যদি (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                প্রত্যাগম false;

        যদি (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

বর্ণ* str_realloc(বর্ণ *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        যদি (!p)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম realloc(p, strlen(p) + 1) ?: p;
}

বর্ণ* string_erase(বর্ণ *x) {
        যদি (!x)
                প্রত্যাগম রিক্ত;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        প্রত্যাগম x;
}

পূর্ণাঙ্ক string_truncate_সারিগুলি(ধ্রুব বর্ণ *s, size_t n_সারিগুলি, বর্ণ **ret) {
        ধ্রুব বর্ণ *p = s, *e = s;
        bool truncation_applied = false;
        বর্ণ *copy;
        size_t n = 0;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        প্রতি (;;) {
                size_t k;

                k = strcspn(p, "\n");

                যদি (p[k] == 0) {
                        যদি (k == 0) /* final empty line */
                                ভঙ্গ;

                        যদি (n >= n_সারিগুলি) /* above threshold */
                                ভঙ্গ;

                        e = p + k; /* last line to include */
                        ভঙ্গ;
                }

                প্রতিজ্ঞা(p[k] == '\n');

                যদি (n >= n_সারিগুলি)
                        ভঙ্গ;

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
                প্রত্যাগম -ENOMEM;

        *ret = copy;
        প্রত্যাগম truncation_applied;
}

পূর্ণাঙ্ক string_extract_সারি(ধ্রুব বর্ণ *s, size_t i, বর্ণ **ret) {
        ধ্রুব বর্ণ *p = s;
        size_t c = 0;

        প্রতিজ্ঞা(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        প্রতি (;;) {
                ধ্রুব বর্ণ *q;

                q = strchr(p, '\n');
                যদি (i == c) {
                        /* The line we are looking for! */

                        যদি (q) {
                                বর্ণ *m;

                                m = strndup(p, q - p);
                                যদি (!m)
                                        প্রত্যাগম -ENOMEM;

                                *ret = m;
                                প্রত্যাগম !isempty(q + 1); /* More coming? */
                        } অন্যথা
                                /* Tell the caller to use the input string if equal */
                                প্রত্যাগম strdup_to(ret, p != s ? p : রিক্ত);
                }

                যদি (!q)
                        /* No more lines, return empty line */
                        প্রত্যাগম strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

পূর্ণাঙ্ক string_contains_word_strv(ধ্রুব বর্ণ *string, ধ্রুব বর্ণ *separators, বর্ণ * ধ্রুব *শব্দগুলি, ধ্রুব বর্ণ **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ধ্রুব ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ধ্রুব বর্ণ *found = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতি (;;) {
                _cleanup_free_ বর্ণ *w = রিক্ত;

                r = extract_first_word(&string, &w, separators, flags);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                found = strv_find(শব্দগুলি, w);
                যদি (found)
                        ভঙ্গ;
        }

        যদি (ret_word)
                *ret_word = found;
        প্রত্যাগম !!found;
}

bool streq_skip_trailing_chars(ধ্রুব বর্ণ *s1, ধ্রুব বর্ণ *s2, ধ্রুব বর্ণ *ok) {
        যদি (!s1 && !s2)
                প্রত্যাগম true;
        যদি (!s1 || !s2)
                প্রত্যাগম false;

        যদি (!ok)
                ok = WHITESPACE;

        প্রতি (; *s1 && *s2; s1++, s2++)
                যদি (*s1 != *s2)
                        ভঙ্গ;

        প্রত্যাগম in_charset(s1, ok) && in_charset(s2, ok);
}

বর্ণ* string_replace_char(বর্ণ *str, বর্ণ old_char, বর্ণ new_char) {
        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(old_char != '\0');
        প্রতিজ্ঞা(new_char != '\0');
        প্রতিজ্ঞা(old_char != new_char);

        প্রতি (বর্ণ *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        প্রত্যাগম str;
}

পূর্ণাঙ্ক make_cstring(ধ্রুব শূন্য *s, size_t n, MakeCStringMode mode, বর্ণ **ret) {
        বর্ণ *b;

        প্রতিজ্ঞা(s || n == 0);
        প্রতিজ্ঞা(mode >= 0);
        প্রতিজ্ঞা(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        যদি (n == 0) {
                যদি (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        প্রত্যাগম -EINVAL;

                যদি (!ret)
                        প্রত্যাগম 0;

                b = new0(বর্ণ, 1);
        } অন্যথা {
                ধ্রুব uint8_t *nul;

                nul = memchr(s, 0, n);
                যদি (nul) {
                        যদি (nul < (ধ্রুব uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                প্রত্যাগম -EINVAL;

                        n--;
                } অন্যথা যদি (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        প্রত্যাগম -EINVAL;

                যদি (!ret)
                        প্রত্যাগম 0;

                b = memdup_suffix0(s, n);
        }
        যদি (!b)
                প্রত্যাগম -ENOMEM;

        *ret = b;
        প্রত্যাগম 0;
}

size_t strspn_from_end(ধ্রুব বর্ণ *str, ধ্রুব বর্ণ *accept) {
        size_t n = 0;

        যদি (isempty(str))
                প্রত্যাগম 0;

        যদি (isempty(accept))
                প্রত্যাগম 0;

        প্রতি (ধ্রুব বর্ণ *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        প্রত্যাগম n;
}

size_t strnspn(ধ্রুব বর্ণ *str, ধ্রুব বর্ণ *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        প্রতিজ্ঞা(str || n == 0);
        প্রতিজ্ঞা(accept);

        প্রতি (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        প্রত্যাগম i;
}

বর্ণ* strdupspn(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *accept) {
        যদি (isempty(a) || isempty(accept))
                প্রত্যাগম strdup("");

        প্রত্যাগম strndup(a, strspn(a, accept));
}

বর্ণ* strdupcspn(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *reject) {
        যদি (isempty(a))
                প্রত্যাগম strdup("");
        যদি (isempty(reject))
                প্রত্যাগম strdup(a);

        প্রত্যাগম strndup(a, strcspn(a, reject));
}

বর্ণ* find_সারি_startswith_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        বর্ণ *p = (বর্ণ*) strstr(haystack, needle);
        যদি (!p)
                প্রত্যাগম রিক্ত;

        যদি (p > haystack)
                যাবত্ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        যদি (!p)
                                প্রত্যাগম রিক্ত;
                }

        প্রত্যাগম p + strlen(needle);
}

বর্ণ* find_সারি_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        বর্ণ *p = (বর্ণ*) find_সারি_startswith(haystack, needle);
        যদি (!p)
                প্রত্যাগম রিক্ত;

        যদি (*p == 0 || strchr(NEWLINE, *p))
                প্রত্যাগম p - strlen(needle);

        প্রত্যাগম রিক্ত;
}

বর্ণ* find_সারি_after_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        বর্ণ *p = (বর্ণ*) find_সারি_startswith(haystack, needle);
        যদি (!p)
                প্রত্যাগম রিক্ত;

        যদি (*p == 0)
                প্রত্যাগম p;
        যদি (strchr(NEWLINE, *p))
                প্রত্যাগম p + 1;

        প্রত্যাগম রিক্ত;
}

bool version_is_valid(ধ্রুব বর্ণ *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        যদি (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                প্রত্যাগম false;

        যদি (!filename_part_is_valid(s))
                প্রত্যাগম false;

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

        বর্ণ charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        যদি (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        যদি (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        প্রত্যাগম in_charset(s, charset);
}

ssize_t strlevenshtein(ধ্রুব বর্ণ *x, ধ্রুব বর্ণ *y) {
        _cleanup_free_ size_t *t0 = রিক্ত, *t1 = রিক্ত, *t2 = রিক্ত;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        যদি (streq_ptr(x, y))
                প্রত্যাগম 0;

        xl = strlen_ptr(x);
        যদি (xl > SSIZE_MAX)
                প্রত্যাগম -E2BIG;

        yl = strlen_ptr(y);
        যদি (yl > SSIZE_MAX)
                প্রত্যাগম -E2BIG;

        যদি (isempty(x))
                প্রত্যাগম yl;
        যদি (isempty(y))
                প্রত্যাগম xl;

        t0 = new0(size_t, yl + 1);
        যদি (!t0)
                প্রত্যাগম -ENOMEM;
        t1 = new0(size_t, yl + 1);
        যদি (!t1)
                প্রত্যাগম -ENOMEM;
        t2 = new0(size_t, yl + 1);
        যদি (!t2)
                প্রত্যাগম -ENOMEM;

        প্রতি (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        প্রতি (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                প্রতি (size_t j = 0; j < yl; j++) {
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

        প্রত্যাগম t1[yl];
}

বর্ণ* strrstr_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        যদি (!haystack || !needle)
                প্রত্যাগম রিক্ত;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        যদি (needle[0] == 0)
                প্রত্যাগম (বর্ণ*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        যদি (needle[1] == 0)
                প্রত্যাগম (বর্ণ*) strrchr(haystack, needle[0]);

        প্রতি (ধ্রুব বর্ণ *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                যদি (!q)
                        প্রত্যাগম (বর্ণ*) p;
        }
        প্রত্যাগম রিক্ত;
}

বর্ণ* strrstr_no_case_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        যদি (!haystack || !needle)
                প্রত্যাগম রিক্ত;

        প্রতি (ধ্রুব বর্ণ *p = strchr(haystack, 0); p > haystack; p--)
                যদি (startswith_no_case(p, needle))
                        প্রত্যাগম (বর্ণ*) p;

        প্রত্যাগম startswith_no_case(haystack, needle) ? (বর্ণ*) haystack : রিক্ত;
}

size_t str_common_prefix(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        প্রতি (size_t n = 0;; n++) {
                বর্ণ c = a[n];
                যদি (c != b[n])
                        প্রত্যাগম n;
                যদি (c == 0)
                        প্রত্যাগম SIZE_MAX;
        }
}
