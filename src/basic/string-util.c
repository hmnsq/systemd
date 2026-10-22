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

अक्षर* first_word(स्थिरांक अक्षर *s, स्थिरांक अक्षर *word) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        जर (isempty(word))
                परत (अक्षर*) s;

        स्थिरांक अक्षर *p = startswith(s, word);
        जर (!p)
                परत रिक्त;
        जर (*p == '\0')
                परत (अक्षर*) p;

        स्थिरांक अक्षर *nw = skip_leading_chars(p, WHITESPACE);
        जर (p == nw)
                परत रिक्त;

        परत (अक्षर*) nw;
}

अक्षर* strextendn(अक्षर **x, स्थिरांक अक्षर *s, माप_प्रकार l) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(s || l == 0);

        जर (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        जर (l > 0 || !*x) {
                माप_प्रकार q;
                अक्षर *m;

                q = strlen_ptr(*x);
                m = पुनः_स्मृति(*x, q + l + 1);
                जर (!m)
                        परत रिक्त;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        परत *x;
}

अक्षर* strstrip(अक्षर *s) {
        जर (!s)
                परत रिक्त;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        परत delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

अक्षर* delete_chars(अक्षर *s, स्थिरांक अक्षर *bad) {
        अक्षर *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        जर (!s)
                परत रिक्त;

        जर (!bad)
                bad = WHITESPACE;

        साठी (f = s, t = s; *f; f++) {
                जर (म_अक्षर(bad, *f))
                        सुरूठेवा;

                *(t++) = *f;
        }

        *t = 0;

        परत s;
}

अक्षर* delete_trailing_chars(अक्षर *s, स्थिरांक अक्षर *bad) {
        अक्षर *c = s;

        /* Drops all specified bad characters, at the end of the string */

        जर (!s)
                परत रिक्त;

        जर (!bad)
                bad = WHITESPACE;

        साठी (अक्षर *p = s; *p; p++)
                जर (!म_अक्षर(bad, *p))
                        c = p + 1;

        *c = 0;

        परत s;
}

अक्षर* truncate_nl_full(अक्षर *s, माप_प्रकार *ret_len) {
        माप_प्रकार n;

        प्रतिज्ञा(s);

        n = म_खोज(s, NEWLINE);
        s[n] = '\0';
        जर (ret_len)
                *ret_len = n;
        परत s;
}

अक्षर ascii_tolower(अक्षर x) {

        जर (x >= 'A' && x <= 'Z')
                परत x - 'A' + 'a';

        परत x;
}

अक्षर ascii_toupper(अक्षर x) {

        जर (x >= 'a' && x <= 'z')
                परत x - 'a' + 'A';

        परत x;
}

अक्षर* ascii_strlower(अक्षर *s) {
        प्रतिज्ञा(s);

        साठी (अक्षर *p = s; *p; p++)
                *p = ascii_tolower(*p);

        परत s;
}

अक्षर* ascii_strupper(अक्षर *s) {
        प्रतिज्ञा(s);

        साठी (अक्षर *p = s; *p; p++)
                *p = ascii_toupper(*p);

        परत s;
}

अक्षर* ascii_strlower_n(अक्षर *s, माप_प्रकार n) {
        प्रतिज्ञा(n <= 0 || s);

        जर (n <= 0)
                परत s;

        साठी (माप_प्रकार i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        परत s;
}

पूर्णांक ascii_strcasecmp_n(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b, माप_प्रकार n) {

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        साठी (; n > 0; a++, b++, n--) {
                पूर्णांक x, y;

                x = (पूर्णांक) (uint8_t) ascii_tolower(*a);
                y = (पूर्णांक) (uint8_t) ascii_tolower(*b);

                जर (x != y)
                        परत x - y;
        }

        परत 0;
}

पूर्णांक ascii_strcasecmp_nn(स्थिरांक अक्षर *a, माप_प्रकार n, स्थिरांक अक्षर *b, माप_प्रकार m) {
        पूर्णांक r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        जर (r != 0)
                परत r;

        परत CMP(n, m);
}

bool chars_intersect(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        /* Returns true if any of the chars in a are in b. */
        साठी (स्थिरांक अक्षर *p = a; *p; p++)
                जर (म_अक्षर(b, *p))
                        परत true;

        परत false;
}

bool string_has_cc(स्थिरांक अक्षर *p, स्थिरांक अक्षर *ok) {
        प्रतिज्ञा(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        साठी (स्थिरांक अक्षर *t = p; *t; t++) {
                जर (ok && म_अक्षर(ok, *t))
                        सुरूठेवा;

                जर (char_is_cc(*t))
                        परत true;
        }

        परत false;
}

स्थिर पूर्णांक write_ellipsis(अक्षर *buf, bool unicode) {
        स्थिरांक अक्षर *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        प्रतिज्ञा(म_माप(s) == 3);
        स_नकल(buf, s, 3);
        परत 3;
}

स्थिर माप_प्रकार ansi_sequence_length(स्थिरांक अक्षर *s, माप_प्रकार len) {
        प्रतिज्ञा(s);

        जर (len < 2)
                परत 0;

        जर (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                परत 0;  /* Not the start of a sequence */

        जर (s[1] == 0x5B) { /* [, start of CSI sequence */
                माप_प्रकार i = 2;

                जर (i == len)
                        परत 0;

                पर्यंत (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        जर (++i == len)
                                परत 0;
                पर्यंत (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        जर (++i == len)
                                परत 0;
                जर (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        परत i + 1;
                परत 0;  /* Bad sequence */

        } अन्यथा जर (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                परत 2;

        परत 0;  /* Bad escape? */
}

स्थिर bool string_has_ansi_sequence(स्थिरांक अक्षर *s, माप_प्रकार len) {
        स्थिरांक अक्षर *t = s;

        पर्यंत ((t = स_प्रथम(t, 0x1B, len - (t - s)))) {
                जर (ansi_sequence_length(t, len - (t - s)) > 0)
                        परत true;
                t++;
        }
        परत false;
}

स्थिर माप_प्रकार previous_ansi_sequence(स्थिरांक अक्षर *s, माप_प्रकार length, स्थिरांक अक्षर **ret_where) {

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        जर (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = रिक्त;
                परत 0;
        }

        साठी (माप_प्रकार i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                माप_प्रकार slen = ansi_sequence_length(s + i, length - i);
                जर (slen > 0) {
                        *ret_where = s + i;
                        परत slen;
                }

                जर (i == 0)
                        थांबा;
        }

        *ret_where = रिक्त;
        परत 0;
}

स्थिर अक्षर *ascii_ellipsize_mem(स्थिरांक अक्षर *s, माप_प्रकार old_length, माप_प्रकार new_length, अचिन्हित percent) {
        माप_प्रकार x, need_space, suffix_len;
        अक्षर *t;

        प्रतिज्ञा(s);
        प्रतिज्ञा(percent <= 100);
        प्रतिज्ञा(new_length != SIZE_MAX);

        जर (old_length <= new_length)
                परत strndup(s, old_length);

        /* Special case short ellipsations */
        निवड (new_length) {

        स्थिती 0:
                परत strdup("");

        स्थिती 1:
                जर (is_locale_utf8())
                        परत strdup("…");
                अन्यथा
                        परत strdup(".");

        स्थिती 2:
                जर (!is_locale_utf8())
                        परत strdup("..");
                थांबा;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(अक्षर, new_length+3);
        जर (!t)
                परत रिक्त;

        प्रतिज्ञा(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        प्रतिज्ञा(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        स_नकल(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        परत t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
स्थिर स्थिरांक अक्षर* find_previous_unichar(स्थिरांक अक्षर *start, स्थिरांक अक्षर *end, char32_t *ret_c) {
        स्थिरांक अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(start);
        प्रतिज्ञा(end);
        प्रतिज्ञा(end > start);

        साठी (p = end; p > start; ) {
                p--;
                जर (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        थांबा;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        जर (r < 0 || p + r != end)
                परत रिक्त;

        जर (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        परत p;
}

अक्षर* ellipsize_mem(स्थिरांक अक्षर *s, माप_प्रकार old_length, माप_प्रकार new_length, अचिन्हित percent) {
        माप_प्रकार x, k, len, len2;
        स्थिरांक अक्षर *i, *j;
        पूर्णांक r;

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

        जर (new_length == SIZE_MAX)
                परत strndup(s, old_length);

        जर (new_length == 0)
                परत strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        जर (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !स_प्रथम(s, '\t', old_length))
                परत ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        प्रतिज्ञा(x <= new_length - 1);

        k = 0;
        साठी (i = s; i < s + old_length; ) {
                माप_प्रकार slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                जर (slen > 0) {
                        i += slen;
                        सुरूठेवा;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                जर (r < 0)
                        परत रिक्त;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                पूर्णांक w = unichar_console_width(c);
                जर (k + w > x)
                        थांबा;

                k += w;
                i += r;
        }

        स्थिरांक अक्षर *ansi_start = s + old_length;
        माप_प्रकार ansi_len = 0;

        साठी (स्थिरांक अक्षर *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                पूर्णांक w;
                स्थिरांक अक्षर *tt;

                जर (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                जर (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        सुरूठेवा;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                जर (!tt)
                        परत रिक्त;

                w = unichar_console_width(c);
                जर (k + w > new_length)
                        थांबा;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        जर (i >= j)
                परत memdup_suffix0(s, old_length);

        जर (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                जर (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        जर (r < 0)
                                परत रिक्त;

                        j += r;
                } अन्यथा जर (i > s) {
                        स्थिरांक अक्षर *tt = find_previous_unichar(s, i, रिक्त);
                        जर (!tt)
                                परत रिक्त;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        माप_प्रकार alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        अक्षर *e = new(अक्षर, alloc_len);
        जर (!e)
                परत रिक्त;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        अक्षर *dst = e + len + 3;

        जर (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                साठी (स्थिरांक अक्षर *p = s + len; p < j; ) {
                        माप_प्रकार slen = ansi_sequence_length(p, j - p);
                        जर (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } अन्यथा
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        परत e;
}

अक्षर* cellescape(अक्षर *buf, माप_प्रकार len, स्थिरांक अक्षर *s) {
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

        साठी (;;) {
                अक्षर four[4];
                पूर्णांक w;

                जर (*s == 0) /* terminating NUL detected? then we are done! */
                        जा done;

                w = cescape_char(*s, four);
                जर (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        थांबा;

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
        साठी (माप_प्रकार j = 0; j < ELEMENTSOF(last_char_width); j++) {

                जर (i + 4 <= len) /* nice, we reached our space goal */
                        थांबा;

                k = k == 0 ? 3 : k - 1;
                जर (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        थांबा;

                प्रतिज्ञा(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        जर (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        अन्यथा जर (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } अन्यथा जर (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        अन्यथा
                प्रतिज्ञा(i + 1 <= len);

done:
        buf[i] = '\0';
        परत buf;
}

अक्षर* strshorten(अक्षर *s, माप_प्रकार l) {
        प्रतिज्ञा(s);

        जर (l >= SIZE_MAX-1) /* Would not change anything */
                परत s;

        जर (strnlen(s, l+1) > l)
                s[l] = 0;

        परत s;
}

पूर्णांक strgrowpad0(अक्षर **s, माप_प्रकार l) {
        माप_प्रकार sz;

        प्रतिज्ञा(s);

        जर (*s) {
                sz = म_माप(*s) + 1;
                जर (sz >= l) /* never shrink */
                        परत 0;
        } अन्यथा
                sz = 0;

        अक्षर *q = पुनः_स्मृति(*s, l);
        जर (!q)
                परत -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        परत 0;
}

अक्षर* strreplace(स्थिरांक अक्षर *text, स्थिरांक अक्षर *old_string, स्थिरांक अक्षर *new_string) {
        माप_प्रकार l, old_len, new_len;
        अक्षर *t, *ret = रिक्त;
        स्थिरांक अक्षर *f;

        प्रतिज्ञा(old_string);
        प्रतिज्ञा(new_string);

        जर (!text)
                परत रिक्त;

        old_len = म_माप(old_string);
        new_len = म_माप(new_string);

        l = म_माप(text);
        जर (!GREEDY_REALLOC(ret, l+1))
                परत रिक्त;

        f = text;
        t = ret;
        पर्यंत (*f) {
                माप_प्रकार d, nl;

                जर (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        सुरूठेवा;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                जर (!GREEDY_REALLOC(ret, nl + 1))
                        परत mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        परत ret;
}

स्थिर शून्य advance_offsets(
                ssize_t diff,
                माप_प्रकार offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                माप_प्रकार shift[स्थिर 2],
                माप_प्रकार size) {

        जर (!offsets)
                परत;

        प्रतिज्ञा(shift);

        जर ((माप_प्रकार) diff < offsets[0])
                shift[0] += size;
        जर ((माप_प्रकार) diff < offsets[1])
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

        जर (!offsets)
                परत;

        प्रतिज्ञा(shift);
        प्रतिज्ञा(begin <= end);

        साठी (माप_प्रकार k = 0; k < 2; k++)
                जर (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

अक्षर* strip_tab_ansi(अक्षर **ibuf, माप_प्रकार *_isz, माप_प्रकार highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        स्थिरांक अक्षर *begin = रिक्त;
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
        जर (!f)
                परत रिक्त;

        साठी (स्थिरांक अक्षर *i = *ibuf; i < *ibuf + isz; i++) {
                पूर्णांक e;

                साठी (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        जर (e < 0)
                                परत रिक्त;
                        जर (e != ANSI_SEQ_EVENT_ABORT)
                                थांबा;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        जर (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = रिक्त;
                        }
                }

                निवड (e) {

                स्थिती ANSI_SEQ_EVENT_TEXT:
                        जर (*i == '\r') {
                                n_carriage_returns++;
                                सुरूठेवा;
                        }
                        जर (*i == '\n') {
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
                        साठी (; n_carriage_returns > 0; n_carriage_returns--)
                                ख_अक्षर_दो('\r', f);

                        जर (*i == '\t') {
                                ख_माला_दो("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } अन्यथा
                                ख_अक्षर_दो(*i, f);

                        थांबा;

                स्थिती ANSI_SEQ_EVENT_SEQUENCE:
                        जर (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                साठी (; n_carriage_returns > 0; n_carriage_returns--)
                                        ख_अक्षर_दो('\r', f);

                                begin = i;
                        }
                        थांबा;

                स्थिती ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        प्रतिज्ञा(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = रिक्त;
                        थांबा;

                पूर्वनिर्धारित:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        जर (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        अक्षर *obuf;
        जर (memstream_finalize(&m, &obuf, _isz) < 0)
                परत रिक्त;

        free_and_replace(*ibuf, obuf);

        जर (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        परत *ibuf;
}

अक्षर* strextendv_with_separator(अक्षर **x, स्थिरांक अक्षर *separator, बहु_सूची ap) {
        _cleanup_free_ अक्षर *buffer = रिक्त;
        माप_प्रकार f, l, l_separator;
        bool need_separator;
        अक्षर *nr, *p;

        जर (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        बहु_सूची aq;
        va_copy(aq, ap);
        साठी (स्थिरांक अक्षर *t;;) {
                माप_प्रकार n;

                t = बहु_तर्क(aq, स्थिरांक अक्षर *);
                जर (!t)
                        थांबा;
                जर (t == POINTER_MAX)
                        सुरूठेवा;

                n = म_माप(t);

                जर (need_separator)
                        n += l_separator;

                जर (n >= SIZE_MAX - l) {
                        बहु_पूर्ण(aq);
                        परत रिक्त;
                }

                l += n;
                need_separator = true;
        }
        बहु_पूर्ण(aq);

        need_separator = !isempty(*x);

        nr = पुनः_स्मृति(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        जर (!nr)
                परत रिक्त;

        *x = nr;
        p = nr + f;

        साठी (;;) {
                स्थिरांक अक्षर *t;

                t = बहु_तर्क(ap, स्थिरांक अक्षर *);
                जर (!t)
                        थांबा;
                जर (t == POINTER_MAX)
                        सुरूठेवा;

                जर (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        प्रतिज्ञा(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        जर (buffer)
                परत TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        परत p;
}

अक्षर* strextend_with_separator_internal(अक्षर **x, स्थिरांक अक्षर *separator, ...) {
        बहु_सूची ap;
        अक्षर *ret;

        बहु_शुरू(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        बहु_पूर्ण(ap);

        परत ret;
}

पूर्णांक strextendf_with_separator(अक्षर **x, स्थिरांक अक्षर *separator, स्थिरांक अक्षर *format, ...) {
        माप_प्रकार m, a, l_separator;
        बहु_सूची ap;
        पूर्णांक l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        प्रतिज्ञा(x);
        प्रतिज्ञा(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        जर (*x) {
                m = म_माप(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                प्रतिज्ञा(a >= m + 1);
        } अन्यथा
                m = a = 0;

        जर (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                अक्षर *n;

                जर (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        परत -ENOMEM;
                जर (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        परत -ENOMEM;

                n = पुनः_स्मृति(*x, m + 64 + l_separator);
                जर (!n)
                        परत -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        बहु_शुरू(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        बहु_पूर्ण(ap);

        प्रतिज्ञा(l >= 0);

        जर ((माप_प्रकार) l < a - m - l_separator) {
                अक्षर *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = पुनः_स्मृति(*x, m + (माप_प्रकार) l + l_separator + 1);
                जर (n)
                        *x = n;
        } अन्यथा {
                अक्षर *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                जर (_unlikely_((माप_प्रकार) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        जा oom;
                जर (_unlikely_(m > SIZE_MAX - ((माप_प्रकार) l + l_separator + 1))) /* overflow check #2 */
                        जा oom;

                a = m + (माप_प्रकार) l + l_separator + 1;
                n = पुनः_स्मृति(*x, a);
                जर (!n)
                        जा oom;
                *x = n;

                बहु_शुरू(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                बहु_पूर्ण(ap);

                प्रतिज्ञा((माप_प्रकार) l < a - m - l_separator);
        }

        परत 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        परत -ENOMEM;
}

अक्षर* strrep(स्थिरांक अक्षर *s, माप_प्रकार n) {
        अक्षर *ret, *p;
        माप_प्रकार l;

        प्रतिज्ञा(s);

        l = म_माप(s);
        जर (!MUL_ASSIGN_SAFE(&l, n))
                परत रिक्त;
        जर (!INC_SAFE(&l, 1))
                परत रिक्त;

        p = ret = दो_स्मृति(l);
        जर (!ret)
                परत रिक्त;

        साठी (माप_प्रकार i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        परत ret;
}

पूर्णांक split_pair(स्थिरांक अक्षर *s, स्थिरांक अक्षर *sep, अक्षर **ret_first, अक्षर **ret_second) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(!isempty(sep));

        स्थिरांक अक्षर *x = म_माला(s, sep);
        जर (!x)
                परत -EINVAL;

        _cleanup_free_ अक्षर *a = रिक्त;
        जर (ret_first) {
                a = strndup(s, x - s);
                जर (!a)
                        परत -ENOMEM;
        }

        _cleanup_free_ अक्षर *b = रिक्त;
        जर (ret_second) {
                b = strdup(x + म_माप(sep));
                जर (!b)
                        परत -ENOMEM;
        }

        जर (ret_first)
                *ret_first = TAKE_PTR(a);
        जर (ret_second)
                *ret_second = TAKE_PTR(b);
        परत 0;
}

पूर्णांक free_and_strdup(अक्षर **p, स्थिरांक अक्षर *s) {
        अक्षर *t;

        प्रतिज्ञा(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        जर (streq_ptr(*p, s))
                परत 0;

        जर (s) {
                t = strdup(s);
                जर (!t)
                        परत -ENOMEM;
        } अन्यथा
                t = रिक्त;

        free_and_replace(*p, t);

        परत 1;
}

पूर्णांक free_and_strdup_warn(अक्षर **p, स्थिरांक अक्षर *s) {
        पूर्णांक r;

        r = free_and_strdup(p, s);
        जर (r < 0)
                परत log_oom();
        परत r;
}

पूर्णांक free_and_strndup(अक्षर **p, स्थिरांक अक्षर *s, माप_प्रकार l) {
        अक्षर *t;

        प्रतिज्ञा(p);
        प्रतिज्ञा(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        जर (!*p && !s)
                परत 0;

        जर (*p && s && strneq(*p, s, l) && (l > म_माप(*p) || (*p)[l] == '\0'))
                परत 0;

        जर (s) {
                t = strndup(s, l);
                जर (!t)
                        परत -ENOMEM;
        } अन्यथा
                t = रिक्त;

        free_and_replace(*p, t);
        परत 1;
}

पूर्णांक strdup_to_full(अक्षर **ret, स्थिरांक अक्षर *src) {
        जर (!src) {
                जर (ret)
                        *ret = रिक्त;

                परत 0;
        } अन्यथा {
                जर (ret) {
                        अक्षर *t = strdup(src);
                        जर (!t)
                                परत -ENOMEM;
                        *ret = t;
                }

                परत 1;
        }
};

bool string_is_safe(स्थिरांक अक्षर *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        जर (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                परत false;

        जर (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                परत false;

        साठी (स्थिरांक अक्षर *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                जर ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        परत false;

                जर (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        परत false;

                जर (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        परत false;

                जर (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && म_अक्षर(QUOTES, *t))
                        परत false;

                जर (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && म_अक्षर(GLOB_CHARS, *t))
                        परत false;

                जर (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && म_अक्षर(WHITESPACE, *t))
                        परत false;

                जर (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        परत false;
        }

        जर (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                परत false;

        जर (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                परत false;

        परत true;
}

अक्षर* str_realloc(अक्षर *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        जर (!p)
                परत रिक्त;

        परत पुनः_स्मृति(p, म_माप(p) + 1) ?: p;
}

अक्षर* string_erase(अक्षर *x) {
        जर (!x)
                परत रिक्त;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, म_माप(x));
        परत x;
}

पूर्णांक string_truncate_ओळी(स्थिरांक अक्षर *s, माप_प्रकार n_ओळी, अक्षर **ret) {
        स्थिरांक अक्षर *p = s, *e = s;
        bool truncation_applied = false;
        अक्षर *copy;
        माप_प्रकार n = 0;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        साठी (;;) {
                माप_प्रकार k;

                k = म_खोज(p, "\n");

                जर (p[k] == 0) {
                        जर (k == 0) /* final empty line */
                                थांबा;

                        जर (n >= n_ओळी) /* above threshold */
                                थांबा;

                        e = p + k; /* last line to include */
                        थांबा;
                }

                प्रतिज्ञा(p[k] == '\n');

                जर (n >= n_ओळी)
                        थांबा;

                जर (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        जर (isempty(e))
                copy = strdup(s);
        अन्यथा {
                जर (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        जर (!copy)
                परत -ENOMEM;

        *ret = copy;
        परत truncation_applied;
}

पूर्णांक string_extract_ओळ(स्थिरांक अक्षर *s, माप_प्रकार i, अक्षर **ret) {
        स्थिरांक अक्षर *p = s;
        माप_प्रकार c = 0;

        प्रतिज्ञा(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        साठी (;;) {
                स्थिरांक अक्षर *q;

                q = म_अक्षर(p, '\n');
                जर (i == c) {
                        /* The line we are looking for! */

                        जर (q) {
                                अक्षर *m;

                                m = strndup(p, q - p);
                                जर (!m)
                                        परत -ENOMEM;

                                *ret = m;
                                परत !isempty(q + 1); /* More coming? */
                        } अन्यथा
                                /* Tell the caller to use the input string if equal */
                                परत strdup_to(ret, p != s ? p : रिक्त);
                }

                जर (!q)
                        /* No more lines, return empty line */
                        परत strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

पूर्णांक string_contains_word_strv(स्थिरांक अक्षर *string, स्थिरांक अक्षर *separators, अक्षर * स्थिरांक *शब्द, स्थिरांक अक्षर **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        स्थिरांक ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        स्थिरांक अक्षर *found = रिक्त;
        पूर्णांक r;

        साठी (;;) {
                _cleanup_free_ अक्षर *w = रिक्त;

                r = extract_first_word(&string, &w, separators, flags);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                found = strv_find(शब्द, w);
                जर (found)
                        थांबा;
        }

        जर (ret_word)
                *ret_word = found;
        परत !!found;
}

bool streq_skip_trailing_chars(स्थिरांक अक्षर *s1, स्थिरांक अक्षर *s2, स्थिरांक अक्षर *ok) {
        जर (!s1 && !s2)
                परत true;
        जर (!s1 || !s2)
                परत false;

        जर (!ok)
                ok = WHITESPACE;

        साठी (; *s1 && *s2; s1++, s2++)
                जर (*s1 != *s2)
                        थांबा;

        परत in_charset(s1, ok) && in_charset(s2, ok);
}

अक्षर* string_replace_char(अक्षर *str, अक्षर old_char, अक्षर new_char) {
        प्रतिज्ञा(str);
        प्रतिज्ञा(old_char != '\0');
        प्रतिज्ञा(new_char != '\0');
        प्रतिज्ञा(old_char != new_char);

        साठी (अक्षर *p = म_अक्षर(str, old_char); p; p = म_अक्षर(p + 1, old_char))
                *p = new_char;

        परत str;
}

पूर्णांक make_cstring(स्थिरांक शून्य *s, माप_प्रकार n, MakeCStringMode mode, अक्षर **ret) {
        अक्षर *b;

        प्रतिज्ञा(s || n == 0);
        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        जर (n == 0) {
                जर (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        परत -EINVAL;

                जर (!ret)
                        परत 0;

                b = new0(अक्षर, 1);
        } अन्यथा {
                स्थिरांक uint8_t *nul;

                nul = स_प्रथम(s, 0, n);
                जर (nul) {
                        जर (nul < (स्थिरांक uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                परत -EINVAL;

                        n--;
                } अन्यथा जर (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        परत -EINVAL;

                जर (!ret)
                        परत 0;

                b = memdup_suffix0(s, n);
        }
        जर (!b)
                परत -ENOMEM;

        *ret = b;
        परत 0;
}

माप_प्रकार strspn_from_end(स्थिरांक अक्षर *str, स्थिरांक अक्षर *accept) {
        माप_प्रकार n = 0;

        जर (isempty(str))
                परत 0;

        जर (isempty(accept))
                परत 0;

        साठी (स्थिरांक अक्षर *p = str + म_माप(str); p > str && म_अक्षर(accept, p[-1]); p--)
                n++;

        परत n;
}

माप_प्रकार strnspn(स्थिरांक अक्षर *str, स्थिरांक अक्षर *accept, माप_प्रकार n) {
        माप_प्रकार i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        प्रतिज्ञा(str || n == 0);
        प्रतिज्ञा(accept);

        साठी (i = 0; i < n && str[i] != '\0' && म_अक्षर(accept, str[i]); i++)
                ;

        परत i;
}

अक्षर* strdupspn(स्थिरांक अक्षर *a, स्थिरांक अक्षर *accept) {
        जर (isempty(a) || isempty(accept))
                परत strdup("");

        परत strndup(a, म_अखोज(a, accept));
}

अक्षर* strdupcspn(स्थिरांक अक्षर *a, स्थिरांक अक्षर *reject) {
        जर (isempty(a))
                परत strdup("");
        जर (isempty(reject))
                परत strdup(a);

        परत strndup(a, म_खोज(a, reject));
}

अक्षर* find_ओळ_startswith_internal(स्थिरांक अक्षर *haystack, स्थिरांक अक्षर *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        अक्षर *p = (अक्षर*) म_माला(haystack, needle);
        जर (!p)
                परत रिक्त;

        जर (p > haystack)
                पर्यंत (p[-1] != '\n') {
                        p = म_माला(p + 1, needle);
                        जर (!p)
                                परत रिक्त;
                }

        परत p + म_माप(needle);
}

अक्षर* find_ओळ_internal(स्थिरांक अक्षर *haystack, स्थिरांक अक्षर *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        अक्षर *p = (अक्षर*) find_ओळ_startswith(haystack, needle);
        जर (!p)
                परत रिक्त;

        जर (*p == 0 || म_अक्षर(NEWLINE, *p))
                परत p - म_माप(needle);

        परत रिक्त;
}

अक्षर* find_ओळ_after_internal(स्थिरांक अक्षर *haystack, स्थिरांक अक्षर *needle) {
        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        अक्षर *p = (अक्षर*) find_ओळ_startswith(haystack, needle);
        जर (!p)
                परत रिक्त;

        जर (*p == 0)
                परत p;
        जर (म_अक्षर(NEWLINE, *p))
                परत p + 1;

        परत रिक्त;
}

bool version_is_valid(स्थिरांक अक्षर *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        जर (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                परत false;

        जर (!filename_part_is_valid(s))
                परत false;

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

        अक्षर charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        माप_प्रकार l = म_माप(charset);

        जर (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        जर (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        परत in_charset(s, charset);
}

ssize_t strlevenshtein(स्थिरांक अक्षर *x, स्थिरांक अक्षर *y) {
        _cleanup_free_ माप_प्रकार *t0 = रिक्त, *t1 = रिक्त, *t2 = रिक्त;
        माप_प्रकार xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        जर (streq_ptr(x, y))
                परत 0;

        xl = strlen_ptr(x);
        जर (xl > SSIZE_MAX)
                परत -E2BIG;

        yl = strlen_ptr(y);
        जर (yl > SSIZE_MAX)
                परत -E2BIG;

        जर (isempty(x))
                परत yl;
        जर (isempty(y))
                परत xl;

        t0 = new0(माप_प्रकार, yl + 1);
        जर (!t0)
                परत -ENOMEM;
        t1 = new0(माप_प्रकार, yl + 1);
        जर (!t1)
                परत -ENOMEM;
        t2 = new0(माप_प्रकार, yl + 1);
        जर (!t2)
                परत -ENOMEM;

        साठी (माप_प्रकार i = 0; i <= yl; i++)
                t1[i] = i;

        साठी (माप_प्रकार i = 0; i < xl; i++) {
                t2[0] = i + 1;

                साठी (माप_प्रकार j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        जर (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        जर (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        जर (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                माप_प्रकार *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        परत t1[yl];
}

अक्षर* strrstr_internal(स्थिरांक अक्षर *haystack, स्थिरांक अक्षर *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        जर (!haystack || !needle)
                परत रिक्त;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        जर (needle[0] == 0)
                परत (अक्षर*) म_अक्षर(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        जर (needle[1] == 0)
                परत (अक्षर*) म_खोजप(haystack, needle[0]);

        साठी (स्थिरांक अक्षर *p = म_माला(haystack, needle), *q; p; p = q) {
                q = म_माला(p + 1, needle);
                जर (!q)
                        परत (अक्षर*) p;
        }
        परत रिक्त;
}

अक्षर* strrstr_no_case_internal(स्थिरांक अक्षर *haystack, स्थिरांक अक्षर *needle) {
        जर (!haystack || !needle)
                परत रिक्त;

        साठी (स्थिरांक अक्षर *p = म_अक्षर(haystack, 0); p > haystack; p--)
                जर (startswith_no_case(p, needle))
                        परत (अक्षर*) p;

        परत startswith_no_case(haystack, needle) ? (अक्षर*) haystack : रिक्त;
}

माप_प्रकार str_common_prefix(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        साठी (माप_प्रकार n = 0;; n++) {
                अक्षर c = a[n];
                जर (c != b[n])
                        परत n;
                जर (c == 0)
                        परत SIZE_MAX;
        }
}
