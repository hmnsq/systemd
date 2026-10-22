/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "ansi-seq.h"
#include "escape.h"
#include "extract-word.h"
#include "glyph-util.h"
#include "gunicode.h"
#include "locale-util.h"
#include "log.h"
#include "memory-util.h"
#include "memstream-util.h"
#include "path-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

var_na* first_word(const var_na *s, const var_na *word) {
        assert(s);
        assert(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        ถ้า (isempty(word))
                คืน (var_na*) s;

        const var_na *p = startswith(s, word);
        ถ้า (!p)
                คืน NULL;
        ถ้า (*p == '\0')
                คืน (var_na*) p;

        const var_na *nw = skip_leading_chars(p, WHITESPACE);
        ถ้า (p == nw)
                คืน NULL;

        คืน (var_na*) nw;
}

var_na* strextendn(var_na **x, const var_na *s, size_t l) {
        assert(x);
        assert(s || l == 0);

        ถ้า (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        ถ้า (l > 0 || !*x) {
                size_t q;
                var_na *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                ถ้า (!m)
                        คืน NULL;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        คืน *x;
}

var_na* strstrip(var_na *s) {
        ถ้า (!s)
                คืน NULL;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        คืน delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

var_na* delete_chars(var_na *s, const var_na *bad) {
        var_na *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        ถ้า (!s)
                คืน NULL;

        ถ้า (!bad)
                bad = WHITESPACE;

        สำหรับ (f = s, t = s; *f; f++) {
                ถ้า (strchr(bad, *f))
                        continue;

                *(t++) = *f;
        }

        *t = 0;

        คืน s;
}

var_na* delete_trailing_chars(var_na *s, const var_na *bad) {
        var_na *c = s;

        /* Drops all specified bad characters, at the end of the string */

        ถ้า (!s)
                คืน NULL;

        ถ้า (!bad)
                bad = WHITESPACE;

        สำหรับ (var_na *p = s; *p; p++)
                ถ้า (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        คืน s;
}

var_na* truncate_nl_full(var_na *s, size_t *ret_len) {
        size_t n;

        assert(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        ถ้า (ret_len)
                *ret_len = n;
        คืน s;
}

var_na ascii_tolower(var_na x) {

        ถ้า (x >= 'A' && x <= 'Z')
                คืน x - 'A' + 'a';

        คืน x;
}

var_na ascii_toupper(var_na x) {

        ถ้า (x >= 'a' && x <= 'z')
                คืน x - 'a' + 'A';

        คืน x;
}

var_na* ascii_strlower(var_na *s) {
        assert(s);

        สำหรับ (var_na *p = s; *p; p++)
                *p = ascii_tolower(*p);

        คืน s;
}

var_na* ascii_strupper(var_na *s) {
        assert(s);

        สำหรับ (var_na *p = s; *p; p++)
                *p = ascii_toupper(*p);

        คืน s;
}

var_na* ascii_strlower_n(var_na *s, size_t n) {
        assert(n <= 0 || s);

        ถ้า (n <= 0)
                คืน s;

        สำหรับ (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        คืน s;
}

จำนวนเต็ม ascii_strcasecmp_n(const var_na *a, const var_na *b, size_t n) {

        assert(a);
        assert(b);

        สำหรับ (; n > 0; a++, b++, n--) {
                จำนวนเต็ม x, y;

                x = (จำนวนเต็ม) (uint8_t) ascii_tolower(*a);
                y = (จำนวนเต็ม) (uint8_t) ascii_tolower(*b);

                ถ้า (x != y)
                        คืน x - y;
        }

        คืน 0;
}

จำนวนเต็ม ascii_strcasecmp_nn(const var_na *a, size_t n, const var_na *b, size_t m) {
        จำนวนเต็ม r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        ถ้า (r != 0)
                คืน r;

        คืน CMP(n, m);
}

bool chars_intersect(const var_na *a, const var_na *b) {
        /* Returns true if any of the chars in a are in b. */
        สำหรับ (const var_na *p = a; *p; p++)
                ถ้า (strchr(b, *p))
                        คืน true;

        คืน false;
}

bool string_has_cc(const var_na *p, const var_na *ok) {
        assert(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        สำหรับ (const var_na *t = p; *t; t++) {
                ถ้า (ok && strchr(ok, *t))
                        continue;

                ถ้า (char_is_cc(*t))
                        คืน true;
        }

        คืน false;
}

static จำนวนเต็ม write_ellipsis(var_na *buf, bool unicode) {
        const var_na *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        assert(strlen(s) == 3);
        memcpy(buf, s, 3);
        คืน 3;
}

static size_t ansi_sequence_length(const var_na *s, size_t len) {
        assert(s);

        ถ้า (len < 2)
                คืน 0;

        ถ้า (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                คืน 0;  /* Not the start of a sequence */

        ถ้า (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                ถ้า (i == len)
                        คืน 0;

                ขณะ (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        ถ้า (++i == len)
                                คืน 0;
                ขณะ (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        ถ้า (++i == len)
                                คืน 0;
                ถ้า (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        คืน i + 1;
                คืน 0;  /* Bad sequence */

        } มิฉะนั้น ถ้า (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                คืน 2;

        คืน 0;  /* Bad escape? */
}

static bool string_has_ansi_sequence(const var_na *s, size_t len) {
        const var_na *t = s;

        ขณะ ((t = memchr(t, 0x1B, len - (t - s)))) {
                ถ้า (ansi_sequence_length(t, len - (t - s)) > 0)
                        คืน true;
                t++;
        }
        คืน false;
}

static size_t previous_ansi_sequence(const var_na *s, size_t length, const var_na **ret_where) {

        assert(s);
        assert(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        ถ้า (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = NULL;
                คืน 0;
        }

        สำหรับ (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                ถ้า (slen > 0) {
                        *ret_where = s + i;
                        คืน slen;
                }

                ถ้า (i == 0)
                        bha_nga;
        }

        *ret_where = NULL;
        คืน 0;
}

static var_na *ascii_ellipsize_mem(const var_na *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        var_na *t;

        assert(s);
        assert(percent <= 100);
        assert(new_length != SIZE_MAX);

        ถ้า (old_length <= new_length)
                คืน strndup(s, old_length);

        /* Special case short ellipsations */
        switch (new_length) {

        pak_sa 0:
                คืน strdup("");

        pak_sa 1:
                ถ้า (is_locale_utf8())
                        คืน strdup("…");
                มิฉะนั้น
                        คืน strdup(".");

        pak_sa 2:
                ถ้า (!is_locale_utf8())
                        คืน strdup("..");
                bha_nga;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(var_na, new_length+3);
        ถ้า (!t)
                คืน NULL;

        assert(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        assert(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        คืน t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
static const var_na* find_previous_unichar(const var_na *start, const var_na *end, char32_t *ret_c) {
        const var_na *p;
        จำนวนเต็ม r;

        assert(start);
        assert(end);
        assert(end > start);

        สำหรับ (p = end; p > start; ) {
                p--;
                ถ้า (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        bha_nga;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        ถ้า (r < 0 || p + r != end)
                คืน NULL;

        ถ้า (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        คืน p;
}

var_na* ellipsize_mem(const var_na *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        const var_na *i, *j;
        จำนวนเต็ม r;

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

        assert(s);
        assert(percent <= 100);

        ถ้า (new_length == SIZE_MAX)
                คืน strndup(s, old_length);

        ถ้า (new_length == 0)
                คืน strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        ถ้า (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                คืน ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        assert(x <= new_length - 1);

        k = 0;
        สำหรับ (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                ถ้า (slen > 0) {
                        i += slen;
                        continue;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                ถ้า (r < 0)
                        คืน NULL;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                จำนวนเต็ม w = unichar_console_width(c);
                ถ้า (k + w > x)
                        bha_nga;

                k += w;
                i += r;
        }

        const var_na *ansi_start = s + old_length;
        size_t ansi_len = 0;

        สำหรับ (const var_na *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                จำนวนเต็ม w;
                const var_na *tt;

                ถ้า (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                ถ้า (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        continue;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                ถ้า (!tt)
                        คืน NULL;

                w = unichar_console_width(c);
                ถ้า (k + w > new_length)
                        bha_nga;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        ถ้า (i >= j)
                คืน memdup_suffix0(s, old_length);

        ถ้า (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                ถ้า (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        ถ้า (r < 0)
                                คืน NULL;

                        j += r;
                } มิฉะนั้น ถ้า (i > s) {
                        const var_na *tt = find_previous_unichar(s, i, NULL);
                        ถ้า (!tt)
                                คืน NULL;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        var_na *e = new(var_na, alloc_len);
        ถ้า (!e)
                คืน NULL;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        var_na *dst = e + len + 3;

        ถ้า (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                สำหรับ (const var_na *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        ถ้า (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } มิฉะนั้น
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        คืน e;
}

var_na* cellescape(var_na *buf, size_t len, const var_na *s) {
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

        assert(buf);
        assert(len > 0); /* at least a terminating NUL */
        assert(s);

        สำหรับ (;;) {
                var_na four[4];
                จำนวนเต็ม w;

                ถ้า (*s == 0) /* terminating NUL detected? then we are done! */
                        goto done;

                w = cescape_char(*s, four);
                ถ้า (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        bha_nga;

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
        สำหรับ (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                ถ้า (i + 4 <= len) /* nice, we reached our space goal */
                        bha_nga;

                k = k == 0 ? 3 : k - 1;
                ถ้า (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        bha_nga;

                assert(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        ถ้า (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        มิฉะนั้น ถ้า (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } มิฉะนั้น ถ้า (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        มิฉะนั้น
                assert(i + 1 <= len);

done:
        buf[i] = '\0';
        คืน buf;
}

var_na* strshorten(var_na *s, size_t l) {
        assert(s);

        ถ้า (l >= SIZE_MAX-1) /* Would not change anything */
                คืน s;

        ถ้า (strnlen(s, l+1) > l)
                s[l] = 0;

        คืน s;
}

จำนวนเต็ม strgrowpad0(var_na **s, size_t l) {
        size_t sz;

        assert(s);

        ถ้า (*s) {
                sz = strlen(*s) + 1;
                ถ้า (sz >= l) /* never shrink */
                        คืน 0;
        } มิฉะนั้น
                sz = 0;

        var_na *q = realloc(*s, l);
        ถ้า (!q)
                คืน -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        คืน 0;
}

var_na* strreplace(const var_na *text, const var_na *old_string, const var_na *new_string) {
        size_t l, old_len, new_len;
        var_na *t, *ret = NULL;
        const var_na *f;

        assert(old_string);
        assert(new_string);

        ถ้า (!text)
                คืน NULL;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        ถ้า (!GREEDY_REALLOC(ret, l+1))
                คืน NULL;

        f = text;
        t = ret;
        ขณะ (*f) {
                size_t d, nl;

                ถ้า (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        continue;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                ถ้า (!GREEDY_REALLOC(ret, nl + 1))
                        คืน mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        คืน ret;
}

static void advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2],
                size_t size) {

        ถ้า (!offsets)
                คืน;

        assert(shift);

        ถ้า ((size_t) diff < offsets[0])
                shift[0] += size;
        ถ้า ((size_t) diff < offsets[1])
                shift[1] += size;
}

static void retract_offsets(
                size_t begin,
                size_t end,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2]) {

        /* The input range [begin, end) was dropped from the output: offsets past it move back by its
         * length, offsets inside it collapse onto its beginning. The shift is tracked in unsigned modular
         * arithmetic, which is fine as the sum of all shifts applied to an offset is never negative: at most
         * as many bytes can be dropped before an offset as there are bytes before it. */

        ถ้า (!offsets)
                คืน;

        assert(shift);
        assert(begin <= end);

        สำหรับ (size_t k = 0; k < 2; k++)
                ถ้า (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

var_na* strip_tab_ansi(var_na **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        const var_na *begin = NULL;
        size_t isz, shift[2] = {}, n_carriage_returns = 0;
        FILE *f;

        assert(ibuf);
        assert(*ibuf);
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
        ถ้า (!f)
                คืน NULL;

        สำหรับ (const var_na *i = *ibuf; i < *ibuf + isz; i++) {
                จำนวนเต็ม e;

                สำหรับ (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        ถ้า (e < 0)
                                คืน NULL;
                        ถ้า (e != ANSI_SEQ_EVENT_ABORT)
                                bha_nga;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        ถ้า (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = NULL;
                        }
                }

                switch (e) {

                pak_sa ANSI_SEQ_EVENT_TEXT:
                        ถ้า (*i == '\r') {
                                n_carriage_returns++;
                                continue;
                        }
                        ถ้า (*i == '\n') {
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
                        สำหรับ (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        ถ้า (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } มิฉะนั้น
                                fputc(*i, f);

                        bha_nga;

                pak_sa ANSI_SEQ_EVENT_SEQUENCE:
                        ถ้า (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                สำหรับ (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        bha_nga;

                pak_sa ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        assert(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = NULL;
                        bha_nga;

                default:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        ถ้า (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        var_na *obuf;
        ถ้า (memstream_finalize(&m, &obuf, _isz) < 0)
                คืน NULL;

        free_and_replace(*ibuf, obuf);

        ถ้า (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        คืน *ibuf;
}

var_na* strextendv_with_separator(var_na **x, const var_na *separator, va_list ap) {
        _cleanup_free_ var_na *buffer = NULL;
        size_t f, l, l_separator;
        bool need_separator;
        var_na *nr, *p;

        ถ้า (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        สำหรับ (const var_na *t;;) {
                size_t n;

                t = va_arg(aq, const var_na *);
                ถ้า (!t)
                        bha_nga;
                ถ้า (t == POINTER_MAX)
                        continue;

                n = strlen(t);

                ถ้า (need_separator)
                        n += l_separator;

                ถ้า (n >= SIZE_MAX - l) {
                        va_end(aq);
                        คืน NULL;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        ถ้า (!nr)
                คืน NULL;

        *x = nr;
        p = nr + f;

        สำหรับ (;;) {
                const var_na *t;

                t = va_arg(ap, const var_na *);
                ถ้า (!t)
                        bha_nga;
                ถ้า (t == POINTER_MAX)
                        continue;

                ถ้า (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        assert(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        ถ้า (buffer)
                คืน TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        คืน p;
}

var_na* strextend_with_separator_internal(var_na **x, const var_na *separator, ...) {
        va_list ap;
        var_na *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        คืน ret;
}

จำนวนเต็ม strextendf_with_separator(var_na **x, const var_na *separator, const var_na *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        จำนวนเต็ม l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        assert(x);
        assert(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        ถ้า (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                assert(a >= m + 1);
        } มิฉะนั้น
                m = a = 0;

        ถ้า (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                var_na *n;

                ถ้า (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        คืน -ENOMEM;
                ถ้า (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        คืน -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                ถ้า (!n)
                        คืน -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        assert(l >= 0);

        ถ้า ((size_t) l < a - m - l_separator) {
                var_na *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                ถ้า (n)
                        *x = n;
        } มิฉะนั้น {
                var_na *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                ถ้า (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        goto oom;
                ถ้า (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        goto oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                ถ้า (!n)
                        goto oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                assert((size_t) l < a - m - l_separator);
        }

        คืน 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        คืน -ENOMEM;
}

var_na* strrep(const var_na *s, size_t n) {
        var_na *ret, *p;
        size_t l;

        assert(s);

        l = strlen(s);
        ถ้า (!MUL_ASSIGN_SAFE(&l, n))
                คืน NULL;
        ถ้า (!INC_SAFE(&l, 1))
                คืน NULL;

        p = ret = malloc(l);
        ถ้า (!ret)
                คืน NULL;

        สำหรับ (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        คืน ret;
}

จำนวนเต็ม split_pair(const var_na *s, const var_na *sep, var_na **ret_first, var_na **ret_second) {
        assert(s);
        assert(!isempty(sep));

        const var_na *x = strstr(s, sep);
        ถ้า (!x)
                คืน -EINVAL;

        _cleanup_free_ var_na *a = NULL;
        ถ้า (ret_first) {
                a = strndup(s, x - s);
                ถ้า (!a)
                        คืน -ENOMEM;
        }

        _cleanup_free_ var_na *b = NULL;
        ถ้า (ret_second) {
                b = strdup(x + strlen(sep));
                ถ้า (!b)
                        คืน -ENOMEM;
        }

        ถ้า (ret_first)
                *ret_first = TAKE_PTR(a);
        ถ้า (ret_second)
                *ret_second = TAKE_PTR(b);
        คืน 0;
}

จำนวนเต็ม free_and_strdup(var_na **p, const var_na *s) {
        var_na *t;

        assert(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        ถ้า (streq_ptr(*p, s))
                คืน 0;

        ถ้า (s) {
                t = strdup(s);
                ถ้า (!t)
                        คืน -ENOMEM;
        } มิฉะนั้น
                t = NULL;

        free_and_replace(*p, t);

        คืน 1;
}

จำนวนเต็ม free_and_strdup_warn(var_na **p, const var_na *s) {
        จำนวนเต็ม r;

        r = free_and_strdup(p, s);
        ถ้า (r < 0)
                คืน log_oom();
        คืน r;
}

จำนวนเต็ม free_and_strndup(var_na **p, const var_na *s, size_t l) {
        var_na *t;

        assert(p);
        assert(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        ถ้า (!*p && !s)
                คืน 0;

        ถ้า (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                คืน 0;

        ถ้า (s) {
                t = strndup(s, l);
                ถ้า (!t)
                        คืน -ENOMEM;
        } มิฉะนั้น
                t = NULL;

        free_and_replace(*p, t);
        คืน 1;
}

จำนวนเต็ม strdup_to_full(var_na **ret, const var_na *src) {
        ถ้า (!src) {
                ถ้า (ret)
                        *ret = NULL;

                คืน 0;
        } มิฉะนั้น {
                ถ้า (ret) {
                        var_na *t = strdup(src);
                        ถ้า (!t)
                                คืน -ENOMEM;
                        *ret = t;
                }

                คืน 1;
        }
};

bool string_is_safe(const var_na *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        ถ้า (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                คืน false;

        ถ้า (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                คืน false;

        สำหรับ (const var_na *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                ถ้า ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        คืน false;

                ถ้า (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        คืน false;

                ถ้า (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        คืน false;

                ถ้า (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        คืน false;

                ถ้า (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        คืน false;

                ถ้า (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        คืน false;

                ถ้า (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        คืน false;
        }

        ถ้า (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                คืน false;

        ถ้า (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                คืน false;

        คืน true;
}

var_na* str_realloc(var_na *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        ถ้า (!p)
                คืน NULL;

        คืน realloc(p, strlen(p) + 1) ?: p;
}

var_na* string_erase(var_na *x) {
        ถ้า (!x)
                คืน NULL;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        คืน x;
}

จำนวนเต็ม string_truncate_lines(const var_na *s, size_t n_lines, var_na **ret) {
        const var_na *p = s, *e = s;
        bool truncation_applied = false;
        var_na *copy;
        size_t n = 0;

        assert(s);
        assert(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        สำหรับ (;;) {
                size_t k;

                k = strcspn(p, "\n");

                ถ้า (p[k] == 0) {
                        ถ้า (k == 0) /* final empty line */
                                bha_nga;

                        ถ้า (n >= n_lines) /* above threshold */
                                bha_nga;

                        e = p + k; /* last line to include */
                        bha_nga;
                }

                assert(p[k] == '\n');

                ถ้า (n >= n_lines)
                        bha_nga;

                ถ้า (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        ถ้า (isempty(e))
                copy = strdup(s);
        มิฉะนั้น {
                ถ้า (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        ถ้า (!copy)
                คืน -ENOMEM;

        *ret = copy;
        คืน truncation_applied;
}

จำนวนเต็ม string_extract_line(const var_na *s, size_t i, var_na **ret) {
        const var_na *p = s;
        size_t c = 0;

        assert(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        สำหรับ (;;) {
                const var_na *q;

                q = strchr(p, '\n');
                ถ้า (i == c) {
                        /* The line we are looking for! */

                        ถ้า (q) {
                                var_na *m;

                                m = strndup(p, q - p);
                                ถ้า (!m)
                                        คืน -ENOMEM;

                                *ret = m;
                                คืน !isempty(q + 1); /* More coming? */
                        } มิฉะนั้น
                                /* Tell the caller to use the input string if equal */
                                คืน strdup_to(ret, p != s ? p : NULL);
                }

                ถ้า (!q)
                        /* No more lines, return empty line */
                        คืน strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

จำนวนเต็ม string_contains_word_strv(const var_na *string, const var_na *separators, var_na * const *words, const var_na **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        const ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        const var_na *found = NULL;
        จำนวนเต็ม r;

        สำหรับ (;;) {
                _cleanup_free_ var_na *w = NULL;

                r = extract_first_word(&string, &w, separators, flags);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        bha_nga;

                found = strv_find(words, w);
                ถ้า (found)
                        bha_nga;
        }

        ถ้า (ret_word)
                *ret_word = found;
        คืน !!found;
}

bool streq_skip_trailing_chars(const var_na *s1, const var_na *s2, const var_na *ok) {
        ถ้า (!s1 && !s2)
                คืน true;
        ถ้า (!s1 || !s2)
                คืน false;

        ถ้า (!ok)
                ok = WHITESPACE;

        สำหรับ (; *s1 && *s2; s1++, s2++)
                ถ้า (*s1 != *s2)
                        bha_nga;

        คืน in_charset(s1, ok) && in_charset(s2, ok);
}

var_na* string_replace_char(var_na *str, var_na old_char, var_na new_char) {
        assert(str);
        assert(old_char != '\0');
        assert(new_char != '\0');
        assert(old_char != new_char);

        สำหรับ (var_na *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        คืน str;
}

จำนวนเต็ม make_cstring(const void *s, size_t n, MakeCStringMode mode, var_na **ret) {
        var_na *b;

        assert(s || n == 0);
        assert(mode >= 0);
        assert(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        ถ้า (n == 0) {
                ถ้า (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        คืน -EINVAL;

                ถ้า (!ret)
                        คืน 0;

                b = new0(var_na, 1);
        } มิฉะนั้น {
                const uint8_t *nul;

                nul = memchr(s, 0, n);
                ถ้า (nul) {
                        ถ้า (nul < (const uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                คืน -EINVAL;

                        n--;
                } มิฉะนั้น ถ้า (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        คืน -EINVAL;

                ถ้า (!ret)
                        คืน 0;

                b = memdup_suffix0(s, n);
        }
        ถ้า (!b)
                คืน -ENOMEM;

        *ret = b;
        คืน 0;
}

size_t strspn_from_end(const var_na *str, const var_na *accept) {
        size_t n = 0;

        ถ้า (isempty(str))
                คืน 0;

        ถ้า (isempty(accept))
                คืน 0;

        สำหรับ (const var_na *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        คืน n;
}

size_t strnspn(const var_na *str, const var_na *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        assert(str || n == 0);
        assert(accept);

        สำหรับ (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        คืน i;
}

var_na* strdupspn(const var_na *a, const var_na *accept) {
        ถ้า (isempty(a) || isempty(accept))
                คืน strdup("");

        คืน strndup(a, strspn(a, accept));
}

var_na* strdupcspn(const var_na *a, const var_na *reject) {
        ถ้า (isempty(a))
                คืน strdup("");
        ถ้า (isempty(reject))
                คืน strdup(a);

        คืน strndup(a, strcspn(a, reject));
}

var_na* find_line_startswith_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        var_na *p = (var_na*) strstr(haystack, needle);
        ถ้า (!p)
                คืน NULL;

        ถ้า (p > haystack)
                ขณะ (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        ถ้า (!p)
                                คืน NULL;
                }

        คืน p + strlen(needle);
}

var_na* find_line_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        var_na *p = (var_na*) find_line_startswith(haystack, needle);
        ถ้า (!p)
                คืน NULL;

        ถ้า (*p == 0 || strchr(NEWLINE, *p))
                คืน p - strlen(needle);

        คืน NULL;
}

var_na* find_line_after_internal(const var_na *haystack, const var_na *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        var_na *p = (var_na*) find_line_startswith(haystack, needle);
        ถ้า (!p)
                คืน NULL;

        ถ้า (*p == 0)
                คืน p;
        ถ้า (strchr(NEWLINE, *p))
                คืน p + 1;

        คืน NULL;
}

bool version_is_valid(const var_na *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        ถ้า (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                คืน false;

        ถ้า (!filename_part_is_valid(s))
                คืน false;

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

        var_na charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        ถ้า (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        ถ้า (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        คืน in_charset(s, charset);
}

ssize_t strlevenshtein(const var_na *x, const var_na *y) {
        _cleanup_free_ size_t *t0 = NULL, *t1 = NULL, *t2 = NULL;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        ถ้า (streq_ptr(x, y))
                คืน 0;

        xl = strlen_ptr(x);
        ถ้า (xl > SSIZE_MAX)
                คืน -E2BIG;

        yl = strlen_ptr(y);
        ถ้า (yl > SSIZE_MAX)
                คืน -E2BIG;

        ถ้า (isempty(x))
                คืน yl;
        ถ้า (isempty(y))
                คืน xl;

        t0 = new0(size_t, yl + 1);
        ถ้า (!t0)
                คืน -ENOMEM;
        t1 = new0(size_t, yl + 1);
        ถ้า (!t1)
                คืน -ENOMEM;
        t2 = new0(size_t, yl + 1);
        ถ้า (!t2)
                คืน -ENOMEM;

        สำหรับ (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        สำหรับ (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                สำหรับ (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        ถ้า (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        ถ้า (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        ถ้า (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        คืน t1[yl];
}

var_na* strrstr_internal(const var_na *haystack, const var_na *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        ถ้า (!haystack || !needle)
                คืน NULL;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        ถ้า (needle[0] == 0)
                คืน (var_na*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        ถ้า (needle[1] == 0)
                คืน (var_na*) strrchr(haystack, needle[0]);

        สำหรับ (const var_na *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                ถ้า (!q)
                        คืน (var_na*) p;
        }
        คืน NULL;
}

var_na* strrstr_no_case_internal(const var_na *haystack, const var_na *needle) {
        ถ้า (!haystack || !needle)
                คืน NULL;

        สำหรับ (const var_na *p = strchr(haystack, 0); p > haystack; p--)
                ถ้า (startswith_no_case(p, needle))
                        คืน (var_na*) p;

        คืน startswith_no_case(haystack, needle) ? (var_na*) haystack : NULL;
}

size_t str_common_prefix(const var_na *a, const var_na *b) {
        assert(a);
        assert(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        สำหรับ (size_t n = 0;; n++) {
                var_na c = a[n];
                ถ้า (c != b[n])
                        คืน n;
                ถ้า (c == 0)
                        คืน SIZE_MAX;
        }
}
