/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "ansi-seq.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "glyph-util.h"
#அந்தர்பாவ "gunicode.h"
#அந்தர்பாவ "locale-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "memstream-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "utf8.h"

எழுத்து* first_word(மாறிலி எழுத்து *s, மாறிலி எழுத்து *word) {
        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        எனில் (isempty(word))
                திரும்பு (எழுத்து*) s;

        மாறிலி எழுத்து *p = startswith(s, word);
        எனில் (!p)
                திரும்பு வெற்று;
        எனில் (*p == '\0')
                திரும்பு (எழுத்து*) p;

        மாறிலி எழுத்து *nw = skip_leading_chars(p, WHITESPACE);
        எனில் (p == nw)
                திரும்பு வெற்று;

        திரும்பு (எழுத்து*) nw;
}

எழுத்து* strextendn(எழுத்து **x, மாறிலி எழுத்து *s, size_t l) {
        ப்ரதிஜ்ஞா(x);
        ப்ரதிஜ்ஞா(s || l == 0);

        எனில் (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        எனில் (l > 0 || !*x) {
                size_t q;
                எழுத்து *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                எனில் (!m)
                        திரும்பு வெற்று;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        திரும்பு *x;
}

எழுத்து* strstrip(எழுத்து *s) {
        எனில் (!s)
                திரும்பு வெற்று;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        திரும்பு delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

எழுத்து* delete_chars(எழுத்து *s, மாறிலி எழுத்து *bad) {
        எழுத்து *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        எனில் (!s)
                திரும்பு வெற்று;

        எனில் (!bad)
                bad = WHITESPACE;

        க்கு (f = s, t = s; *f; f++) {
                எனில் (strchr(bad, *f))
                        தொடர்;

                *(t++) = *f;
        }

        *t = 0;

        திரும்பு s;
}

எழுத்து* delete_trailing_chars(எழுத்து *s, மாறிலி எழுத்து *bad) {
        எழுத்து *c = s;

        /* Drops all specified bad characters, at the end of the string */

        எனில் (!s)
                திரும்பு வெற்று;

        எனில் (!bad)
                bad = WHITESPACE;

        க்கு (எழுத்து *p = s; *p; p++)
                எனில் (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        திரும்பு s;
}

எழுத்து* truncate_nl_full(எழுத்து *s, size_t *ret_len) {
        size_t n;

        ப்ரதிஜ்ஞா(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        எனில் (ret_len)
                *ret_len = n;
        திரும்பு s;
}

எழுத்து ascii_tolower(எழுத்து x) {

        எனில் (x >= 'A' && x <= 'Z')
                திரும்பு x - 'A' + 'a';

        திரும்பு x;
}

எழுத்து ascii_toupper(எழுத்து x) {

        எனில் (x >= 'a' && x <= 'z')
                திரும்பு x - 'a' + 'A';

        திரும்பு x;
}

எழுத்து* ascii_strlower(எழுத்து *s) {
        ப்ரதிஜ்ஞா(s);

        க்கு (எழுத்து *p = s; *p; p++)
                *p = ascii_tolower(*p);

        திரும்பு s;
}

எழுத்து* ascii_strupper(எழுத்து *s) {
        ப்ரதிஜ்ஞா(s);

        க்கு (எழுத்து *p = s; *p; p++)
                *p = ascii_toupper(*p);

        திரும்பு s;
}

எழுத்து* ascii_strlower_n(எழுத்து *s, size_t n) {
        ப்ரதிஜ்ஞா(n <= 0 || s);

        எனில் (n <= 0)
                திரும்பு s;

        க்கு (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        திரும்பு s;
}

முழு ascii_strcasecmp_n(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b, size_t n) {

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        க்கு (; n > 0; a++, b++, n--) {
                முழு x, y;

                x = (முழு) (uint8_t) ascii_tolower(*a);
                y = (முழு) (uint8_t) ascii_tolower(*b);

                எனில் (x != y)
                        திரும்பு x - y;
        }

        திரும்பு 0;
}

முழு ascii_strcasecmp_nn(மாறிலி எழுத்து *a, size_t n, மாறிலி எழுத்து *b, size_t m) {
        முழு r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        எனில் (r != 0)
                திரும்பு r;

        திரும்பு CMP(n, m);
}

bool chars_intersect(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        /* Returns true if any of the chars in a are in b. */
        க்கு (மாறிலி எழுத்து *p = a; *p; p++)
                எனில் (strchr(b, *p))
                        திரும்பு true;

        திரும்பு false;
}

bool string_has_cc(மாறிலி எழுத்து *p, மாறிலி எழுத்து *ok) {
        ப்ரதிஜ்ஞா(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        க்கு (மாறிலி எழுத்து *t = p; *t; t++) {
                எனில் (ok && strchr(ok, *t))
                        தொடர்;

                எனில் (char_is_cc(*t))
                        திரும்பு true;
        }

        திரும்பு false;
}

நிலையான முழு write_ellipsis(எழுத்து *buf, bool unicode) {
        மாறிலி எழுத்து *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        ப்ரதிஜ்ஞா(strlen(s) == 3);
        memcpy(buf, s, 3);
        திரும்பு 3;
}

நிலையான size_t ansi_sequence_length(மாறிலி எழுத்து *s, size_t len) {
        ப்ரதிஜ்ஞா(s);

        எனில் (len < 2)
                திரும்பு 0;

        எனில் (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                திரும்பு 0;  /* Not the start of a sequence */

        எனில் (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                எனில் (i == len)
                        திரும்பு 0;

                போது (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        எனில் (++i == len)
                                திரும்பு 0;
                போது (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        எனில் (++i == len)
                                திரும்பு 0;
                எனில் (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        திரும்பு i + 1;
                திரும்பு 0;  /* Bad sequence */

        } இல்லையெனில் எனில் (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                திரும்பு 2;

        திரும்பு 0;  /* Bad escape? */
}

நிலையான bool string_has_ansi_sequence(மாறிலி எழுத்து *s, size_t len) {
        மாறிலி எழுத்து *t = s;

        போது ((t = memchr(t, 0x1B, len - (t - s)))) {
                எனில் (ansi_sequence_length(t, len - (t - s)) > 0)
                        திரும்பு true;
                t++;
        }
        திரும்பு false;
}

நிலையான size_t previous_ansi_sequence(மாறிலி எழுத்து *s, size_t length, மாறிலி எழுத்து **ret_where) {

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        எனில் (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = வெற்று;
                திரும்பு 0;
        }

        க்கு (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                எனில் (slen > 0) {
                        *ret_where = s + i;
                        திரும்பு slen;
                }

                எனில் (i == 0)
                        நிறுத்து;
        }

        *ret_where = வெற்று;
        திரும்பு 0;
}

நிலையான எழுத்து *ascii_ellipsize_mem(மாறிலி எழுத்து *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        எழுத்து *t;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(percent <= 100);
        ப்ரதிஜ்ஞா(new_length != SIZE_MAX);

        எனில் (old_length <= new_length)
                திரும்பு strndup(s, old_length);

        /* Special case short ellipsations */
        தேர்வு (new_length) {

        நிலை 0:
                திரும்பு strdup("");

        நிலை 1:
                எனில் (is_locale_utf8())
                        திரும்பு strdup("…");
                இல்லையெனில்
                        திரும்பு strdup(".");

        நிலை 2:
                எனில் (!is_locale_utf8())
                        திரும்பு strdup("..");
                நிறுத்து;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(எழுத்து, new_length+3);
        எனில் (!t)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        ப்ரதிஜ்ஞா(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        திரும்பு t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
நிலையான மாறிலி எழுத்து* find_previous_unichar(மாறிலி எழுத்து *start, மாறிலி எழுத்து *end, char32_t *ret_c) {
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(start);
        ப்ரதிஜ்ஞா(end);
        ப்ரதிஜ்ஞா(end > start);

        க்கு (p = end; p > start; ) {
                p--;
                எனில் (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        நிறுத்து;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        எனில் (r < 0 || p + r != end)
                திரும்பு வெற்று;

        எனில் (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        திரும்பு p;
}

எழுத்து* ellipsize_mem(மாறிலி எழுத்து *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        மாறிலி எழுத்து *i, *j;
        முழு r;

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

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(percent <= 100);

        எனில் (new_length == SIZE_MAX)
                திரும்பு strndup(s, old_length);

        எனில் (new_length == 0)
                திரும்பு strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        எனில் (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                திரும்பு ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        ப்ரதிஜ்ஞா(x <= new_length - 1);

        k = 0;
        க்கு (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                எனில் (slen > 0) {
                        i += slen;
                        தொடர்;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                எனில் (r < 0)
                        திரும்பு வெற்று;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                முழு w = unichar_console_width(c);
                எனில் (k + w > x)
                        நிறுத்து;

                k += w;
                i += r;
        }

        மாறிலி எழுத்து *ansi_start = s + old_length;
        size_t ansi_len = 0;

        க்கு (மாறிலி எழுத்து *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                முழு w;
                மாறிலி எழுத்து *tt;

                எனில் (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                எனில் (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        தொடர்;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                எனில் (!tt)
                        திரும்பு வெற்று;

                w = unichar_console_width(c);
                எனில் (k + w > new_length)
                        நிறுத்து;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        எனில் (i >= j)
                திரும்பு memdup_suffix0(s, old_length);

        எனில் (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                எனில் (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        எனில் (r < 0)
                                திரும்பு வெற்று;

                        j += r;
                } இல்லையெனில் எனில் (i > s) {
                        மாறிலி எழுத்து *tt = find_previous_unichar(s, i, வெற்று);
                        எனில் (!tt)
                                திரும்பு வெற்று;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        எழுத்து *e = new(எழுத்து, alloc_len);
        எனில் (!e)
                திரும்பு வெற்று;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        எழுத்து *dst = e + len + 3;

        எனில் (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                க்கு (மாறிலி எழுத்து *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        எனில் (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } இல்லையெனில்
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        திரும்பு e;
}

எழுத்து* cellescape(எழுத்து *buf, size_t len, மாறிலி எழுத்து *s) {
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

        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(len > 0); /* at least a terminating NUL */
        ப்ரதிஜ்ஞா(s);

        க்கு (;;) {
                எழுத்து four[4];
                முழு w;

                எனில் (*s == 0) /* terminating NUL detected? then we are done! */
                        செல் done;

                w = cescape_char(*s, four);
                எனில் (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        நிறுத்து;

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
        க்கு (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                எனில் (i + 4 <= len) /* nice, we reached our space goal */
                        நிறுத்து;

                k = k == 0 ? 3 : k - 1;
                எனில் (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        நிறுத்து;

                ப்ரதிஜ்ஞா(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        எனில் (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        இல்லையெனில் எனில் (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } இல்லையெனில் எனில் (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        இல்லையெனில்
                ப்ரதிஜ்ஞா(i + 1 <= len);

done:
        buf[i] = '\0';
        திரும்பு buf;
}

எழுத்து* strshorten(எழுத்து *s, size_t l) {
        ப்ரதிஜ்ஞா(s);

        எனில் (l >= SIZE_MAX-1) /* Would not change anything */
                திரும்பு s;

        எனில் (strnlen(s, l+1) > l)
                s[l] = 0;

        திரும்பு s;
}

முழு strgrowpad0(எழுத்து **s, size_t l) {
        size_t sz;

        ப்ரதிஜ்ஞா(s);

        எனில் (*s) {
                sz = strlen(*s) + 1;
                எனில் (sz >= l) /* never shrink */
                        திரும்பு 0;
        } இல்லையெனில்
                sz = 0;

        எழுத்து *q = realloc(*s, l);
        எனில் (!q)
                திரும்பு -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        திரும்பு 0;
}

எழுத்து* strreplace(மாறிலி எழுத்து *text, மாறிலி எழுத்து *old_string, மாறிலி எழுத்து *new_string) {
        size_t l, old_len, new_len;
        எழுத்து *t, *ret = வெற்று;
        மாறிலி எழுத்து *f;

        ப்ரதிஜ்ஞா(old_string);
        ப்ரதிஜ்ஞா(new_string);

        எனில் (!text)
                திரும்பு வெற்று;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        எனில் (!GREEDY_REALLOC(ret, l+1))
                திரும்பு வெற்று;

        f = text;
        t = ret;
        போது (*f) {
                size_t d, nl;

                எனில் (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        தொடர்;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                எனில் (!GREEDY_REALLOC(ret, nl + 1))
                        திரும்பு mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        திரும்பு ret;
}

நிலையான வெறுமை advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[நிலையான 2],
                size_t size) {

        எனில் (!offsets)
                திரும்பு;

        ப்ரதிஜ்ஞா(shift);

        எனில் ((size_t) diff < offsets[0])
                shift[0] += size;
        எனில் ((size_t) diff < offsets[1])
                shift[1] += size;
}

நிலையான வெறுமை retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[நிலையான 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        எனில் (!offsets)
                திரும்பு;

        ப்ரதிஜ்ஞா(shift);
        ப்ரதிஜ்ஞா(begin <= end);

        க்கு (size_t k = 0; k < 2; k++)
                எனில் (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

எழுத்து* strip_tab_ansi(எழுத்து **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        மாறிலி எழுத்து *begin = வெற்று;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        ப்ரதிஜ்ஞா(ibuf);
        ப்ரதிஜ்ஞா(*ibuf);
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
        எனில் (!f)
                திரும்பு வெற்று;

        க்கு (மாறிலி எழுத்து *i = *ibuf; i < *ibuf + isz; i++) {
                முழு e;

                க்கு (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        எனில் (e < 0)
                                திரும்பு வெற்று;
                        எனில் (e != ANSI_SEQ_EVENT_ABORT)
                                நிறுத்து;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        எனில் (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = வெற்று;
                        }
                }

                தேர்வு (e) {

                நிலை ANSI_SEQ_EVENT_TEXT:
                        எனில் (*i == '\r') {
                                n_carriage_returns++;
                                தொடர்;
                        }
                        எனில் (*i == '\n') {
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
                        க்கு (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        எனில் (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } இல்லையெனில்
                                fputc(*i, f);

                        நிறுத்து;

                நிலை ANSI_SEQ_EVENT_SEQUENCE:
                        எனில் (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                க்கு (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        நிறுத்து;

                நிலை ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        ப்ரதிஜ்ஞா(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = வெற்று;
                        நிறுத்து;

                இயல்பு:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        எனில் (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        எழுத்து *obuf;
        எனில் (memstream_finalize(&m, &obuf, _isz) < 0)
                திரும்பு வெற்று;

        free_and_replace(*ibuf, obuf);

        எனில் (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        திரும்பு *ibuf;
}

எழுத்து* strextendv_with_separator(எழுத்து **x, மாறிலி எழுத்து *separator, va_list ap) {
        _cleanup_free_ எழுத்து *buffer = வெற்று;
        size_t f, l, l_separator;
        bool need_separator;
        எழுத்து *nr, *p;

        எனில் (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        க்கு (மாறிலி எழுத்து *t;;) {
                size_t n;

                t = va_arg(aq, மாறிலி எழுத்து *);
                எனில் (!t)
                        நிறுத்து;
                எனில் (t == POINTER_MAX)
                        தொடர்;

                n = strlen(t);

                எனில் (need_separator)
                        n += l_separator;

                எனில் (n >= SIZE_MAX - l) {
                        va_end(aq);
                        திரும்பு வெற்று;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        எனில் (!nr)
                திரும்பு வெற்று;

        *x = nr;
        p = nr + f;

        க்கு (;;) {
                மாறிலி எழுத்து *t;

                t = va_arg(ap, மாறிலி எழுத்து *);
                எனில் (!t)
                        நிறுத்து;
                எனில் (t == POINTER_MAX)
                        தொடர்;

                எனில் (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        ப்ரதிஜ்ஞா(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        எனில் (buffer)
                திரும்பு TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        திரும்பு p;
}

எழுத்து* strextend_with_separator_internal(எழுத்து **x, மாறிலி எழுத்து *separator, ...) {
        va_list ap;
        எழுத்து *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        திரும்பு ret;
}

முழு strextendf_with_separator(எழுத்து **x, மாறிலி எழுத்து *separator, மாறிலி எழுத்து *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        முழு l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        ப்ரதிஜ்ஞா(x);
        ப்ரதிஜ்ஞா(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        எனில் (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                ப்ரதிஜ்ஞா(a >= m + 1);
        } இல்லையெனில்
                m = a = 0;

        எனில் (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                எழுத்து *n;

                எனில் (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        திரும்பு -ENOMEM;
                எனில் (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        திரும்பு -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                எனில் (!n)
                        திரும்பு -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        ப்ரதிஜ்ஞா(l >= 0);

        எனில் ((size_t) l < a - m - l_separator) {
                எழுத்து *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                எனில் (n)
                        *x = n;
        } இல்லையெனில் {
                எழுத்து *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                எனில் (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        செல் oom;
                எனில் (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        செல் oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                எனில் (!n)
                        செல் oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                ப்ரதிஜ்ஞா((size_t) l < a - m - l_separator);
        }

        திரும்பு 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        திரும்பு -ENOMEM;
}

எழுத்து* strrep(மாறிலி எழுத்து *s, size_t n) {
        எழுத்து *ret, *p;
        size_t l;

        ப்ரதிஜ்ஞா(s);

        l = strlen(s);
        எனில் (!MUL_ASSIGN_SAFE(&l, n))
                திரும்பு வெற்று;
        எனில் (!INC_SAFE(&l, 1))
                திரும்பு வெற்று;

        p = ret = malloc(l);
        எனில் (!ret)
                திரும்பு வெற்று;

        க்கு (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        திரும்பு ret;
}

முழு split_pair(மாறிலி எழுத்து *s, மாறிலி எழுத்து *sep, எழுத்து **ret_first, எழுத்து **ret_second) {
        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(!isempty(sep));

        மாறிலி எழுத்து *x = strstr(s, sep);
        எனில் (!x)
                திரும்பு -EINVAL;

        _cleanup_free_ எழுத்து *a = வெற்று;
        எனில் (ret_first) {
                a = strndup(s, x - s);
                எனில் (!a)
                        திரும்பு -ENOMEM;
        }

        _cleanup_free_ எழுத்து *b = வெற்று;
        எனில் (ret_second) {
                b = strdup(x + strlen(sep));
                எனில் (!b)
                        திரும்பு -ENOMEM;
        }

        எனில் (ret_first)
                *ret_first = TAKE_PTR(a);
        எனில் (ret_second)
                *ret_second = TAKE_PTR(b);
        திரும்பு 0;
}

முழு free_and_strdup(எழுத்து **p, மாறிலி எழுத்து *s) {
        எழுத்து *t;

        ப்ரதிஜ்ஞா(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        எனில் (streq_ptr(*p, s))
                திரும்பு 0;

        எனில் (s) {
                t = strdup(s);
                எனில் (!t)
                        திரும்பு -ENOMEM;
        } இல்லையெனில்
                t = வெற்று;

        free_and_replace(*p, t);

        திரும்பு 1;
}

முழு free_and_strdup_warn(எழுத்து **p, மாறிலி எழுத்து *s) {
        முழு r;

        r = free_and_strdup(p, s);
        எனில் (r < 0)
                திரும்பு log_oom();
        திரும்பு r;
}

முழு free_and_strndup(எழுத்து **p, மாறிலி எழுத்து *s, size_t l) {
        எழுத்து *t;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        எனில் (!*p && !s)
                திரும்பு 0;

        எனில் (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                திரும்பு 0;

        எனில் (s) {
                t = strndup(s, l);
                எனில் (!t)
                        திரும்பு -ENOMEM;
        } இல்லையெனில்
                t = வெற்று;

        free_and_replace(*p, t);
        திரும்பு 1;
}

முழு strdup_to_full(எழுத்து **ret, மாறிலி எழுத்து *src) {
        எனில் (!src) {
                எனில் (ret)
                        *ret = வெற்று;

                திரும்பு 0;
        } இல்லையெனில் {
                எனில் (ret) {
                        எழுத்து *t = strdup(src);
                        எனில் (!t)
                                திரும்பு -ENOMEM;
                        *ret = t;
                }

                திரும்பு 1;
        }
};

bool string_is_safe(மாறிலி எழுத்து *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        எனில் (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                திரும்பு false;

        எனில் (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                திரும்பு false;

        க்கு (மாறிலி எழுத்து *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                எனில் ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        திரும்பு false;

                எனில் (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        திரும்பு false;

                எனில் (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        திரும்பு false;

                எனில் (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        திரும்பு false;

                எனில் (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        திரும்பு false;

                எனில் (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        திரும்பு false;

                எனில் (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        திரும்பு false;
        }

        எனில் (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                திரும்பு false;

        எனில் (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                திரும்பு false;

        திரும்பு true;
}

எழுத்து* str_realloc(எழுத்து *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        எனில் (!p)
                திரும்பு வெற்று;

        திரும்பு realloc(p, strlen(p) + 1) ?: p;
}

எழுத்து* string_erase(எழுத்து *x) {
        எனில் (!x)
                திரும்பு வெற்று;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        திரும்பு x;
}

முழு string_truncate_வரிகள்(மாறிலி எழுத்து *s, size_t n_வரிகள், எழுத்து **ret) {
        மாறிலி எழுத்து *p = s, *e = s;
        bool truncation_applied = false;
        எழுத்து *copy;
        size_t n = 0;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        க்கு (;;) {
                size_t k;

                k = strcspn(p, "\n");

                எனில் (p[k] == 0) {
                        எனில் (k == 0) /* final empty line */
                                நிறுத்து;

                        எனில் (n >= n_வரிகள்) /* above threshold */
                                நிறுத்து;

                        e = p + k; /* last line to include */
                        நிறுத்து;
                }

                ப்ரதிஜ்ஞா(p[k] == '\n');

                எனில் (n >= n_வரிகள்)
                        நிறுத்து;

                எனில் (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        எனில் (isempty(e))
                copy = strdup(s);
        இல்லையெனில் {
                எனில் (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        எனில் (!copy)
                திரும்பு -ENOMEM;

        *ret = copy;
        திரும்பு truncation_applied;
}

முழு string_extract_வரி(மாறிலி எழுத்து *s, size_t i, எழுத்து **ret) {
        மாறிலி எழுத்து *p = s;
        size_t c = 0;

        ப்ரதிஜ்ஞா(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        க்கு (;;) {
                மாறிலி எழுத்து *q;

                q = strchr(p, '\n');
                எனில் (i == c) {
                        /* The line we are looking for! */

                        எனில் (q) {
                                எழுத்து *m;

                                m = strndup(p, q - p);
                                எனில் (!m)
                                        திரும்பு -ENOMEM;

                                *ret = m;
                                திரும்பு !isempty(q + 1); /* More coming? */
                        } இல்லையெனில்
                                /* Tell the caller to use the input string if equal */
                                திரும்பு strdup_to(ret, p != s ? p : வெற்று);
                }

                எனில் (!q)
                        /* No more lines, return empty line */
                        திரும்பு strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

முழு string_contains_word_strv(மாறிலி எழுத்து *string, மாறிலி எழுத்து *separators, எழுத்து * மாறிலி *சொற்கள், மாறிலி எழுத்து **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        மாறிலி ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        மாறிலி எழுத்து *found = வெற்று;
        முழு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *w = வெற்று;

                r = extract_first_word(&string, &w, separators, flags);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                found = strv_find(சொற்கள், w);
                எனில் (found)
                        நிறுத்து;
        }

        எனில் (ret_word)
                *ret_word = found;
        திரும்பு !!found;
}

bool streq_skip_trailing_chars(மாறிலி எழுத்து *s1, மாறிலி எழுத்து *s2, மாறிலி எழுத்து *ok) {
        எனில் (!s1 && !s2)
                திரும்பு true;
        எனில் (!s1 || !s2)
                திரும்பு false;

        எனில் (!ok)
                ok = WHITESPACE;

        க்கு (; *s1 && *s2; s1++, s2++)
                எனில் (*s1 != *s2)
                        நிறுத்து;

        திரும்பு in_charset(s1, ok) && in_charset(s2, ok);
}

எழுத்து* string_replace_char(எழுத்து *str, எழுத்து old_char, எழுத்து new_char) {
        ப்ரதிஜ்ஞா(str);
        ப்ரதிஜ்ஞா(old_char != '\0');
        ப்ரதிஜ்ஞா(new_char != '\0');
        ப்ரதிஜ்ஞா(old_char != new_char);

        க்கு (எழுத்து *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        திரும்பு str;
}

முழு make_cstring(மாறிலி வெறுமை *s, size_t n, MakeCStringMode mode, எழுத்து **ret) {
        எழுத்து *b;

        ப்ரதிஜ்ஞா(s || n == 0);
        ப்ரதிஜ்ஞா(mode >= 0);
        ப்ரதிஜ்ஞா(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        எனில் (n == 0) {
                எனில் (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        திரும்பு -EINVAL;

                எனில் (!ret)
                        திரும்பு 0;

                b = new0(எழுத்து, 1);
        } இல்லையெனில் {
                மாறிலி uint8_t *nul;

                nul = memchr(s, 0, n);
                எனில் (nul) {
                        எனில் (nul < (மாறிலி uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                திரும்பு -EINVAL;

                        n--;
                } இல்லையெனில் எனில் (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        திரும்பு -EINVAL;

                எனில் (!ret)
                        திரும்பு 0;

                b = memdup_suffix0(s, n);
        }
        எனில் (!b)
                திரும்பு -ENOMEM;

        *ret = b;
        திரும்பு 0;
}

size_t strspn_from_end(மாறிலி எழுத்து *str, மாறிலி எழுத்து *accept) {
        size_t n = 0;

        எனில் (isempty(str))
                திரும்பு 0;

        எனில் (isempty(accept))
                திரும்பு 0;

        க்கு (மாறிலி எழுத்து *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        திரும்பு n;
}

size_t strnspn(மாறிலி எழுத்து *str, மாறிலி எழுத்து *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        ப்ரதிஜ்ஞா(str || n == 0);
        ப்ரதிஜ்ஞா(accept);

        க்கு (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        திரும்பு i;
}

எழுத்து* strdupspn(மாறிலி எழுத்து *a, மாறிலி எழுத்து *accept) {
        எனில் (isempty(a) || isempty(accept))
                திரும்பு strdup("");

        திரும்பு strndup(a, strspn(a, accept));
}

எழுத்து* strdupcspn(மாறிலி எழுத்து *a, மாறிலி எழுத்து *reject) {
        எனில் (isempty(a))
                திரும்பு strdup("");
        எனில் (isempty(reject))
                திரும்பு strdup(a);

        திரும்பு strndup(a, strcspn(a, reject));
}

எழுத்து* find_வரி_startswith_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        ப்ரதிஜ்ஞா(haystack);
        ப்ரதிஜ்ஞா(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        எழுத்து *p = (எழுத்து*) strstr(haystack, needle);
        எனில் (!p)
                திரும்பு வெற்று;

        எனில் (p > haystack)
                போது (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        எனில் (!p)
                                திரும்பு வெற்று;
                }

        திரும்பு p + strlen(needle);
}

எழுத்து* find_வரி_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        ப்ரதிஜ்ஞா(haystack);
        ப்ரதிஜ்ஞா(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        எழுத்து *p = (எழுத்து*) find_வரி_startswith(haystack, needle);
        எனில் (!p)
                திரும்பு வெற்று;

        எனில் (*p == 0 || strchr(NEWLINE, *p))
                திரும்பு p - strlen(needle);

        திரும்பு வெற்று;
}

எழுத்து* find_வரி_after_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        ப்ரதிஜ்ஞா(haystack);
        ப்ரதிஜ்ஞா(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        எழுத்து *p = (எழுத்து*) find_வரி_startswith(haystack, needle);
        எனில் (!p)
                திரும்பு வெற்று;

        எனில் (*p == 0)
                திரும்பு p;
        எனில் (strchr(NEWLINE, *p))
                திரும்பு p + 1;

        திரும்பு வெற்று;
}

bool version_is_valid(மாறிலி எழுத்து *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        எனில் (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                திரும்பு false;

        எனில் (!filename_part_is_valid(s))
                திரும்பு false;

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

        எழுத்து charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        எனில் (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        எனில் (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        திரும்பு in_charset(s, charset);
}

ssize_t strlevenshtein(மாறிலி எழுத்து *x, மாறிலி எழுத்து *y) {
        _cleanup_free_ size_t *t0 = வெற்று, *t1 = வெற்று, *t2 = வெற்று;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        எனில் (streq_ptr(x, y))
                திரும்பு 0;

        xl = strlen_ptr(x);
        எனில் (xl > SSIZE_MAX)
                திரும்பு -E2BIG;

        yl = strlen_ptr(y);
        எனில் (yl > SSIZE_MAX)
                திரும்பு -E2BIG;

        எனில் (isempty(x))
                திரும்பு yl;
        எனில் (isempty(y))
                திரும்பு xl;

        t0 = new0(size_t, yl + 1);
        எனில் (!t0)
                திரும்பு -ENOMEM;
        t1 = new0(size_t, yl + 1);
        எனில் (!t1)
                திரும்பு -ENOMEM;
        t2 = new0(size_t, yl + 1);
        எனில் (!t2)
                திரும்பு -ENOMEM;

        க்கு (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        க்கு (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                க்கு (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        எனில் (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        எனில் (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        எனில் (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        திரும்பு t1[yl];
}

எழுத்து* strrstr_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        எனில் (!haystack || !needle)
                திரும்பு வெற்று;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        எனில் (needle[0] == 0)
                திரும்பு (எழுத்து*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        எனில் (needle[1] == 0)
                திரும்பு (எழுத்து*) strrchr(haystack, needle[0]);

        க்கு (மாறிலி எழுத்து *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                எனில் (!q)
                        திரும்பு (எழுத்து*) p;
        }
        திரும்பு வெற்று;
}

எழுத்து* strrstr_no_case_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        எனில் (!haystack || !needle)
                திரும்பு வெற்று;

        க்கு (மாறிலி எழுத்து *p = strchr(haystack, 0); p > haystack; p--)
                எனில் (startswith_no_case(p, needle))
                        திரும்பு (எழுத்து*) p;

        திரும்பு startswith_no_case(haystack, needle) ? (எழுத்து*) haystack : வெற்று;
}

size_t str_common_prefix(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        க்கு (size_t n = 0;; n++) {
                எழுத்து c = a[n];
                எனில் (c != b[n])
                        திரும்பு n;
                எனில் (c == 0)
                        திரும்பு SIZE_MAX;
        }
}
