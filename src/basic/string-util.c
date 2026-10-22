/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-seq.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "glyph-util.h"
#अन्तर्भाव "gunicode.h"
#अन्तर्भाव "locale-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "memstream-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"

वर्ण* first_word(ध्रुव वर्ण *s, ध्रुव वर्ण *word) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        सचे (isempty(word))
                पटिनिवत्त (वर्ण*) s;

        ध्रुव वर्ण *p = startswith(s, word);
        सचे (!p)
                पटिनिवत्त रिक्त;
        सचे (*p == '\0')
                पटिनिवत्त (वर्ण*) p;

        ध्रुव वर्ण *nw = skip_leading_chars(p, WHITESPACE);
        सचे (p == nw)
                पटिनिवत्त रिक्त;

        पटिनिवत्त (वर्ण*) nw;
}

वर्ण* strextendn(वर्ण **x, ध्रुव वर्ण *s, size_t l) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(s || l == 0);

        सचे (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        सचे (l > 0 || !*x) {
                size_t q;
                वर्ण *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                सचे (!m)
                        पटिनिवत्त रिक्त;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        पटिनिवत्त *x;
}

वर्ण* strstrip(वर्ण *s) {
        सचे (!s)
                पटिनिवत्त रिक्त;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        पटिनिवत्त delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

वर्ण* delete_chars(वर्ण *s, ध्रुव वर्ण *bad) {
        वर्ण *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        सचे (!s)
                पटिनिवत्त रिक्त;

        सचे (!bad)
                bad = WHITESPACE;

        अत्थाय (f = s, t = s; *f; f++) {
                सचे (strchr(bad, *f))
                        अनुवर्त;

                *(t++) = *f;
        }

        *t = 0;

        पटिनिवत्त s;
}

वर्ण* delete_trailing_chars(वर्ण *s, ध्रुव वर्ण *bad) {
        वर्ण *c = s;

        /* Drops all specified bad characters, at the end of the string */

        सचे (!s)
                पटिनिवत्त रिक्त;

        सचे (!bad)
                bad = WHITESPACE;

        अत्थाय (वर्ण *p = s; *p; p++)
                सचे (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        पटिनिवत्त s;
}

वर्ण* truncate_nl_full(वर्ण *s, size_t *ret_len) {
        size_t n;

        प्रतिज्ञा(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        सचे (ret_len)
                *ret_len = n;
        पटिनिवत्त s;
}

वर्ण ascii_tolower(वर्ण x) {

        सचे (x >= 'A' && x <= 'Z')
                पटिनिवत्त x - 'A' + 'a';

        पटिनिवत्त x;
}

वर्ण ascii_toupper(वर्ण x) {

        सचे (x >= 'a' && x <= 'z')
                पटिनिवत्त x - 'a' + 'A';

        पटिनिवत्त x;
}

वर्ण* ascii_strlower(वर्ण *s) {
        प्रतिज्ञा(s);

        अत्थाय (वर्ण *p = s; *p; p++)
                *p = ascii_tolower(*p);

        पटिनिवत्त s;
}

वर्ण* ascii_strupper(वर्ण *s) {
        प्रतिज्ञा(s);

        अत्थाय (वर्ण *p = s; *p; p++)
                *p = ascii_toupper(*p);

        पटिनिवत्त s;
}

वर्ण* ascii_strlower_n(वर्ण *s, size_t n) {
        प्रतिज्ञा(n <= 0 || s);

        सचे (n <= 0)
                पटिनिवत्त s;

        अत्थाय (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        पटिनिवत्त s;
}

पूर्णाङ्क ascii_strcasecmp_n(ध्रुव वर्ण *a, ध्रुव वर्ण *b, size_t n) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        अत्थाय (; n > 0; a++, b++, n--) {
                पूर्णाङ्क x, y;

                x = (पूर्णाङ्क) (uint8_t) ascii_tolower(*a);
                y = (पूर्णाङ्क) (uint8_t) ascii_tolower(*b);

                सचे (x != y)
                        पटिनिवत्त x - y;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क ascii_strcasecmp_nn(ध्रुव वर्ण *a, size_t n, ध्रुव वर्ण *b, size_t m) {
        पूर्णाङ्क r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        सचे (r != 0)
                पटिनिवत्त r;

        पटिनिवत्त CMP(n, m);
}

bool chars_intersect(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        /* Returns true if any of the chars in a are in b. */
        अत्थाय (ध्रुव वर्ण *p = a; *p; p++)
                सचे (strchr(b, *p))
                        पटिनिवत्त true;

        पटिनिवत्त false;
}

bool string_has_cc(ध्रुव वर्ण *p, ध्रुव वर्ण *ok) {
        प्रतिज्ञा(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        अत्थाय (ध्रुव वर्ण *t = p; *t; t++) {
                सचे (ok && strchr(ok, *t))
                        अनुवर्त;

                सचे (char_is_cc(*t))
                        पटिनिवत्त true;
        }

        पटिनिवत्त false;
}

स्थिर पूर्णाङ्क write_ellipsis(वर्ण *buf, bool unicode) {
        ध्रुव वर्ण *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        प्रतिज्ञा(strlen(s) == 3);
        memcpy(buf, s, 3);
        पटिनिवत्त 3;
}

स्थिर size_t ansi_sequence_length(ध्रुव वर्ण *s, size_t len) {
        प्रतिज्ञा(s);

        सचे (len < 2)
                पटिनिवत्त 0;

        सचे (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                पटिनिवत्त 0;  /* Not the start of a sequence */

        सचे (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                सचे (i == len)
                        पटिनिवत्त 0;

                याव (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        सचे (++i == len)
                                पटिनिवत्त 0;
                याव (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        सचे (++i == len)
                                पटिनिवत्त 0;
                सचे (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        पटिनिवत्त i + 1;
                पटिनिवत्त 0;  /* Bad sequence */

        } अञ्ञथा सचे (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                पटिनिवत्त 2;

        पटिनिवत्त 0;  /* Bad escape? */
}

स्थिर bool string_has_ansi_sequence(ध्रुव वर्ण *s, size_t len) {
        ध्रुव वर्ण *t = s;

        याव ((t = memchr(t, 0x1B, len - (t - s)))) {
                सचे (ansi_sequence_length(t, len - (t - s)) > 0)
                        पटिनिवत्त true;
                t++;
        }
        पटिनिवत्त false;
}

स्थिर size_t previous_ansi_sequence(ध्रुव वर्ण *s, size_t length, ध्रुव वर्ण **ret_where) {

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        सचे (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = रिक्त;
                पटिनिवत्त 0;
        }

        अत्थाय (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                सचे (slen > 0) {
                        *ret_where = s + i;
                        पटिनिवत्त slen;
                }

                सचे (i == 0)
                        भङ्ग;
        }

        *ret_where = रिक्त;
        पटिनिवत्त 0;
}

स्थिर वर्ण *ascii_ellipsize_mem(ध्रुव वर्ण *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        वर्ण *t;

        प्रतिज्ञा(s);
        प्रतिज्ञा(percent <= 100);
        प्रतिज्ञा(new_length != SIZE_MAX);

        सचे (old_length <= new_length)
                पटिनिवत्त strndup(s, old_length);

        /* Special case short ellipsations */
        विकल्प (new_length) {

        पक्ष 0:
                पटिनिवत्त strdup("");

        पक्ष 1:
                सचे (is_locale_utf8())
                        पटिनिवत्त strdup("…");
                अञ्ञथा
                        पटिनिवत्त strdup(".");

        पक्ष 2:
                सचे (!is_locale_utf8())
                        पटिनिवत्त strdup("..");
                भङ्ग;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(वर्ण, new_length+3);
        सचे (!t)
                पटिनिवत्त रिक्त;

        प्रतिज्ञा(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        प्रतिज्ञा(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        पटिनिवत्त t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
स्थिर ध्रुव वर्ण* find_previous_unichar(ध्रुव वर्ण *start, ध्रुव वर्ण *end, char32_t *ret_c) {
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(start);
        प्रतिज्ञा(end);
        प्रतिज्ञा(end > start);

        अत्थाय (p = end; p > start; ) {
                p--;
                सचे (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        भङ्ग;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        सचे (r < 0 || p + r != end)
                पटिनिवत्त रिक्त;

        सचे (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        पटिनिवत्त p;
}

वर्ण* ellipsize_mem(ध्रुव वर्ण *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        ध्रुव वर्ण *i, *j;
        पूर्णाङ्क r;

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

        प्रतिज्ञा(s);
        प्रतिज्ञा(percent <= 100);

        सचे (new_length == SIZE_MAX)
                पटिनिवत्त strndup(s, old_length);

        सचे (new_length == 0)
                पटिनिवत्त strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        सचे (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                पटिनिवत्त ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        प्रतिज्ञा(x <= new_length - 1);

        k = 0;
        अत्थाय (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                सचे (slen > 0) {
                        i += slen;
                        अनुवर्त;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                सचे (r < 0)
                        पटिनिवत्त रिक्त;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                पूर्णाङ्क w = unichar_console_width(c);
                सचे (k + w > x)
                        भङ्ग;

                k += w;
                i += r;
        }

        ध्रुव वर्ण *ansi_start = s + old_length;
        size_t ansi_len = 0;

        अत्थाय (ध्रुव वर्ण *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                पूर्णाङ्क w;
                ध्रुव वर्ण *tt;

                सचे (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                सचे (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        अनुवर्त;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                सचे (!tt)
                        पटिनिवत्त रिक्त;

                w = unichar_console_width(c);
                सचे (k + w > new_length)
                        भङ्ग;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        सचे (i >= j)
                पटिनिवत्त memdup_suffix0(s, old_length);

        सचे (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                सचे (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        सचे (r < 0)
                                पटिनिवत्त रिक्त;

                        j += r;
                } अञ्ञथा सचे (i > s) {
                        ध्रुव वर्ण *tt = find_previous_unichar(s, i, रिक्त);
                        सचे (!tt)
                                पटिनिवत्त रिक्त;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        वर्ण *e = new(वर्ण, alloc_len);
        सचे (!e)
                पटिनिवत्त रिक्त;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        वर्ण *dst = e + len + 3;

        सचे (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                अत्थाय (ध्रुव वर्ण *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        सचे (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } अञ्ञथा
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        पटिनिवत्त e;
}

वर्ण* cellescape(वर्ण *buf, size_t len, ध्रुव वर्ण *s) {
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

        प्रतिज्ञा(buf);
        प्रतिज्ञा(len > 0); /* at least a terminating NUL */
        प्रतिज्ञा(s);

        अत्थाय (;;) {
                वर्ण four[4];
                पूर्णाङ्क w;

                सचे (*s == 0) /* terminating NUL detected? then we are done! */
                        गच्छ done;

                w = cescape_char(*s, four);
                सचे (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        भङ्ग;

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
        अत्थाय (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                सचे (i + 4 <= len) /* nice, we reached our space goal */
                        भङ्ग;

                k = k == 0 ? 3 : k - 1;
                सचे (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        भङ्ग;

                प्रतिज्ञा(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        सचे (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        अञ्ञथा सचे (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } अञ्ञथा सचे (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        अञ्ञथा
                प्रतिज्ञा(i + 1 <= len);

done:
        buf[i] = '\0';
        पटिनिवत्त buf;
}

वर्ण* strshorten(वर्ण *s, size_t l) {
        प्रतिज्ञा(s);

        सचे (l >= SIZE_MAX-1) /* Would not change anything */
                पटिनिवत्त s;

        सचे (strnlen(s, l+1) > l)
                s[l] = 0;

        पटिनिवत्त s;
}

पूर्णाङ्क strgrowpad0(वर्ण **s, size_t l) {
        size_t sz;

        प्रतिज्ञा(s);

        सचे (*s) {
                sz = strlen(*s) + 1;
                सचे (sz >= l) /* never shrink */
                        पटिनिवत्त 0;
        } अञ्ञथा
                sz = 0;

        वर्ण *q = realloc(*s, l);
        सचे (!q)
                पटिनिवत्त -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        पटिनिवत्त 0;
}

वर्ण* strreplace(ध्रुव वर्ण *text, ध्रुव वर्ण *old_string, ध्रुव वर्ण *new_string) {
        size_t l, old_len, new_len;
        वर्ण *t, *ret = रिक्त;
        ध्रुव वर्ण *f;

        प्रतिज्ञा(old_string);
        प्रतिज्ञा(new_string);

        सचे (!text)
                पटिनिवत्त रिक्त;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        सचे (!GREEDY_REALLOC(ret, l+1))
                पटिनिवत्त रिक्त;

        f = text;
        t = ret;
        याव (*f) {
                size_t d, nl;

                सचे (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        अनुवर्त;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                सचे (!GREEDY_REALLOC(ret, nl + 1))
                        पटिनिवत्त mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        पटिनिवत्त ret;
}

स्थिर शून्य advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[स्थिर 2],
                size_t size) {

        सचे (!offsets)
                पटिनिवत्त;

        प्रतिज्ञा(shift);

        सचे ((size_t) diff < offsets[0])
                shift[0] += size;
        सचे ((size_t) diff < offsets[1])
                shift[1] += size;
}

स्थिर शून्य retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[स्थिर 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        सचे (!offsets)
                पटिनिवत्त;

        प्रतिज्ञा(shift);
        प्रतिज्ञा(begin <= end);

        अत्थाय (size_t k = 0; k < 2; k++)
                सचे (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

वर्ण* strip_tab_ansi(वर्ण **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ध्रुव वर्ण *begin = रिक्त;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        प्रतिज्ञा(ibuf);
        प्रतिज्ञा(*ibuf);
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
        सचे (!f)
                पटिनिवत्त रिक्त;

        अत्थाय (ध्रुव वर्ण *i = *ibuf; i < *ibuf + isz; i++) {
                पूर्णाङ्क e;

                अत्थाय (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        सचे (e < 0)
                                पटिनिवत्त रिक्त;
                        सचे (e != ANSI_SEQ_EVENT_ABORT)
                                भङ्ग;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        सचे (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = रिक्त;
                        }
                }

                विकल्प (e) {

                पक्ष ANSI_SEQ_EVENT_TEXT:
                        सचे (*i == '\r') {
                                n_carriage_returns++;
                                अनुवर्त;
                        }
                        सचे (*i == '\n') {
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
                        अत्थाय (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        सचे (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } अञ्ञथा
                                fputc(*i, f);

                        भङ्ग;

                पक्ष ANSI_SEQ_EVENT_SEQUENCE:
                        सचे (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                अत्थाय (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        भङ्ग;

                पक्ष ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        प्रतिज्ञा(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = रिक्त;
                        भङ्ग;

                स्वभाव:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        सचे (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        वर्ण *obuf;
        सचे (memstream_finalize(&m, &obuf, _isz) < 0)
                पटिनिवत्त रिक्त;

        free_and_replace(*ibuf, obuf);

        सचे (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        पटिनिवत्त *ibuf;
}

वर्ण* strextendv_with_separator(वर्ण **x, ध्रुव वर्ण *separator, va_list ap) {
        _cleanup_free_ वर्ण *buffer = रिक्त;
        size_t f, l, l_separator;
        bool need_separator;
        वर्ण *nr, *p;

        सचे (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        अत्थाय (ध्रुव वर्ण *t;;) {
                size_t n;

                t = va_arg(aq, ध्रुव वर्ण *);
                सचे (!t)
                        भङ्ग;
                सचे (t == POINTER_MAX)
                        अनुवर्त;

                n = strlen(t);

                सचे (need_separator)
                        n += l_separator;

                सचे (n >= SIZE_MAX - l) {
                        va_end(aq);
                        पटिनिवत्त रिक्त;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        सचे (!nr)
                पटिनिवत्त रिक्त;

        *x = nr;
        p = nr + f;

        अत्थाय (;;) {
                ध्रुव वर्ण *t;

                t = va_arg(ap, ध्रुव वर्ण *);
                सचे (!t)
                        भङ्ग;
                सचे (t == POINTER_MAX)
                        अनुवर्त;

                सचे (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        प्रतिज्ञा(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        सचे (buffer)
                पटिनिवत्त TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        पटिनिवत्त p;
}

वर्ण* strextend_with_separator_internal(वर्ण **x, ध्रुव वर्ण *separator, ...) {
        va_list ap;
        वर्ण *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        पटिनिवत्त ret;
}

पूर्णाङ्क strextendf_with_separator(वर्ण **x, ध्रुव वर्ण *separator, ध्रुव वर्ण *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        पूर्णाङ्क l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        प्रतिज्ञा(x);
        प्रतिज्ञा(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        सचे (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                प्रतिज्ञा(a >= m + 1);
        } अञ्ञथा
                m = a = 0;

        सचे (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                वर्ण *n;

                सचे (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        पटिनिवत्त -ENOMEM;
                सचे (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        पटिनिवत्त -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                सचे (!n)
                        पटिनिवत्त -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        प्रतिज्ञा(l >= 0);

        सचे ((size_t) l < a - m - l_separator) {
                वर्ण *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                सचे (n)
                        *x = n;
        } अञ्ञथा {
                वर्ण *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                सचे (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        गच्छ oom;
                सचे (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        गच्छ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                सचे (!n)
                        गच्छ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                प्रतिज्ञा((size_t) l < a - m - l_separator);
        }

        पटिनिवत्त 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        पटिनिवत्त -ENOMEM;
}

वर्ण* strrep(ध्रुव वर्ण *s, size_t n) {
        वर्ण *ret, *p;
        size_t l;

        प्रतिज्ञा(s);

        l = strlen(s);
        सचे (!MUL_ASSIGN_SAFE(&l, n))
                पटिनिवत्त रिक्त;
        सचे (!INC_SAFE(&l, 1))
                पटिनिवत्त रिक्त;

        p = ret = malloc(l);
        सचे (!ret)
                पटिनिवत्त रिक्त;

        अत्थाय (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        पटिनिवत्त ret;
}

पूर्णाङ्क split_pair(ध्रुव वर्ण *s, ध्रुव वर्ण *sep, वर्ण **ret_first, वर्ण **ret_second) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(!isempty(sep));

        ध्रुव वर्ण *x = strstr(s, sep);
        सचे (!x)
                पटिनिवत्त -EINVAL;

        _cleanup_free_ वर्ण *a = रिक्त;
        सचे (ret_first) {
                a = strndup(s, x - s);
                सचे (!a)
                        पटिनिवत्त -ENOMEM;
        }

        _cleanup_free_ वर्ण *b = रिक्त;
        सचे (ret_second) {
                b = strdup(x + strlen(sep));
                सचे (!b)
                        पटिनिवत्त -ENOMEM;
        }

        सचे (ret_first)
                *ret_first = TAKE_PTR(a);
        सचे (ret_second)
                *ret_second = TAKE_PTR(b);
        पटिनिवत्त 0;
}

पूर्णाङ्क free_and_strdup(वर्ण **p, ध्रुव वर्ण *s) {
        वर्ण *t;

        प्रतिज्ञा(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        सचे (streq_ptr(*p, s))
                पटिनिवत्त 0;

        सचे (s) {
                t = strdup(s);
                सचे (!t)
                        पटिनिवत्त -ENOMEM;
        } अञ्ञथा
                t = रिक्त;

        free_and_replace(*p, t);

        पटिनिवत्त 1;
}

पूर्णाङ्क free_and_strdup_warn(वर्ण **p, ध्रुव वर्ण *s) {
        पूर्णाङ्क r;

        r = free_and_strdup(p, s);
        सचे (r < 0)
                पटिनिवत्त log_oom();
        पटिनिवत्त r;
}

पूर्णाङ्क free_and_strndup(वर्ण **p, ध्रुव वर्ण *s, size_t l) {
        वर्ण *t;

        प्रतिज्ञा(p);
        प्रतिज्ञा(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        सचे (!*p && !s)
                पटिनिवत्त 0;

        सचे (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                पटिनिवत्त 0;

        सचे (s) {
                t = strndup(s, l);
                सचे (!t)
                        पटिनिवत्त -ENOMEM;
        } अञ्ञथा
                t = रिक्त;

        free_and_replace(*p, t);
        पटिनिवत्त 1;
}

पूर्णाङ्क strdup_to_full(वर्ण **ret, ध्रुव वर्ण *src) {
        सचे (!src) {
                सचे (ret)
                        *ret = रिक्त;

                पटिनिवत्त 0;
        } अञ्ञथा {
                सचे (ret) {
                        वर्ण *t = strdup(src);
                        सचे (!t)
                                पटिनिवत्त -ENOMEM;
                        *ret = t;
                }

                पटिनिवत्त 1;
        }
};

bool string_is_safe(ध्रुव वर्ण *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        सचे (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                पटिनिवत्त false;

        सचे (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                पटिनिवत्त false;

        अत्थाय (ध्रुव वर्ण *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                सचे ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        पटिनिवत्त false;

                सचे (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        पटिनिवत्त false;

                सचे (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        पटिनिवत्त false;

                सचे (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        पटिनिवत्त false;

                सचे (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        पटिनिवत्त false;

                सचे (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        पटिनिवत्त false;

                सचे (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        पटिनिवत्त false;
        }

        सचे (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                पटिनिवत्त false;

        सचे (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                पटिनिवत्त false;

        पटिनिवत्त true;
}

वर्ण* str_realloc(वर्ण *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        सचे (!p)
                पटिनिवत्त रिक्त;

        पटिनिवत्त realloc(p, strlen(p) + 1) ?: p;
}

वर्ण* string_erase(वर्ण *x) {
        सचे (!x)
                पटिनिवत्त रिक्त;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        पटिनिवत्त x;
}

पूर्णाङ्क string_truncate_पङ्क्तयः(ध्रुव वर्ण *s, size_t n_पङ्क्तयः, वर्ण **ret) {
        ध्रुव वर्ण *p = s, *e = s;
        bool truncation_applied = false;
        वर्ण *copy;
        size_t n = 0;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        अत्थाय (;;) {
                size_t k;

                k = strcspn(p, "\n");

                सचे (p[k] == 0) {
                        सचे (k == 0) /* final empty line */
                                भङ्ग;

                        सचे (n >= n_पङ्क्तयः) /* above threshold */
                                भङ्ग;

                        e = p + k; /* last line to include */
                        भङ्ग;
                }

                प्रतिज्ञा(p[k] == '\n');

                सचे (n >= n_पङ्क्तयः)
                        भङ्ग;

                सचे (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        सचे (isempty(e))
                copy = strdup(s);
        अञ्ञथा {
                सचे (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        सचे (!copy)
                पटिनिवत्त -ENOMEM;

        *ret = copy;
        पटिनिवत्त truncation_applied;
}

पूर्णाङ्क string_extract_पङ्क्तिः(ध्रुव वर्ण *s, size_t i, वर्ण **ret) {
        ध्रुव वर्ण *p = s;
        size_t c = 0;

        प्रतिज्ञा(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        अत्थाय (;;) {
                ध्रुव वर्ण *q;

                q = strchr(p, '\n');
                सचे (i == c) {
                        /* The line we are looking for! */

                        सचे (q) {
                                वर्ण *m;

                                m = strndup(p, q - p);
                                सचे (!m)
                                        पटिनिवत्त -ENOMEM;

                                *ret = m;
                                पटिनिवत्त !isempty(q + 1); /* More coming? */
                        } अञ्ञथा
                                /* Tell the caller to use the input string if equal */
                                पटिनिवत्त strdup_to(ret, p != s ? p : रिक्त);
                }

                सचे (!q)
                        /* No more lines, return empty line */
                        पटिनिवत्त strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

पूर्णाङ्क string_contains_word_strv(ध्रुव वर्ण *string, ध्रुव वर्ण *separators, वर्ण * ध्रुव *शब्दाः, ध्रुव वर्ण **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ध्रुव ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ध्रुव वर्ण *found = रिक्त;
        पूर्णाङ्क r;

        अत्थाय (;;) {
                _cleanup_free_ वर्ण *w = रिक्त;

                r = extract_first_word(&string, &w, separators, flags);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        भङ्ग;

                found = strv_find(शब्दाः, w);
                सचे (found)
                        भङ्ग;
        }

        सचे (ret_word)
                *ret_word = found;
        पटिनिवत्त !!found;
}

bool streq_skip_trailing_chars(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, ध्रुव वर्ण *ok) {
        सचे (!s1 && !s2)
                पटिनिवत्त true;
        सचे (!s1 || !s2)
                पटिनिवत्त false;

        सचे (!ok)
                ok = WHITESPACE;

        अत्थाय (; *s1 && *s2; s1++, s2++)
                सचे (*s1 != *s2)
                        भङ्ग;

        पटिनिवत्त in_charset(s1, ok) && in_charset(s2, ok);
}

वर्ण* string_replace_char(वर्ण *str, वर्ण old_char, वर्ण new_char) {
        प्रतिज्ञा(str);
        प्रतिज्ञा(old_char != '\0');
        प्रतिज्ञा(new_char != '\0');
        प्रतिज्ञा(old_char != new_char);

        अत्थाय (वर्ण *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        पटिनिवत्त str;
}

पूर्णाङ्क make_cstring(ध्रुव शून्य *s, size_t n, MakeCStringMode mode, वर्ण **ret) {
        वर्ण *b;

        प्रतिज्ञा(s || n == 0);
        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        सचे (n == 0) {
                सचे (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        पटिनिवत्त -EINVAL;

                सचे (!ret)
                        पटिनिवत्त 0;

                b = new0(वर्ण, 1);
        } अञ्ञथा {
                ध्रुव uint8_t *nul;

                nul = memchr(s, 0, n);
                सचे (nul) {
                        सचे (nul < (ध्रुव uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                पटिनिवत्त -EINVAL;

                        n--;
                } अञ्ञथा सचे (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        पटिनिवत्त -EINVAL;

                सचे (!ret)
                        पटिनिवत्त 0;

                b = memdup_suffix0(s, n);
        }
        सचे (!b)
                पटिनिवत्त -ENOMEM;

        *ret = b;
        पटिनिवत्त 0;
}

size_t strspn_from_end(ध्रुव वर्ण *str, ध्रुव वर्ण *accept) {
        size_t n = 0;

        सचे (isempty(str))
                पटिनिवत्त 0;

        सचे (isempty(accept))
                पटिनिवत्त 0;

        अत्थाय (ध्रुव वर्ण *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        पटिनिवत्त n;
}

size_t strnspn(ध्रुव वर्ण *str, ध्रुव वर्ण *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        प्रतिज्ञा(str || n == 0);
        प्रतिज्ञा(accept);

        अत्थाय (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        पटिनिवत्त i;
}

वर्ण* strdupspn(ध्रुव वर्ण *a, ध्रुव वर्ण *accept) {
        सचे (isempty(a) || isempty(accept))
                पटिनिवत्त strdup("");

        पटिनिवत्त strndup(a, strspn(a, accept));
}

वर्ण* strdupcspn(ध्रुव वर्ण *a, ध्रुव वर्ण *reject) {
        सचे (isempty(a))
                पटिनिवत्त strdup("");
        सचे (isempty(reject))
                पटिनिवत्त strdup(a);

        पटिनिवत्त strndup(a, strcspn(a, reject));
}

वर्ण* find_पङ्क्तिः_startswith_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        वर्ण *p = (वर्ण*) strstr(haystack, needle);
        सचे (!p)
                पटिनिवत्त रिक्त;

        सचे (p > haystack)
                याव (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        सचे (!p)
                                पटिनिवत्त रिक्त;
                }

        पटिनिवत्त p + strlen(needle);
}

वर्ण* find_पङ्क्तिः_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        वर्ण *p = (वर्ण*) find_पङ्क्तिः_startswith(haystack, needle);
        सचे (!p)
                पटिनिवत्त रिक्त;

        सचे (*p == 0 || strchr(NEWLINE, *p))
                पटिनिवत्त p - strlen(needle);

        पटिनिवत्त रिक्त;
}

वर्ण* find_पङ्क्तिः_after_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        वर्ण *p = (वर्ण*) find_पङ्क्तिः_startswith(haystack, needle);
        सचे (!p)
                पटिनिवत्त रिक्त;

        सचे (*p == 0)
                पटिनिवत्त p;
        सचे (strchr(NEWLINE, *p))
                पटिनिवत्त p + 1;

        पटिनिवत्त रिक्त;
}

bool version_is_valid(ध्रुव वर्ण *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        सचे (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                पटिनिवत्त false;

        सचे (!filename_part_is_valid(s))
                पटिनिवत्त false;

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

        वर्ण charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        सचे (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        सचे (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        पटिनिवत्त in_charset(s, charset);
}

ssize_t strlevenshtein(ध्रुव वर्ण *x, ध्रुव वर्ण *y) {
        _cleanup_free_ size_t *t0 = रिक्त, *t1 = रिक्त, *t2 = रिक्त;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        सचे (streq_ptr(x, y))
                पटिनिवत्त 0;

        xl = strlen_ptr(x);
        सचे (xl > SSIZE_MAX)
                पटिनिवत्त -E2BIG;

        yl = strlen_ptr(y);
        सचे (yl > SSIZE_MAX)
                पटिनिवत्त -E2BIG;

        सचे (isempty(x))
                पटिनिवत्त yl;
        सचे (isempty(y))
                पटिनिवत्त xl;

        t0 = new0(size_t, yl + 1);
        सचे (!t0)
                पटिनिवत्त -ENOMEM;
        t1 = new0(size_t, yl + 1);
        सचे (!t1)
                पटिनिवत्त -ENOMEM;
        t2 = new0(size_t, yl + 1);
        सचे (!t2)
                पटिनिवत्त -ENOMEM;

        अत्थाय (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        अत्थाय (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                अत्थाय (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        सचे (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        सचे (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        सचे (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        पटिनिवत्त t1[yl];
}

वर्ण* strrstr_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        सचे (!haystack || !needle)
                पटिनिवत्त रिक्त;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        सचे (needle[0] == 0)
                पटिनिवत्त (वर्ण*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        सचे (needle[1] == 0)
                पटिनिवत्त (वर्ण*) strrchr(haystack, needle[0]);

        अत्थाय (ध्रुव वर्ण *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                सचे (!q)
                        पटिनिवत्त (वर्ण*) p;
        }
        पटिनिवत्त रिक्त;
}

वर्ण* strrstr_no_case_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        सचे (!haystack || !needle)
                पटिनिवत्त रिक्त;

        अत्थाय (ध्रुव वर्ण *p = strchr(haystack, 0); p > haystack; p--)
                सचे (startswith_no_case(p, needle))
                        पटिनिवत्त (वर्ण*) p;

        पटिनिवत्त startswith_no_case(haystack, needle) ? (वर्ण*) haystack : रिक्त;
}

size_t str_common_prefix(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        अत्थाय (size_t n = 0;; n++) {
                वर्ण c = a[n];
                सचे (c != b[n])
                        पटिनिवत्त n;
                सचे (c == 0)
                        पटिनिवत्त SIZE_MAX;
        }
}
