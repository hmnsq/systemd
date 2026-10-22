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

char* first_word(const char *s, const char *word) {
        assert(s);
        assert(word);

        /* Checks if the string starts with the specified word, either followed by NUL or by whitespace.
         * Returns a pointer to the NUL or the first character after the whitespace. */

        если (isempty(word))
                вернуть (char*) s;

        const char *p = startswith(s, word);
        если (!p)
                вернуть NULL;
        если (*p == '\0')
                вернуть (char*) p;

        const char *nw = skip_leading_chars(p, WHITESPACE);
        если (p == nw)
                вернуть NULL;

        вернуть (char*) nw;
}

char* strextendn(char **x, const char *s, size_t l) {
        assert(x);
        assert(s || l == 0);

        если (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        если (l > 0 || !*x) {
                size_t q;
                char *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                если (!m)
                        вернуть NULL;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        вернуть *x;
}

char* strstrip(char *s) {
        если (!s)
                вернуть NULL;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        вернуть delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

char* delete_chars(char *s, const char *bad) {
        char *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        если (!s)
                вернуть NULL;

        если (!bad)
                bad = WHITESPACE;

        для (f = s, t = s; *f; f++) {
                если (strchr(bad, *f))
                        continue;

                *(t++) = *f;
        }

        *t = 0;

        вернуть s;
}

char* delete_trailing_chars(char *s, const char *bad) {
        char *c = s;

        /* Drops all specified bad characters, at the end of the string */

        если (!s)
                вернуть NULL;

        если (!bad)
                bad = WHITESPACE;

        для (char *p = s; *p; p++)
                если (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        вернуть s;
}

char* truncate_nl_full(char *s, size_t *ret_len) {
        size_t n;

        assert(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        если (ret_len)
                *ret_len = n;
        вернуть s;
}

char ascii_tolower(char x) {

        если (x >= 'A' && x <= 'Z')
                вернуть x - 'A' + 'a';

        вернуть x;
}

char ascii_toupper(char x) {

        если (x >= 'a' && x <= 'z')
                вернуть x - 'a' + 'A';

        вернуть x;
}

char* ascii_strlower(char *s) {
        assert(s);

        для (char *p = s; *p; p++)
                *p = ascii_tolower(*p);

        вернуть s;
}

char* ascii_strupper(char *s) {
        assert(s);

        для (char *p = s; *p; p++)
                *p = ascii_toupper(*p);

        вернуть s;
}

char* ascii_strlower_n(char *s, size_t n) {
        assert(n <= 0 || s);

        если (n <= 0)
                вернуть s;

        для (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        вернуть s;
}

целое ascii_strcasecmp_n(const char *a, const char *b, size_t n) {

        assert(a);
        assert(b);

        для (; n > 0; a++, b++, n--) {
                целое x, y;

                x = (целое) (uint8_t) ascii_tolower(*a);
                y = (целое) (uint8_t) ascii_tolower(*b);

                если (x != y)
                        вернуть x - y;
        }

        вернуть 0;
}

целое ascii_strcasecmp_nn(const char *a, size_t n, const char *b, size_t m) {
        целое r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        если (r != 0)
                вернуть r;

        вернуть CMP(n, m);
}

bool chars_intersect(const char *a, const char *b) {
        /* Returns true if any of the chars in a are in b. */
        для (const char *p = a; *p; p++)
                если (strchr(b, *p))
                        вернуть true;

        вернуть false;
}

bool string_has_cc(const char *p, const char *ok) {
        assert(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        для (const char *t = p; *t; t++) {
                если (ok && strchr(ok, *t))
                        continue;

                если (char_is_cc(*t))
                        вернуть true;
        }

        вернуть false;
}

static целое write_ellipsis(char *buf, bool unicode) {
        const char *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        assert(strlen(s) == 3);
        memcpy(buf, s, 3);
        вернуть 3;
}

static size_t ansi_sequence_length(const char *s, size_t len) {
        assert(s);

        если (len < 2)
                вернуть 0;

        если (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                вернуть 0;  /* Not the start of a sequence */

        если (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                если (i == len)
                        вернуть 0;

                пока (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        если (++i == len)
                                вернуть 0;
                пока (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        если (++i == len)
                                вернуть 0;
                если (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        вернуть i + 1;
                вернуть 0;  /* Bad sequence */

        } иначе если (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                вернуть 2;

        вернуть 0;  /* Bad escape? */
}

static bool string_has_ansi_sequence(const char *s, size_t len) {
        const char *t = s;

        пока ((t = memchr(t, 0x1B, len - (t - s)))) {
                если (ansi_sequence_length(t, len - (t - s)) > 0)
                        вернуть true;
                t++;
        }
        вернуть false;
}

static size_t previous_ansi_sequence(const char *s, size_t length, const char **ret_where) {

        assert(s);
        assert(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        если (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = NULL;
                вернуть 0;
        }

        для (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                если (slen > 0) {
                        *ret_where = s + i;
                        вернуть slen;
                }

                если (i == 0)
                        break;
        }

        *ret_where = NULL;
        вернуть 0;
}

static char *ascii_ellipsize_mem(const char *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        char *t;

        assert(s);
        assert(percent <= 100);
        assert(new_length != SIZE_MAX);

        если (old_length <= new_length)
                вернуть strndup(s, old_length);

        /* Special case short ellipsations */
        switch (new_length) {

        case 0:
                вернуть strdup("");

        case 1:
                если (is_locale_utf8())
                        вернуть strdup("…");
                иначе
                        вернуть strdup(".");

        case 2:
                если (!is_locale_utf8())
                        вернуть strdup("..");
                break;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(char, new_length+3);
        если (!t)
                вернуть NULL;

        assert(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        assert(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        вернуть t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
static const char* find_previous_unichar(const char *start, const char *end, char32_t *ret_c) {
        const char *p;
        целое r;

        assert(start);
        assert(end);
        assert(end > start);

        для (p = end; p > start; ) {
                p--;
                если (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        break;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        если (r < 0 || p + r != end)
                вернуть NULL;

        если (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        вернуть p;
}

char* ellipsize_mem(const char *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        const char *i, *j;
        целое r;

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

        если (new_length == SIZE_MAX)
                вернуть strndup(s, old_length);

        если (new_length == 0)
                вернуть strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        если (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                вернуть ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        assert(x <= new_length - 1);

        k = 0;
        для (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                если (slen > 0) {
                        i += slen;
                        continue;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                если (r < 0)
                        вернуть NULL;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                целое w = unichar_console_width(c);
                если (k + w > x)
                        break;

                k += w;
                i += r;
        }

        const char *ansi_start = s + old_length;
        size_t ansi_len = 0;

        для (const char *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                целое w;
                const char *tt;

                если (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                если (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        continue;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                если (!tt)
                        вернуть NULL;

                w = unichar_console_width(c);
                если (k + w > new_length)
                        break;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        если (i >= j)
                вернуть memdup_suffix0(s, old_length);

        если (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                если (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        если (r < 0)
                                вернуть NULL;

                        j += r;
                } иначе если (i > s) {
                        const char *tt = find_previous_unichar(s, i, NULL);
                        если (!tt)
                                вернуть NULL;

                        i = tt;
                }
        }

        len = i - s;
        len2 = s + old_length - j;

        /* If we have ANSI, allow the same length as the source string + ellipsis. It'd be too involved to
         * figure out what exact space is needed. Strings with ANSI sequences are most likely to be fairly
         * short anyway. */
        size_t alloc_len = has_ansi_seq ? old_length + 3 + 1 : len + 3 + len2 + 1;

        char *e = new(char, alloc_len);
        если (!e)
                вернуть NULL;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        char *dst = e + len + 3;

        если (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                для (const char *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        если (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } иначе
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        вернуть e;
}

char* cellescape(char *buf, size_t len, const char *s) {
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

        для (;;) {
                char four[4];
                целое w;

                если (*s == 0) /* terminating NUL detected? then we are done! */
                        goto done;

                w = cescape_char(*s, four);
                если (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
                                      * ellipsize at the previous location */
                        break;

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
        для (size_t j = 0; j < ELEMENTSOF(last_char_width); j++) {

                если (i + 4 <= len) /* nice, we reached our space goal */
                        break;

                k = k == 0 ? 3 : k - 1;
                если (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        break;

                assert(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        если (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        иначе если (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } иначе если (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        иначе
                assert(i + 1 <= len);

done:
        buf[i] = '\0';
        вернуть buf;
}

char* strshorten(char *s, size_t l) {
        assert(s);

        если (l >= SIZE_MAX-1) /* Would not change anything */
                вернуть s;

        если (strnlen(s, l+1) > l)
                s[l] = 0;

        вернуть s;
}

целое strgrowpad0(char **s, size_t l) {
        size_t sz;

        assert(s);

        если (*s) {
                sz = strlen(*s) + 1;
                если (sz >= l) /* never shrink */
                        вернуть 0;
        } иначе
                sz = 0;

        char *q = realloc(*s, l);
        если (!q)
                вернуть -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        вернуть 0;
}

char* strreplace(const char *text, const char *old_string, const char *new_string) {
        size_t l, old_len, new_len;
        char *t, *ret = NULL;
        const char *f;

        assert(old_string);
        assert(new_string);

        если (!text)
                вернуть NULL;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        если (!GREEDY_REALLOC(ret, l+1))
                вернуть NULL;

        f = text;
        t = ret;
        пока (*f) {
                size_t d, nl;

                если (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        continue;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                если (!GREEDY_REALLOC(ret, nl + 1))
                        вернуть mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        вернуть ret;
}

static void advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2],
                size_t size) {

        если (!offsets)
                вернуть;

        assert(shift);

        если ((size_t) diff < offsets[0])
                shift[0] += size;
        если ((size_t) diff < offsets[1])
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

        если (!offsets)
                вернуть;

        assert(shift);
        assert(begin <= end);

        для (size_t k = 0; k < 2; k++)
                если (offsets[k] > begin)
                        shift[k] -= MIN(offsets[k], end) - begin;
}

char* strip_tab_ansi(char **ibuf, size_t *_isz, size_t highlight[2]) {
        _cleanup_(ansi_seq_parser_done) AnsiSeqParser parser = {};
        _cleanup_(memstream_done) MemStream m = {};
        const char *begin = NULL;
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
        если (!f)
                вернуть NULL;

        для (const char *i = *ibuf; i < *ibuf + isz; i++) {
                целое e;

                для (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        если (e < 0)
                                вернуть NULL;
                        если (e != ANSI_SEQ_EVENT_ABORT)
                                break;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        если (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = NULL;
                        }
                }

                switch (e) {

                case ANSI_SEQ_EVENT_TEXT:
                        если (*i == '\r') {
                                n_carriage_returns++;
                                continue;
                        }
                        если (*i == '\n') {
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
                        для (; n_carriage_returns > 0; n_carriage_returns--)
                                fputc('\r', f);

                        если (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } иначе
                                fputc(*i, f);

                        break;

                case ANSI_SEQ_EVENT_SEQUENCE:
                        если (!begin) {
                                /* This starts a new sequence, flush any pending carriage returns first */
                                для (; n_carriage_returns > 0; n_carriage_returns--)
                                        fputc('\r', f);

                                begin = i;
                        }
                        break;

                case ANSI_SEQ_EVENT_END:
                        /* A complete sequence, drop it (the current character is its last one) */
                        assert(begin);
                        retract_offsets(begin - *ibuf, i + 1 - *ibuf, highlight, shift);
                        begin = NULL;
                        break;

                default:
                        assert_not_reached();
                }
        }

        /* If the input ends in the middle of a sequence the truncated sequence is dropped as well, hence
         * the offsets need to account for it, too. */
        если (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        char *obuf;
        если (memstream_finalize(&m, &obuf, _isz) < 0)
                вернуть NULL;

        free_and_replace(*ibuf, obuf);

        если (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        вернуть *ibuf;
}

char* strextendv_with_separator(char **x, const char *separator, va_list ap) {
        _cleanup_free_ char *buffer = NULL;
        size_t f, l, l_separator;
        bool need_separator;
        char *nr, *p;

        если (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        для (const char *t;;) {
                size_t n;

                t = va_arg(aq, const char *);
                если (!t)
                        break;
                если (t == POINTER_MAX)
                        continue;

                n = strlen(t);

                если (need_separator)
                        n += l_separator;

                если (n >= SIZE_MAX - l) {
                        va_end(aq);
                        вернуть NULL;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        если (!nr)
                вернуть NULL;

        *x = nr;
        p = nr + f;

        для (;;) {
                const char *t;

                t = va_arg(ap, const char *);
                если (!t)
                        break;
                если (t == POINTER_MAX)
                        continue;

                если (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        assert(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        если (buffer)
                вернуть TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        вернуть p;
}

char* strextend_with_separator_internal(char **x, const char *separator, ...) {
        va_list ap;
        char *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        вернуть ret;
}

целое strextendf_with_separator(char **x, const char *separator, const char *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        целое l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        assert(x);
        assert(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        если (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                assert(a >= m + 1);
        } иначе
                m = a = 0;

        если (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                char *n;

                если (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        вернуть -ENOMEM;
                если (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        вернуть -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                если (!n)
                        вернуть -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        assert(l >= 0);

        если ((size_t) l < a - m - l_separator) {
                char *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                если (n)
                        *x = n;
        } иначе {
                char *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                если (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        goto oom;
                если (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        goto oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                если (!n)
                        goto oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                assert((size_t) l < a - m - l_separator);
        }

        вернуть 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        вернуть -ENOMEM;
}

char* strrep(const char *s, size_t n) {
        char *ret, *p;
        size_t l;

        assert(s);

        l = strlen(s);
        если (!MUL_ASSIGN_SAFE(&l, n))
                вернуть NULL;
        если (!INC_SAFE(&l, 1))
                вернуть NULL;

        p = ret = malloc(l);
        если (!ret)
                вернуть NULL;

        для (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        вернуть ret;
}

целое split_pair(const char *s, const char *sep, char **ret_first, char **ret_second) {
        assert(s);
        assert(!isempty(sep));

        const char *x = strstr(s, sep);
        если (!x)
                вернуть -EINVAL;

        _cleanup_free_ char *a = NULL;
        если (ret_first) {
                a = strndup(s, x - s);
                если (!a)
                        вернуть -ENOMEM;
        }

        _cleanup_free_ char *b = NULL;
        если (ret_second) {
                b = strdup(x + strlen(sep));
                если (!b)
                        вернуть -ENOMEM;
        }

        если (ret_first)
                *ret_first = TAKE_PTR(a);
        если (ret_second)
                *ret_second = TAKE_PTR(b);
        вернуть 0;
}

целое free_and_strdup(char **p, const char *s) {
        char *t;

        assert(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        если (streq_ptr(*p, s))
                вернуть 0;

        если (s) {
                t = strdup(s);
                если (!t)
                        вернуть -ENOMEM;
        } иначе
                t = NULL;

        free_and_replace(*p, t);

        вернуть 1;
}

целое free_and_strdup_warn(char **p, const char *s) {
        целое r;

        r = free_and_strdup(p, s);
        если (r < 0)
                вернуть log_oom();
        вернуть r;
}

целое free_and_strndup(char **p, const char *s, size_t l) {
        char *t;

        assert(p);
        assert(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        если (!*p && !s)
                вернуть 0;

        если (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                вернуть 0;

        если (s) {
                t = strndup(s, l);
                если (!t)
                        вернуть -ENOMEM;
        } иначе
                t = NULL;

        free_and_replace(*p, t);
        вернуть 1;
}

целое strdup_to_full(char **ret, const char *src) {
        если (!src) {
                если (ret)
                        *ret = NULL;

                вернуть 0;
        } иначе {
                если (ret) {
                        char *t = strdup(src);
                        если (!t)
                                вернуть -ENOMEM;
                        *ret = t;
                }

                вернуть 1;
        }
};

bool string_is_safe(const char *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        если (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                вернуть false;

        если (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                вернуть false;

        для (const char *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                если ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        вернуть false;

                если (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        вернуть false;

                если (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        вернуть false;

                если (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        вернуть false;

                если (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        вернуть false;

                если (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        вернуть false;

                если (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        вернуть false;
        }

        если (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                вернуть false;

        если (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                вернуть false;

        вернуть true;
}

char* str_realloc(char *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        если (!p)
                вернуть NULL;

        вернуть realloc(p, strlen(p) + 1) ?: p;
}

char* string_erase(char *x) {
        если (!x)
                вернуть NULL;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        вернуть x;
}

целое string_truncate_lines(const char *s, size_t n_lines, char **ret) {
        const char *p = s, *e = s;
        bool truncation_applied = false;
        char *copy;
        size_t n = 0;

        assert(s);
        assert(ret);

        /* Truncate after the specified number of lines. Returns > 0 if a truncation was applied or == 0 if
         * there were fewer lines in the string anyway. Trailing newlines on input are ignored, and not
         * generated either. */

        для (;;) {
                size_t k;

                k = strcspn(p, "\n");

                если (p[k] == 0) {
                        если (k == 0) /* final empty line */
                                break;

                        если (n >= n_lines) /* above threshold */
                                break;

                        e = p + k; /* last line to include */
                        break;
                }

                assert(p[k] == '\n');

                если (n >= n_lines)
                        break;

                если (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        если (isempty(e))
                copy = strdup(s);
        иначе {
                если (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        если (!copy)
                вернуть -ENOMEM;

        *ret = copy;
        вернуть truncation_applied;
}

целое string_extract_line(const char *s, size_t i, char **ret) {
        const char *p = s;
        size_t c = 0;

        assert(ret);

        /* Extract the i'nth line from the specified string. Returns > 0 if there are more lines after that,
         * and == 0 if we are looking at the last line or already beyond the last line. As special
         * optimization, if the first line is requested and the string only consists of one line we return
         * NULL, indicating the input string should be used as is, and avoid a memory allocation for a very
         * common case. */

        для (;;) {
                const char *q;

                q = strchr(p, '\n');
                если (i == c) {
                        /* The line we are looking for! */

                        если (q) {
                                char *m;

                                m = strndup(p, q - p);
                                если (!m)
                                        вернуть -ENOMEM;

                                *ret = m;
                                вернуть !isempty(q + 1); /* More coming? */
                        } иначе
                                /* Tell the caller to use the input string if equal */
                                вернуть strdup_to(ret, p != s ? p : NULL);
                }

                если (!q)
                        /* No more lines, return empty line */
                        вернуть strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

целое string_contains_word_strv(const char *string, const char *separators, char * const *words, const char **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        const ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        const char *found = NULL;
        целое r;

        для (;;) {
                _cleanup_free_ char *w = NULL;

                r = extract_first_word(&string, &w, separators, flags);
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        break;

                found = strv_find(words, w);
                если (found)
                        break;
        }

        если (ret_word)
                *ret_word = found;
        вернуть !!found;
}

bool streq_skip_trailing_chars(const char *s1, const char *s2, const char *ok) {
        если (!s1 && !s2)
                вернуть true;
        если (!s1 || !s2)
                вернуть false;

        если (!ok)
                ok = WHITESPACE;

        для (; *s1 && *s2; s1++, s2++)
                если (*s1 != *s2)
                        break;

        вернуть in_charset(s1, ok) && in_charset(s2, ok);
}

char* string_replace_char(char *str, char old_char, char new_char) {
        assert(str);
        assert(old_char != '\0');
        assert(new_char != '\0');
        assert(old_char != new_char);

        для (char *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        вернуть str;
}

целое make_cstring(const void *s, size_t n, MakeCStringMode mode, char **ret) {
        char *b;

        assert(s || n == 0);
        assert(mode >= 0);
        assert(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        если (n == 0) {
                если (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        вернуть -EINVAL;

                если (!ret)
                        вернуть 0;

                b = new0(char, 1);
        } иначе {
                const uint8_t *nul;

                nul = memchr(s, 0, n);
                если (nul) {
                        если (nul < (const uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                вернуть -EINVAL;

                        n--;
                } иначе если (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        вернуть -EINVAL;

                если (!ret)
                        вернуть 0;

                b = memdup_suffix0(s, n);
        }
        если (!b)
                вернуть -ENOMEM;

        *ret = b;
        вернуть 0;
}

size_t strspn_from_end(const char *str, const char *accept) {
        size_t n = 0;

        если (isempty(str))
                вернуть 0;

        если (isempty(accept))
                вернуть 0;

        для (const char *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        вернуть n;
}

size_t strnspn(const char *str, const char *accept, size_t n) {
        size_t i;

        /* Like strspn(), but reads at most 'n' bytes from 'str'. Returns the length of the initial
         * run of 'str' (capped at 'n') that consists solely of bytes found in 'accept'. Stops at a
         * NUL byte too. Unlike strspn() this is safe on a buffer that is not NUL terminated within
         * 'n' bytes. */

        assert(str || n == 0);
        assert(accept);

        для (i = 0; i < n && str[i] != '\0' && strchr(accept, str[i]); i++)
                ;

        вернуть i;
}

char* strdupspn(const char *a, const char *accept) {
        если (isempty(a) || isempty(accept))
                вернуть strdup("");

        вернуть strndup(a, strspn(a, accept));
}

char* strdupcspn(const char *a, const char *reject) {
        если (isempty(a))
                вернуть strdup("");
        если (isempty(reject))
                вернуть strdup(a);

        вернуть strndup(a, strcspn(a, reject));
}

char* find_line_startswith_internal(const char *haystack, const char *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        char *p = (char*) strstr(haystack, needle);
        если (!p)
                вернуть NULL;

        если (p > haystack)
                пока (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        если (!p)
                                вернуть NULL;
                }

        вернуть p + strlen(needle);
}

char* find_line_internal(const char *haystack, const char *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        char *p = (char*) find_line_startswith(haystack, needle);
        если (!p)
                вернуть NULL;

        если (*p == 0 || strchr(NEWLINE, *p))
                вернуть p - strlen(needle);

        вернуть NULL;
}

char* find_line_after_internal(const char *haystack, const char *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        char *p = (char*) find_line_startswith(haystack, needle);
        если (!p)
                вернуть NULL;

        если (*p == 0)
                вернуть p;
        если (strchr(NEWLINE, *p))
                вернуть p + 1;

        вернуть NULL;
}

bool version_is_valid(const char *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        если (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                вернуть false;

        если (!filename_part_is_valid(s))
                вернуть false;

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

        char charset[] = ALPHANUMERICAL ".-~^" /* plus room for the two chars below: */ "\0\0";
        size_t l = strlen(charset);

        если (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        если (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        вернуть in_charset(s, charset);
}

ssize_t strlevenshtein(const char *x, const char *y) {
        _cleanup_free_ size_t *t0 = NULL, *t1 = NULL, *t2 = NULL;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        если (streq_ptr(x, y))
                вернуть 0;

        xl = strlen_ptr(x);
        если (xl > SSIZE_MAX)
                вернуть -E2BIG;

        yl = strlen_ptr(y);
        если (yl > SSIZE_MAX)
                вернуть -E2BIG;

        если (isempty(x))
                вернуть yl;
        если (isempty(y))
                вернуть xl;

        t0 = new0(size_t, yl + 1);
        если (!t0)
                вернуть -ENOMEM;
        t1 = new0(size_t, yl + 1);
        если (!t1)
                вернуть -ENOMEM;
        t2 = new0(size_t, yl + 1);
        если (!t2)
                вернуть -ENOMEM;

        для (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        для (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                для (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        если (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        если (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        если (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        вернуть t1[yl];
}

char* strrstr_internal(const char *haystack, const char *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        если (!haystack || !needle)
                вернуть NULL;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        если (needle[0] == 0)
                вернуть (char*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        если (needle[1] == 0)
                вернуть (char*) strrchr(haystack, needle[0]);

        для (const char *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                если (!q)
                        вернуть (char*) p;
        }
        вернуть NULL;
}

char* strrstr_no_case_internal(const char *haystack, const char *needle) {
        если (!haystack || !needle)
                вернуть NULL;

        для (const char *p = strchr(haystack, 0); p > haystack; p--)
                если (startswith_no_case(p, needle))
                        вернуть (char*) p;

        вернуть startswith_no_case(haystack, needle) ? (char*) haystack : NULL;
}

size_t str_common_prefix(const char *a, const char *b) {
        assert(a);
        assert(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        для (size_t n = 0;; n++) {
                char c = a[n];
                если (c != b[n])
                        вернуть n;
                если (c == 0)
                        вернуть SIZE_MAX;
        }
}
