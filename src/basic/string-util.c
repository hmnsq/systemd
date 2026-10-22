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

        यदि (isempty(word))
                प्रत्यागम (वर्ण*) s;

        ध्रुव वर्ण *p = startswith(s, word);
        यदि (!p)
                प्रत्यागम रिक्त;
        यदि (*p == '\0')
                प्रत्यागम (वर्ण*) p;

        ध्रुव वर्ण *nw = skip_leading_chars(p, WHITESPACE);
        यदि (p == nw)
                प्रत्यागम रिक्त;

        प्रत्यागम (वर्ण*) nw;
}

वर्ण* strextendn(वर्ण **x, ध्रुव वर्ण *s, माप_प्रकार l) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(s || l == 0);

        यदि (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        यदि (l > 0 || !*x) {
                माप_प्रकार q;
                वर्ण *m;

                q = strlen_ptr(*x);
                m = पुनः_स्मृति(*x, q + l + 1);
                यदि (!m)
                        प्रत्यागम रिक्त;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        प्रत्यागम *x;
}

वर्ण* strstrip(वर्ण *s) {
        यदि (!s)
                प्रत्यागम रिक्त;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        प्रत्यागम delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

वर्ण* delete_chars(वर्ण *s, ध्रुव वर्ण *bad) {
        वर्ण *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        यदि (!s)
                प्रत्यागम रिक्त;

        यदि (!bad)
                bad = WHITESPACE;

        प्रति (f = s, t = s; *f; f++) {
                यदि (म_अक्षर(bad, *f))
                        अनुवर्त;

                *(t++) = *f;
        }

        *t = 0;

        प्रत्यागम s;
}

वर्ण* delete_trailing_chars(वर्ण *s, ध्रुव वर्ण *bad) {
        वर्ण *c = s;

        /* Drops all specified bad characters, at the end of the string */

        यदि (!s)
                प्रत्यागम रिक्त;

        यदि (!bad)
                bad = WHITESPACE;

        प्रति (वर्ण *p = s; *p; p++)
                यदि (!म_अक्षर(bad, *p))
                        c = p + 1;

        *c = 0;

        प्रत्यागम s;
}

वर्ण* truncate_nl_full(वर्ण *s, माप_प्रकार *ret_len) {
        माप_प्रकार n;

        प्रतिज्ञा(s);

        n = म_खोज(s, NEWLINE);
        s[n] = '\0';
        यदि (ret_len)
                *ret_len = n;
        प्रत्यागम s;
}

वर्ण ascii_tolower(वर्ण x) {

        यदि (x >= 'A' && x <= 'Z')
                प्रत्यागम x - 'A' + 'a';

        प्रत्यागम x;
}

वर्ण ascii_toupper(वर्ण x) {

        यदि (x >= 'a' && x <= 'z')
                प्रत्यागम x - 'a' + 'A';

        प्रत्यागम x;
}

वर्ण* ascii_strlower(वर्ण *s) {
        प्रतिज्ञा(s);

        प्रति (वर्ण *p = s; *p; p++)
                *p = ascii_tolower(*p);

        प्रत्यागम s;
}

वर्ण* ascii_strupper(वर्ण *s) {
        प्रतिज्ञा(s);

        प्रति (वर्ण *p = s; *p; p++)
                *p = ascii_toupper(*p);

        प्रत्यागम s;
}

वर्ण* ascii_strlower_n(वर्ण *s, माप_प्रकार n) {
        प्रतिज्ञा(n <= 0 || s);

        यदि (n <= 0)
                प्रत्यागम s;

        प्रति (माप_प्रकार i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        प्रत्यागम s;
}

पूर्णाङ्क ascii_strcasecmp_n(ध्रुव वर्ण *a, ध्रुव वर्ण *b, माप_प्रकार n) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        प्रति (; n > 0; a++, b++, n--) {
                पूर्णाङ्क x, y;

                x = (पूर्णाङ्क) (uint8_t) ascii_tolower(*a);
                y = (पूर्णाङ्क) (uint8_t) ascii_tolower(*b);

                यदि (x != y)
                        प्रत्यागम x - y;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क ascii_strcasecmp_nn(ध्रुव वर्ण *a, माप_प्रकार n, ध्रुव वर्ण *b, माप_प्रकार m) {
        पूर्णाङ्क r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        यदि (r != 0)
                प्रत्यागम r;

        प्रत्यागम CMP(n, m);
}

bool chars_intersect(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        /* Returns true if any of the chars in a are in b. */
        प्रति (ध्रुव वर्ण *p = a; *p; p++)
                यदि (म_अक्षर(b, *p))
                        प्रत्यागम true;

        प्रत्यागम false;
}

bool string_has_cc(ध्रुव वर्ण *p, ध्रुव वर्ण *ok) {
        प्रतिज्ञा(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        प्रति (ध्रुव वर्ण *t = p; *t; t++) {
                यदि (ok && म_अक्षर(ok, *t))
                        अनुवर्त;

                यदि (char_is_cc(*t))
                        प्रत्यागम true;
        }

        प्रत्यागम false;
}

स्थिर पूर्णाङ्क write_ellipsis(वर्ण *buf, bool unicode) {
        ध्रुव वर्ण *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        प्रतिज्ञा(म_माप(s) == 3);
        स_नकल(buf, s, 3);
        प्रत्यागम 3;
}

स्थिर माप_प्रकार ansi_sequence_length(ध्रुव वर्ण *s, माप_प्रकार len) {
        प्रतिज्ञा(s);

        यदि (len < 2)
                प्रत्यागम 0;

        यदि (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                प्रत्यागम 0;  /* Not the start of a sequence */

        यदि (s[1] == 0x5B) { /* [, start of CSI sequence */
                माप_प्रकार i = 2;

                यदि (i == len)
                        प्रत्यागम 0;

                यावत् (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        यदि (++i == len)
                                प्रत्यागम 0;
                यावत् (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        यदि (++i == len)
                                प्रत्यागम 0;
                यदि (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        प्रत्यागम i + 1;
                प्रत्यागम 0;  /* Bad sequence */

        } अन्यथा यदि (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                प्रत्यागम 2;

        प्रत्यागम 0;  /* Bad escape? */
}

स्थिर bool string_has_ansi_sequence(ध्रुव वर्ण *s, माप_प्रकार len) {
        ध्रुव वर्ण *t = s;

        यावत् ((t = स_प्रथम(t, 0x1B, len - (t - s)))) {
                यदि (ansi_sequence_length(t, len - (t - s)) > 0)
                        प्रत्यागम true;
                t++;
        }
        प्रत्यागम false;
}

स्थिर माप_प्रकार previous_ansi_sequence(ध्रुव वर्ण *s, माप_प्रकार length, ध्रुव वर्ण **ret_where) {

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        यदि (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = रिक्त;
                प्रत्यागम 0;
        }

        प्रति (माप_प्रकार i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                माप_प्रकार slen = ansi_sequence_length(s + i, length - i);
                यदि (slen > 0) {
                        *ret_where = s + i;
                        प्रत्यागम slen;
                }

                यदि (i == 0)
                        भङ्ग;
        }

        *ret_where = रिक्त;
        प्रत्यागम 0;
}

स्थिर वर्ण *ascii_ellipsize_mem(ध्रुव वर्ण *s, माप_प्रकार old_length, माप_प्रकार new_length, अचिन्हित percent) {
        माप_प्रकार x, need_space, suffix_len;
        वर्ण *t;

        प्रतिज्ञा(s);
        प्रतिज्ञा(percent <= 100);
        प्रतिज्ञा(new_length != SIZE_MAX);

        यदि (old_length <= new_length)
                प्रत्यागम strndup(s, old_length);

        /* Special case short ellipsations */
        विकल्प (new_length) {

        पक्ष 0:
                प्रत्यागम strdup("");

        पक्ष 1:
                यदि (is_locale_utf8())
                        प्रत्यागम strdup("…");
                अन्यथा
                        प्रत्यागम strdup(".");

        पक्ष 2:
                यदि (!is_locale_utf8())
                        प्रत्यागम strdup("..");
                भङ्ग;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(वर्ण, new_length+3);
        यदि (!t)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        प्रतिज्ञा(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        स_नकल(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        प्रत्यागम t;
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

        प्रति (p = end; p > start; ) {
                p--;
                यदि (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        भङ्ग;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        यदि (r < 0 || p + r != end)
                प्रत्यागम रिक्त;

        यदि (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        प्रत्यागम p;
}

वर्ण* ellipsize_mem(ध्रुव वर्ण *s, माप_प्रकार old_length, माप_प्रकार new_length, अचिन्हित percent) {
        माप_प्रकार x, k, len, len2;
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

        यदि (new_length == SIZE_MAX)
                प्रत्यागम strndup(s, old_length);

        यदि (new_length == 0)
                प्रत्यागम strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        यदि (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !स_प्रथम(s, '\t', old_length))
                प्रत्यागम ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        प्रतिज्ञा(x <= new_length - 1);

        k = 0;
        प्रति (i = s; i < s + old_length; ) {
                माप_प्रकार slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                यदि (slen > 0) {
                        i += slen;
                        अनुवर्त;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                यदि (r < 0)
                        प्रत्यागम रिक्त;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                पूर्णाङ्क w = unichar_console_width(c);
                यदि (k + w > x)
                        भङ्ग;

                k += w;
                i += r;
        }

        ध्रुव वर्ण *ansi_start = s + old_length;
        माप_प्रकार ansi_len = 0;

        प्रति (ध्रुव वर्ण *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                पूर्णाङ्क w;
                ध्रुव वर्ण *tt;

                यदि (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                यदि (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        अनुवर्त;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                यदि (!tt)
                        प्रत्यागम रिक्त;

                w = unichar_console_width(c);
                यदि (k + w > new_length)
                        भङ्ग;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        यदि (i >= j)
                प्रत्यागम memdup_suffix0(s, old_length);

        यदि (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                यदि (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        यदि (r < 0)
                                प्रत्यागम रिक्त;

                        j += r;
                } अन्यथा यदि (i > s) {
                        ध्रुव वर्ण *tt = find_previous_unichar(s, i, रिक्त);
                        यदि (!tt)
                                प्रत्यागम रिक्त;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        माप_प्रकार alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        वर्ण *e = new(वर्ण, alloc_len);
        यदि (!e)
                प्रत्यागम रिक्त;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        वर्ण *dst = e + len + 3;

        यदि (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                प्रति (ध्रुव वर्ण *p = s + len; p < j; ) {
                        माप_प्रकार slen = ansi_sequence_length(p, j - p);
                        यदि (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } अन्यथा
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        प्रत्यागम e;
}

वर्ण* cellescape(वर्ण *buf, माप_प्रकार len, ध्रुव वर्ण *s) {
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

        माप_प्रकार i = 0, last_char_width[4] = {}, k = 0;

        प्रतिज्ञा(buf);
        प्रतिज्ञा(len > 0); /* at least a terminating NUL */
        प्रतिज्ञा(s);

        प्रति (;;) {
                वर्ण four[4];
                पूर्णाङ्क w;

                यदि (*s == 0) /* terminating NUL detected? then we are done! */
                        गच्छ done;

                w = cescape_char(*s, four);
                यदि (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        भङ्ग;

                /* OK, there was space, let's add this escaped character to the buffer */
                स_नकल(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                last_char_width[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        प्रति (माप_प्रकार j = 0; j < ELEMENTSOF(last_char_width); j++) {

                यदि (i + 4 <= len) /* nice, we reached our space goal */
                        भङ्ग;

                k = k == 0 ? 3 : k - 1;
                यदि (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        भङ्ग;

                प्रतिज्ञा(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        यदि (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        अन्यथा यदि (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } अन्यथा यदि (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        अन्यथा
                प्रतिज्ञा(i + 1 <= len);

done:
        buf[i] = '\0';
        प्रत्यागम buf;
}

वर्ण* strshorten(वर्ण *s, माप_प्रकार l) {
        प्रतिज्ञा(s);

        यदि (l >= SIZE_MAX-1) /* Would not change anything */
                प्रत्यागम s;

        यदि (strnlen(s, l+1) > l)
                s[l] = 0;

        प्रत्यागम s;
}

पूर्णाङ्क strgrowpad0(वर्ण **s, माप_प्रकार l) {
        माप_प्रकार sz;

        प्रतिज्ञा(s);

        यदि (*s) {
                sz = म_माप(*s) + 1;
                यदि (sz >= l) /* never shrink */
                        प्रत्यागम 0;
        } अन्यथा
                sz = 0;

        वर्ण *q = पुनः_स्मृति(*s, l);
        यदि (!q)
                प्रत्यागम -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        प्रत्यागम 0;
}

वर्ण* strreplace(ध्रुव वर्ण *text, ध्रुव वर्ण *old_string, ध्रुव वर्ण *new_string) {
        माप_प्रकार l, old_len, new_len;
        वर्ण *t, *ret = रिक्त;
        ध्रुव वर्ण *f;

        प्रतिज्ञा(old_string);
        प्रतिज्ञा(new_string);

        यदि (!text)
                प्रत्यागम रिक्त;

        old_len = म_माप(old_string);
        new_len = म_माप(new_string);

        l = म_माप(text);
        यदि (!GREEDY_REALLOC(ret, l+1))
                प्रत्यागम रिक्त;

        f = text;
        t = ret;
        यावत् (*f) {
                माप_प्रकार d, nl;

                यदि (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        अनुवर्त;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                यदि (!GREEDY_REALLOC(ret, nl + 1))
                        प्रत्यागम mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        प्रत्यागम ret;
}

स्थिर शून्य advance_offsets(
                ssize_t diff,
                माप_प्रकार offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                माप_प्रकार shift[स्थिर 2],
                माप_प्रकार size) {

        यदि (!offsets)
                प्रत्यागम;

        प्रतिज्ञा(shift);

        यदि ((माप_प्रकार) diff < offsets[0])
                shift[0] += size;
        यदि ((माप_प्रकार) diff < offsets[1])
                shift[1] += size;
}

स्थिर शून्य retract_offsets(
                माप_प्रकार begin,
                माप_प्रकार end,
                माप_प्रकार offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                माप_प्रकार shift[स्थिर 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        यदि (!offsets)
                प्रत्यागम;

        प्रतिज्ञा(shift);
        प्रतिज्ञा(begin <= end);

        प्रति (माप_प्रकार k = 0; k < 2; k++)
                यदि (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

वर्ण* strip_tab_ansi(वर्ण **ibuf, माप_प्रकार *_isz, माप_प्रकार highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        ध्रुव वर्ण *begin = रिक्त;
        माप_प्रकार isz, shift[2] = {}, n_carriage_returns = 0;
        खाता *f;

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

        isz = _isz ? *_isz : म_माप(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        यदि (!f)
                प्रत्यागम रिक्त;

        प्रति (ध्रुव वर्ण *i = *ibuf; i < *ibuf + isz; i++) {
                पूर्णाङ्क e;

                प्रति (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        यदि (e < 0)
                                प्रत्यागम रिक्त;
                        यदि (e != ANSI_SEQ_EVENT_ABORT)
                                भङ्ग;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        यदि (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = रिक्त;
                        }
                }

                विकल्प (e) {

                पक्ष ANSI_SEQ_EVENT_TEXT:
                        यदि (*i == '\r') {
                                n_carriage_returns++;
                                अनुवर्त;
                        }
                        यदि (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (माप_प्रकार) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                shift);
                                n_carriage_returns = 0;
                        }
                        प्रति (; n_carriage_returns > 0; n_carriage_returns--)
                                ख_अक्षर_दो('\r', f);

                        यदि (*i == '\t') {
                                ख_माला_दो("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } अन्यथा
                                ख_अक्षर_दो(*i, f);

                        भङ्ग;

                पक्ष ANSI_SEQ_EVENT_SEQUENCE:
                        यदि (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                प्रति (; n_carriage_returns > 0; n_carriage_returns--)
                                        ख_अक्षर_दो('\r', f);

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
        यदि (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        वर्ण *obuf;
        यदि (memstream_finalize(&m, &obuf, _isz) < 0)
                प्रत्यागम रिक्त;

        free_and_replace(*ibuf, obuf);

        यदि (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        प्रत्यागम *ibuf;
}

वर्ण* strextendv_with_separator(वर्ण **x, ध्रुव वर्ण *separator, बहु_सूची ap) {
        _cleanup_free_ वर्ण *buffer = रिक्त;
        माप_प्रकार f, l, l_separator;
        bool need_separator;
        वर्ण *nr, *p;

        यदि (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        बहु_सूची aq;
        va_copy(aq, ap);
        प्रति (ध्रुव वर्ण *t;;) {
                माप_प्रकार n;

                t = बहु_तर्क(aq, ध्रुव वर्ण *);
                यदि (!t)
                        भङ्ग;
                यदि (t == POINTER_MAX)
                        अनुवर्त;

                n = म_माप(t);

                यदि (need_separator)
                        n += l_separator;

                यदि (n >= SIZE_MAX - l) {
                        बहु_पूर्ण(aq);
                        प्रत्यागम रिक्त;
                }

                l += n;
                need_separator = true;
        }
        बहु_पूर्ण(aq);

        need_separator = !isempty(*x);

        nr = पुनः_स्मृति(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        यदि (!nr)
                प्रत्यागम रिक्त;

        *x = nr;
        p = nr + f;

        प्रति (;;) {
                ध्रुव वर्ण *t;

                t = बहु_तर्क(ap, ध्रुव वर्ण *);
                यदि (!t)
                        भङ्ग;
                यदि (t == POINTER_MAX)
                        अनुवर्त;

                यदि (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        प्रतिज्ञा(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        यदि (buffer)
                प्रत्यागम TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        प्रत्यागम p;
}

वर्ण* strextend_with_separator_internal(वर्ण **x, ध्रुव वर्ण *separator, ...) {
        बहु_सूची ap;
        वर्ण *ret;

        बहु_शुरू(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        बहु_पूर्ण(ap);

        प्रत्यागम ret;
}

पूर्णाङ्क strextendf_with_separator(वर्ण **x, ध्रुव वर्ण *separator, ध्रुव वर्ण *format, ...) {
        माप_प्रकार m, a, l_separator;
        बहु_सूची ap;
        पूर्णाङ्क l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        प्रतिज्ञा(x);
        प्रतिज्ञा(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        यदि (*x) {
                m = म_माप(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                प्रतिज्ञा(a >= m + 1);
        } अन्यथा
                m = a = 0;

        यदि (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                वर्ण *n;

                यदि (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        प्रत्यागम -ENOMEM;
                यदि (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        प्रत्यागम -ENOMEM;

                n = पुनः_स्मृति(*x, m + 64 + l_separator);
                यदि (!n)
                        प्रत्यागम -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        बहु_शुरू(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        बहु_पूर्ण(ap);

        प्रतिज्ञा(l >= 0);

        यदि ((माप_प्रकार) l < a - m - l_separator) {
                वर्ण *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = पुनः_स्मृति(*x, m + (माप_प्रकार) l + l_separator + 1);
                यदि (n)
                        *x = n;
        } अन्यथा {
                वर्ण *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                यदि (_unlikely_((माप_प्रकार) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        गच्छ oom;
                यदि (_unlikely_(m > SIZE_MAX - ((माप_प्रकार) l + l_separator + 1))) /* overflow check #2 */
                        गच्छ oom;

                a = m + (माप_प्रकार) l + l_separator + 1;
                n = पुनः_स्मृति(*x, a);
                यदि (!n)
                        गच्छ oom;
                *x = n;

                बहु_शुरू(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                बहु_पूर्ण(ap);

                प्रतिज्ञा((माप_प्रकार) l < a - m - l_separator);
        }

        प्रत्यागम 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        प्रत्यागम -ENOMEM;
}

वर्ण* strrep(ध्रुव वर्ण *s, माप_प्रकार n) {
        वर्ण *ret, *p;
        माप_प्रकार l;

        प्रतिज्ञा(s);

        l = म_माप(s);
        यदि (!MUL_ASSIGN_SAFE(&l, n))
                प्रत्यागम रिक्त;
        यदि (!INC_SAFE(&l, 1))
                प्रत्यागम रिक्त;

        p = ret = दो_स्मृति(l);
        यदि (!ret)
                प्रत्यागम रिक्त;

        प्रति (माप_प्रकार i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        प्रत्यागम ret;
}

पूर्णाङ्क split_pair(ध्रुव वर्ण *s, ध्रुव वर्ण *sep, वर्ण **ret_first, वर्ण **ret_second) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(!isempty(sep));

        ध्रुव वर्ण *x = म_माला(s, sep);
        यदि (!x)
                प्रत्यागम -EINVAL;

        _cleanup_free_ वर्ण *a = रिक्त;
        यदि (ret_first) {
                a = strndup(s, x - s);
                यदि (!a)
                        प्रत्यागम -ENOMEM;
        }

        _cleanup_free_ वर्ण *b = रिक्त;
        यदि (ret_second) {
                b = strdup(x + म_माप(sep));
                यदि (!b)
                        प्रत्यागम -ENOMEM;
        }

        यदि (ret_first)
                *ret_first = TAKE_PTR(a);
        यदि (ret_second)
                *ret_second = TAKE_PTR(b);
        प्रत्यागम 0;
}

पूर्णाङ्क free_and_strdup(वर्ण **p, ध्रुव वर्ण *s) {
        वर्ण *t;

        प्रतिज्ञा(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        यदि (streq_ptr(*p, s))
                प्रत्यागम 0;

        यदि (s) {
                t = strdup(s);
                यदि (!t)
                        प्रत्यागम -ENOMEM;
        } अन्यथा
                t = रिक्त;

        free_and_replace(*p, t);

        प्रत्यागम 1;
}

पूर्णाङ्क free_and_strdup_warn(वर्ण **p, ध्रुव वर्ण *s) {
        पूर्णाङ्क r;

        r = free_and_strdup(p, s);
        यदि (r < 0)
                प्रत्यागम log_oom();
        प्रत्यागम r;
}

पूर्णाङ्क free_and_strndup(वर्ण **p, ध्रुव वर्ण *s, माप_प्रकार l) {
        वर्ण *t;

        प्रतिज्ञा(p);
        प्रतिज्ञा(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        यदि (!*p && !s)
                प्रत्यागम 0;

        यदि (*p && s && strneq(*p, s, l) && (l > म_माप(*p) || (*p)[l] == '\0'))
                प्रत्यागम 0;

        यदि (s) {
                t = strndup(s, l);
                यदि (!t)
                        प्रत्यागम -ENOMEM;
        } अन्यथा
                t = रिक्त;

        free_and_replace(*p, t);
        प्रत्यागम 1;
}

पूर्णाङ्क strdup_to_full(वर्ण **ret, ध्रुव वर्ण *src) {
        यदि (!src) {
                यदि (ret)
                        *ret = रिक्त;

                प्रत्यागम 0;
        } अन्यथा {
                यदि (ret) {
                        वर्ण *t = strdup(src);
                        यदि (!t)
                                प्रत्यागम -ENOMEM;
                        *ret = t;
                }

                प्रत्यागम 1;
        }
};

bool string_is_safe(ध्रुव वर्ण *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        यदि (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                प्रत्यागम false;

        यदि (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                प्रत्यागम false;

        प्रति (ध्रुव वर्ण *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                यदि ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        प्रत्यागम false;

                यदि (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        प्रत्यागम false;

                यदि (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        प्रत्यागम false;

                यदि (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && म_अक्षर(QUOTES, *t))
                        प्रत्यागम false;

                यदि (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && म_अक्षर(GLOB_CHARS, *t))
                        प्रत्यागम false;

                यदि (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && म_अक्षर(WHITESPACE, *t))
                        प्रत्यागम false;

                यदि (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        प्रत्यागम false;
        }

        यदि (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                प्रत्यागम false;

        यदि (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                प्रत्यागम false;

        प्रत्यागम true;
}

वर्ण* str_realloc(वर्ण *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        यदि (!p)
                प्रत्यागम रिक्त;

        प्रत्यागम पुनः_स्मृति(p, म_माप(p) + 1) ?: p;
}

वर्ण* string_erase(वर्ण *x) {
        यदि (!x)
                प्रत्यागम रिक्त;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, म_माप(x));
        प्रत्यागम x;
}

पूर्णाङ्क string_truncate_पंक्तियाँ(ध्रुव वर्ण *s, माप_प्रकार n_पंक्तियाँ, वर्ण **ret) {
        ध्रुव वर्ण *p = s, *e = s;
        bool truncation_applied = false;
        वर्ण *copy;
        माप_प्रकार n = 0;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        प्रति (;;) {
                माप_प्रकार k;

                k = म_खोज(p, "\n");

                यदि (p[k] == 0) {
                        यदि (k == 0) /* final empty line */
                                भङ्ग;

                        यदि (n >= n_पंक्तियाँ) /* above threshold */
                                भङ्ग;

                        e = p + k; /* last line to include */
                        भङ्ग;
                }

                प्रतिज्ञा(p[k] == '\n');

                यदि (n >= n_पंक्तियाँ)
                        भङ्ग;

                यदि (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        यदि (isempty(e))
                copy = strdup(s);
        अन्यथा {
                यदि (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        यदि (!copy)
                प्रत्यागम -ENOMEM;

        *ret = copy;
        प्रत्यागम truncation_applied;
}

पूर्णाङ्क string_extract_पंक्ति(ध्रुव वर्ण *s, माप_प्रकार i, वर्ण **ret) {
        ध्रुव वर्ण *p = s;
        माप_प्रकार c = 0;

        प्रतिज्ञा(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        प्रति (;;) {
                ध्रुव वर्ण *q;

                q = म_अक्षर(p, '\n');
                यदि (i == c) {
                        /* The line we are looking for! */

                        यदि (q) {
                                वर्ण *m;

                                m = strndup(p, q - p);
                                यदि (!m)
                                        प्रत्यागम -ENOMEM;

                                *ret = m;
                                प्रत्यागम !isempty(q + 1); /* More coming? */
                        } अन्यथा
                                /* Tell the caller to use the input string if equal */
                                प्रत्यागम strdup_to(ret, p != s ? p : रिक्त);
                }

                यदि (!q)
                        /* No more lines, return empty line */
                        प्रत्यागम strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

पूर्णाङ्क string_contains_word_strv(ध्रुव वर्ण *string, ध्रुव वर्ण *separators, वर्ण * ध्रुव *शब्दगण, ध्रुव वर्ण **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ध्रुव ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        ध्रुव वर्ण *found = रिक्त;
        पूर्णाङ्क r;

        प्रति (;;) {
                _cleanup_free_ वर्ण *w = रिक्त;

                r = extract_first_word(&string, &w, separators, flags);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                found = strv_find(शब्दगण, w);
                यदि (found)
                        भङ्ग;
        }

        यदि (ret_word)
                *ret_word = found;
        प्रत्यागम !!found;
}

bool streq_skip_trailing_chars(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, ध्रुव वर्ण *ok) {
        यदि (!s1 && !s2)
                प्रत्यागम true;
        यदि (!s1 || !s2)
                प्रत्यागम false;

        यदि (!ok)
                ok = WHITESPACE;

        प्रति (; *s1 && *s2; s1++, s2++)
                यदि (*s1 != *s2)
                        भङ्ग;

        प्रत्यागम in_charset(s1, ok) && in_charset(s2, ok);
}

वर्ण* string_replace_char(वर्ण *str, वर्ण old_char, वर्ण new_char) {
        प्रतिज्ञा(str);
        प्रतिज्ञा(old_char != '\0');
        प्रतिज्ञा(new_char != '\0');
        प्रतिज्ञा(old_char != new_char);

        प्रति (वर्ण *p = म_अक्षर(str, old_char); p; p = म_अक्षर(p + 1, old_char))
                *p = new_char;

        प्रत्यागम str;
}

पूर्णाङ्क make_cstring(ध्रुव शून्य *s, माप_प्रकार n, MakeCStringMode mode, वर्ण **ret) {
        वर्ण *b;

        प्रतिज्ञा(s || n == 0);
        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        यदि (n == 0) {
                यदि (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        प्रत्यागम -EINVAL;

                यदि (!ret)
                        प्रत्यागम 0;

                b = new0(वर्ण, 1);
        } अन्यथा {
                ध्रुव uint8_t *nul;

                nul = स_प्रथम(s, 0, n);
                यदि (nul) {
                        यदि (nul < (ध्रुव uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                प्रत्यागम -EINVAL;

                        n--;
                } अन्यथा यदि (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        प्रत्यागम -EINVAL;

                यदि (!ret)
                        प्रत्यागम 0;

                b = memdup_suffix0(s, n);
        }
        यदि (!b)
                प्रत्यागम -ENOMEM;

        *ret = b;
        प्रत्यागम 0;
}

माप_प्रकार strspn_from_end(ध्रुव वर्ण *str, ध्रुव वर्ण *accept) {
        माप_प्रकार n = 0;

        यदि (isempty(str))
                प्रत्यागम 0;

        यदि (isempty(accept))
                प्रत्यागम 0;

        प्रति (ध्रुव वर्ण *p = str + म_माप(str); p > str && म_अक्षर(accept, p[-1]); p--)
                n++;

        प्रत्यागम n;
}

माप_प्रकार strnspn(ध्रुव वर्ण *str, ध्रुव वर्ण *accept, माप_प्रकार n) {
        माप_प्रकार i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        प्रतिज्ञा(str || n == 0);
        प्रतिज्ञा(accept);

        प्रति (i = 0; i < n && str[i] != '\0' && म_अक्षर(accept, str[i]); i++)
                ;

        प्रत्यागम i;
}

वर्ण* strdupspn(ध्रुव वर्ण *a, ध्रुव वर्ण *accept) {
        यदि (isempty(a) || isempty(accept))
                प्रत्यागम strdup("");

        प्रत्यागम strndup(a, म_अखोज(a, accept));
}

वर्ण* strdupcspn(ध्रुव वर्ण *a, ध्रुव वर्ण *reject) {
        यदि (isempty(a))
                प्रत्यागम strdup("");
        यदि (isempty(reject))
                प्रत्यागम strdup(a);

        प्रत्यागम strndup(a, म_खोज(a, reject));
}

वर्ण* find_पंक्ति_startswith_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        वर्ण *p = (वर्ण*) म_माला(haystack, needle);
        यदि (!p)
                प्रत्यागम रिक्त;

        यदि (p > haystack)
                यावत् (p[-1] != '\n') {
                        p = म_माला(p + 1, needle);
                        यदि (!p)
                                प्रत्यागम रिक्त;
                }

        प्रत्यागम p + म_माप(needle);
}

वर्ण* find_पंक्ति_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        वर्ण *p = (वर्ण*) find_पंक्ति_startswith(haystack, needle);
        यदि (!p)
                प्रत्यागम रिक्त;

        यदि (*p == 0 || म_अक्षर(NEWLINE, *p))
                प्रत्यागम p - म_माप(needle);

        प्रत्यागम रिक्त;
}

वर्ण* find_पंक्ति_after_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        वर्ण *p = (वर्ण*) find_पंक्ति_startswith(haystack, needle);
        यदि (!p)
                प्रत्यागम रिक्त;

        यदि (*p == 0)
                प्रत्यागम p;
        यदि (म_अक्षर(NEWLINE, *p))
                प्रत्यागम p + 1;

        प्रत्यागम रिक्त;
}

bool version_is_valid(ध्रुव वर्ण *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        यदि (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                प्रत्यागम false;

        यदि (!filename_part_is_valid(s))
                प्रत्यागम false;

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
        माप_प्रकार l = म_माप(charset);

        यदि (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        यदि (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        प्रत्यागम in_charset(s, charset);
}

ssize_t strlevenshtein(ध्रुव वर्ण *x, ध्रुव वर्ण *y) {
        _cleanup_free_ माप_प्रकार *t0 = रिक्त, *t1 = रिक्त, *t2 = रिक्त;
        माप_प्रकार xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        यदि (streq_ptr(x, y))
                प्रत्यागम 0;

        xl = strlen_ptr(x);
        यदि (xl > SSIZE_MAX)
                प्रत्यागम -E2BIG;

        yl = strlen_ptr(y);
        यदि (yl > SSIZE_MAX)
                प्रत्यागम -E2BIG;

        यदि (isempty(x))
                प्रत्यागम yl;
        यदि (isempty(y))
                प्रत्यागम xl;

        t0 = new0(माप_प्रकार, yl + 1);
        यदि (!t0)
                प्रत्यागम -ENOMEM;
        t1 = new0(माप_प्रकार, yl + 1);
        यदि (!t1)
                प्रत्यागम -ENOMEM;
        t2 = new0(माप_प्रकार, yl + 1);
        यदि (!t2)
                प्रत्यागम -ENOMEM;

        प्रति (माप_प्रकार i = 0; i <= yl; i++)
                t1[i] = i;

        प्रति (माप_प्रकार i = 0; i < xl; i++) {
                t2[0] = i + 1;

                प्रति (माप_प्रकार j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        यदि (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        यदि (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        यदि (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                माप_प्रकार *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        प्रत्यागम t1[yl];
}

वर्ण* strrstr_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        यदि (!haystack || !needle)
                प्रत्यागम रिक्त;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        यदि (needle[0] == 0)
                प्रत्यागम (वर्ण*) म_अक्षर(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        यदि (needle[1] == 0)
                प्रत्यागम (वर्ण*) म_खोजप(haystack, needle[0]);

        प्रति (ध्रुव वर्ण *p = म_माला(haystack, needle), *q; p; p = q) {
                q = म_माला(p + 1, needle);
                यदि (!q)
                        प्रत्यागम (वर्ण*) p;
        }
        प्रत्यागम रिक्त;
}

वर्ण* strrstr_no_case_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        यदि (!haystack || !needle)
                प्रत्यागम रिक्त;

        प्रति (ध्रुव वर्ण *p = म_अक्षर(haystack, 0); p > haystack; p--)
                यदि (startswith_no_case(p, needle))
                        प्रत्यागम (वर्ण*) p;

        प्रत्यागम startswith_no_case(haystack, needle) ? (वर्ण*) haystack : रिक्त;
}

माप_प्रकार str_common_prefix(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        प्रति (माप_प्रकार n = 0;; n++) {
                वर्ण c = a[n];
                यदि (c != b[n])
                        प्रत्यागम n;
                यदि (c == 0)
                        प्रत्यागम SIZE_MAX;
        }
}
