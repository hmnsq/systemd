/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "ansi-seq.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "glyph-util.h"
#അന്തര്ഭാവ "gunicode.h"
#അന്തര്ഭാവ "locale-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "memstream-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "utf8.h"

അക്ഷരം* first_word(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *word) {
        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        എങ്കിൽ (isempty(word))
                തിരികെ (അക്ഷരം*) s;

        സ്ഥിരം അക്ഷരം *p = startswith(s, word);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;
        എങ്കിൽ (*p == '\0')
                തിരികെ (അക്ഷരം*) p;

        സ്ഥിരം അക്ഷരം *nw = skip_leading_chars(p, WHITESPACE);
        എങ്കിൽ (p == nw)
                തിരികെ ശൂന്യ;

        തിരികെ (അക്ഷരം*) nw;
}

അക്ഷരം* strextendn(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *s, size_t l) {
        പ്രതിജ്ഞാ(x);
        പ്രതിജ്ഞാ(s || l == 0);

        എങ്കിൽ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        എങ്കിൽ (l > 0 || !*x) {
                size_t q;
                അക്ഷരം *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                എങ്കിൽ (!m)
                        തിരികെ ശൂന്യ;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        തിരികെ *x;
}

അക്ഷരം* strstrip(അക്ഷരം *s) {
        എങ്കിൽ (!s)
                തിരികെ ശൂന്യ;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        തിരികെ delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

അക്ഷരം* delete_chars(അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad) {
        അക്ഷരം *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        എങ്കിൽ (!s)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (!bad)
                bad = WHITESPACE;

        വേണ്ടി (f = s, t = s; *f; f++) {
                എങ്കിൽ (strchr(bad, *f))
                        തുടരുക;

                *(t++) = *f;
        }

        *t = 0;

        തിരികെ s;
}

അക്ഷരം* delete_trailing_chars(അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad) {
        അക്ഷരം *c = s;

        /* Drops all specified bad characters, at the end of the string */

        എങ്കിൽ (!s)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (!bad)
                bad = WHITESPACE;

        വേണ്ടി (അക്ഷരം *p = s; *p; p++)
                എങ്കിൽ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        തിരികെ s;
}

അക്ഷരം* truncate_nl_full(അക്ഷരം *s, size_t *ret_len) {
        size_t n;

        പ്രതിജ്ഞാ(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        എങ്കിൽ (ret_len)
                *ret_len = n;
        തിരികെ s;
}

അക്ഷരം ascii_tolower(അക്ഷരം x) {

        എങ്കിൽ (x >= 'A' && x <= 'Z')
                തിരികെ x - 'A' + 'a';

        തിരികെ x;
}

അക്ഷരം ascii_toupper(അക്ഷരം x) {

        എങ്കിൽ (x >= 'a' && x <= 'z')
                തിരികെ x - 'a' + 'A';

        തിരികെ x;
}

അക്ഷരം* ascii_strlower(അക്ഷരം *s) {
        പ്രതിജ്ഞാ(s);

        വേണ്ടി (അക്ഷരം *p = s; *p; p++)
                *p = ascii_tolower(*p);

        തിരികെ s;
}

അക്ഷരം* ascii_strupper(അക്ഷരം *s) {
        പ്രതിജ്ഞാ(s);

        വേണ്ടി (അക്ഷരം *p = s; *p; p++)
                *p = ascii_toupper(*p);

        തിരികെ s;
}

അക്ഷരം* ascii_strlower_n(അക്ഷരം *s, size_t n) {
        പ്രതിജ്ഞാ(n <= 0 || s);

        എങ്കിൽ (n <= 0)
                തിരികെ s;

        വേണ്ടി (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        തിരികെ s;
}

പൂർണ്ണസംഖ്യ ascii_strcasecmp_n(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b, size_t n) {

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        വേണ്ടി (; n > 0; a++, b++, n--) {
                പൂർണ്ണസംഖ്യ x, y;

                x = (പൂർണ്ണസംഖ്യ) (uint8_t) ascii_tolower(*a);
                y = (പൂർണ്ണസംഖ്യ) (uint8_t) ascii_tolower(*b);

                എങ്കിൽ (x != y)
                        തിരികെ x - y;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ ascii_strcasecmp_nn(സ്ഥിരം അക്ഷരം *a, size_t n, സ്ഥിരം അക്ഷരം *b, size_t m) {
        പൂർണ്ണസംഖ്യ r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        എങ്കിൽ (r != 0)
                തിരികെ r;

        തിരികെ CMP(n, m);
}

bool chars_intersect(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        /* Returns true if any of the chars in a are in b. */
        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = a; *p; p++)
                എങ്കിൽ (strchr(b, *p))
                        തിരികെ true;

        തിരികെ false;
}

bool string_has_cc(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *ok) {
        പ്രതിജ്ഞാ(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        വേണ്ടി (സ്ഥിരം അക്ഷരം *t = p; *t; t++) {
                എങ്കിൽ (ok && strchr(ok, *t))
                        തുടരുക;

                എങ്കിൽ (char_is_cc(*t))
                        തിരികെ true;
        }

        തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_ellipsis(അക്ഷരം *buf, bool unicode) {
        സ്ഥിരം അക്ഷരം *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        പ്രതിജ്ഞാ(strlen(s) == 3);
        memcpy(buf, s, 3);
        തിരികെ 3;
}

സ്ഥിതം size_t ansi_sequence_length(സ്ഥിരം അക്ഷരം *s, size_t len) {
        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (len < 2)
                തിരികെ 0;

        എങ്കിൽ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                തിരികെ 0;  /* Not the start of a sequence */

        എങ്കിൽ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                എങ്കിൽ (i == len)
                        തിരികെ 0;

                ആകുമ്പോൾ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        എങ്കിൽ (++i == len)
                                തിരികെ 0;
                ആകുമ്പോൾ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        എങ്കിൽ (++i == len)
                                തിരികെ 0;
                എങ്കിൽ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        തിരികെ i + 1;
                തിരികെ 0;  /* Bad sequence */

        } അല്ലെങ്കിൽ എങ്കിൽ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                തിരികെ 2;

        തിരികെ 0;  /* Bad escape? */
}

സ്ഥിതം bool string_has_ansi_sequence(സ്ഥിരം അക്ഷരം *s, size_t len) {
        സ്ഥിരം അക്ഷരം *t = s;

        ആകുമ്പോൾ ((t = memchr(t, 0x1B, len - (t - s)))) {
                എങ്കിൽ (ansi_sequence_length(t, len - (t - s)) > 0)
                        തിരികെ true;
                t++;
        }
        തിരികെ false;
}

സ്ഥിതം size_t previous_ansi_sequence(സ്ഥിരം അക്ഷരം *s, size_t length, സ്ഥിരം അക്ഷരം **ret_where) {

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        എങ്കിൽ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ശൂന്യ;
                തിരികെ 0;
        }

        വേണ്ടി (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                എങ്കിൽ (slen > 0) {
                        *ret_where = s + i;
                        തിരികെ slen;
                }

                എങ്കിൽ (i == 0)
                        നിർത്തുക;
        }

        *ret_where = ശൂന്യ;
        തിരികെ 0;
}

സ്ഥിതം അക്ഷരം *ascii_ellipsize_mem(സ്ഥിരം അക്ഷരം *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        അക്ഷരം *t;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(percent <= 100);
        പ്രതിജ്ഞാ(new_length != SIZE_MAX);

        എങ്കിൽ (old_length <= new_length)
                തിരികെ strndup(s, old_length);

        /* Special case short ellipsations */
        തിരഞ്ഞെടുപ്പ് (new_length) {

        അവസ്ഥ 0:
                തിരികെ strdup("");

        അവസ്ഥ 1:
                എങ്കിൽ (is_locale_utf8())
                        തിരികെ strdup("…");
                അല്ലെങ്കിൽ
                        തിരികെ strdup(".");

        അവസ്ഥ 2:
                എങ്കിൽ (!is_locale_utf8())
                        തിരികെ strdup("..");
                നിർത്തുക;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(അക്ഷരം, new_length+3);
        എങ്കിൽ (!t)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        പ്രതിജ്ഞാ(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        തിരികെ t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
സ്ഥിതം സ്ഥിരം അക്ഷരം* find_previous_unichar(സ്ഥിരം അക്ഷരം *start, സ്ഥിരം അക്ഷരം *end, char32_t *ret_c) {
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(start);
        പ്രതിജ്ഞാ(end);
        പ്രതിജ്ഞാ(end > start);

        വേണ്ടി (p = end; p > start; ) {
                p--;
                എങ്കിൽ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        നിർത്തുക;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        എങ്കിൽ (r < 0 || p + r != end)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        തിരികെ p;
}

അക്ഷരം* ellipsize_mem(സ്ഥിരം അക്ഷരം *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        സ്ഥിരം അക്ഷരം *i, *j;
        പൂർണ്ണസംഖ്യ r;

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

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(percent <= 100);

        എങ്കിൽ (new_length == SIZE_MAX)
                തിരികെ strndup(s, old_length);

        എങ്കിൽ (new_length == 0)
                തിരികെ strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        എങ്കിൽ (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                തിരികെ ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        പ്രതിജ്ഞാ(x <= new_length - 1);

        k = 0;
        വേണ്ടി (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                എങ്കിൽ (slen > 0) {
                        i += slen;
                        തുടരുക;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                എങ്കിൽ (r < 0)
                        തിരികെ ശൂന്യ;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                പൂർണ്ണസംഖ്യ w = unichar_console_width(c);
                എങ്കിൽ (k + w > x)
                        നിർത്തുക;

                k += w;
                i += r;
        }

        സ്ഥിരം അക്ഷരം *ansi_start = s + old_length;
        size_t ansi_len = 0;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                പൂർണ്ണസംഖ്യ w;
                സ്ഥിരം അക്ഷരം *tt;

                എങ്കിൽ (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                എങ്കിൽ (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        തുടരുക;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                എങ്കിൽ (!tt)
                        തിരികെ ശൂന്യ;

                w = unichar_console_width(c);
                എങ്കിൽ (k + w > new_length)
                        നിർത്തുക;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        എങ്കിൽ (i >= j)
                തിരികെ memdup_suffix0(s, old_length);

        എങ്കിൽ (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                എങ്കിൽ (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        എങ്കിൽ (r < 0)
                                തിരികെ ശൂന്യ;

                        j += r;
                } അല്ലെങ്കിൽ എങ്കിൽ (i > s) {
                        സ്ഥിരം അക്ഷരം *tt = find_previous_unichar(s, i, ശൂന്യ);
                        എങ്കിൽ (!tt)
                                തിരികെ ശൂന്യ;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        അക്ഷരം *e = new(അക്ഷരം, alloc_len);
        എങ്കിൽ (!e)
                തിരികെ ശൂന്യ;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        അക്ഷരം *dst = e + len + 3;

        എങ്കിൽ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                വേണ്ടി (സ്ഥിരം അക്ഷരം *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        എങ്കിൽ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } അല്ലെങ്കിൽ
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        തിരികെ e;
}

അക്ഷരം* cellescape(അക്ഷരം *buf, size_t len, സ്ഥിരം അക്ഷരം *s) {
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

        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(len > 0); /* at least a terminating NUL */
        പ്രതിജ്ഞാ(s);

        വേണ്ടി (;;) {
                അക്ഷരം four[4];
                പൂർണ്ണസംഖ്യ w;

                എങ്കിൽ (*s == 0) /* terminating NUL detected? then we are done! */
                        പോകുക done;

                w = cescape_char(*s, four);
                എങ്കിൽ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        നിർത്തുക;

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
        വേണ്ടി (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                എങ്കിൽ (i + 4 <= len) /* nice, we reached our space goal */
                        നിർത്തുക;

                k = k == 0 ? 3 : k - 1;
                എങ്കിൽ (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        നിർത്തുക;

                പ്രതിജ്ഞാ(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        എങ്കിൽ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        അല്ലെങ്കിൽ എങ്കിൽ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } അല്ലെങ്കിൽ എങ്കിൽ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        അല്ലെങ്കിൽ
                പ്രതിജ്ഞാ(i + 1 <= len);

done:
        buf[i] = '\0';
        തിരികെ buf;
}

അക്ഷരം* strshorten(അക്ഷരം *s, size_t l) {
        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (l >= SIZE_MAX-1) /* Would not change anything */
                തിരികെ s;

        എങ്കിൽ (strnlen(s, l+1) > l)
                s[l] = 0;

        തിരികെ s;
}

പൂർണ്ണസംഖ്യ strgrowpad0(അക്ഷരം **s, size_t l) {
        size_t sz;

        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (*s) {
                sz = strlen(*s) + 1;
                എങ്കിൽ (sz >= l) /* never shrink */
                        തിരികെ 0;
        } അല്ലെങ്കിൽ
                sz = 0;

        അക്ഷരം *q = realloc(*s, l);
        എങ്കിൽ (!q)
                തിരികെ -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        തിരികെ 0;
}

അക്ഷരം* strreplace(സ്ഥിരം അക്ഷരം *text, സ്ഥിരം അക്ഷരം *old_string, സ്ഥിരം അക്ഷരം *new_string) {
        size_t l, old_len, new_len;
        അക്ഷരം *t, *ret = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *f;

        പ്രതിജ്ഞാ(old_string);
        പ്രതിജ്ഞാ(new_string);

        എങ്കിൽ (!text)
                തിരികെ ശൂന്യ;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        എങ്കിൽ (!GREEDY_REALLOC(ret, l+1))
                തിരികെ ശൂന്യ;

        f = text;
        t = ret;
        ആകുമ്പോൾ (*f) {
                size_t d, nl;

                എങ്കിൽ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        തുടരുക;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                എങ്കിൽ (!GREEDY_REALLOC(ret, nl + 1))
                        തിരികെ mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        തിരികെ ret;
}

സ്ഥിതം ശൂന്യം advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[സ്ഥിതം 2],
                size_t size) {

        എങ്കിൽ (!offsets)
                തിരികെ;

        പ്രതിജ്ഞാ(shift);

        എങ്കിൽ ((size_t) diff < offsets[0])
                shift[0] += size;
        എങ്കിൽ ((size_t) diff < offsets[1])
                shift[1] += size;
}

സ്ഥിതം ശൂന്യം retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[സ്ഥിതം 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        എങ്കിൽ (!offsets)
                തിരികെ;

        പ്രതിജ്ഞാ(shift);
        പ്രതിജ്ഞാ(begin <= end);

        വേണ്ടി (size_t k = 0; k < 2; k++)
                എങ്കിൽ (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

അക്ഷരം* strip_tab_ansi(അക്ഷരം **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        സ്ഥിരം അക്ഷരം *begin = ശൂന്യ;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        പ്രതിജ്ഞാ(ibuf);
        പ്രതിജ്ഞാ(*ibuf);
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
        എങ്കിൽ (!f)
                തിരികെ ശൂന്യ;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *i = *ibuf; i < *ibuf + isz; i++) {
                പൂർണ്ണസംഖ്യ e;

                വേണ്ടി (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        എങ്കിൽ (e < 0)
                                തിരികെ ശൂന്യ;
                        എങ്കിൽ (e != ANSI_SEQ_EVENT_ABORT)
                                നിർത്തുക;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        എങ്കിൽ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = ശൂന്യ;
                        }
                }

                തിരഞ്ഞെടുപ്പ് (e) {

                അവസ്ഥ ANSI_SEQ_EVENT_TEXT:
                        എങ്കിൽ (*i == '\r') {
                                n_carriage_returns++;
                                തുടരുക;
                        }
                        എങ്കിൽ (*i == '\n') {
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
                        വേണ്ടി (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        എങ്കിൽ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } അല്ലെങ്കിൽ
                                fputc(*i, f);

                        നിർത്തുക;

                അവസ്ഥ ANSI_SEQ_EVENT_SEQUENCE:
                        എങ്കിൽ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                വേണ്ടി (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        നിർത്തുക;

                അവസ്ഥ ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        പ്രതിജ്ഞാ(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = ശൂന്യ;
                        നിർത്തുക;

                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        എങ്കിൽ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        അക്ഷരം *obuf;
        എങ്കിൽ (memstream_finalize(&m, &obuf, _isz) < 0)
                തിരികെ ശൂന്യ;

        free_and_replace(*ibuf, obuf);

        എങ്കിൽ (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        തിരികെ *ibuf;
}

അക്ഷരം* strextendv_with_separator(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *separator, va_list ap) {
        _cleanup_free_ അക്ഷരം *buffer = ശൂന്യ;
        size_t f, l, l_separator;
        bool need_separator;
        അക്ഷരം *nr, *p;

        എങ്കിൽ (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        വേണ്ടി (സ്ഥിരം അക്ഷരം *t;;) {
                size_t n;

                t = va_arg(aq, സ്ഥിരം അക്ഷരം *);
                എങ്കിൽ (!t)
                        നിർത്തുക;
                എങ്കിൽ (t == POINTER_MAX)
                        തുടരുക;

                n = strlen(t);

                എങ്കിൽ (need_separator)
                        n += l_separator;

                എങ്കിൽ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        തിരികെ ശൂന്യ;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        എങ്കിൽ (!nr)
                തിരികെ ശൂന്യ;

        *x = nr;
        p = nr + f;

        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *t;

                t = va_arg(ap, സ്ഥിരം അക്ഷരം *);
                എങ്കിൽ (!t)
                        നിർത്തുക;
                എങ്കിൽ (t == POINTER_MAX)
                        തുടരുക;

                എങ്കിൽ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        പ്രതിജ്ഞാ(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        എങ്കിൽ (buffer)
                തിരികെ TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        തിരികെ p;
}

അക്ഷരം* strextend_with_separator_internal(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *separator, ...) {
        va_list ap;
        അക്ഷരം *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ strextendf_with_separator(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *separator, സ്ഥിരം അക്ഷരം *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        പൂർണ്ണസംഖ്യ l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        പ്രതിജ്ഞാ(x);
        പ്രതിജ്ഞാ(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        എങ്കിൽ (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                പ്രതിജ്ഞാ(a >= m + 1);
        } അല്ലെങ്കിൽ
                m = a = 0;

        എങ്കിൽ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                അക്ഷരം *n;

                എങ്കിൽ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        തിരികെ -ENOMEM;
                എങ്കിൽ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        തിരികെ -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                എങ്കിൽ (!n)
                        തിരികെ -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        പ്രതിജ്ഞാ(l >= 0);

        എങ്കിൽ ((size_t) l < a - m - l_separator) {
                അക്ഷരം *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                എങ്കിൽ (n)
                        *x = n;
        } അല്ലെങ്കിൽ {
                അക്ഷരം *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                എങ്കിൽ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        പോകുക oom;
                എങ്കിൽ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        പോകുക oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                എങ്കിൽ (!n)
                        പോകുക oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                പ്രതിജ്ഞാ((size_t) l < a - m - l_separator);
        }

        തിരികെ 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        തിരികെ -ENOMEM;
}

അക്ഷരം* strrep(സ്ഥിരം അക്ഷരം *s, size_t n) {
        അക്ഷരം *ret, *p;
        size_t l;

        പ്രതിജ്ഞാ(s);

        l = strlen(s);
        എങ്കിൽ (!MUL_ASSIGN_SAFE(&l, n))
                തിരികെ ശൂന്യ;
        എങ്കിൽ (!INC_SAFE(&l, 1))
                തിരികെ ശൂന്യ;

        p = ret = malloc(l);
        എങ്കിൽ (!ret)
                തിരികെ ശൂന്യ;

        വേണ്ടി (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ split_pair(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *sep, അക്ഷരം **ret_first, അക്ഷരം **ret_second) {
        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(!isempty(sep));

        സ്ഥിരം അക്ഷരം *x = strstr(s, sep);
        എങ്കിൽ (!x)
                തിരികെ -EINVAL;

        _cleanup_free_ അക്ഷരം *a = ശൂന്യ;
        എങ്കിൽ (ret_first) {
                a = strndup(s, x - s);
                എങ്കിൽ (!a)
                        തിരികെ -ENOMEM;
        }

        _cleanup_free_ അക്ഷരം *b = ശൂന്യ;
        എങ്കിൽ (ret_second) {
                b = strdup(x + strlen(sep));
                എങ്കിൽ (!b)
                        തിരികെ -ENOMEM;
        }

        എങ്കിൽ (ret_first)
                *ret_first = TAKE_PTR(a);
        എങ്കിൽ (ret_second)
                *ret_second = TAKE_PTR(b);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ free_and_strdup(അക്ഷരം **p, സ്ഥിരം അക്ഷരം *s) {
        അക്ഷരം *t;

        പ്രതിജ്ഞാ(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        എങ്കിൽ (streq_ptr(*p, s))
                തിരികെ 0;

        എങ്കിൽ (s) {
                t = strdup(s);
                എങ്കിൽ (!t)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ
                t = ശൂന്യ;

        free_and_replace(*p, t);

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ free_and_strdup_warn(അക്ഷരം **p, സ്ഥിരം അക്ഷരം *s) {
        പൂർണ്ണസംഖ്യ r;

        r = free_and_strdup(p, s);
        എങ്കിൽ (r < 0)
                തിരികെ log_oom();
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ free_and_strndup(അക്ഷരം **p, സ്ഥിരം അക്ഷരം *s, size_t l) {
        അക്ഷരം *t;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        എങ്കിൽ (!*p && !s)
                തിരികെ 0;

        എങ്കിൽ (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                തിരികെ 0;

        എങ്കിൽ (s) {
                t = strndup(s, l);
                എങ്കിൽ (!t)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ
                t = ശൂന്യ;

        free_and_replace(*p, t);
        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ strdup_to_full(അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *src) {
        എങ്കിൽ (!src) {
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;

                തിരികെ 0;
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (ret) {
                        അക്ഷരം *t = strdup(src);
                        എങ്കിൽ (!t)
                                തിരികെ -ENOMEM;
                        *ret = t;
                }

                തിരികെ 1;
        }
};

bool string_is_safe(സ്ഥിരം അക്ഷരം *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        എങ്കിൽ (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                തിരികെ false;

        എങ്കിൽ (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                തിരികെ false;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                എങ്കിൽ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        തിരികെ false;

                എങ്കിൽ (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        തിരികെ false;

                എങ്കിൽ (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        തിരികെ false;

                എങ്കിൽ (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        തിരികെ false;

                എങ്കിൽ (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        തിരികെ false;

                എങ്കിൽ (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        തിരികെ false;

                എങ്കിൽ (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        തിരികെ false;
        }

        എങ്കിൽ (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                തിരികെ false;

        എങ്കിൽ (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                തിരികെ false;

        തിരികെ true;
}

അക്ഷരം* str_realloc(അക്ഷരം *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        തിരികെ realloc(p, strlen(p) + 1) ?: p;
}

അക്ഷരം* string_erase(അക്ഷരം *x) {
        എങ്കിൽ (!x)
                തിരികെ ശൂന്യ;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        തിരികെ x;
}

പൂർണ്ണസംഖ്യ string_truncate_വരികൾ(സ്ഥിരം അക്ഷരം *s, size_t n_വരികൾ, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p = s, *e = s;
        bool truncation_applied = false;
        അക്ഷരം *copy;
        size_t n = 0;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        വേണ്ടി (;;) {
                size_t k;

                k = strcspn(p, "\n");

                എങ്കിൽ (p[k] == 0) {
                        എങ്കിൽ (k == 0) /* final empty line */
                                നിർത്തുക;

                        എങ്കിൽ (n >= n_വരികൾ) /* above threshold */
                                നിർത്തുക;

                        e = p + k; /* last line to include */
                        നിർത്തുക;
                }

                പ്രതിജ്ഞാ(p[k] == '\n');

                എങ്കിൽ (n >= n_വരികൾ)
                        നിർത്തുക;

                എങ്കിൽ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        എങ്കിൽ (isempty(e))
                copy = strdup(s);
        അല്ലെങ്കിൽ {
                എങ്കിൽ (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        എങ്കിൽ (!copy)
                തിരികെ -ENOMEM;

        *ret = copy;
        തിരികെ truncation_applied;
}

പൂർണ്ണസംഖ്യ string_extract_വരി(സ്ഥിരം അക്ഷരം *s, size_t i, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p = s;
        size_t c = 0;

        പ്രതിജ്ഞാ(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *q;

                q = strchr(p, '\n');
                എങ്കിൽ (i == c) {
                        /* The line we are looking for! */

                        എങ്കിൽ (q) {
                                അക്ഷരം *m;

                                m = strndup(p, q - p);
                                എങ്കിൽ (!m)
                                        തിരികെ -ENOMEM;

                                *ret = m;
                                തിരികെ !isempty(q + 1); /* More coming? */
                        } അല്ലെങ്കിൽ
                                /* Tell the caller to use the input string if equal */
                                തിരികെ strdup_to(ret, p != s ? p : ശൂന്യ);
                }

                എങ്കിൽ (!q)
                        /* No more lines, return empty line */
                        തിരികെ strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

പൂർണ്ണസംഖ്യ string_contains_word_strv(സ്ഥിരം അക്ഷരം *string, സ്ഥിരം അക്ഷരം *separators, അക്ഷരം * സ്ഥിരം *വാക്കുകൾ, സ്ഥിരം അക്ഷരം **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        സ്ഥിരം ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        സ്ഥിരം അക്ഷരം *found = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *w = ശൂന്യ;

                r = extract_first_word(&string, &w, separators, flags);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                found = strv_find(വാക്കുകൾ, w);
                എങ്കിൽ (found)
                        നിർത്തുക;
        }

        എങ്കിൽ (ret_word)
                *ret_word = found;
        തിരികെ !!found;
}

bool streq_skip_trailing_chars(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2, സ്ഥിരം അക്ഷരം *ok) {
        എങ്കിൽ (!s1 && !s2)
                തിരികെ true;
        എങ്കിൽ (!s1 || !s2)
                തിരികെ false;

        എങ്കിൽ (!ok)
                ok = WHITESPACE;

        വേണ്ടി (; *s1 && *s2; s1++, s2++)
                എങ്കിൽ (*s1 != *s2)
                        നിർത്തുക;

        തിരികെ in_charset(s1, ok) && in_charset(s2, ok);
}

അക്ഷരം* string_replace_char(അക്ഷരം *str, അക്ഷരം old_char, അക്ഷരം new_char) {
        പ്രതിജ്ഞാ(str);
        പ്രതിജ്ഞാ(old_char != '\0');
        പ്രതിജ്ഞാ(new_char != '\0');
        പ്രതിജ്ഞാ(old_char != new_char);

        വേണ്ടി (അക്ഷരം *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        തിരികെ str;
}

പൂർണ്ണസംഖ്യ make_cstring(സ്ഥിരം ശൂന്യം *s, size_t n, MakeCStringMode mode, അക്ഷരം **ret) {
        അക്ഷരം *b;

        പ്രതിജ്ഞാ(s || n == 0);
        പ്രതിജ്ഞാ(mode >= 0);
        പ്രതിജ്ഞാ(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        എങ്കിൽ (n == 0) {
                എങ്കിൽ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        തിരികെ -EINVAL;

                എങ്കിൽ (!ret)
                        തിരികെ 0;

                b = new0(അക്ഷരം, 1);
        } അല്ലെങ്കിൽ {
                സ്ഥിരം uint8_t *nul;

                nul = memchr(s, 0, n);
                എങ്കിൽ (nul) {
                        എങ്കിൽ (nul < (സ്ഥിരം uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                തിരികെ -EINVAL;

                        n--;
                } അല്ലെങ്കിൽ എങ്കിൽ (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        തിരികെ -EINVAL;

                എങ്കിൽ (!ret)
                        തിരികെ 0;

                b = memdup_suffix0(s, n);
        }
        എങ്കിൽ (!b)
                തിരികെ -ENOMEM;

        *ret = b;
        തിരികെ 0;
}

size_t strspn_from_end(സ്ഥിരം അക്ഷരം *str, സ്ഥിരം അക്ഷരം *accept) {
        size_t n = 0;

        എങ്കിൽ (isempty(str))
                തിരികെ 0;

        എങ്കിൽ (isempty(accept))
                തിരികെ 0;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        തിരികെ n;
}

size_t strnspn(സ്ഥിരം അക്ഷരം *str, സ്ഥിരം അക്ഷരം *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        പ്രതിജ്ഞാ(str || n == 0);
        പ്രതിജ്ഞാ(accept);

        വേണ്ടി (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        തിരികെ i;
}

അക്ഷരം* strdupspn(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *accept) {
        എങ്കിൽ (isempty(a) || isempty(accept))
                തിരികെ strdup("");

        തിരികെ strndup(a, strspn(a, accept));
}

അക്ഷരം* strdupcspn(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *reject) {
        എങ്കിൽ (isempty(a))
                തിരികെ strdup("");
        എങ്കിൽ (isempty(reject))
                തിരികെ strdup(a);

        തിരികെ strndup(a, strcspn(a, reject));
}

അക്ഷരം* find_വരി_startswith_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        പ്രതിജ്ഞാ(haystack);
        പ്രതിജ്ഞാ(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        അക്ഷരം *p = (അക്ഷരം*) strstr(haystack, needle);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (p > haystack)
                ആകുമ്പോൾ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        എങ്കിൽ (!p)
                                തിരികെ ശൂന്യ;
                }

        തിരികെ p + strlen(needle);
}

അക്ഷരം* find_വരി_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        പ്രതിജ്ഞാ(haystack);
        പ്രതിജ്ഞാ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        അക്ഷരം *p = (അക്ഷരം*) find_വരി_startswith(haystack, needle);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (*p == 0 || strchr(NEWLINE, *p))
                തിരികെ p - strlen(needle);

        തിരികെ ശൂന്യ;
}

അക്ഷരം* find_വരി_after_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        പ്രതിജ്ഞാ(haystack);
        പ്രതിജ്ഞാ(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        അക്ഷരം *p = (അക്ഷരം*) find_വരി_startswith(haystack, needle);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (*p == 0)
                തിരികെ p;
        എങ്കിൽ (strchr(NEWLINE, *p))
                തിരികെ p + 1;

        തിരികെ ശൂന്യ;
}

bool version_is_valid(സ്ഥിരം അക്ഷരം *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        എങ്കിൽ (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                തിരികെ false;

        എങ്കിൽ (!filename_part_is_valid(s))
                തിരികെ false;

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

        അക്ഷരം charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        എങ്കിൽ (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        എങ്കിൽ (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        തിരികെ in_charset(s, charset);
}

ssize_t strlevenshtein(സ്ഥിരം അക്ഷരം *x, സ്ഥിരം അക്ഷരം *y) {
        _cleanup_free_ size_t *t0 = ശൂന്യ, *t1 = ശൂന്യ, *t2 = ശൂന്യ;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        എങ്കിൽ (streq_ptr(x, y))
                തിരികെ 0;

        xl = strlen_ptr(x);
        എങ്കിൽ (xl > SSIZE_MAX)
                തിരികെ -E2BIG;

        yl = strlen_ptr(y);
        എങ്കിൽ (yl > SSIZE_MAX)
                തിരികെ -E2BIG;

        എങ്കിൽ (isempty(x))
                തിരികെ yl;
        എങ്കിൽ (isempty(y))
                തിരികെ xl;

        t0 = new0(size_t, yl + 1);
        എങ്കിൽ (!t0)
                തിരികെ -ENOMEM;
        t1 = new0(size_t, yl + 1);
        എങ്കിൽ (!t1)
                തിരികെ -ENOMEM;
        t2 = new0(size_t, yl + 1);
        എങ്കിൽ (!t2)
                തിരികെ -ENOMEM;

        വേണ്ടി (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        വേണ്ടി (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                വേണ്ടി (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        എങ്കിൽ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        എങ്കിൽ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        എങ്കിൽ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        തിരികെ t1[yl];
}

അക്ഷരം* strrstr_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        എങ്കിൽ (!haystack || !needle)
                തിരികെ ശൂന്യ;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        എങ്കിൽ (needle[0] == 0)
                തിരികെ (അക്ഷരം*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        എങ്കിൽ (needle[1] == 0)
                തിരികെ (അക്ഷരം*) strrchr(haystack, needle[0]);

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                എങ്കിൽ (!q)
                        തിരികെ (അക്ഷരം*) p;
        }
        തിരികെ ശൂന്യ;
}

അക്ഷരം* strrstr_no_case_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        എങ്കിൽ (!haystack || !needle)
                തിരികെ ശൂന്യ;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = strchr(haystack, 0); p > haystack; p--)
                എങ്കിൽ (startswith_no_case(p, needle))
                        തിരികെ (അക്ഷരം*) p;

        തിരികെ startswith_no_case(haystack, needle) ? (അക്ഷരം*) haystack : ശൂന്യ;
}

size_t str_common_prefix(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        വേണ്ടി (size_t n = 0;; n++) {
                അക്ഷരം c = a[n];
                എങ്കിൽ (c != b[n])
                        തിരികെ n;
                എങ്കിൽ (c == 0)
                        തിരികെ SIZE_MAX;
        }
}
