/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "ansi-seq.h"
#समावेश "escape.h"
#समावेश "extract-word.h"
#समावेश "glyph-util.h"
#समावेश "gunicode.h"
#समावेश "locale-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "memstream-util.h"
#समावेश "path-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "utf8.h"

अक्षर* first_word(स्थिर अक्षर *s, स्थिर अक्षर *word) {
        निश्चित(s);
        निश्चित(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        अगर (isempty(word))
                वापस (अक्षर*) s;

        स्थिर अक्षर *p = startswith(s, word);
        अगर (!p)
                वापस शून्य;
        अगर (*p == '\0')
                वापस (अक्षर*) p;

        स्थिर अक्षर *nw = skip_leading_chars(p, WHITESPACE);
        अगर (p == nw)
                वापस शून्य;

        वापस (अक्षर*) nw;
}

अक्षर* strextendn(अक्षर **x, स्थिर अक्षर *s, माप_प्रकार l) {
        निश्चित(x);
        निश्चित(s || l == 0);

        अगर (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        अगर (l > 0 || !*x) {
                माप_प्रकार q;
                अक्षर *m;

                q = strlen_ptr(*x);
                m = पुनः_स्मृति(*x, q + l + 1);
                अगर (!m)
                        वापस शून्य;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        वापस *x;
}

अक्षर* strstrip(अक्षर *s) {
        अगर (!s)
                वापस शून्य;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        वापस delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

अक्षर* delete_chars(अक्षर *s, स्थिर अक्षर *bad) {
        अक्षर *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        अगर (!s)
                वापस शून्य;

        अगर (!bad)
                bad = WHITESPACE;

        क्रम (f = s, t = s; *f; f++) {
                अगर (म_अक्षर(bad, *f))
                        जारी;

                *(t++) = *f;
        }

        *t = 0;

        वापस s;
}

अक्षर* delete_trailing_chars(अक्षर *s, स्थिर अक्षर *bad) {
        अक्षर *c = s;

        /* Drops all specified bad characters, at the end of the string */

        अगर (!s)
                वापस शून्य;

        अगर (!bad)
                bad = WHITESPACE;

        क्रम (अक्षर *p = s; *p; p++)
                अगर (!म_अक्षर(bad, *p))
                        c = p + 1;

        *c = 0;

        वापस s;
}

अक्षर* truncate_nl_full(अक्षर *s, माप_प्रकार *ret_len) {
        माप_प्रकार n;

        निश्चित(s);

        n = म_खोज(s, NEWLINE);
        s[n] = '\0';
        अगर (ret_len)
                *ret_len = n;
        वापस s;
}

अक्षर ascii_tolower(अक्षर x) {

        अगर (x >= 'A' && x <= 'Z')
                वापस x - 'A' + 'a';

        वापस x;
}

अक्षर ascii_toupper(अक्षर x) {

        अगर (x >= 'a' && x <= 'z')
                वापस x - 'a' + 'A';

        वापस x;
}

अक्षर* ascii_strlower(अक्षर *s) {
        निश्चित(s);

        क्रम (अक्षर *p = s; *p; p++)
                *p = ascii_tolower(*p);

        वापस s;
}

अक्षर* ascii_strupper(अक्षर *s) {
        निश्चित(s);

        क्रम (अक्षर *p = s; *p; p++)
                *p = ascii_toupper(*p);

        वापस s;
}

अक्षर* ascii_strlower_n(अक्षर *s, माप_प्रकार n) {
        निश्चित(n <= 0 || s);

        अगर (n <= 0)
                वापस s;

        क्रम (माप_प्रकार i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        वापस s;
}

पूर्णांक ascii_strcasecmp_n(स्थिर अक्षर *a, स्थिर अक्षर *b, माप_प्रकार n) {

        निश्चित(a);
        निश्चित(b);

        क्रम (; n > 0; a++, b++, n--) {
                पूर्णांक x, y;

                x = (पूर्णांक) (uint8_t) ascii_tolower(*a);
                y = (पूर्णांक) (uint8_t) ascii_tolower(*b);

                अगर (x != y)
                        वापस x - y;
        }

        वापस 0;
}

पूर्णांक ascii_strcasecmp_nn(स्थिर अक्षर *a, माप_प्रकार n, स्थिर अक्षर *b, माप_प्रकार m) {
        पूर्णांक r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        अगर (r != 0)
                वापस r;

        वापस CMP(n, m);
}

bool chars_intersect(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        /* Returns true if any of the chars in a are in b. */
        क्रम (स्थिर अक्षर *p = a; *p; p++)
                अगर (म_अक्षर(b, *p))
                        वापस true;

        वापस false;
}

bool string_has_cc(स्थिर अक्षर *p, स्थिर अक्षर *ok) {
        निश्चित(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        क्रम (स्थिर अक्षर *t = p; *t; t++) {
                अगर (ok && म_अक्षर(ok, *t))
                        जारी;

                अगर (char_is_cc(*t))
                        वापस true;
        }

        वापस false;
}

अटल पूर्णांक write_ellipsis(अक्षर *buf, bool unicode) {
        स्थिर अक्षर *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        निश्चित(म_माप(s) == 3);
        स_नकल(buf, s, 3);
        वापस 3;
}

अटल माप_प्रकार ansi_sequence_लंबाई(स्थिर अक्षर *s, माप_प्रकार len) {
        निश्चित(s);

        अगर (len < 2)
                वापस 0;

        अगर (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                वापस 0;  /* Not the start of a sequence */

        अगर (s[1] == 0x5B) { /* [, start of CSI sequence */
                माप_प्रकार i = 2;

                अगर (i == len)
                        वापस 0;

                जबतक (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        अगर (++i == len)
                                वापस 0;
                जबतक (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        अगर (++i == len)
                                वापस 0;
                अगर (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        वापस i + 1;
                वापस 0;  /* Bad sequence */

        } अन्यथा अगर (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                वापस 2;

        वापस 0;  /* Bad escape? */
}

अटल bool string_has_ansi_sequence(स्थिर अक्षर *s, माप_प्रकार len) {
        स्थिर अक्षर *t = s;

        जबतक ((t = स_प्रथम(t, 0x1B, len - (t - s)))) {
                अगर (ansi_sequence_लंबाई(t, len - (t - s)) > 0)
                        वापस true;
                t++;
        }
        वापस false;
}

अटल माप_प्रकार previous_ansi_sequence(स्थिर अक्षर *s, माप_प्रकार length, स्थिर अक्षर **ret_where) {

        निश्चित(s);
        निश्चित(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        अगर (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = शून्य;
                वापस 0;
        }

        क्रम (माप_प्रकार i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                माप_प्रकार slen = ansi_sequence_लंबाई(s + i, length - i);
                अगर (slen > 0) {
                        *ret_where = s + i;
                        वापस slen;
                }

                अगर (i == 0)
                        अवरोध;
        }

        *ret_where = शून्य;
        वापस 0;
}

अटल अक्षर *ascii_ellipsize_mem(स्थिर अक्षर *s, माप_प्रकार old_लंबाई, माप_प्रकार new_लंबाई, अचिन्हित percent) {
        माप_प्रकार x, need_space, suffix_len;
        अक्षर *t;

        निश्चित(s);
        निश्चित(percent <= 100);
        निश्चित(new_लंबाई != SIZE_MAX);

        अगर (old_लंबाई <= new_लंबाई)
                वापस strndup(s, old_लंबाई);

        /* Special case short ellipsations */
        चयन (new_लंबाई) {

        हाल 0:
                वापस strdup("");

        हाल 1:
                अगर (is_locale_utf8())
                        वापस strdup("…");
                अन्यथा
                        वापस strdup(".");

        हाल 2:
                अगर (!is_locale_utf8())
                        वापस strdup("..");
                अवरोध;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(अक्षर, new_लंबाई+3);
        अगर (!t)
                वापस शून्य;

        निश्चित(new_लंबाई >= need_space);

        x = ((new_लंबाई - need_space) * percent + 50) / 100;
        निश्चित(x <= new_लंबाई - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_लंबाई - x - need_space;
        स_नकल(t + x + 3, s + old_लंबाई - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        वापस t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
अटल स्थिर अक्षर* find_previous_unichar(स्थिर अक्षर *start, स्थिर अक्षर *end, char32_t *ret_c) {
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(start);
        निश्चित(end);
        निश्चित(end > start);

        क्रम (p = end; p > start; ) {
                p--;
                अगर (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        अवरोध;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        अगर (r < 0 || p + r != end)
                वापस शून्य;

        अगर (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        वापस p;
}

अक्षर* ellipsize_mem(स्थिर अक्षर *s, माप_प्रकार old_लंबाई, माप_प्रकार new_लंबाई, अचिन्हित percent) {
        माप_प्रकार x, k, len, len2;
        स्थिर अक्षर *i, *j;
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

        निश्चित(s);
        निश्चित(percent <= 100);

        अगर (new_लंबाई == SIZE_MAX)
                वापस strndup(s, old_लंबाई);

        अगर (new_लंबाई == 0)
                वापस strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_लंबाई);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        अगर (!has_ansi_seq && ascii_is_valid_n(s, old_लंबाई) && !स_प्रथम(s, '\t', old_लंबाई))
                वापस ascii_ellipsize_mem(s, old_लंबाई, new_लंबाई, percent);

        x = (new_लंबाई - 1) * percent / 100;
        निश्चित(x <= new_लंबाई - 1);

        k = 0;
        क्रम (i = s; i < s + old_लंबाई; ) {
                माप_प्रकार slen = has_ansi_seq ? ansi_sequence_लंबाई(i, old_लंबाई - (i - s)) : 0;
                अगर (slen > 0) {
                        i += slen;
                        जारी;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_लंबाई - i);
                अगर (r < 0)
                        वापस शून्य;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                पूर्णांक w = unichar_console_width(c);
                अगर (k + w > x)
                        अवरोध;

                k += w;
                i += r;
        }

        स्थिर अक्षर *ansi_start = s + old_लंबाई;
        माप_प्रकार ansi_len = 0;

        क्रम (स्थिर अक्षर *t = j = s + old_लंबाई; t > i && k < new_लंबाई; ) {
                char32_t c;
                पूर्णांक w;
                स्थिर अक्षर *tt;

                अगर (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                अगर (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        जारी;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                अगर (!tt)
                        वापस शून्य;

                w = unichar_console_width(c);
                अगर (k + w > new_लंबाई)
                        अवरोध;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        अगर (i >= j)
                वापस memdup_suffix0(s, old_लंबाई);

        अगर (k >= new_लंबाई) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                अगर (j < s + old_लंबाई) {
                        r = utf8_encoded_valid_unichar(j, s + old_लंबाई - j);
                        अगर (r < 0)
                                वापस शून्य;

                        j += r;
                } अन्यथा अगर (i > s) {
                        स्थिर अक्षर *tt = find_previous_unichar(s, i, शून्य);
                        अगर (!tt)
                                वापस शून्य;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_लंबाई - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        माप_प्रकार alloc_len = has_ansi_seq ? old_लंबाई + 3 + 1 : len + 3 + len2 + 1;

        अक्षर *e = new(अक्षर, alloc_len);
        अगर (!e)
                वापस शून्य;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        अक्षर *dst = e + len + 3;

        अगर (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                क्रम (स्थिर अक्षर *p = s + len; p < j; ) {
                        माप_प्रकार slen = ansi_sequence_लंबाई(p, j - p);
                        अगर (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } अन्यथा
                                p = utf8_अग्र_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        वापस e;
}

अक्षर* cellescape(अक्षर *buf, माप_प्रकार len, स्थिर अक्षर *s) {
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

        निश्चित(buf);
        निश्चित(len > 0); /* at least a terminating NUL */
        निश्चित(s);

        क्रम (;;) {
                अक्षर four[4];
                पूर्णांक w;

                अगर (*s == 0) /* terminating NUL detected? then we are done! */
                        जाओ done;

                w = cescape_char(*s, four);
                अगर (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        अवरोध;

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
        क्रम (माप_प्रकार j = 0; j < ELEMENTSOF(last_char_width); j++) {

                अगर (i + 4 <= len) /* nice, we reached our space goal */
                        अवरोध;

                k = k == 0 ? 3 : k - 1;
                अगर (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        अवरोध;

                निश्चित(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        अगर (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        अन्यथा अगर (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } अन्यथा अगर (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        अन्यथा
                निश्चित(i + 1 <= len);

done:
        buf[i] = '\0';
        वापस buf;
}

अक्षर* strshorten(अक्षर *s, माप_प्रकार l) {
        निश्चित(s);

        अगर (l >= SIZE_MAX-1) /* Would not change anything */
                वापस s;

        अगर (strnlen(s, l+1) > l)
                s[l] = 0;

        वापस s;
}

पूर्णांक strgrowpad0(अक्षर **s, माप_प्रकार l) {
        माप_प्रकार sz;

        निश्चित(s);

        अगर (*s) {
                sz = म_माप(*s) + 1;
                अगर (sz >= l) /* never shrink */
                        वापस 0;
        } अन्यथा
                sz = 0;

        अक्षर *q = पुनः_स्मृति(*s, l);
        अगर (!q)
                वापस -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        वापस 0;
}

अक्षर* strreplace(स्थिर अक्षर *text, स्थिर अक्षर *old_string, स्थिर अक्षर *new_string) {
        माप_प्रकार l, old_len, new_len;
        अक्षर *t, *ret = शून्य;
        स्थिर अक्षर *f;

        निश्चित(old_string);
        निश्चित(new_string);

        अगर (!text)
                वापस शून्य;

        old_len = म_माप(old_string);
        new_len = म_माप(new_string);

        l = म_माप(text);
        अगर (!GREEDY_REALLOC(ret, l+1))
                वापस शून्य;

        f = text;
        t = ret;
        जबतक (*f) {
                माप_प्रकार d, nl;

                अगर (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        जारी;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                अगर (!GREEDY_REALLOC(ret, nl + 1))
                        वापस mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        वापस ret;
}

अटल व्योम advance_offsets(
                ssize_t diff,
                माप_प्रकार offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                माप_प्रकार shift[अटल 2],
                माप_प्रकार आकार) {

        अगर (!offsets)
                वापस;

        निश्चित(shift);

        अगर ((माप_प्रकार) diff < offsets[0])
                shift[0] += आकार;
        अगर ((माप_प्रकार) diff < offsets[1])
                shift[1] += आकार;
}

अटल व्योम retract_offsets(
                माप_प्रकार begin,
                माप_प्रकार end,
                माप_प्रकार offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                माप_प्रकार shift[अटल 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        अगर (!offsets)
                वापस;

        निश्चित(shift);
        निश्चित(begin <= end);

        क्रम (माप_प्रकार k = 0; k < 2; k++)
                अगर (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

अक्षर* strip_tab_ansi(अक्षर **ibuf, माप_प्रकार *_isz, माप_प्रकार highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        स्थिर अक्षर *begin = शून्य;
        माप_प्रकार isz, shift[2] = {}, n_carriage_returns = 0;
        खाता *f;

        निश्चित(ibuf);
        निश्चित(*ibuf);
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
        अगर (!f)
                वापस शून्य;

        क्रम (स्थिर अक्षर *i = *ibuf; i < *ibuf + isz; i++) {
                पूर्णांक e;

                क्रम (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        अगर (e < 0)
                                वापस शून्य;
                        अगर (e != ANSI_SEQ_EVENT_ABORT)
                                अवरोध;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        अगर (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = शून्य;
                        }
                }

                चयन (e) {

                हाल ANSI_SEQ_EVENT_TEXT:
                        अगर (*i == '\r') {
                                n_carriage_returns++;
                                जारी;
                        }
                        अगर (*i == '\n') {
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
                        क्रम (; n_carriage_returns > 0; n_carriage_returns--)
                                ख_अक्षर_दो('\r', f);

                        अगर (*i == '\t') {
                                ख_माला_दो("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } अन्यथा
                                ख_अक्षर_दो(*i, f);

                        अवरोध;

                हाल ANSI_SEQ_EVENT_SEQUENCE:
                        अगर (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                क्रम (; n_carriage_returns > 0; n_carriage_returns--)
                                        ख_अक्षर_दो('\r', f);

                                begin = i;
                        }
                        अवरोध;

                हाल ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        निश्चित(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = शून्य;
                        अवरोध;

                शेष:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        अगर (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        अक्षर *obuf;
        अगर (memstream_finalize(&m, &obuf, _isz) < 0)
                वापस शून्य;

        free_and_replace(*ibuf, obuf);

        अगर (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        वापस *ibuf;
}

अक्षर* strextendv_with_separator(अक्षर **x, स्थिर अक्षर *separator, बहु_सूची ap) {
        _cleanup_free_ अक्षर *बफर = शून्य;
        माप_प्रकार f, l, l_separator;
        bool need_separator;
        अक्षर *nr, *p;

        अगर (!x)
                x = &बफर;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        बहु_सूची aq;
        va_copy(aq, ap);
        क्रम (स्थिर अक्षर *t;;) {
                माप_प्रकार n;

                t = बहु_तर्क(aq, स्थिर अक्षर *);
                अगर (!t)
                        अवरोध;
                अगर (t == POINTER_MAX)
                        जारी;

                n = म_माप(t);

                अगर (need_separator)
                        n += l_separator;

                अगर (n >= SIZE_MAX - l) {
                        बहु_पूर्ण(aq);
                        वापस शून्य;
                }

                l += n;
                need_separator = true;
        }
        बहु_पूर्ण(aq);

        need_separator = !isempty(*x);

        nr = पुनः_स्मृति(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        अगर (!nr)
                वापस शून्य;

        *x = nr;
        p = nr + f;

        क्रम (;;) {
                स्थिर अक्षर *t;

                t = बहु_तर्क(ap, स्थिर अक्षर *);
                अगर (!t)
                        अवरोध;
                अगर (t == POINTER_MAX)
                        जारी;

                अगर (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        निश्चित(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        अगर (बफर)
                वापस TAKE_PTR(बफर);

        /* Otherwise we extended the buffer: return the end */
        वापस p;
}

अक्षर* strextend_with_separator_internal(अक्षर **x, स्थिर अक्षर *separator, ...) {
        बहु_सूची ap;
        अक्षर *ret;

        बहु_शुरू(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        बहु_पूर्ण(ap);

        वापस ret;
}

पूर्णांक strextendf_with_separator(अक्षर **x, स्थिर अक्षर *separator, स्थिर अक्षर *format, ...) {
        माप_प्रकार m, a, l_separator;
        बहु_सूची ap;
        पूर्णांक l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        निश्चित(x);
        निश्चित(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        अगर (*x) {
                m = म_माप(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                निश्चित(a >= m + 1);
        } अन्यथा
                m = a = 0;

        अगर (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                अक्षर *n;

                अगर (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        वापस -ENOMEM;
                अगर (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        वापस -ENOMEM;

                n = पुनः_स्मृति(*x, m + 64 + l_separator);
                अगर (!n)
                        वापस -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        बहु_शुरू(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        बहु_पूर्ण(ap);

        निश्चित(l >= 0);

        अगर ((माप_प्रकार) l < a - m - l_separator) {
                अक्षर *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = पुनः_स्मृति(*x, m + (माप_प्रकार) l + l_separator + 1);
                अगर (n)
                        *x = n;
        } अन्यथा {
                अक्षर *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                अगर (_unlikely_((माप_प्रकार) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        जाओ oom;
                अगर (_unlikely_(m > SIZE_MAX - ((माप_प्रकार) l + l_separator + 1))) /* overflow check #2 */
                        जाओ oom;

                a = m + (माप_प्रकार) l + l_separator + 1;
                n = पुनः_स्मृति(*x, a);
                अगर (!n)
                        जाओ oom;
                *x = n;

                बहु_शुरू(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                बहु_पूर्ण(ap);

                निश्चित((माप_प्रकार) l < a - m - l_separator);
        }

        वापस 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        वापस -ENOMEM;
}

अक्षर* strrep(स्थिर अक्षर *s, माप_प्रकार n) {
        अक्षर *ret, *p;
        माप_प्रकार l;

        निश्चित(s);

        l = म_माप(s);
        अगर (!MUL_ASSIGN_SAFE(&l, n))
                वापस शून्य;
        अगर (!INC_SAFE(&l, 1))
                वापस शून्य;

        p = ret = दो_स्मृति(l);
        अगर (!ret)
                वापस शून्य;

        क्रम (माप_प्रकार i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        वापस ret;
}

पूर्णांक split_pair(स्थिर अक्षर *s, स्थिर अक्षर *sep, अक्षर **ret_first, अक्षर **ret_second) {
        निश्चित(s);
        निश्चित(!isempty(sep));

        स्थिर अक्षर *x = म_माला(s, sep);
        अगर (!x)
                वापस -EINVAL;

        _cleanup_free_ अक्षर *a = शून्य;
        अगर (ret_first) {
                a = strndup(s, x - s);
                अगर (!a)
                        वापस -ENOMEM;
        }

        _cleanup_free_ अक्षर *b = शून्य;
        अगर (ret_second) {
                b = strdup(x + म_माप(sep));
                अगर (!b)
                        वापस -ENOMEM;
        }

        अगर (ret_first)
                *ret_first = TAKE_PTR(a);
        अगर (ret_second)
                *ret_second = TAKE_PTR(b);
        वापस 0;
}

पूर्णांक free_and_strdup(अक्षर **p, स्थिर अक्षर *s) {
        अक्षर *t;

        निश्चित(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        अगर (streq_ptr(*p, s))
                वापस 0;

        अगर (s) {
                t = strdup(s);
                अगर (!t)
                        वापस -ENOMEM;
        } अन्यथा
                t = शून्य;

        free_and_replace(*p, t);

        वापस 1;
}

पूर्णांक free_and_strdup_warn(अक्षर **p, स्थिर अक्षर *s) {
        पूर्णांक r;

        r = free_and_strdup(p, s);
        अगर (r < 0)
                वापस log_oom();
        वापस r;
}

पूर्णांक free_and_strndup(अक्षर **p, स्थिर अक्षर *s, माप_प्रकार l) {
        अक्षर *t;

        निश्चित(p);
        निश्चित(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        अगर (!*p && !s)
                वापस 0;

        अगर (*p && s && strneq(*p, s, l) && (l > म_माप(*p) || (*p)[l] == '\0'))
                वापस 0;

        अगर (s) {
                t = strndup(s, l);
                अगर (!t)
                        वापस -ENOMEM;
        } अन्यथा
                t = शून्य;

        free_and_replace(*p, t);
        वापस 1;
}

पूर्णांक strdup_to_full(अक्षर **ret, स्थिर अक्षर *src) {
        अगर (!src) {
                अगर (ret)
                        *ret = शून्य;

                वापस 0;
        } अन्यथा {
                अगर (ret) {
                        अक्षर *t = strdup(src);
                        अगर (!t)
                                वापस -ENOMEM;
                        *ret = t;
                }

                वापस 1;
        }
};

bool string_is_safe(स्थिर अक्षर *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        अगर (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                वापस false;

        अगर (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                वापस false;

        क्रम (स्थिर अक्षर *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                अगर ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        वापस false;

                अगर (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        वापस false;

                अगर (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        वापस false;

                अगर (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && म_अक्षर(QUOTES, *t))
                        वापस false;

                अगर (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && म_अक्षर(GLOB_CHARS, *t))
                        वापस false;

                अगर (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && म_अक्षर(WHITESPACE, *t))
                        वापस false;

                अगर (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        वापस false;
        }

        अगर (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                वापस false;

        अगर (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                वापस false;

        वापस true;
}

अक्षर* str_realloc(अक्षर *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        अगर (!p)
                वापस शून्य;

        वापस पुनः_स्मृति(p, म_माप(p) + 1) ?: p;
}

अक्षर* string_erase(अक्षर *x) {
        अगर (!x)
                वापस शून्य;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, म_माप(x));
        वापस x;
}

पूर्णांक string_truncate_पंक्तियाँ(स्थिर अक्षर *s, माप_प्रकार n_पंक्तियाँ, अक्षर **ret) {
        स्थिर अक्षर *p = s, *e = s;
        bool truncation_applied = false;
        अक्षर *copy;
        माप_प्रकार n = 0;

        निश्चित(s);
        निश्चित(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        क्रम (;;) {
                माप_प्रकार k;

                k = म_खोज(p, "\n");

                अगर (p[k] == 0) {
                        अगर (k == 0) /* final empty line */
                                अवरोध;

                        अगर (n >= n_पंक्तियाँ) /* above threshold */
                                अवरोध;

                        e = p + k; /* last line to include */
                        अवरोध;
                }

                निश्चित(p[k] == '\n');

                अगर (n >= n_पंक्तियाँ)
                        अवरोध;

                अगर (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        अगर (isempty(e))
                copy = strdup(s);
        अन्यथा {
                अगर (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        अगर (!copy)
                वापस -ENOMEM;

        *ret = copy;
        वापस truncation_applied;
}

पूर्णांक string_extract_पंक्ति(स्थिर अक्षर *s, माप_प्रकार i, अक्षर **ret) {
        स्थिर अक्षर *p = s;
        माप_प्रकार c = 0;

        निश्चित(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        क्रम (;;) {
                स्थिर अक्षर *q;

                q = म_अक्षर(p, '\n');
                अगर (i == c) {
                        /* The line we are looking for! */

                        अगर (q) {
                                अक्षर *m;

                                m = strndup(p, q - p);
                                अगर (!m)
                                        वापस -ENOMEM;

                                *ret = m;
                                वापस !isempty(q + 1); /* More coming? */
                        } अन्यथा
                                /* Tell the caller to use the input string if equal */
                                वापस strdup_to(ret, p != s ? p : शून्य);
                }

                अगर (!q)
                        /* No more lines, return empty line */
                        वापस strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

पूर्णांक string_contains_word_strv(स्थिर अक्षर *string, स्थिर अक्षर *separators, अक्षर * स्थिर *शब्दगण, स्थिर अक्षर **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        स्थिर ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        स्थिर अक्षर *found = शून्य;
        पूर्णांक r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *w = शून्य;

                r = extract_first_word(&string, &w, separators, flags);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                found = strv_find(शब्दगण, w);
                अगर (found)
                        अवरोध;
        }

        अगर (ret_word)
                *ret_word = found;
        वापस !!found;
}

bool streq_skip_trailing_chars(स्थिर अक्षर *s1, स्थिर अक्षर *s2, स्थिर अक्षर *ok) {
        अगर (!s1 && !s2)
                वापस true;
        अगर (!s1 || !s2)
                वापस false;

        अगर (!ok)
                ok = WHITESPACE;

        क्रम (; *s1 && *s2; s1++, s2++)
                अगर (*s1 != *s2)
                        अवरोध;

        वापस in_charset(s1, ok) && in_charset(s2, ok);
}

अक्षर* string_replace_char(अक्षर *str, अक्षर old_char, अक्षर new_char) {
        निश्चित(str);
        निश्चित(old_char != '\0');
        निश्चित(new_char != '\0');
        निश्चित(old_char != new_char);

        क्रम (अक्षर *p = म_अक्षर(str, old_char); p; p = म_अक्षर(p + 1, old_char))
                *p = new_char;

        वापस str;
}

पूर्णांक make_cstring(स्थिर व्योम *s, माप_प्रकार n, MakeCStringMode mode, अक्षर **ret) {
        अक्षर *b;

        निश्चित(s || n == 0);
        निश्चित(mode >= 0);
        निश्चित(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        अगर (n == 0) {
                अगर (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        वापस -EINVAL;

                अगर (!ret)
                        वापस 0;

                b = new0(अक्षर, 1);
        } अन्यथा {
                स्थिर uint8_t *nul;

                nul = स_प्रथम(s, 0, n);
                अगर (nul) {
                        अगर (nul < (स्थिर uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                वापस -EINVAL;

                        n--;
                } अन्यथा अगर (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        वापस -EINVAL;

                अगर (!ret)
                        वापस 0;

                b = memdup_suffix0(s, n);
        }
        अगर (!b)
                वापस -ENOMEM;

        *ret = b;
        वापस 0;
}

माप_प्रकार strspn_from_end(स्थिर अक्षर *str, स्थिर अक्षर *accept) {
        माप_प्रकार n = 0;

        अगर (isempty(str))
                वापस 0;

        अगर (isempty(accept))
                वापस 0;

        क्रम (स्थिर अक्षर *p = str + म_माप(str); p > str && म_अक्षर(accept, p[-1]); p--)
                n++;

        वापस n;
}

माप_प्रकार strnspn(स्थिर अक्षर *str, स्थिर अक्षर *accept, माप_प्रकार n) {
        माप_प्रकार i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        निश्चित(str || n == 0);
        निश्चित(accept);

        क्रम (i = 0; i < n && str[i] != '\0' && म_अक्षर(accept, str[i]); i++)
                ;

        वापस i;
}

अक्षर* strdupspn(स्थिर अक्षर *a, स्थिर अक्षर *accept) {
        अगर (isempty(a) || isempty(accept))
                वापस strdup("");

        वापस strndup(a, म_अखोज(a, accept));
}

अक्षर* strdupcspn(स्थिर अक्षर *a, स्थिर अक्षर *reject) {
        अगर (isempty(a))
                वापस strdup("");
        अगर (isempty(reject))
                वापस strdup(a);

        वापस strndup(a, म_खोज(a, reject));
}

अक्षर* find_पंक्ति_startswith_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        निश्चित(haystack);
        निश्चित(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        अक्षर *p = (अक्षर*) म_माला(haystack, needle);
        अगर (!p)
                वापस शून्य;

        अगर (p > haystack)
                जबतक (p[-1] != '\n') {
                        p = म_माला(p + 1, needle);
                        अगर (!p)
                                वापस शून्य;
                }

        वापस p + म_माप(needle);
}

अक्षर* find_पंक्ति_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        निश्चित(haystack);
        निश्चित(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        अक्षर *p = (अक्षर*) find_पंक्ति_startswith(haystack, needle);
        अगर (!p)
                वापस शून्य;

        अगर (*p == 0 || म_अक्षर(NEWLINE, *p))
                वापस p - म_माप(needle);

        वापस शून्य;
}

अक्षर* find_पंक्ति_after_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        निश्चित(haystack);
        निश्चित(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        अक्षर *p = (अक्षर*) find_पंक्ति_startswith(haystack, needle);
        अगर (!p)
                वापस शून्य;

        अगर (*p == 0)
                वापस p;
        अगर (म_अक्षर(NEWLINE, *p))
                वापस p + 1;

        वापस शून्य;
}

bool version_is_valid(स्थिर अक्षर *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        अगर (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                वापस false;

        अगर (!filename_part_is_valid(s))
                वापस false;

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

        अगर (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        अगर (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        वापस in_charset(s, charset);
}

ssize_t strlevenshtein(स्थिर अक्षर *x, स्थिर अक्षर *y) {
        _cleanup_free_ माप_प्रकार *t0 = शून्य, *t1 = शून्य, *t2 = शून्य;
        माप_प्रकार xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        अगर (streq_ptr(x, y))
                वापस 0;

        xl = strlen_ptr(x);
        अगर (xl > SSIZE_MAX)
                वापस -E2BIG;

        yl = strlen_ptr(y);
        अगर (yl > SSIZE_MAX)
                वापस -E2BIG;

        अगर (isempty(x))
                वापस yl;
        अगर (isempty(y))
                वापस xl;

        t0 = new0(माप_प्रकार, yl + 1);
        अगर (!t0)
                वापस -ENOMEM;
        t1 = new0(माप_प्रकार, yl + 1);
        अगर (!t1)
                वापस -ENOMEM;
        t2 = new0(माप_प्रकार, yl + 1);
        अगर (!t2)
                वापस -ENOMEM;

        क्रम (माप_प्रकार i = 0; i <= yl; i++)
                t1[i] = i;

        क्रम (माप_प्रकार i = 0; i < xl; i++) {
                t2[0] = i + 1;

                क्रम (माप_प्रकार j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        अगर (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        अगर (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        अगर (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                माप_प्रकार *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        वापस t1[yl];
}

अक्षर* strrstr_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        अगर (!haystack || !needle)
                वापस शून्य;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        अगर (needle[0] == 0)
                वापस (अक्षर*) म_अक्षर(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        अगर (needle[1] == 0)
                वापस (अक्षर*) म_खोजप(haystack, needle[0]);

        क्रम (स्थिर अक्षर *p = म_माला(haystack, needle), *q; p; p = q) {
                q = म_माला(p + 1, needle);
                अगर (!q)
                        वापस (अक्षर*) p;
        }
        वापस शून्य;
}

अक्षर* strrstr_no_case_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        अगर (!haystack || !needle)
                वापस शून्य;

        क्रम (स्थिर अक्षर *p = म_अक्षर(haystack, 0); p > haystack; p--)
                अगर (startswith_no_case(p, needle))
                        वापस (अक्षर*) p;

        वापस startswith_no_case(haystack, needle) ? (अक्षर*) haystack : शून्य;
}

माप_प्रकार str_common_prefix(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        निश्चित(a);
        निश्चित(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        क्रम (माप_प्रकार n = 0;; n++) {
                अक्षर c = a[n];
                अगर (c != b[n])
                        वापस n;
                अगर (c == 0)
                        वापस SIZE_MAX;
        }
}
