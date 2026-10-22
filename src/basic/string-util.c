/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "ansi-seq.h"
#شامل "escape.h"
#شامل "extract-word.h"
#شامل "glyph-util.h"
#شامل "gunicode.h"
#شامل "locale-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "memstream-util.h"
#شامل "path-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"

حرف* پہلا_لفظ(ثابت حرف *s, ثابت حرف *لفظ) {
        أكد(s);
        أكد(لفظ);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        اگر (isempty(لفظ))
                واپس (حرف*) s;

        ثابت حرف *p = startswith(s, لفظ);
        اگر (!p)
                واپس تہی;
        اگر (*p == '\0')
                واپس (حرف*) p;

        ثابت حرف *nw = skip_leading_chars(p, WHITESPACE);
        اگر (p == nw)
                واپس تہی;

        واپس (حرف*) nw;
}

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) {
        أكد(x);
        أكد(s || l == 0);

        اگر (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        اگر (l > 0 || !*x) {
                size_t q;
                حرف *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                اگر (!m)
                        واپس تہی;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        واپس *x;
}

حرف* strstrip(حرف *s) {
        اگر (!s)
                واپس تہی;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        واپس delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

حرف* delete_chars(حرف *s, ثابت حرف *bad) {
        حرف *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        اگر (!s)
                واپس تہی;

        اگر (!bad)
                bad = WHITESPACE;

        برائے (f = s, t = s; *f; f++) {
                اگر (strchr(bad, *f))
                        جاری;

                *(t++) = *f;
        }

        *t = 0;

        واپس s;
}

حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad) {
        حرف *c = s;

        /* Drops all specified bad characters, at the end of the string */

        اگر (!s)
                واپس تہی;

        اگر (!bad)
                bad = WHITESPACE;

        برائے (حرف *p = s; *p; p++)
                اگر (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        واپس s;
}

حرف* truncate_nl_بھرا(حرف *s, size_t *ret_len) {
        size_t n;

        أكد(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        اگر (ret_len)
                *ret_len = n;
        واپس s;
}

حرف ascii_tolower(حرف x) {

        اگر (x >= 'A' && x <= 'Z')
                واپس x - 'A' + 'a';

        واپس x;
}

حرف ascii_toupper(حرف x) {

        اگر (x >= 'a' && x <= 'z')
                واپس x - 'a' + 'A';

        واپس x;
}

حرف* ascii_strlower(حرف *s) {
        أكد(s);

        برائے (حرف *p = s; *p; p++)
                *p = ascii_tolower(*p);

        واپس s;
}

حرف* ascii_strupper(حرف *s) {
        أكد(s);

        برائے (حرف *p = s; *p; p++)
                *p = ascii_toupper(*p);

        واپس s;
}

حرف* ascii_strlower_n(حرف *s, size_t n) {
        أكد(n <= 0 || s);

        اگر (n <= 0)
                واپس s;

        برائے (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        واپس s;
}

صحیح ascii_strcasecmp_n(ثابت حرف *a, ثابت حرف *b, size_t n) {

        أكد(a);
        أكد(b);

        برائے (; n > 0; a++, b++, n--) {
                صحیح x, y;

                x = (صحیح) (uint8_t) ascii_tolower(*a);
                y = (صحیح) (uint8_t) ascii_tolower(*b);

                اگر (x != y)
                        واپس x - y;
        }

        واپس 0;
}

صحیح ascii_strcasecmp_nn(ثابت حرف *a, size_t n, ثابت حرف *b, size_t m) {
        صحیح r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        اگر (r != 0)
                واپس r;

        واپس CMP(n, m);
}

bool chars_intersect(ثابت حرف *a, ثابت حرف *b) {
        /* Returns true if any of the chars in a are in b. */
        برائے (ثابت حرف *p = a; *p; p++)
                اگر (strchr(b, *p))
                        واپس true;

        واپس false;
}

bool string_has_cc(ثابت حرف *p, ثابت حرف *ok) {
        أكد(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        برائے (ثابت حرف *t = p; *t; t++) {
                اگر (ok && strchr(ok, *t))
                        جاری;

                اگر (char_is_cc(*t))
                        واپس true;
        }

        واپس false;
}

ساکن صحیح write_ellipsis(حرف *buf, bool unicode) {
        ثابت حرف *s = glyph_بھرا(GLYPH_ELLIPSIS, unicode);
        أكد(لڑی_لمبائی(s) == 3);
        حافظہ_نقل(buf, s, 3);
        واپس 3;
}

ساکن size_t ansi_sequence_لمبائی(ثابت حرف *s, size_t len) {
        أكد(s);

        اگر (len < 2)
                واپس 0;

        اگر (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                واپس 0;  /* Not the start of a sequence */

        اگر (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                اگر (i == len)
                        واپس 0;

                جب (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        اگر (++i == len)
                                واپس 0;
                جب (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        اگر (++i == len)
                                واپس 0;
                اگر (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        واپس i + 1;
                واپس 0;  /* Bad sequence */

        } ورنہ اگر (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                واپس 2;

        واپس 0;  /* Bad escape? */
}

ساکن bool string_has_ansi_sequence(ثابت حرف *s, size_t len) {
        ثابت حرف *t = s;

        جب ((t = memchr(t, 0x1B, len - (t - s)))) {
                اگر (ansi_sequence_لمبائی(t, len - (t - s)) > 0)
                        واپس true;
                t++;
        }
        واپس false;
}

ساکن size_t پچھلا_ansi_sequence(ثابت حرف *s, size_t length, ثابت حرف **ret_where) {

        أكد(s);
        أكد(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        اگر (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = تہی;
                واپس 0;
        }

        برائے (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_لمبائی(s + i, length - i);
                اگر (slen > 0) {
                        *ret_where = s + i;
                        واپس slen;
                }

                اگر (i == 0)
                        توڑو;
        }

        *ret_where = تہی;
        واپس 0;
}

ساکن حرف *ascii_ellipsize_mem(ثابت حرف *s, size_t old_لمبائی, size_t new_لمبائی, غیرعلامتی percent) {
        size_t x, need_space, suffix_len;
        حرف *t;

        أكد(s);
        أكد(percent <= 100);
        أكد(new_لمبائی != SIZE_MAX);

        اگر (old_لمبائی <= new_لمبائی)
                واپس strndup(s, old_لمبائی);

        /* Special case short ellipsations */
        انتخاب (new_لمبائی) {

        حالت 0:
                واپس strdup("");

        حالت 1:
                اگر (is_locale_utf8())
                        واپس strdup("…");
                ورنہ
                        واپس strdup(".");

        حالت 2:
                اگر (!is_locale_utf8())
                        واپس strdup("..");
                توڑو;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(حرف, new_لمبائی+3);
        اگر (!t)
                واپس تہی;

        أكد(new_لمبائی >= need_space);

        x = ((new_لمبائی - need_space) * percent + 50) / 100;
        أكد(x <= new_لمبائی - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_لمبائی - x - need_space;
        حافظہ_نقل(t + x + 3, s + old_لمبائی - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        واپس t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ساکن ثابت حرف* ڈھونڈو_پچھلا_unichar(ثابت حرف *آغاز, ثابت حرف *انتہا, char32_t *ret_c) {
        ثابت حرف *p;
        صحیح r;

        أكد(آغاز);
        أكد(انتہا);
        أكد(انتہا > آغاز);

        برائے (p = انتہا; p > آغاز; ) {
                p--;
                اگر (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        توڑو;
        }

        r = utf8_encoded_valid_unichar(p, انتہا - p);
        اگر (r < 0 || p + r != انتہا)
                واپس تہی;

        اگر (ret_c)
                assert_se(utf8_encoded_مقصد_unichar(p, ret_c) == r);

        واپس p;
}

حرف* ellipsize_mem(ثابت حرف *s, size_t old_لمبائی, size_t new_لمبائی, غیرعلامتی percent) {
        size_t x, k, len, len2;
        ثابت حرف *i, *j;
        صحیح r;

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

        اگر (new_لمبائی == SIZE_MAX)
                واپس strndup(s, old_لمبائی);

        اگر (new_لمبائی == 0)
                واپس strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_لمبائی);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        اگر (!has_ansi_seq && ascii_is_valid_n(s, old_لمبائی) && !memchr(s, '\t', old_لمبائی))
                واپس ascii_ellipsize_mem(s, old_لمبائی, new_لمبائی, percent);

        x = (new_لمبائی - 1) * percent / 100;
        أكد(x <= new_لمبائی - 1);

        k = 0;
        برائے (i = s; i < s + old_لمبائی; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_لمبائی(i, old_لمبائی - (i - s)) : 0;
                اگر (slen > 0) {
                        i += slen;
                        جاری;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_لمبائی - i);
                اگر (r < 0)
                        واپس تہی;

                char32_t c;
                assert_se(utf8_encoded_مقصد_unichar(i, &c) == r);

                صحیح w = unichar_console_چوڑائی(c);
                اگر (k + w > x)
                        توڑو;

                k += w;
                i += r;
        }

        ثابت حرف *ansi_آغاز = s + old_لمبائی;
        size_t ansi_len = 0;

        برائے (ثابت حرف *t = j = s + old_لمبائی; t > i && k < new_لمبائی; ) {
                char32_t c;
                صحیح w;
                ثابت حرف *tt;

                اگر (has_ansi_seq && ansi_آغاز >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = پچھلا_ansi_sequence(s, t - s, &ansi_آغاز);

                /* If the sequence extends all the way to the current position, skip it. */
                اگر (has_ansi_seq && ansi_len > 0 && ansi_آغاز + ansi_len == t) {
                        t = ansi_آغاز;
                        جاری;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = ڈھونڈو_پچھلا_unichar(i, t, &c);
                اگر (!tt)
                        واپس تہی;

                w = unichar_console_چوڑائی(c);
                اگر (k + w > new_لمبائی)
                        توڑو;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        اگر (i >= j)
                واپس memdup_suffix0(s, old_لمبائی);

        اگر (k >= new_لمبائی) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                اگر (j < s + old_لمبائی) {
                        r = utf8_encoded_valid_unichar(j, s + old_لمبائی - j);
                        اگر (r < 0)
                                واپس تہی;

                        j += r;
                } ورنہ اگر (i > s) {
                        ثابت حرف *tt = ڈھونڈو_پچھلا_unichar(s, i, تہی);
                        اگر (!tt)
                                واپس تہی;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_لمبائی - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_لمبائی + 3 + 1 : len + 3 + len2 + 1;

        حرف *e = new(حرف, alloc_len);
        اگر (!e)
                واپس تہی;

        memcpy_محفوظ(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        حرف *dst = e + len + 3;

        اگر (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                برائے (ثابت حرف *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_لمبائی(p, j - p);
                        اگر (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } ورنہ
                                p = utf8_اگلا_char(p);
                }

        memcpy_محفوظ(dst, j, len2);
        dst[len2] = '\0';

        واپس e;
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

        size_t i = 0, آخری_char_چوڑائی[4] = {}, k = 0;

        أكد(buf);
        أكد(len > 0); /* at least a terminating NUL */
        أكد(s);

        برائے (;;) {
                حرف four[4];
                صحیح w;

                اگر (*s == 0) /* terminating NUL detected? then we are done! */
                        جاؤ مکمل;

                w = cescape_char(*s, four);
                اگر (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        توڑو;

                /* OK, there was space, let's add this escaped character to the buffer */
                حافظہ_نقل(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                آخری_char_چوڑائی[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        برائے (size_t j = 0; j < ELEMENTSOF(آخری_char_چوڑائی); j++) {

                اگر (i + 4 <= len) /* nice, we reached our space goal */
                        توڑو;

                k = k == 0 ? 3 : k - 1;
                اگر (آخری_char_چوڑائی[k] == 0) /* bummer, we reached the beginning of the strings */
                        توڑو;

                أكد(i >= آخری_char_چوڑائی[k]);
                i -= آخری_char_چوڑائی[k];
        }

        اگر (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        ورنہ اگر (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } ورنہ اگر (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        ورنہ
                أكد(i + 1 <= len);

مکمل:
        buf[i] = '\0';
        واپس buf;
}

حرف* strshorten(حرف *s, size_t l) {
        أكد(s);

        اگر (l >= SIZE_MAX-1) /* Would not change anything */
                واپس s;

        اگر (strnlen(s, l+1) > l)
                s[l] = 0;

        واپس s;
}

صحیح strgrowpad0(حرف **s, size_t l) {
        size_t sz;

        أكد(s);

        اگر (*s) {
                sz = لڑی_لمبائی(*s) + 1;
                اگر (sz >= l) /* never shrink */
                        واپس 0;
        } ورنہ
                sz = 0;

        حرف *q = realloc(*s, l);
        اگر (!q)
                واپس -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        واپس 0;
}

حرف* strreplace(ثابت حرف *متن, ثابت حرف *old_string, ثابت حرف *new_string) {
        size_t l, old_len, new_len;
        حرف *t, *ret = تہی;
        ثابت حرف *f;

        أكد(old_string);
        أكد(new_string);

        اگر (!متن)
                واپس تہی;

        old_len = لڑی_لمبائی(old_string);
        new_len = لڑی_لمبائی(new_string);

        l = لڑی_لمبائی(متن);
        اگر (!GREEDY_REALLOC(ret, l+1))
                واپس تہی;

        f = متن;
        t = ret;
        جب (*f) {
                size_t d, nl;

                اگر (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        جاری;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                اگر (!GREEDY_REALLOC(ret, nl + 1))
                        واپس mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        واپس ret;
}

ساکن خالی advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t سرکاؤ[ساکن 2],
                size_t سائز) {

        اگر (!offsets)
                واپس;

        أكد(سرکاؤ);

        اگر ((size_t) diff < offsets[0])
                سرکاؤ[0] += سائز;
        اگر ((size_t) diff < offsets[1])
                سرکاؤ[1] += سائز;
}

ساکن خالی retract_offsets(
                size_t begin,
                size_t انتہا,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t سرکاؤ[ساکن 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        اگر (!offsets)
                واپس;

        أكد(سرکاؤ);
        أكد(begin <= انتہا);

        برائے (size_t k = 0; k < 2; k++)
                اگر (offsets[k] > begin)
                        سرکاؤ[k] -= MIN(offsets[k], انتہا) - begin;
}

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_مکمل) AnsiSeqParser parser = {};
        _cleanup_(memstream_مکمل) Mem_دھارا m = {};
        ثابت حرف *begin = تہی;
        size_t isz, سرکاؤ[2] = {}, n_carriage_returns = 0;
        FILE *f;

        أكد(ibuf);
        أكد(*ibuf);
        اشارہ_گر_MAY_BE_NULL(_isz);

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

        isz = _isz ? *_isz : لڑی_لمبائی(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        اگر (!f)
                واپس تہی;

        برائے (ثابت حرف *i = *ibuf; i < *ibuf + isz; i++) {
                صحیح e;

                برائے (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        اگر (e < 0)
                                واپس تہی;
                        اگر (e != ANSI_SEQ_EVENT_ABORT)
                                توڑو;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        اگر (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, سرکاؤ);
                                begin = تہی;
                        }
                }

                انتخاب (e) {

                حالت ANSI_SEQ_EVENT_متن:
                        اگر (*i == '\r') {
                                n_carriage_returns++;
                                جاری;
                        }
                        اگر (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (size_t) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                سرکاؤ);
                                n_carriage_returns = 0;
                        }
                        برائے (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        اگر (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, سرکاؤ, 7);
                        } ورنہ
                                fputc(*i, f);

                        توڑو;

                حالت ANSI_SEQ_EVENT_SEQUENCE:
                        اگر (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                برائے (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        توڑو;

                حالت ANSI_SEQ_EVENT_انتہا:
                        /* A complete sequence, drop it (the current character is its last one) */
                        أكد(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, سرکاؤ);
                        begin = تہی;
                        توڑو;

                طےشدہ:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        اگر (begin)
                retract_offsets(begin - *ibuf, isz, highlight, سرکاؤ);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, سرکاؤ);

        حرف *obuf;
        اگر (memstream_finalize(&m, &obuf, _isz) < 0)
                واپس تہی;

        free_and_replace(*ibuf, obuf);

        اگر (highlight) {
                highlight[0] += سرکاؤ[0];
                highlight[1] += سرکاؤ[1];
        }

        واپس *ibuf;
}

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap) {
        _cleanup_free_ حرف *بفر = تہی;
        size_t f, l, l_separator;
        bool need_separator;
        حرف *nr, *p;

        اگر (!x)
                x = &بفر;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        برائے (ثابت حرف *t;;) {
                size_t n;

                t = va_arg(aq, ثابت حرف *);
                اگر (!t)
                        توڑو;
                اگر (t == اشارہ_گر_MAX)
                        جاری;

                n = لڑی_لمبائی(t);

                اگر (need_separator)
                        n += l_separator;

                اگر (n >= SIZE_MAX - l) {
                        va_end(aq);
                        واپس تہی;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_باری_اوپر(l+1));
        اگر (!nr)
                واپس تہی;

        *x = nr;
        p = nr + f;

        برائے (;;) {
                ثابت حرف *t;

                t = va_arg(ap, ثابت حرف *);
                اگر (!t)
                        توڑو;
                اگر (t == اشارہ_گر_MAX)
                        جاری;

                اگر (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        أكد(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        اگر (بفر)
                واپس TAKE_PTR(بفر);

        /* Otherwise we extended the buffer: return the end */
        واپس p;
}

حرف* strextend_with_separator_اندرونی(حرف **x, ثابت حرف *separator, ...) {
        va_list ap;
        حرف *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        واپس ret;
}

صحیح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        صحیح l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        أكد(x);
        أكد(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        اگر (*x) {
                m = لڑی_لمبائی(*x);
                a = MALLOC_SIZEOF_محفوظ(*x);
                أكد(a >= m + 1);
        } ورنہ
                m = a = 0;

        اگر (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                حرف *n;

                اگر (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        واپس -ENOMEM;
                اگر (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        واپس -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                اگر (!n)
                        واپس -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_محفوظ(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_محفوظ(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        أكد(l >= 0);

        اگر ((size_t) l < a - m - l_separator) {
                حرف *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                اگر (n)
                        *x = n;
        } ورنہ {
                حرف *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                اگر (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        جاؤ oom;
                اگر (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        جاؤ oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                اگر (!n)
                        جاؤ oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                أكد((size_t) l < a - m - l_separator);
        }

        واپس 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        واپس -ENOMEM;
}

حرف* strrep(ثابت حرف *s, size_t n) {
        حرف *ret, *p;
        size_t l;

        أكد(s);

        l = لڑی_لمبائی(s);
        اگر (!MUL_تفویض_محفوظ(&l, n))
                واپس تہی;
        اگر (!INC_محفوظ(&l, 1))
                واپس تہی;

        p = ret = حافظہ_دو(l);
        اگر (!ret)
                واپس تہی;

        برائے (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        واپس ret;
}

صحیح بانٹو_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_پہلا, حرف **ret_دوسرا) {
        أكد(s);
        أكد(!isempty(sep));

        ثابت حرف *x = لڑی_تلاش(s, sep);
        اگر (!x)
                واپس -EINVAL;

        _cleanup_free_ حرف *a = تہی;
        اگر (ret_پہلا) {
                a = strndup(s, x - s);
                اگر (!a)
                        واپس -ENOMEM;
        }

        _cleanup_free_ حرف *b = تہی;
        اگر (ret_دوسرا) {
                b = strdup(x + لڑی_لمبائی(sep));
                اگر (!b)
                        واپس -ENOMEM;
        }

        اگر (ret_پہلا)
                *ret_پہلا = TAKE_PTR(a);
        اگر (ret_دوسرا)
                *ret_دوسرا = TAKE_PTR(b);
        واپس 0;
}

صحیح free_and_strdup(حرف **p, ثابت حرف *s) {
        حرف *t;

        أكد(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        اگر (streq_ptr(*p, s))
                واپس 0;

        اگر (s) {
                t = strdup(s);
                اگر (!t)
                        واپس -ENOMEM;
        } ورنہ
                t = تہی;

        free_and_replace(*p, t);

        واپس 1;
}

صحیح free_and_strdup_warn(حرف **p, ثابت حرف *s) {
        صحیح r;

        r = free_and_strdup(p, s);
        اگر (r < 0)
                واپس log_oom();
        واپس r;
}

صحیح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) {
        حرف *t;

        أكد(p);
        أكد(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        اگر (!*p && !s)
                واپس 0;

        اگر (*p && s && strneq(*p, s, l) && (l > لڑی_لمبائی(*p) || (*p)[l] == '\0'))
                واپس 0;

        اگر (s) {
                t = strndup(s, l);
                اگر (!t)
                        واپس -ENOMEM;
        } ورنہ
                t = تہی;

        free_and_replace(*p, t);
        واپس 1;
}

صحیح strdup_مقصد_بھرا(حرف **ret, ثابت حرف *src) {
        اگر (!src) {
                اگر (ret)
                        *ret = تہی;

                واپس 0;
        } ورنہ {
                اگر (ret) {
                        حرف *t = strdup(src);
                        اگر (!t)
                                واپس -ENOMEM;
                        *ret = t;
                }

                واپس 1;
        }
};

bool string_is_محفوظ(ثابت حرف *p, String_محفوظ_Flags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        اگر (FLAGS_سیٹ(flags, STRING_ALLOW_خالی_خانے) ? !p : isempty(p))
                واپس false;

        اگر (!FLAGS_سیٹ(flags, STRING_ASCII) && !utf8_is_valid(p))
                واپس false;

        برائے (ثابت حرف *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                اگر ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        واپس false;

                اگر (!FLAGS_سیٹ(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        واپس false;

                اگر (!FLAGS_سیٹ(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        واپس false;

                اگر (!FLAGS_سیٹ(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        واپس false;

                اگر (!FLAGS_سیٹ(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        واپس false;

                اگر (FLAGS_سیٹ(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        واپس false;

                اگر (FLAGS_سیٹ(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        واپس false;
        }

        اگر (FLAGS_سیٹ(flags, STRING_FILENAME) && !filename_is_valid(p))
                واپس false;

        اگر (FLAGS_سیٹ(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                واپس false;

        واپس true;
}

حرف* str_realloc(حرف *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        اگر (!p)
                واپس تہی;

        واپس realloc(p, لڑی_لمبائی(p) + 1) ?: p;
}

حرف* string_erase(حرف *x) {
        اگر (!x)
                واپس تہی;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_محفوظ(x, لڑی_لمبائی(x));
        واپس x;
}

صحیح string_truncate_سطریں(ثابت حرف *s, size_t n_سطریں, حرف **ret) {
        ثابت حرف *p = s, *e = s;
        bool truncation_applied = false;
        حرف *copy;
        size_t n = 0;

        أكد(s);
        أكد(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        برائے (;;) {
                size_t k;

                k = strcspn(p, "\n");

                اگر (p[k] == 0) {
                        اگر (k == 0) /* final empty line */
                                توڑو;

                        اگر (n >= n_سطریں) /* above threshold */
                                توڑو;

                        e = p + k; /* last line to include */
                        توڑو;
                }

                أكد(p[k] == '\n');

                اگر (n >= n_سطریں)
                        توڑو;

                اگر (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        اگر (isempty(e))
                copy = strdup(s);
        ورنہ {
                اگر (!اندر_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        اگر (!copy)
                واپس -ENOMEM;

        *ret = copy;
        واپس truncation_applied;
}

صحیح string_نکالو_سطر(ثابت حرف *s, size_t i, حرف **ret) {
        ثابت حرف *p = s;
        size_t c = 0;

        أكد(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        برائے (;;) {
                ثابت حرف *q;

                q = strchr(p, '\n');
                اگر (i == c) {
                        /* The line we are looking for! */

                        اگر (q) {
                                حرف *m;

                                m = strndup(p, q - p);
                                اگر (!m)
                                        واپس -ENOMEM;

                                *ret = m;
                                واپس !isempty(q + 1); /* More coming? */
                        } ورنہ
                                /* Tell the caller to use the input string if equal */
                                واپس strdup_مقصد(ret, p != s ? p : تہی);
                }

                اگر (!q)
                        /* No more lines, return empty line */
                        واپس strdup_مقصد(ret, "");

                p = q + 1;
                c++;
        }
}

صحیح string_شامل_ہے_لفظ_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *الفاظ, ثابت حرف **ret_لفظ) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ثابت نکالو_Flags flags = separators ? نکالو_DONT_COALESCE_SEPARATORS : 0;
        ثابت حرف *ملا = تہی;
        صحیح r;

        برائے (;;) {
                _cleanup_free_ حرف *w = تہی;

                r = نکالو_پہلا_لفظ(&string, &w, separators, flags);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                ملا = strv_ڈھونڈو(الفاظ, w);
                اگر (ملا)
                        توڑو;
        }

        اگر (ret_لفظ)
                *ret_لفظ = ملا;
        واپس !!ملا;
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok) {
        اگر (!s1 && !s2)
                واپس true;
        اگر (!s1 || !s2)
                واپس false;

        اگر (!ok)
                ok = WHITESPACE;

        برائے (; *s1 && *s2; s1++, s2++)
                اگر (*s1 != *s2)
                        توڑو;

        واپس اندر_charset(s1, ok) && اندر_charset(s2, ok);
}

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char) {
        أكد(str);
        أكد(old_char != '\0');
        أكد(new_char != '\0');
        أكد(old_char != new_char);

        برائے (حرف *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        واپس str;
}

صحیح make_cstring(ثابت خالی *s, size_t n, Make_CString_منوال منوال, حرف **ret) {
        حرف *b;

        أكد(s || n == 0);
        أكد(منوال >= 0);
        أكد(منوال < MAKE_CSTRING_منوال_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        اگر (n == 0) {
                اگر (منوال == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        واپس -EINVAL;

                اگر (!ret)
                        واپس 0;

                b = new0(حرف, 1);
        } ورنہ {
                ثابت uint8_t *nul;

                nul = memchr(s, 0, n);
                اگر (nul) {
                        اگر (nul < (ثابت uint8_t*) s + n - 1 || /* embedded NUL? */
                            منوال == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                واپس -EINVAL;

                        n--;
                } ورنہ اگر (منوال == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        واپس -EINVAL;

                اگر (!ret)
                        واپس 0;

                b = memdup_suffix0(s, n);
        }
        اگر (!b)
                واپس -ENOMEM;

        *ret = b;
        واپس 0;
}

size_t strspn_from_انتہا(ثابت حرف *str, ثابت حرف *accept) {
        size_t n = 0;

        اگر (isempty(str))
                واپس 0;

        اگر (isempty(accept))
                واپس 0;

        برائے (ثابت حرف *p = str + لڑی_لمبائی(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        واپس n;
}

size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        أكد(str || n == 0);
        أكد(accept);

        برائے (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        واپس i;
}

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept) {
        اگر (isempty(a) || isempty(accept))
                واپس strdup("");

        واپس strndup(a, strspn(a, accept));
}

حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject) {
        اگر (isempty(a))
                واپس strdup("");
        اگر (isempty(reject))
                واپس strdup(a);

        واپس strndup(a, strcspn(a, reject));
}

حرف* ڈھونڈو_سطر_startswith_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        حرف *p = (حرف*) لڑی_تلاش(haystack, needle);
        اگر (!p)
                واپس تہی;

        اگر (p > haystack)
                جب (p[-1] != '\n') {
                        p = لڑی_تلاش(p + 1, needle);
                        اگر (!p)
                                واپس تہی;
                }

        واپس p + لڑی_لمبائی(needle);
}

حرف* ڈھونڈو_سطر_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        حرف *p = (حرف*) ڈھونڈو_سطر_startswith(haystack, needle);
        اگر (!p)
                واپس تہی;

        اگر (*p == 0 || strchr(NEWLINE, *p))
                واپس p - لڑی_لمبائی(needle);

        واپس تہی;
}

حرف* ڈھونڈو_سطر_after_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        حرف *p = (حرف*) ڈھونڈو_سطر_startswith(haystack, needle);
        اگر (!p)
                واپس تہی;

        اگر (*p == 0)
                واپس p;
        اگر (strchr(NEWLINE, *p))
                واپس p + 1;

        واپس تہی;
}

bool نسخہ_is_valid(ثابت حرف *s, نسخہ_Flags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        اگر (FLAGS_سیٹ(flags, نسخہ_ALLOW_خالی_خانے) ? !s : isempty(s))
                واپس false;

        اگر (!filename_part_is_valid(s))
                واپس false;

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
        size_t l = لڑی_لمبائی(charset);

        اگر (FLAGS_سیٹ(flags, نسخہ_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        اگر (FLAGS_سیٹ(flags, نسخہ_ALLOW_PLUS))
                charset[l++] = '+';

        واپس اندر_charset(s, charset);
}

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y) {
        _cleanup_free_ size_t *t0 = تہی, *t1 = تہی, *t2 = تہی;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        اگر (streq_ptr(x, y))
                واپس 0;

        xl = strlen_ptr(x);
        اگر (xl > SSIZE_MAX)
                واپس -E2BIG;

        yl = strlen_ptr(y);
        اگر (yl > SSIZE_MAX)
                واپس -E2BIG;

        اگر (isempty(x))
                واپس yl;
        اگر (isempty(y))
                واپس xl;

        t0 = new0(size_t, yl + 1);
        اگر (!t0)
                واپس -ENOMEM;
        t1 = new0(size_t, yl + 1);
        اگر (!t1)
                واپس -ENOMEM;
        t2 = new0(size_t, yl + 1);
        اگر (!t2)
                واپس -ENOMEM;

        برائے (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        برائے (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                برائے (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /*تبادلہ*/
                        اگر (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        اگر (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        اگر (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        واپس t1[yl];
}

حرف* strrstr_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        اگر (!haystack || !needle)
                واپس تہی;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        اگر (needle[0] == 0)
                واپس (حرف*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        اگر (needle[1] == 0)
                واپس (حرف*) strrchr(haystack, needle[0]);

        برائے (ثابت حرف *p = لڑی_تلاش(haystack, needle), *q; p; p = q) {
                q = لڑی_تلاش(p + 1, needle);
                اگر (!q)
                        واپس (حرف*) p;
        }
        واپس تہی;
}

حرف* strrstr_no_case_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        اگر (!haystack || !needle)
                واپس تہی;

        برائے (ثابت حرف *p = strchr(haystack, 0); p > haystack; p--)
                اگر (startswith_no_case(p, needle))
                        واپس (حرف*) p;

        واپس startswith_no_case(haystack, needle) ? (حرف*) haystack : تہی;
}

size_t str_common_سابقہ(ثابت حرف *a, ثابت حرف *b) {
        أكد(a);
        أكد(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        برائے (size_t n = 0;; n++) {
                حرف c = a[n];
                اگر (c != b[n])
                        واپس n;
                اگر (c == 0)
                        واپس SIZE_MAX;
        }
}
