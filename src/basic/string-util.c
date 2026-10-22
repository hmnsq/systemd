/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "ansi-seq.h"
#ضمن "escape.h"
#ضمن "extract-word.h"
#ضمن "glyph-util.h"
#ضمن "gunicode.h"
#ضمن "locale-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "memstream-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"

حرف* اولی_کلمه(ثابت حرف *s, ثابت حرف *کلمه) {
        أكد(s);
        أكد(کلمه);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        إذا (isempty(کلمه))
                أرجع (حرف*) s;

        ثابت حرف *p = startswith(s, کلمه);
        إذا (!p)
                أرجع عدم;
        إذا (*p == '\0')
                أرجع (حرف*) p;

        ثابت حرف *nw = skip_leading_chars(p, WHITESPACE);
        إذا (p == nw)
                أرجع عدم;

        أرجع (حرف*) nw;
}

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) {
        أكد(x);
        أكد(s || l == 0);

        إذا (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        إذا (l > 0 || !*x) {
                size_t q;
                حرف *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                إذا (!m)
                        أرجع عدم;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        أرجع *x;
}

حرف* strstrip(حرف *s) {
        إذا (!s)
                أرجع عدم;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        أرجع delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

حرف* delete_chars(حرف *s, ثابت حرف *bad) {
        حرف *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        إذا (!s)
                أرجع عدم;

        إذا (!bad)
                bad = WHITESPACE;

        لكل (f = s, t = s; *f; f++) {
                إذا (strchr(bad, *f))
                        استمر;

                *(t++) = *f;
        }

        *t = 0;

        أرجع s;
}

حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad) {
        حرف *c = s;

        /* Drops all specified bad characters, at the end of the string */

        إذا (!s)
                أرجع عدم;

        إذا (!bad)
                bad = WHITESPACE;

        لكل (حرف *p = s; *p; p++)
                إذا (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        أرجع s;
}

حرف* truncate_nl_پر(حرف *s, size_t *ret_len) {
        size_t n;

        أكد(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        إذا (ret_len)
                *ret_len = n;
        أرجع s;
}

حرف ascii_tolower(حرف x) {

        إذا (x >= 'A' && x <= 'Z')
                أرجع x - 'A' + 'a';

        أرجع x;
}

حرف ascii_toupper(حرف x) {

        إذا (x >= 'a' && x <= 'z')
                أرجع x - 'a' + 'A';

        أرجع x;
}

حرف* ascii_strlower(حرف *s) {
        أكد(s);

        لكل (حرف *p = s; *p; p++)
                *p = ascii_tolower(*p);

        أرجع s;
}

حرف* ascii_strupper(حرف *s) {
        أكد(s);

        لكل (حرف *p = s; *p; p++)
                *p = ascii_toupper(*p);

        أرجع s;
}

حرف* ascii_strlower_n(حرف *s, size_t n) {
        أكد(n <= 0 || s);

        إذا (n <= 0)
                أرجع s;

        لكل (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        أرجع s;
}

صحيح ascii_strcasecmp_n(ثابت حرف *a, ثابت حرف *b, size_t n) {

        أكد(a);
        أكد(b);

        لكل (; n > 0; a++, b++, n--) {
                صحيح x, y;

                x = (صحيح) (uint8_t) ascii_tolower(*a);
                y = (صحيح) (uint8_t) ascii_tolower(*b);

                إذا (x != y)
                        أرجع x - y;
        }

        أرجع 0;
}

صحيح ascii_strcasecmp_nn(ثابت حرف *a, size_t n, ثابت حرف *b, size_t m) {
        صحيح r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        إذا (r != 0)
                أرجع r;

        أرجع CMP(n, m);
}

bool chars_intersect(ثابت حرف *a, ثابت حرف *b) {
        /* Returns true if any of the chars in a are in b. */
        لكل (ثابت حرف *p = a; *p; p++)
                إذا (strchr(b, *p))
                        أرجع true;

        أرجع false;
}

bool string_has_cc(ثابت حرف *p, ثابت حرف *ok) {
        أكد(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        لكل (ثابت حرف *t = p; *t; t++) {
                إذا (ok && strchr(ok, *t))
                        استمر;

                إذا (char_is_cc(*t))
                        أرجع true;
        }

        أرجع false;
}

ساكن صحيح write_ellipsis(حرف *buf, bool unicode) {
        ثابت حرف *s = glyph_پر(GLYPH_ELLIPSIS, unicode);
        أكد(طول_رشته(s) == 3);
        کپی_حافظه(buf, s, 3);
        أرجع 3;
}

ساكن size_t ansi_sequence_درازی(ثابت حرف *s, size_t len) {
        أكد(s);

        إذا (len < 2)
                أرجع 0;

        إذا (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                أرجع 0;  /* Not the start of a sequence */

        إذا (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                إذا (i == len)
                        أرجع 0;

                طالما (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        إذا (++i == len)
                                أرجع 0;
                طالما (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        إذا (++i == len)
                                أرجع 0;
                إذا (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        أرجع i + 1;
                أرجع 0;  /* Bad sequence */

        } وإلا إذا (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                أرجع 2;

        أرجع 0;  /* Bad escape? */
}

ساكن bool string_has_ansi_sequence(ثابت حرف *s, size_t len) {
        ثابت حرف *t = s;

        طالما ((t = memchr(t, 0x1B, len - (t - s)))) {
                إذا (ansi_sequence_درازی(t, len - (t - s)) > 0)
                        أرجع true;
                t++;
        }
        أرجع false;
}

ساكن size_t قبلی_ansi_sequence(ثابت حرف *s, size_t length, ثابت حرف **ret_where) {

        أكد(s);
        أكد(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        إذا (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = عدم;
                أرجع 0;
        }

        لكل (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_درازی(s + i, length - i);
                إذا (slen > 0) {
                        *ret_where = s + i;
                        أرجع slen;
                }

                إذا (i == 0)
                        اكسر;
        }

        *ret_where = عدم;
        أرجع 0;
}

ساكن حرف *ascii_ellipsize_mem(ثابت حرف *s, size_t old_درازی, size_t new_درازی, بیعلامت percent) {
        size_t x, need_space, suffix_len;
        حرف *t;

        أكد(s);
        أكد(percent <= 100);
        أكد(new_درازی != SIZE_MAX);

        إذا (old_درازی <= new_درازی)
                أرجع strndup(s, old_درازی);

        /* Special case short ellipsations */
        بدل (new_درازی) {

        حالة 0:
                أرجع strdup("");

        حالة 1:
                إذا (is_locale_utf8())
                        أرجع strdup("…");
                وإلا
                        أرجع strdup(".");

        حالة 2:
                إذا (!is_locale_utf8())
                        أرجع strdup("..");
                اكسر;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(حرف, new_درازی+3);
        إذا (!t)
                أرجع عدم;

        أكد(new_درازی >= need_space);

        x = ((new_درازی - need_space) * percent + 50) / 100;
        أكد(x <= new_درازی - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_درازی - x - need_space;
        کپی_حافظه(t + x + 3, s + old_درازی - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        أرجع t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
ساكن ثابت حرف* پیدا_کن_قبلی_unichar(ثابت حرف *شروع, ثابت حرف *انتها, char32_t *ret_c) {
        ثابت حرف *p;
        صحيح r;

        أكد(شروع);
        أكد(انتها);
        أكد(انتها > شروع);

        لكل (p = انتها; p > شروع; ) {
                p--;
                إذا (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        اكسر;
        }

        r = utf8_encoded_valid_unichar(p, انتها - p);
        إذا (r < 0 || p + r != انتها)
                أرجع عدم;

        إذا (ret_c)
                assert_se(utf8_encoded_مقصد_unichar(p, ret_c) == r);

        أرجع p;
}

حرف* ellipsize_mem(ثابت حرف *s, size_t old_درازی, size_t new_درازی, بیعلامت percent) {
        size_t x, k, len, len2;
        ثابت حرف *i, *j;
        صحيح r;

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

        إذا (new_درازی == SIZE_MAX)
                أرجع strndup(s, old_درازی);

        إذا (new_درازی == 0)
                أرجع strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_درازی);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        إذا (!has_ansi_seq && ascii_is_valid_n(s, old_درازی) && !memchr(s, '\t', old_درازی))
                أرجع ascii_ellipsize_mem(s, old_درازی, new_درازی, percent);

        x = (new_درازی - 1) * percent / 100;
        أكد(x <= new_درازی - 1);

        k = 0;
        لكل (i = s; i < s + old_درازی; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_درازی(i, old_درازی - (i - s)) : 0;
                إذا (slen > 0) {
                        i += slen;
                        استمر;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_درازی - i);
                إذا (r < 0)
                        أرجع عدم;

                char32_t c;
                assert_se(utf8_encoded_مقصد_unichar(i, &c) == r);

                صحيح w = unichar_console_عرض(c);
                إذا (k + w > x)
                        اكسر;

                k += w;
                i += r;
        }

        ثابت حرف *ansi_شروع = s + old_درازی;
        size_t ansi_len = 0;

        لكل (ثابت حرف *t = j = s + old_درازی; t > i && k < new_درازی; ) {
                char32_t c;
                صحيح w;
                ثابت حرف *tt;

                إذا (has_ansi_seq && ansi_شروع >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = قبلی_ansi_sequence(s, t - s, &ansi_شروع);

                /* If the sequence extends all the way to the current position, skip it. */
                إذا (has_ansi_seq && ansi_len > 0 && ansi_شروع + ansi_len == t) {
                        t = ansi_شروع;
                        استمر;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = پیدا_کن_قبلی_unichar(i, t, &c);
                إذا (!tt)
                        أرجع عدم;

                w = unichar_console_عرض(c);
                إذا (k + w > new_درازی)
                        اكسر;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        إذا (i >= j)
                أرجع memdup_suffix0(s, old_درازی);

        إذا (k >= new_درازی) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                إذا (j < s + old_درازی) {
                        r = utf8_encoded_valid_unichar(j, s + old_درازی - j);
                        إذا (r < 0)
                                أرجع عدم;

                        j += r;
                } وإلا إذا (i > s) {
                        ثابت حرف *tt = پیدا_کن_قبلی_unichar(s, i, عدم);
                        إذا (!tt)
                                أرجع عدم;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_درازی - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_درازی + 3 + 1 : len + 3 + len2 + 1;

        حرف *e = new(حرف, alloc_len);
        إذا (!e)
                أرجع عدم;

        memcpy_امن(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        حرف *dst = e + len + 3;

        إذا (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                لكل (ثابت حرف *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_درازی(p, j - p);
                        إذا (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } وإلا
                                p = utf8_بعدی_char(p);
                }

        memcpy_امن(dst, j, len2);
        dst[len2] = '\0';

        أرجع e;
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

        size_t i = 0, آخرین_char_عرض[4] = {}, k = 0;

        أكد(buf);
        أكد(len > 0); /* at least a terminating NUL */
        أكد(s);

        لكل (;;) {
                حرف four[4];
                صحيح w;

                إذا (*s == 0) /* terminating NUL detected? then we are done! */
                        اذهب انجامشده;

                w = cescape_char(*s, four);
                إذا (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        اكسر;

                /* OK, there was space, let's add this escaped character to the buffer */
                کپی_حافظه(buf + i, four, w);
                i += w;

                /* And remember its width in the ring buffer */
                آخرین_char_عرض[k] = w;
                k = (k + 1) % 4;

                s++;
        }

        /* Ellipsation is necessary. This means we might need to truncate the string again to make space for 4
         * characters ideally, but the buffer is shorter than that in the first place take what we can get */
        لكل (size_t j = 0; j < ELEMENTSOF(آخرین_char_عرض); j++) {

                إذا (i + 4 <= len) /* nice, we reached our space goal */
                        اكسر;

                k = k == 0 ? 3 : k - 1;
                إذا (آخرین_char_عرض[k] == 0) /* bummer, we reached the beginning of the strings */
                        اكسر;

                أكد(i >= آخرین_char_عرض[k]);
                i -= آخرین_char_عرض[k];
        }

        إذا (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        وإلا إذا (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } وإلا إذا (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        وإلا
                أكد(i + 1 <= len);

انجامشده:
        buf[i] = '\0';
        أرجع buf;
}

حرف* strshorten(حرف *s, size_t l) {
        أكد(s);

        إذا (l >= SIZE_MAX-1) /* Would not change anything */
                أرجع s;

        إذا (strnlen(s, l+1) > l)
                s[l] = 0;

        أرجع s;
}

صحيح strgrowpad0(حرف **s, size_t l) {
        size_t sz;

        أكد(s);

        إذا (*s) {
                sz = طول_رشته(*s) + 1;
                إذا (sz >= l) /* never shrink */
                        أرجع 0;
        } وإلا
                sz = 0;

        حرف *q = realloc(*s, l);
        إذا (!q)
                أرجع -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        أرجع 0;
}

حرف* strreplace(ثابت حرف *متن, ثابت حرف *old_string, ثابت حرف *new_string) {
        size_t l, old_len, new_len;
        حرف *t, *ret = عدم;
        ثابت حرف *f;

        أكد(old_string);
        أكد(new_string);

        إذا (!متن)
                أرجع عدم;

        old_len = طول_رشته(old_string);
        new_len = طول_رشته(new_string);

        l = طول_رشته(متن);
        إذا (!GREEDY_REALLOC(ret, l+1))
                أرجع عدم;

        f = متن;
        t = ret;
        طالما (*f) {
                size_t d, nl;

                إذا (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        استمر;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                إذا (!GREEDY_REALLOC(ret, nl + 1))
                        أرجع mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        أرجع ret;
}

ساكن فراغ advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t لغزش[ساكن 2],
                size_t اندازه) {

        إذا (!offsets)
                أرجع;

        أكد(لغزش);

        إذا ((size_t) diff < offsets[0])
                لغزش[0] += اندازه;
        إذا ((size_t) diff < offsets[1])
                لغزش[1] += اندازه;
}

ساكن فراغ retract_offsets(
                size_t begin,
                size_t انتها,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t لغزش[ساكن 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        إذا (!offsets)
                أرجع;

        أكد(لغزش);
        أكد(begin <= انتها);

        لكل (size_t k = 0; k < 2; k++)
                إذا (offsets[k] > begin)
                        لغزش[k] -= MIN(offsets[k], انتها) - begin;
}

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_انجامشده) AnsiSeqParser parser = {};
        _cleanup_(memstream_انجامشده) Mem_جریان m = {};
        ثابت حرف *begin = عدم;
        size_t isz, لغزش[2] = {}, n_carriage_returns = 0;
        FILE *f;

        أكد(ibuf);
        أكد(*ibuf);
        اشاره_گر_MAY_BE_NULL(_isz);

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

        isz = _isz ? *_isz : طول_رشته(*ibuf);

        /* Note we turn off internal locking on f for performance reasons. It's safe to do so since we
         * created f here and it doesn't leave our scope. */
        f = memstream_init(&m);
        إذا (!f)
                أرجع عدم;

        لكل (ثابت حرف *i = *ibuf; i < *ibuf + isz; i++) {
                صحيح e;

                لكل (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        إذا (e < 0)
                                أرجع عدم;
                        إذا (e != ANSI_SEQ_EVENT_ABORT)
                                اكسر;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        إذا (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, لغزش);
                                begin = عدم;
                        }
                }

                بدل (e) {

                حالة ANSI_SEQ_EVENT_متن:
                        إذا (*i == '\r') {
                                n_carriage_returns++;
                                استمر;
                        }
                        إذا (*i == '\n') {
                                /* Ignore carriage returns before new line. They are dropped from the output,
                                 * hence the offsets need to account for them. (Note that pending carriage
                                 * returns always immediately precede the current character, since anything
                                 * else flushes them.) */
                                retract_offsets(
                                                (size_t) (i - *ibuf) - n_carriage_returns,
                                                i - *ibuf,
                                                highlight,
                                                لغزش);
                                n_carriage_returns = 0;
                        }
                        لكل (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        إذا (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, لغزش, 7);
                        } وإلا
                                fputc(*i, f);

                        اكسر;

                حالة ANSI_SEQ_EVENT_SEQUENCE:
                        إذا (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                لكل (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        اكسر;

                حالة ANSI_SEQ_EVENT_انتها:
                        /* A complete sequence, drop it (the current character is its last one) */
                        أكد(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, لغزش);
                        begin = عدم;
                        اكسر;

                افتراضي:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        إذا (begin)
                retract_offsets(begin - *ibuf, isz, highlight, لغزش);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, لغزش);

        حرف *obuf;
        إذا (memstream_finalize(&m, &obuf, _isz) < 0)
                أرجع عدم;

        free_and_replace(*ibuf, obuf);

        إذا (highlight) {
                highlight[0] += لغزش[0];
                highlight[1] += لغزش[1];
        }

        أرجع *ibuf;
}

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap) {
        _cleanup_free_ حرف *میانگیر = عدم;
        size_t f, l, l_separator;
        bool need_separator;
        حرف *nr, *p;

        إذا (!x)
                x = &میانگیر;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        لكل (ثابت حرف *t;;) {
                size_t n;

                t = va_arg(aq, ثابت حرف *);
                إذا (!t)
                        اكسر;
                إذا (t == اشاره_گر_MAX)
                        استمر;

                n = طول_رشته(t);

                إذا (need_separator)
                        n += l_separator;

                إذا (n >= SIZE_MAX - l) {
                        va_end(aq);
                        أرجع عدم;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_دور_به_بالا(l+1));
        إذا (!nr)
                أرجع عدم;

        *x = nr;
        p = nr + f;

        لكل (;;) {
                ثابت حرف *t;

                t = va_arg(ap, ثابت حرف *);
                إذا (!t)
                        اكسر;
                إذا (t == اشاره_گر_MAX)
                        استمر;

                إذا (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        أكد(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        إذا (میانگیر)
                أرجع TAKE_PTR(میانگیر);

        /* Otherwise we extended the buffer: return the end */
        أرجع p;
}

حرف* strextend_with_separator_داخلی(حرف **x, ثابت حرف *separator, ...) {
        va_list ap;
        حرف *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        أرجع ret;
}

صحيح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        صحيح l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        أكد(x);
        أكد(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        إذا (*x) {
                m = طول_رشته(*x);
                a = MALLOC_SIZEOF_امن(*x);
                أكد(a >= m + 1);
        } وإلا
                m = a = 0;

        إذا (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                حرف *n;

                إذا (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        أرجع -ENOMEM;
                إذا (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        أرجع -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                إذا (!n)
                        أرجع -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_امن(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_امن(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        أكد(l >= 0);

        إذا ((size_t) l < a - m - l_separator) {
                حرف *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                إذا (n)
                        *x = n;
        } وإلا {
                حرف *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                إذا (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        اذهب oom;
                إذا (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        اذهب oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                إذا (!n)
                        اذهب oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                أكد((size_t) l < a - m - l_separator);
        }

        أرجع 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        أرجع -ENOMEM;
}

حرف* strrep(ثابت حرف *s, size_t n) {
        حرف *ret, *p;
        size_t l;

        أكد(s);

        l = طول_رشته(s);
        إذا (!MUL_تخصیص_بده_امن(&l, n))
                أرجع عدم;
        إذا (!INC_امن(&l, 1))
                أرجع عدم;

        p = ret = حافظه_بده(l);
        إذا (!ret)
                أرجع عدم;

        لكل (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        أرجع ret;
}

صحيح تقسیم_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_اولی, حرف **ret_دومی) {
        أكد(s);
        أكد(!isempty(sep));

        ثابت حرف *x = جستجوی_رشته(s, sep);
        إذا (!x)
                أرجع -EINVAL;

        _cleanup_free_ حرف *a = عدم;
        إذا (ret_اولی) {
                a = strndup(s, x - s);
                إذا (!a)
                        أرجع -ENOMEM;
        }

        _cleanup_free_ حرف *b = عدم;
        إذا (ret_دومی) {
                b = strdup(x + طول_رشته(sep));
                إذا (!b)
                        أرجع -ENOMEM;
        }

        إذا (ret_اولی)
                *ret_اولی = TAKE_PTR(a);
        إذا (ret_دومی)
                *ret_دومی = TAKE_PTR(b);
        أرجع 0;
}

صحيح free_and_strdup(حرف **p, ثابت حرف *s) {
        حرف *t;

        أكد(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        إذا (streq_ptr(*p, s))
                أرجع 0;

        إذا (s) {
                t = strdup(s);
                إذا (!t)
                        أرجع -ENOMEM;
        } وإلا
                t = عدم;

        free_and_replace(*p, t);

        أرجع 1;
}

صحيح free_and_strdup_warn(حرف **p, ثابت حرف *s) {
        صحيح r;

        r = free_and_strdup(p, s);
        إذا (r < 0)
                أرجع log_oom();
        أرجع r;
}

صحيح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) {
        حرف *t;

        أكد(p);
        أكد(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        إذا (!*p && !s)
                أرجع 0;

        إذا (*p && s && strneq(*p, s, l) && (l > طول_رشته(*p) || (*p)[l] == '\0'))
                أرجع 0;

        إذا (s) {
                t = strndup(s, l);
                إذا (!t)
                        أرجع -ENOMEM;
        } وإلا
                t = عدم;

        free_and_replace(*p, t);
        أرجع 1;
}

صحيح strdup_مقصد_پر(حرف **ret, ثابت حرف *src) {
        إذا (!src) {
                إذا (ret)
                        *ret = عدم;

                أرجع 0;
        } وإلا {
                إذا (ret) {
                        حرف *t = strdup(src);
                        إذا (!t)
                                أرجع -ENOMEM;
                        *ret = t;
                }

                أرجع 1;
        }
};

bool string_is_امن(ثابت حرف *p, String_امن_Flags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        إذا (FLAGS_مجموعه(flags, STRING_ALLOW_خالی) ? !p : isempty(p))
                أرجع false;

        إذا (!FLAGS_مجموعه(flags, STRING_ASCII) && !utf8_is_valid(p))
                أرجع false;

        لكل (ثابت حرف *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                إذا ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        أرجع false;

                إذا (!FLAGS_مجموعه(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        أرجع false;

                إذا (!FLAGS_مجموعه(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        أرجع false;

                إذا (!FLAGS_مجموعه(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        أرجع false;

                إذا (!FLAGS_مجموعه(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        أرجع false;

                إذا (FLAGS_مجموعه(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        أرجع false;

                إذا (FLAGS_مجموعه(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        أرجع false;
        }

        إذا (FLAGS_مجموعه(flags, STRING_FILENAME) && !filename_is_valid(p))
                أرجع false;

        إذا (FLAGS_مجموعه(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                أرجع false;

        أرجع true;
}

حرف* str_realloc(حرف *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        إذا (!p)
                أرجع عدم;

        أرجع realloc(p, طول_رشته(p) + 1) ?: p;
}

حرف* string_erase(حرف *x) {
        إذا (!x)
                أرجع عدم;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_امن(x, طول_رشته(x));
        أرجع x;
}

صحيح string_truncate_سطرها(ثابت حرف *s, size_t n_سطرها, حرف **ret) {
        ثابت حرف *p = s, *e = s;
        bool truncation_applied = false;
        حرف *copy;
        size_t n = 0;

        أكد(s);
        أكد(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        لكل (;;) {
                size_t k;

                k = strcspn(p, "\n");

                إذا (p[k] == 0) {
                        إذا (k == 0) /* final empty line */
                                اكسر;

                        إذا (n >= n_سطرها) /* above threshold */
                                اكسر;

                        e = p + k; /* last line to include */
                        اكسر;
                }

                أكد(p[k] == '\n');

                إذا (n >= n_سطرها)
                        اكسر;

                إذا (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        إذا (isempty(e))
                copy = strdup(s);
        وإلا {
                إذا (!درون_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        إذا (!copy)
                أرجع -ENOMEM;

        *ret = copy;
        أرجع truncation_applied;
}

صحيح string_استخراج_سطر(ثابت حرف *s, size_t i, حرف **ret) {
        ثابت حرف *p = s;
        size_t c = 0;

        أكد(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        لكل (;;) {
                ثابت حرف *q;

                q = strchr(p, '\n');
                إذا (i == c) {
                        /* The line we are looking for! */

                        إذا (q) {
                                حرف *m;

                                m = strndup(p, q - p);
                                إذا (!m)
                                        أرجع -ENOMEM;

                                *ret = m;
                                أرجع !isempty(q + 1); /* More coming? */
                        } وإلا
                                /* Tell the caller to use the input string if equal */
                                أرجع strdup_مقصد(ret, p != s ? p : عدم);
                }

                إذا (!q)
                        /* No more lines, return empty line */
                        أرجع strdup_مقصد(ret, "");

                p = q + 1;
                c++;
        }
}

صحيح string_شامل_است_کلمه_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *واژگان, ثابت حرف **ret_کلمه) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        ثابت استخراج_Flags flags = separators ? استخراج_DONT_COALESCE_SEPARATORS : 0;
        ثابت حرف *یافت = عدم;
        صحيح r;

        لكل (;;) {
                _cleanup_free_ حرف *w = عدم;

                r = استخراج_اولی_کلمه(&string, &w, separators, flags);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                یافت = strv_پیدا_کن(واژگان, w);
                إذا (یافت)
                        اكسر;
        }

        إذا (ret_کلمه)
                *ret_کلمه = یافت;
        أرجع !!یافت;
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok) {
        إذا (!s1 && !s2)
                أرجع true;
        إذا (!s1 || !s2)
                أرجع false;

        إذا (!ok)
                ok = WHITESPACE;

        لكل (; *s1 && *s2; s1++, s2++)
                إذا (*s1 != *s2)
                        اكسر;

        أرجع درون_charset(s1, ok) && درون_charset(s2, ok);
}

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char) {
        أكد(str);
        أكد(old_char != '\0');
        أكد(new_char != '\0');
        أكد(old_char != new_char);

        لكل (حرف *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        أرجع str;
}

صحيح make_cstring(ثابت فراغ *s, size_t n, Make_CString_نما نما, حرف **ret) {
        حرف *b;

        أكد(s || n == 0);
        أكد(نما >= 0);
        أكد(نما < MAKE_CSTRING_نما_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        إذا (n == 0) {
                إذا (نما == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        أرجع -EINVAL;

                إذا (!ret)
                        أرجع 0;

                b = new0(حرف, 1);
        } وإلا {
                ثابت uint8_t *nul;

                nul = memchr(s, 0, n);
                إذا (nul) {
                        إذا (nul < (ثابت uint8_t*) s + n - 1 || /* embedded NUL? */
                            نما == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                أرجع -EINVAL;

                        n--;
                } وإلا إذا (نما == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        أرجع -EINVAL;

                إذا (!ret)
                        أرجع 0;

                b = memdup_suffix0(s, n);
        }
        إذا (!b)
                أرجع -ENOMEM;

        *ret = b;
        أرجع 0;
}

size_t strspn_from_انتها(ثابت حرف *str, ثابت حرف *accept) {
        size_t n = 0;

        إذا (isempty(str))
                أرجع 0;

        إذا (isempty(accept))
                أرجع 0;

        لكل (ثابت حرف *p = str + طول_رشته(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        أرجع n;
}

size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        أكد(str || n == 0);
        أكد(accept);

        لكل (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        أرجع i;
}

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept) {
        إذا (isempty(a) || isempty(accept))
                أرجع strdup("");

        أرجع strndup(a, strspn(a, accept));
}

حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject) {
        إذا (isempty(a))
                أرجع strdup("");
        إذا (isempty(reject))
                أرجع strdup(a);

        أرجع strndup(a, strcspn(a, reject));
}

حرف* پیدا_کن_سطر_startswith_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        حرف *p = (حرف*) جستجوی_رشته(haystack, needle);
        إذا (!p)
                أرجع عدم;

        إذا (p > haystack)
                طالما (p[-1] != '\n') {
                        p = جستجوی_رشته(p + 1, needle);
                        إذا (!p)
                                أرجع عدم;
                }

        أرجع p + طول_رشته(needle);
}

حرف* پیدا_کن_سطر_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        حرف *p = (حرف*) پیدا_کن_سطر_startswith(haystack, needle);
        إذا (!p)
                أرجع عدم;

        إذا (*p == 0 || strchr(NEWLINE, *p))
                أرجع p - طول_رشته(needle);

        أرجع عدم;
}

حرف* پیدا_کن_سطر_after_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        أكد(haystack);
        أكد(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        حرف *p = (حرف*) پیدا_کن_سطر_startswith(haystack, needle);
        إذا (!p)
                أرجع عدم;

        إذا (*p == 0)
                أرجع p;
        إذا (strchr(NEWLINE, *p))
                أرجع p + 1;

        أرجع عدم;
}

bool نسخه_is_valid(ثابت حرف *s, نسخه_Flags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        إذا (FLAGS_مجموعه(flags, نسخه_ALLOW_خالی) ? !s : isempty(s))
                أرجع false;

        إذا (!filename_part_is_valid(s))
                أرجع false;

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
        size_t l = طول_رشته(charset);

        إذا (FLAGS_مجموعه(flags, نسخه_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        إذا (FLAGS_مجموعه(flags, نسخه_ALLOW_PLUS))
                charset[l++] = '+';

        أرجع درون_charset(s, charset);
}

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y) {
        _cleanup_free_ size_t *t0 = عدم, *t1 = عدم, *t2 = عدم;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        إذا (streq_ptr(x, y))
                أرجع 0;

        xl = strlen_ptr(x);
        إذا (xl > SSIZE_MAX)
                أرجع -E2BIG;

        yl = strlen_ptr(y);
        إذا (yl > SSIZE_MAX)
                أرجع -E2BIG;

        إذا (isempty(x))
                أرجع yl;
        إذا (isempty(y))
                أرجع xl;

        t0 = new0(size_t, yl + 1);
        إذا (!t0)
                أرجع -ENOMEM;
        t1 = new0(size_t, yl + 1);
        إذا (!t1)
                أرجع -ENOMEM;
        t2 = new0(size_t, yl + 1);
        إذا (!t2)
                أرجع -ENOMEM;

        لكل (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        لكل (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                لكل (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /*جابجا*/
                        إذا (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        إذا (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        إذا (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        أرجع t1[yl];
}

حرف* strrstr_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        إذا (!haystack || !needle)
                أرجع عدم;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        إذا (needle[0] == 0)
                أرجع (حرف*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        إذا (needle[1] == 0)
                أرجع (حرف*) strrchr(haystack, needle[0]);

        لكل (ثابت حرف *p = جستجوی_رشته(haystack, needle), *q; p; p = q) {
                q = جستجوی_رشته(p + 1, needle);
                إذا (!q)
                        أرجع (حرف*) p;
        }
        أرجع عدم;
}

حرف* strrstr_no_case_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        إذا (!haystack || !needle)
                أرجع عدم;

        لكل (ثابت حرف *p = strchr(haystack, 0); p > haystack; p--)
                إذا (startswith_no_case(p, needle))
                        أرجع (حرف*) p;

        أرجع startswith_no_case(haystack, needle) ? (حرف*) haystack : عدم;
}

size_t str_common_پیشوند(ثابت حرف *a, ثابت حرف *b) {
        أكد(a);
        أكد(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        لكل (size_t n = 0;; n++) {
                حرف c = a[n];
                إذا (c != b[n])
                        أرجع n;
                إذا (c == 0)
                        أرجع SIZE_MAX;
        }
}
