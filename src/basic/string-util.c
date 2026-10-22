/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "ansi-seq.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "glyph-util.h"
#అన్తర్భావ "gunicode.h"
#అన్తర్భావ "locale-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "memstream-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "utf8.h"

అక్షరం* మొదటి_మాట(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *మాట) {
        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(మాట);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ఒకవేళ (isempty(మాట))
                తిరిగి (అక్షరం*) s;

        స్థిరరాశి అక్షరం *p = startswith(s, మాట);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;
        ఒకవేళ (*p == '\0')
                తిరిగి (అక్షరం*) p;

        స్థిరరాశి అక్షరం *nw = skip_leading_chars(p, WHITESPACE);
        ఒకవేళ (p == nw)
                తిరిగి ఖాళీ;

        తిరిగి (అక్షరం*) nw;
}

అక్షరం* strextendn(అక్షరం **x, స్థిరరాశి అక్షరం *s, size_t l) {
        ప్రతిజ్ఞా(x);
        ప్రతిజ్ఞా(s || l == 0);

        ఒకవేళ (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ఒకవేళ (l > 0 || !*x) {
                size_t q;
                అక్షరం *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ఒకవేళ (!m)
                        తిరిగి ఖాళీ;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        తిరిగి *x;
}

అక్షరం* strstrip(అక్షరం *s) {
        ఒకవేళ (!s)
                తిరిగి ఖాళీ;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        తిరిగి delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

అక్షరం* delete_chars(అక్షరం *s, స్థిరరాశి అక్షరం *bad) {
        అక్షరం *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ఒకవేళ (!s)
                తిరిగి ఖాళీ;

        ఒకవేళ (!bad)
                bad = WHITESPACE;

        కొరకు (f = s, t = s; *f; f++) {
                ఒకవేళ (strchr(bad, *f))
                        కొనసాగు;

                *(t++) = *f;
        }

        *t = 0;

        తిరిగి s;
}

అక్షరం* delete_trailing_chars(అక్షరం *s, స్థిరరాశి అక్షరం *bad) {
        అక్షరం *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ఒకవేళ (!s)
                తిరిగి ఖాళీ;

        ఒకవేళ (!bad)
                bad = WHITESPACE;

        కొరకు (అక్షరం *p = s; *p; p++)
                ఒకవేళ (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        తిరిగి s;
}

అక్షరం* truncate_nl_నిండిన(అక్షరం *s, size_t *ret_len) {
        size_t n;

        ప్రతిజ్ఞా(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ఒకవేళ (ret_len)
                *ret_len = n;
        తిరిగి s;
}

అక్షరం ascii_tolower(అక్షరం x) {

        ఒకవేళ (x >= 'A' && x <= 'Z')
                తిరిగి x - 'A' + 'a';

        తిరిగి x;
}

అక్షరం ascii_toupper(అక్షరం x) {

        ఒకవేళ (x >= 'a' && x <= 'z')
                తిరిగి x - 'a' + 'A';

        తిరిగి x;
}

అక్షరం* ascii_strlower(అక్షరం *s) {
        ప్రతిజ్ఞా(s);

        కొరకు (అక్షరం *p = s; *p; p++)
                *p = ascii_tolower(*p);

        తిరిగి s;
}

అక్షరం* ascii_strupper(అక్షరం *s) {
        ప్రతిజ్ఞా(s);

        కొరకు (అక్షరం *p = s; *p; p++)
                *p = ascii_toupper(*p);

        తిరిగి s;
}

అక్షరం* ascii_strlower_n(అక్షరం *s, size_t n) {
        ప్రతిజ్ఞా(n <= 0 || s);

        ఒకవేళ (n <= 0)
                తిరిగి s;

        కొరకు (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        తిరిగి s;
}

పూర్ణాంకం ascii_strcasecmp_n(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b, size_t n) {

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        కొరకు (; n > 0; a++, b++, n--) {
                పూర్ణాంకం x, y;

                x = (పూర్ణాంకం) (uint8_t) ascii_tolower(*a);
                y = (పూర్ణాంకం) (uint8_t) ascii_tolower(*b);

                ఒకవేళ (x != y)
                        తిరిగి x - y;
        }

        తిరిగి 0;
}

పూర్ణాంకం ascii_strcasecmp_nn(స్థిరరాశి అక్షరం *a, size_t n, స్థిరరాశి అక్షరం *b, size_t m) {
        పూర్ణాంకం r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ఒకవేళ (r != 0)
                తిరిగి r;

        తిరిగి CMP(n, m);
}

bool chars_intersect(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        /* Returns true if any of the chars in a are in b. */
        కొరకు (స్థిరరాశి అక్షరం *p = a; *p; p++)
                ఒకవేళ (strchr(b, *p))
                        తిరిగి true;

        తిరిగి false;
}

bool string_has_cc(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *ok) {
        ప్రతిజ్ఞా(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        కొరకు (స్థిరరాశి అక్షరం *t = p; *t; t++) {
                ఒకవేళ (ok && strchr(ok, *t))
                        కొనసాగు;

                ఒకవేళ (char_is_cc(*t))
                        తిరిగి true;
        }

        తిరిగి false;
}

స్థిర పూర్ణాంకం write_ellipsis(అక్షరం *buf, bool unicode) {
        స్థిరరాశి అక్షరం *s = glyph_నిండిన(GLYPH_ELLIPSIS, unicode);
        ప్రతిజ్ఞా(అక్షరశ్రేణి_పొడవు(s) == 3);
        జ్ఞాపకం_నకలు(buf, s, 3);
        తిరిగి 3;
}

స్థిర size_t ansi_sequence_నిడివి(స్థిరరాశి అక్షరం *s, size_t len) {
        ప్రతిజ్ఞా(s);

        ఒకవేళ (len < 2)
                తిరిగి 0;

        ఒకవేళ (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                తిరిగి 0;  /* Not the start of a sequence */

        ఒకవేళ (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ఒకవేళ (i == len)
                        తిరిగి 0;

                అయితే (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ఒకవేళ (++i == len)
                                తిరిగి 0;
                అయితే (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ఒకవేళ (++i == len)
                                తిరిగి 0;
                ఒకవేళ (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        తిరిగి i + 1;
                తిరిగి 0;  /* Bad sequence */

        } లేకపోతే ఒకవేళ (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                తిరిగి 2;

        తిరిగి 0;  /* Bad escape? */
}

స్థిర bool string_has_ansi_sequence(స్థిరరాశి అక్షరం *s, size_t len) {
        స్థిరరాశి అక్షరం *t = s;

        అయితే ((t = memchr(t, 0x1B, len - (t - s)))) {
                ఒకవేళ (ansi_sequence_నిడివి(t, len - (t - s)) > 0)
                        తిరిగి true;
                t++;
        }
        తిరిగి false;
}

స్థిర size_t మునుపటి_ansi_sequence(స్థిరరాశి అక్షరం *s, size_t length, స్థిరరాశి అక్షరం **ret_where) {

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ఒకవేళ (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = ఖాళీ;
                తిరిగి 0;
        }

        కొరకు (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_నిడివి(s + i, length - i);
                ఒకవేళ (slen > 0) {
                        *ret_where = s + i;
                        తిరిగి slen;
                }

                ఒకవేళ (i == 0)
                        ఆపు;
        }

        *ret_where = ఖాళీ;
        తిరిగి 0;
}

స్థిర అక్షరం *ascii_ellipsize_mem(స్థిరరాశి అక్షరం *s, size_t old_నిడివి, size_t new_నిడివి, గుర్తులేని percent) {
        size_t x, need_space, suffix_len;
        అక్షరం *t;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(percent <= 100);
        ప్రతిజ్ఞా(new_నిడివి != SIZE_MAX);

        ఒకవేళ (old_నిడివి <= new_నిడివి)
                తిరిగి strndup(s, old_నిడివి);

        /* Special case short ellipsations */
        ఎంపిక (new_నిడివి) {

        స్థితి 0:
                తిరిగి strdup("");

        స్థితి 1:
                ఒకవేళ (is_locale_utf8())
                        తిరిగి strdup("…");
                లేకపోతే
                        తిరిగి strdup(".");

        స్థితి 2:
                ఒకవేళ (!is_locale_utf8())
                        తిరిగి strdup("..");
                ఆపు;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(అక్షరం, new_నిడివి+3);
        ఒకవేళ (!t)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(new_నిడివి >= need_space);

        x = ((new_నిడివి - need_space) * percent + 50) / 100;
        ప్రతిజ్ఞా(x <= new_నిడివి - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_నిడివి - x - need_space;
        జ్ఞాపకం_నకలు(t + x + 3, s + old_నిడివి - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        తిరిగి t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
స్థిర స్థిరరాశి అక్షరం* కనుగొను_మునుపటి_unichar(స్థిరరాశి అక్షరం *ప్రారంభం, స్థిరరాశి అక్షరం *చివర, char32_t *ret_c) {
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ప్రారంభం);
        ప్రతిజ్ఞా(చివర);
        ప్రతిజ్ఞా(చివర > ప్రారంభం);

        కొరకు (p = చివర; p > ప్రారంభం; ) {
                p--;
                ఒకవేళ (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        ఆపు;
        }

        r = utf8_encoded_valid_unichar(p, చివర - p);
        ఒకవేళ (r < 0 || p + r != చివర)
                తిరిగి ఖాళీ;

        ఒకవేళ (ret_c)
                assert_se(utf8_encoded_గమ్యం_unichar(p, ret_c) == r);

        తిరిగి p;
}

అక్షరం* ellipsize_mem(స్థిరరాశి అక్షరం *s, size_t old_నిడివి, size_t new_నిడివి, గుర్తులేని percent) {
        size_t x, k, len, len2;
        స్థిరరాశి అక్షరం *i, *j;
        పూర్ణాంకం r;

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

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(percent <= 100);

        ఒకవేళ (new_నిడివి == SIZE_MAX)
                తిరిగి strndup(s, old_నిడివి);

        ఒకవేళ (new_నిడివి == 0)
                తిరిగి strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_నిడివి);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ఒకవేళ (!has_ansi_seq && ascii_is_valid_n(s, old_నిడివి) && !memchr(s, '\t', old_నిడివి))
                తిరిగి ascii_ellipsize_mem(s, old_నిడివి, new_నిడివి, percent);

        x = (new_నిడివి - 1) * percent / 100;
        ప్రతిజ్ఞా(x <= new_నిడివి - 1);

        k = 0;
        కొరకు (i = s; i < s + old_నిడివి; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_నిడివి(i, old_నిడివి - (i - s)) : 0;
                ఒకవేళ (slen > 0) {
                        i += slen;
                        కొనసాగు;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_నిడివి - i);
                ఒకవేళ (r < 0)
                        తిరిగి ఖాళీ;

                char32_t c;
                assert_se(utf8_encoded_గమ్యం_unichar(i, &c) == r);

                పూర్ణాంకం w = unichar_console_వెడల్పు(c);
                ఒకవేళ (k + w > x)
                        ఆపు;

                k += w;
                i += r;
        }

        స్థిరరాశి అక్షరం *ansi_ప్రారంభం = s + old_నిడివి;
        size_t ansi_len = 0;

        కొరకు (స్థిరరాశి అక్షరం *t = j = s + old_నిడివి; t > i && k < new_నిడివి; ) {
                char32_t c;
                పూర్ణాంకం w;
                స్థిరరాశి అక్షరం *tt;

                ఒకవేళ (has_ansi_seq && ansi_ప్రారంభం >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = మునుపటి_ansi_sequence(s, t - s, &ansi_ప్రారంభం);

                /* If the sequence extends all the way to the current position, skip it. */
                ఒకవేళ (has_ansi_seq && ansi_len > 0 && ansi_ప్రారంభం + ansi_len == t) {
                        t = ansi_ప్రారంభం;
                        కొనసాగు;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = కనుగొను_మునుపటి_unichar(i, t, &c);
                ఒకవేళ (!tt)
                        తిరిగి ఖాళీ;

                w = unichar_console_వెడల్పు(c);
                ఒకవేళ (k + w > new_నిడివి)
                        ఆపు;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ఒకవేళ (i >= j)
                తిరిగి memdup_suffix0(s, old_నిడివి);

        ఒకవేళ (k >= new_నిడివి) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ఒకవేళ (j < s + old_నిడివి) {
                        r = utf8_encoded_valid_unichar(j, s + old_నిడివి - j);
                        ఒకవేళ (r < 0)
                                తిరిగి ఖాళీ;

                        j += r;
                } లేకపోతే ఒకవేళ (i > s) {
                        స్థిరరాశి అక్షరం *tt = కనుగొను_మునుపటి_unichar(s, i, ఖాళీ);
                        ఒకవేళ (!tt)
                                తిరిగి ఖాళీ;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_నిడివి - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_నిడివి + 3 + 1 : len + 3 + len2 + 1;

        అక్షరం *e = new(అక్షరం, alloc_len);
        ఒకవేళ (!e)
                తిరిగి ఖాళీ;

        memcpy_సురక్షిత(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        అక్షరం *dst = e + len + 3;

        ఒకవేళ (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                కొరకు (స్థిరరాశి అక్షరం *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_నిడివి(p, j - p);
                        ఒకవేళ (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } లేకపోతే
                                p = utf8_తదుపరి_char(p);
                }

        memcpy_సురక్షిత(dst, j, len2);
        dst[len2] = '\0';

        తిరిగి e;
}

అక్షరం* cellescape(అక్షరం *buf, size_t len, స్థిరరాశి అక్షరం *s) {
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

        size_t i = 0, చివరి_char_వెడల్పు[4] = {}, k = 0;

        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(len > 0); /* at least a terminating NUL */
        ప్రతిజ్ఞా(s);

        కొరకు (;;) {
                అక్షరం four[4];
                పూర్ణాంకం w;

                ఒకవేళ (*s == 0) /* terminating NUL detected? then we are done! */
                        వెళ్ళు పూర్తి;

                w = cescape_char(*s, four);
                ఒకవేళ (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        ఆపు;

                /* OK, there was space, let's add this escaped character to the buffer */
                జ్ఞాపకం_నకలు(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                చివరి_char_వెడల్పు[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        కొరకు (size_t j = 0; j < ELEMENTSOF(చివరి_char_వెడల్పు); j++) {

                ఒకవేళ (i + 4 <= len) /* nice, we reached our space goal */
                        ఆపు;

                k = k == 0 ? 3 : k - 1;
                ఒకవేళ (చివరి_char_వెడల్పు[k] == 0) /* bummer, we reached the beginning of the strings */
                        ఆపు;

                ప్రతిజ్ఞా(i >= చివరి_char_వెడల్పు[k]);
                i -= చివరి_char_వెడల్పు[k];
        }

        ఒకవేళ (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        లేకపోతే ఒకవేళ (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } లేకపోతే ఒకవేళ (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        లేకపోతే
                ప్రతిజ్ఞా(i + 1 <= len);

పూర్తి:
        buf[i] = '\0';
        తిరిగి buf;
}

అక్షరం* strshorten(అక్షరం *s, size_t l) {
        ప్రతిజ్ఞా(s);

        ఒకవేళ (l >= SIZE_MAX-1) /* Would not change anything */
                తిరిగి s;

        ఒకవేళ (strnlen(s, l+1) > l)
                s[l] = 0;

        తిరిగి s;
}

పూర్ణాంకం strgrowpad0(అక్షరం **s, size_t l) {
        size_t sz;

        ప్రతిజ్ఞా(s);

        ఒకవేళ (*s) {
                sz = అక్షరశ్రేణి_పొడవు(*s) + 1;
                ఒకవేళ (sz >= l) /* never shrink */
                        తిరిగి 0;
        } లేకపోతే
                sz = 0;

        అక్షరం *q = realloc(*s, l);
        ఒకవేళ (!q)
                తిరిగి -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        తిరిగి 0;
}

అక్షరం* strreplace(స్థిరరాశి అక్షరం *పాఠ్యం, స్థిరరాశి అక్షరం *old_string, స్థిరరాశి అక్షరం *new_string) {
        size_t l, old_len, new_len;
        అక్షరం *t, *ret = ఖాళీ;
        స్థిరరాశి అక్షరం *f;

        ప్రతిజ్ఞా(old_string);
        ప్రతిజ్ఞా(new_string);

        ఒకవేళ (!పాఠ్యం)
                తిరిగి ఖాళీ;

        old_len = అక్షరశ్రేణి_పొడవు(old_string);
        new_len = అక్షరశ్రేణి_పొడవు(new_string);

        l = అక్షరశ్రేణి_పొడవు(పాఠ్యం);
        ఒకవేళ (!GREEDY_REALLOC(ret, l+1))
                తిరిగి ఖాళీ;

        f = పాఠ్యం;
        t = ret;
        అయితే (*f) {
                size_t d, nl;

                ఒకవేళ (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        కొనసాగు;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ఒకవేళ (!GREEDY_REALLOC(ret, nl + 1))
                        తిరిగి mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        తిరిగి ret;
}

స్థిర శూన్య advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t జరుపు[స్థిర 2],
                size_t కొలత) {

        ఒకవేళ (!offsets)
                తిరిగి;

        ప్రతిజ్ఞా(జరుపు);

        ఒకవేళ ((size_t) diff < offsets[0])
                జరుపు[0] += కొలత;
        ఒకవేళ ((size_t) diff < offsets[1])
                జరుపు[1] += కొలత;
}

స్థిర శూన్య retract_offsets(
                size_t begin,
                size_t చివర,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t జరుపు[స్థిర 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ఒకవేళ (!offsets)
                తిరిగి;

        ప్రతిజ్ఞా(జరుపు);
        ప్రతిజ్ఞా(begin <= చివర);

        కొరకు (size_t k = 0; k < 2; k++)
                ఒకవేళ (offsets[k] > begin)
                        జరుపు[k] -= MIN(offsets[k], చివర) - begin;
}

అక్షరం* strip_tab_ansi(అక్షరం **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_పూర్తి) AnsiSeqParser parser = {};
        _cleanup_(memstream_పూర్తి) Mem_ప్రవాహం m = {};
        స్థిరరాశి అక్షరం *begin = ఖాళీ;
        size_t isz, జరుపు[2] = {}, n_carriage_returns = 0;
        FILE *f;

        ప్రతిజ్ఞా(ibuf);
        ప్రతిజ్ఞా(*ibuf);
        సూచిక_MAY_BE_NULL(_isz);

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

        isz = _isz ? *_isz : అక్షరశ్రేణి_పొడవు(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        ఒకవేళ (!f)
                తిరిగి ఖాళీ;

        కొరకు (స్థిరరాశి అక్షరం *i = *ibuf; i < *ibuf + isz; i++) {
                పూర్ణాంకం e;

                కొరకు (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ఒకవేళ (e < 0)
                                తిరిగి ఖాళీ;
                        ఒకవేళ (e != ANSI_SEQ_EVENT_ABORT)
                                ఆపు;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ఒకవేళ (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, జరుపు);
                                begin = ఖాళీ;
                        }
                }

                ఎంపిక (e) {

                స్థితి ANSI_SEQ_EVENT_పాఠ్యం:
                        ఒకవేళ (*i == '\r') {
                                n_carriage_returns++;
                                కొనసాగు;
                        }
                        ఒకవేళ (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (size_t) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                జరుపు);
                                n_carriage_returns = 0;
                        }
                        కొరకు (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ఒకవేళ (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, జరుపు, 7);
                        } లేకపోతే
                                fputc(*i, f);

                        ఆపు;

                స్థితి ANSI_SEQ_EVENT_SEQUENCE:
                        ఒకవేళ (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                కొరకు (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        ఆపు;

                స్థితి ANSI_SEQ_EVENT_చివర:
                        /* A complete sequence, drop it (the current character is its last one) */
                        ప్రతిజ్ఞా(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, జరుపు);
                        begin = ఖాళీ;
                        ఆపు;

                అప్రమేయ:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ఒకవేళ (begin)
                retract_offsets(begin - *ibuf, isz, highlight, జరుపు);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, జరుపు);

        అక్షరం *obuf;
        ఒకవేళ (memstream_finalize(&m, &obuf, _isz) < 0)
                తిరిగి ఖాళీ;

        free_and_replace(*ibuf, obuf);

        ఒకవేళ (highlight) {
                highlight[0] += జరుపు[0];
                highlight[1] += జరుపు[1];
        }

        తిరిగి *ibuf;
}

అక్షరం* strextendv_with_separator(అక్షరం **x, స్థిరరాశి అక్షరం *separator, va_list ap) {
        _cleanup_free_ అక్షరం *బఫరు = ఖాళీ;
        size_t f, l, l_separator;
        bool need_separator;
        అక్షరం *nr, *p;

        ఒకవేళ (!x)
                x = &బఫరు;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        కొరకు (స్థిరరాశి అక్షరం *t;;) {
                size_t n;

                t = va_arg(aq, స్థిరరాశి అక్షరం *);
                ఒకవేళ (!t)
                        ఆపు;
                ఒకవేళ (t == సూచిక_MAX)
                        కొనసాగు;

                n = అక్షరశ్రేణి_పొడవు(t);

                ఒకవేళ (need_separator)
                        n += l_separator;

                ఒకవేళ (n >= SIZE_MAX - l) {
                        va_end(aq);
                        తిరిగి ఖాళీ;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_విడత_పైకి(l+1));
        ఒకవేళ (!nr)
                తిరిగి ఖాళీ;

        *x = nr;
        p = nr + f;

        కొరకు (;;) {
                స్థిరరాశి అక్షరం *t;

                t = va_arg(ap, స్థిరరాశి అక్షరం *);
                ఒకవేళ (!t)
                        ఆపు;
                ఒకవేళ (t == సూచిక_MAX)
                        కొనసాగు;

                ఒకవేళ (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        ప్రతిజ్ఞా(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ఒకవేళ (బఫరు)
                తిరిగి TAKE_PTR(బఫరు);

        /* Otherwise we extended the buffer: return the end */
        తిరిగి p;
}

అక్షరం* strextend_with_separator_అంతర్గత(అక్షరం **x, స్థిరరాశి అక్షరం *separator, ...) {
        va_list ap;
        అక్షరం *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        తిరిగి ret;
}

పూర్ణాంకం strextendf_with_separator(అక్షరం **x, స్థిరరాశి అక్షరం *separator, స్థిరరాశి అక్షరం *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        పూర్ణాంకం l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        ప్రతిజ్ఞా(x);
        ప్రతిజ్ఞా(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ఒకవేళ (*x) {
                m = అక్షరశ్రేణి_పొడవు(*x);
                a = MALLOC_SIZEOF_సురక్షిత(*x);
                ప్రతిజ్ఞా(a >= m + 1);
        } లేకపోతే
                m = a = 0;

        ఒకవేళ (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                అక్షరం *n;

                ఒకవేళ (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        తిరిగి -ENOMEM;
                ఒకవేళ (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        తిరిగి -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ఒకవేళ (!n)
                        తిరిగి -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_సురక్షిత(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_సురక్షిత(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        ప్రతిజ్ఞా(l >= 0);

        ఒకవేళ ((size_t) l < a - m - l_separator) {
                అక్షరం *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ఒకవేళ (n)
                        *x = n;
        } లేకపోతే {
                అక్షరం *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ఒకవేళ (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        వెళ్ళు oom;
                ఒకవేళ (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        వెళ్ళు oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ఒకవేళ (!n)
                        వెళ్ళు oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                ప్రతిజ్ఞా((size_t) l < a - m - l_separator);
        }

        తిరిగి 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        తిరిగి -ENOMEM;
}

అక్షరం* strrep(స్థిరరాశి అక్షరం *s, size_t n) {
        అక్షరం *ret, *p;
        size_t l;

        ప్రతిజ్ఞా(s);

        l = అక్షరశ్రేణి_పొడవు(s);
        ఒకవేళ (!MUL_కేటాయించు_సురక్షిత(&l, n))
                తిరిగి ఖాళీ;
        ఒకవేళ (!INC_సురక్షిత(&l, 1))
                తిరిగి ఖాళీ;

        p = ret = జ్ఞాపకం_ఇవ్వు(l);
        ఒకవేళ (!ret)
                తిరిగి ఖాళీ;

        కొరకు (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        తిరిగి ret;
}

పూర్ణాంకం విభజించు_pair(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *sep, అక్షరం **ret_మొదటి, అక్షరం **ret_రెండవ) {
        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(!isempty(sep));

        స్థిరరాశి అక్షరం *x = అక్షరశ్రేణి_శోధన(s, sep);
        ఒకవేళ (!x)
                తిరిగి -EINVAL;

        _cleanup_free_ అక్షరం *a = ఖాళీ;
        ఒకవేళ (ret_మొదటి) {
                a = strndup(s, x - s);
                ఒకవేళ (!a)
                        తిరిగి -ENOMEM;
        }

        _cleanup_free_ అక్షరం *b = ఖాళీ;
        ఒకవేళ (ret_రెండవ) {
                b = strdup(x + అక్షరశ్రేణి_పొడవు(sep));
                ఒకవేళ (!b)
                        తిరిగి -ENOMEM;
        }

        ఒకవేళ (ret_మొదటి)
                *ret_మొదటి = TAKE_PTR(a);
        ఒకవేళ (ret_రెండవ)
                *ret_రెండవ = TAKE_PTR(b);
        తిరిగి 0;
}

పూర్ణాంకం free_and_strdup(అక్షరం **p, స్థిరరాశి అక్షరం *s) {
        అక్షరం *t;

        ప్రతిజ్ఞా(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ఒకవేళ (streq_ptr(*p, s))
                తిరిగి 0;

        ఒకవేళ (s) {
                t = strdup(s);
                ఒకవేళ (!t)
                        తిరిగి -ENOMEM;
        } లేకపోతే
                t = ఖాళీ;

        free_and_replace(*p, t);

        తిరిగి 1;
}

పూర్ణాంకం free_and_strdup_warn(అక్షరం **p, స్థిరరాశి అక్షరం *s) {
        పూర్ణాంకం r;

        r = free_and_strdup(p, s);
        ఒకవేళ (r < 0)
                తిరిగి log_oom();
        తిరిగి r;
}

పూర్ణాంకం free_and_strndup(అక్షరం **p, స్థిరరాశి అక్షరం *s, size_t l) {
        అక్షరం *t;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ఒకవేళ (!*p && !s)
                తిరిగి 0;

        ఒకవేళ (*p && s && strneq(*p, s, l) && (l > అక్షరశ్రేణి_పొడవు(*p) || (*p)[l] == '\0'))
                తిరిగి 0;

        ఒకవేళ (s) {
                t = strndup(s, l);
                ఒకవేళ (!t)
                        తిరిగి -ENOMEM;
        } లేకపోతే
                t = ఖాళీ;

        free_and_replace(*p, t);
        తిరిగి 1;
}

పూర్ణాంకం strdup_గమ్యం_నిండిన(అక్షరం **ret, స్థిరరాశి అక్షరం *src) {
        ఒకవేళ (!src) {
                ఒకవేళ (ret)
                        *ret = ఖాళీ;

                తిరిగి 0;
        } లేకపోతే {
                ఒకవేళ (ret) {
                        అక్షరం *t = strdup(src);
                        ఒకవేళ (!t)
                                తిరిగి -ENOMEM;
                        *ret = t;
                }

                తిరిగి 1;
        }
};

bool string_is_సురక్షిత(స్థిరరాశి అక్షరం *p, String_సురక్షిత_Flags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ఒకవేళ (FLAGS_సమితి(flags, STRING_ALLOW_ఖాళీలు) ? !p : isempty(p))
                తిరిగి false;

        ఒకవేళ (!FLAGS_సమితి(flags, STRING_ASCII) && !utf8_is_valid(p))
                తిరిగి false;

        కొరకు (స్థిరరాశి అక్షరం *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ఒకవేళ ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        తిరిగి false;

                ఒకవేళ (!FLAGS_సమితి(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        తిరిగి false;

                ఒకవేళ (!FLAGS_సమితి(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        తిరిగి false;

                ఒకవేళ (!FLAGS_సమితి(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        తిరిగి false;

                ఒకవేళ (!FLAGS_సమితి(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        తిరిగి false;

                ఒకవేళ (FLAGS_సమితి(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        తిరిగి false;

                ఒకవేళ (FLAGS_సమితి(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        తిరిగి false;
        }

        ఒకవేళ (FLAGS_సమితి(flags, STRING_FILENAME) && !filename_is_valid(p))
                తిరిగి false;

        ఒకవేళ (FLAGS_సమితి(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                తిరిగి false;

        తిరిగి true;
}

అక్షరం* str_realloc(అక్షరం *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        తిరిగి realloc(p, అక్షరశ్రేణి_పొడవు(p) + 1) ?: p;
}

అక్షరం* string_erase(అక్షరం *x) {
        ఒకవేళ (!x)
                తిరిగి ఖాళీ;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_సురక్షిత(x, అక్షరశ్రేణి_పొడవు(x));
        తిరిగి x;
}

పూర్ణాంకం string_truncate_పంక్తులు(స్థిరరాశి అక్షరం *s, size_t n_పంక్తులు, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p = s, *e = s;
        bool truncation_applied = false;
        అక్షరం *copy;
        size_t n = 0;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        కొరకు (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ఒకవేళ (p[k] == 0) {
                        ఒకవేళ (k == 0) /* final empty line */
                                ఆపు;

                        ఒకవేళ (n >= n_పంక్తులు) /* above threshold */
                                ఆపు;

                        e = p + k; /* last line to include */
                        ఆపు;
                }

                ప్రతిజ్ఞా(p[k] == '\n');

                ఒకవేళ (n >= n_పంక్తులు)
                        ఆపు;

                ఒకవేళ (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ఒకవేళ (isempty(e))
                copy = strdup(s);
        లేకపోతే {
                ఒకవేళ (!లోపల_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ఒకవేళ (!copy)
                తిరిగి -ENOMEM;

        *ret = copy;
        తిరిగి truncation_applied;
}

పూర్ణాంకం string_వెలికితీయి_పంక్తి(స్థిరరాశి అక్షరం *s, size_t i, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p = s;
        size_t c = 0;

        ప్రతిజ్ఞా(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        కొరకు (;;) {
                స్థిరరాశి అక్షరం *q;

                q = strchr(p, '\n');
                ఒకవేళ (i == c) {
                        /* The line we are looking for! */

                        ఒకవేళ (q) {
                                అక్షరం *m;

                                m = strndup(p, q - p);
                                ఒకవేళ (!m)
                                        తిరిగి -ENOMEM;

                                *ret = m;
                                తిరిగి !isempty(q + 1); /* More coming? */
                        } లేకపోతే
                                /* Tell the caller to use the input string if equal */
                                తిరిగి strdup_గమ్యం(ret, p != s ? p : ఖాళీ);
                }

                ఒకవేళ (!q)
                        /* No more lines, return empty line */
                        తిరిగి strdup_గమ్యం(ret, "");

                p = q + 1;
                c++;
        }
}

పూర్ణాంకం string_కలిగియుంది_మాట_strv(స్థిరరాశి అక్షరం *string, స్థిరరాశి అక్షరం *separators, అక్షరం * స్థిరరాశి *పదాలు, స్థిరరాశి అక్షరం **ret_మాట) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        స్థిరరాశి వెలికితీయి_Flags flags = separators ? వెలికితీయి_DONT_COALESCE_SEPARATORS : 0;
        స్థిరరాశి అక్షరం *దొరికింది = ఖాళీ;
        పూర్ణాంకం r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *w = ఖాళీ;

                r = వెలికితీయి_మొదటి_మాట(&string, &w, separators, flags);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                దొరికింది = strv_కనుగొను(పదాలు, w);
                ఒకవేళ (దొరికింది)
                        ఆపు;
        }

        ఒకవేళ (ret_మాట)
                *ret_మాట = దొరికింది;
        తిరిగి !!దొరికింది;
}

bool streq_skip_trailing_chars(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2, స్థిరరాశి అక్షరం *ok) {
        ఒకవేళ (!s1 && !s2)
                తిరిగి true;
        ఒకవేళ (!s1 || !s2)
                తిరిగి false;

        ఒకవేళ (!ok)
                ok = WHITESPACE;

        కొరకు (; *s1 && *s2; s1++, s2++)
                ఒకవేళ (*s1 != *s2)
                        ఆపు;

        తిరిగి లోపల_charset(s1, ok) && లోపల_charset(s2, ok);
}

అక్షరం* string_replace_char(అక్షరం *str, అక్షరం old_char, అక్షరం new_char) {
        ప్రతిజ్ఞా(str);
        ప్రతిజ్ఞా(old_char != '\0');
        ప్రతిజ్ఞా(new_char != '\0');
        ప్రతిజ్ఞా(old_char != new_char);

        కొరకు (అక్షరం *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        తిరిగి str;
}

పూర్ణాంకం make_cstring(స్థిరరాశి శూన్య *s, size_t n, Make_CString_బహుళకం బహుళకం, అక్షరం **ret) {
        అక్షరం *b;

        ప్రతిజ్ఞా(s || n == 0);
        ప్రతిజ్ఞా(బహుళకం >= 0);
        ప్రతిజ్ఞా(బహుళకం < MAKE_CSTRING_బహుళకం_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ఒకవేళ (n == 0) {
                ఒకవేళ (బహుళకం == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        తిరిగి -EINVAL;

                ఒకవేళ (!ret)
                        తిరిగి 0;

                b = new0(అక్షరం, 1);
        } లేకపోతే {
                స్థిరరాశి uint8_t *nul;

                nul = memchr(s, 0, n);
                ఒకవేళ (nul) {
                        ఒకవేళ (nul < (స్థిరరాశి uint8_t*) s + n - 1 || /* embedded NUL? */
                            బహుళకం == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                తిరిగి -EINVAL;

                        n--;
                } లేకపోతే ఒకవేళ (బహుళకం == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        తిరిగి -EINVAL;

                ఒకవేళ (!ret)
                        తిరిగి 0;

                b = memdup_suffix0(s, n);
        }
        ఒకవేళ (!b)
                తిరిగి -ENOMEM;

        *ret = b;
        తిరిగి 0;
}

size_t strspn_from_చివర(స్థిరరాశి అక్షరం *str, స్థిరరాశి అక్షరం *accept) {
        size_t n = 0;

        ఒకవేళ (isempty(str))
                తిరిగి 0;

        ఒకవేళ (isempty(accept))
                తిరిగి 0;

        కొరకు (స్థిరరాశి అక్షరం *p = str + అక్షరశ్రేణి_పొడవు(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        తిరిగి n;
}

size_t strnspn(స్థిరరాశి అక్షరం *str, స్థిరరాశి అక్షరం *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        ప్రతిజ్ఞా(str || n == 0);
        ప్రతిజ్ఞా(accept);

        కొరకు (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        తిరిగి i;
}

అక్షరం* strdupspn(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *accept) {
        ఒకవేళ (isempty(a) || isempty(accept))
                తిరిగి strdup("");

        తిరిగి strndup(a, strspn(a, accept));
}

అక్షరం* strdupcspn(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *reject) {
        ఒకవేళ (isempty(a))
                తిరిగి strdup("");
        ఒకవేళ (isempty(reject))
                తిరిగి strdup(a);

        తిరిగి strndup(a, strcspn(a, reject));
}

అక్షరం* కనుగొను_పంక్తి_startswith_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        ప్రతిజ్ఞా(haystack);
        ప్రతిజ్ఞా(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        అక్షరం *p = (అక్షరం*) అక్షరశ్రేణి_శోధన(haystack, needle);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        ఒకవేళ (p > haystack)
                అయితే (p[-1] != '\n') {
                        p = అక్షరశ్రేణి_శోధన(p + 1, needle);
                        ఒకవేళ (!p)
                                తిరిగి ఖాళీ;
                }

        తిరిగి p + అక్షరశ్రేణి_పొడవు(needle);
}

అక్షరం* కనుగొను_పంక్తి_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        ప్రతిజ్ఞా(haystack);
        ప్రతిజ్ఞా(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        అక్షరం *p = (అక్షరం*) కనుగొను_పంక్తి_startswith(haystack, needle);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        ఒకవేళ (*p == 0 || strchr(NEWLINE, *p))
                తిరిగి p - అక్షరశ్రేణి_పొడవు(needle);

        తిరిగి ఖాళీ;
}

అక్షరం* కనుగొను_పంక్తి_after_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        ప్రతిజ్ఞా(haystack);
        ప్రతిజ్ఞా(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        అక్షరం *p = (అక్షరం*) కనుగొను_పంక్తి_startswith(haystack, needle);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        ఒకవేళ (*p == 0)
                తిరిగి p;
        ఒకవేళ (strchr(NEWLINE, *p))
                తిరిగి p + 1;

        తిరిగి ఖాళీ;
}

bool సంస్కరణ_is_valid(స్థిరరాశి అక్షరం *s, సంస్కరణ_Flags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ఒకవేళ (FLAGS_సమితి(flags, సంస్కరణ_ALLOW_ఖాళీలు) ? !s : isempty(s))
                తిరిగి false;

        ఒకవేళ (!filename_part_is_valid(s))
                తిరిగి false;

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

        అక్షరం charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = అక్షరశ్రేణి_పొడవు(charset);

        ఒకవేళ (FLAGS_సమితి(flags, సంస్కరణ_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ఒకవేళ (FLAGS_సమితి(flags, సంస్కరణ_ALLOW_PLUS))
                charset[l++] = '+';

        తిరిగి లోపల_charset(s, charset);
}

ssize_t strlevenshtein(స్థిరరాశి అక్షరం *x, స్థిరరాశి అక్షరం *y) {
        _cleanup_free_ size_t *t0 = ఖాళీ, *t1 = ఖాళీ, *t2 = ఖాళీ;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ఒకవేళ (streq_ptr(x, y))
                తిరిగి 0;

        xl = strlen_ptr(x);
        ఒకవేళ (xl > SSIZE_MAX)
                తిరిగి -E2BIG;

        yl = strlen_ptr(y);
        ఒకవేళ (yl > SSIZE_MAX)
                తిరిగి -E2BIG;

        ఒకవేళ (isempty(x))
                తిరిగి yl;
        ఒకవేళ (isempty(y))
                తిరిగి xl;

        t0 = new0(size_t, yl + 1);
        ఒకవేళ (!t0)
                తిరిగి -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ఒకవేళ (!t1)
                తిరిగి -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ఒకవేళ (!t2)
                తిరిగి -ENOMEM;

        కొరకు (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        కొరకు (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                కొరకు (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /*మార్పిడి*/
                        ఒకవేళ (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ఒకవేళ (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ఒకవేళ (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        తిరిగి t1[yl];
}

అక్షరం* strrstr_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ఒకవేళ (!haystack || !needle)
                తిరిగి ఖాళీ;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ఒకవేళ (needle[0] == 0)
                తిరిగి (అక్షరం*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ఒకవేళ (needle[1] == 0)
                తిరిగి (అక్షరం*) strrchr(haystack, needle[0]);

        కొరకు (స్థిరరాశి అక్షరం *p = అక్షరశ్రేణి_శోధన(haystack, needle), *q; p; p = q) {
                q = అక్షరశ్రేణి_శోధన(p + 1, needle);
                ఒకవేళ (!q)
                        తిరిగి (అక్షరం*) p;
        }
        తిరిగి ఖాళీ;
}

అక్షరం* strrstr_no_case_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        ఒకవేళ (!haystack || !needle)
                తిరిగి ఖాళీ;

        కొరకు (స్థిరరాశి అక్షరం *p = strchr(haystack, 0); p > haystack; p--)
                ఒకవేళ (startswith_no_case(p, needle))
                        తిరిగి (అక్షరం*) p;

        తిరిగి startswith_no_case(haystack, needle) ? (అక్షరం*) haystack : ఖాళీ;
}

size_t str_common_ఉపసర్గ(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        కొరకు (size_t n = 0;; n++) {
                అక్షరం c = a[n];
                ఒకవేళ (c != b[n])
                        తిరిగి n;
                ఒకవేళ (c == 0)
                        తిరిగి SIZE_MAX;
        }
}
