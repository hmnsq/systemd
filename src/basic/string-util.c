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

        якщо (isempty(word))
                повернути (char*) s;

        const char *p = startswith(s, word);
        якщо (!p)
                повернути NULL;
        якщо (*p == '\0')
                повернути (char*) p;

        const char *nw = skip_leading_chars(p, WHITESPACE);
        якщо (p == nw)
                повернути NULL;

        повернути (char*) nw;
}

char* strextendn(char **x, const char *s, size_t l) {
        assert(x);
        assert(s || l == 0);

        якщо (l > 0)
                l = strnlen(s, l); /* ignore trailing noise */

        якщо (l > 0 || !*x) {
                size_t q;
                char *m;

                q = strlen_ptr(*x);
                m = realloc(*x, q + l + 1);
                якщо (!m)
                        повернути NULL;

                *mempcpy_typesafe(m + q, s, l) = 0;

                *x = m;
        }

        повернути *x;
}

char* strstrip(char *s) {
        якщо (!s)
                повернути NULL;

        /* Drops trailing whitespace. Modifies the string in place. Returns pointer to first non-space character */

        повернути delete_trailing_chars(skip_leading_chars(s, WHITESPACE), WHITESPACE);
}

char* delete_chars(char *s, const char *bad) {
        char *f, *t;

        /* Drops all specified bad characters, regardless where in the string */

        якщо (!s)
                повернути NULL;

        якщо (!bad)
                bad = WHITESPACE;

        для (f = s, t = s; *f; f++) {
                якщо (strchr(bad, *f))
                        continue;

                *(t++) = *f;
        }

        *t = 0;

        повернути s;
}

char* delete_trailing_chars(char *s, const char *bad) {
        char *c = s;

        /* Drops all specified bad characters, at the end of the string */

        якщо (!s)
                повернути NULL;

        якщо (!bad)
                bad = WHITESPACE;

        для (char *p = s; *p; p++)
                якщо (!strchr(bad, *p))
                        c = p + 1;

        *c = 0;

        повернути s;
}

char* truncate_nl_full(char *s, size_t *ret_len) {
        size_t n;

        assert(s);

        n = strcspn(s, NEWLINE);
        s[n] = '\0';
        якщо (ret_len)
                *ret_len = n;
        повернути s;
}

char ascii_tolower(char x) {

        якщо (x >= 'A' && x <= 'Z')
                повернути x - 'A' + 'a';

        повернути x;
}

char ascii_toupper(char x) {

        якщо (x >= 'a' && x <= 'z')
                повернути x - 'a' + 'A';

        повернути x;
}

char* ascii_strlower(char *s) {
        assert(s);

        для (char *p = s; *p; p++)
                *p = ascii_tolower(*p);

        повернути s;
}

char* ascii_strupper(char *s) {
        assert(s);

        для (char *p = s; *p; p++)
                *p = ascii_toupper(*p);

        повернути s;
}

char* ascii_strlower_n(char *s, size_t n) {
        assert(n <= 0 || s);

        якщо (n <= 0)
                повернути s;

        для (size_t i = 0; i < n; i++)
                s[i] = ascii_tolower(s[i]);

        повернути s;
}

ціле ascii_strcasecmp_n(const char *a, const char *b, size_t n) {

        assert(a);
        assert(b);

        для (; n > 0; a++, b++, n--) {
                ціле x, y;

                x = (ціле) (uint8_t) ascii_tolower(*a);
                y = (ціле) (uint8_t) ascii_tolower(*b);

                якщо (x != y)
                        повернути x - y;
        }

        повернути 0;
}

ціле ascii_strcasecmp_nn(const char *a, size_t n, const char *b, size_t m) {
        ціле r;

        r = ascii_strcasecmp_n(a, b, MIN(n, m));
        якщо (r != 0)
                повернути r;

        повернути CMP(n, m);
}

bool chars_intersect(const char *a, const char *b) {
        /* Returns true if any of the chars in a are in b. */
        для (const char *p = a; *p; p++)
                якщо (strchr(b, *p))
                        повернути true;

        повернути false;
}

bool string_has_cc(const char *p, const char *ok) {
        assert(p);

        /*
         * Check if a string contains control characters. If 'ok' is
         * non-NULL it may be a string containing additional CCs to be
         * considered OK.
         */

        для (const char *t = p; *t; t++) {
                якщо (ok && strchr(ok, *t))
                        continue;

                якщо (char_is_cc(*t))
                        повернути true;
        }

        повернути false;
}

static ціле write_ellipsis(char *buf, bool unicode) {
        const char *s = glyph_full(GLYPH_ELLIPSIS, unicode);
        assert(strlen(s) == 3);
        memcpy(buf, s, 3);
        повернути 3;
}

static size_t ansi_sequence_length(const char *s, size_t len) {
        assert(s);

        якщо (len < 2)
                повернути 0;

        якщо (s[0] != 0x1B)  /* ASCII 27, aka ESC, aka Ctrl-[ */
                повернути 0;  /* Not the start of a sequence */

        якщо (s[1] == 0x5B) { /* [, start of CSI sequence */
                size_t i = 2;

                якщо (i == len)
                        повернути 0;

                поки (s[i] >= 0x30 && s[i] <= 0x3F) /* Parameter bytes */
                        якщо (++i == len)
                                повернути 0;
                поки (s[i] >= 0x20 && s[i] <= 0x2F) /* Intermediate bytes */
                        якщо (++i == len)
                                повернути 0;
                якщо (s[i] >= 0x40 && s[i] <= 0x7E) /* Final byte */
                        повернути i + 1;
                повернути 0;  /* Bad sequence */

        } інакше якщо (s[1] >= 0x40 && s[1] <= 0x5F) /* other non-CSI Fe sequence */
                повернути 2;

        повернути 0;  /* Bad escape? */
}

static bool string_has_ansi_sequence(const char *s, size_t len) {
        const char *t = s;

        поки ((t = memchr(t, 0x1B, len - (t - s)))) {
                якщо (ansi_sequence_length(t, len - (t - s)) > 0)
                        повернути true;
                t++;
        }
        повернути false;
}

static size_t previous_ansi_sequence(const char *s, size_t length, const char **ret_where) {

        assert(s);
        assert(ret_where);

        /* Locate the previous ANSI sequence and save its start in *ret_where and return length. */

        якщо (length < 2) {
                /* Need at least two bytes for an ANSI sequence */
                *ret_where = NULL;
                повернути 0;
        }

        для (size_t i = length - 2;; i--) {  /* -2 because at least two bytes are needed */
                size_t slen = ansi_sequence_length(s + i, length - i);
                якщо (slen > 0) {
                        *ret_where = s + i;
                        повернути slen;
                }

                якщо (i == 0)
                        break;
        }

        *ret_where = NULL;
        повернути 0;
}

static char *ascii_ellipsize_mem(const char *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, need_space, suffix_len;
        char *t;

        assert(s);
        assert(percent <= 100);
        assert(new_length != SIZE_MAX);

        якщо (old_length <= new_length)
                повернути strndup(s, old_length);

        /* Special case short ellipsations */
        switch (new_length) {

        case 0:
                повернути strdup("");

        case 1:
                якщо (is_locale_utf8())
                        повернути strdup("…");
                інакше
                        повернути strdup(".");

        case 2:
                якщо (!is_locale_utf8())
                        повернути strdup("..");
                break;
        }

        /* Calculate how much space the ellipsis will take up. If we are in UTF-8 mode we only need space for one
         * character ("…"), otherwise for three characters ("..."). Note that in both cases we need 3 bytes of storage,
         * either for the UTF-8 encoded character or for three ASCII characters. */
        need_space = is_locale_utf8() ? 1 : 3;

        t = new(char, new_length+3);
        якщо (!t)
                повернути NULL;

        assert(new_length >= need_space);

        x = ((new_length - need_space) * percent + 50) / 100;
        assert(x <= new_length - need_space);

        write_ellipsis(mempcpy(t, s, x), /* unicode= */ false);
        suffix_len = new_length - x - need_space;
        memcpy(t + x + 3, s + old_length - suffix_len, suffix_len);
        *(t + x + 3 + suffix_len) = '\0';

        повернути t;
}

/* Walk backwards from 'end' (exclusive) to the start of the last complete UTF-8 character, without
 * descending below 'start', validate it and return a pointer to its first byte, optionally decoding it
 * into *ret_c. Returns NULL on a truncated or otherwise malformed sequence. */
static const char* find_previous_unichar(const char *start, const char *end, char32_t *ret_c) {
        const char *p;
        ціле r;

        assert(start);
        assert(end);
        assert(end > start);

        для (p = end; p > start; ) {
                p--;
                якщо (((uint8_t) *p & 0xc0) != 0x80) /* Found a non-continuation byte, i.e. a character start. */
                        break;
        }

        r = utf8_encoded_valid_unichar(p, end - p);
        якщо (r < 0 || p + r != end)
                повернути NULL;

        якщо (ret_c)
                assert_se(utf8_encoded_to_unichar(p, ret_c) == r);

        повернути p;
}

char* ellipsize_mem(const char *s, size_t old_length, size_t new_length, unsigned percent) {
        size_t x, k, len, len2;
        const char *i, *j;
        ціле r;

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

        якщо (new_length == SIZE_MAX)
                повернути strndup(s, old_length);

        якщо (new_length == 0)
                повернути strdup("");

        bool has_ansi_seq = string_has_ansi_sequence(s, old_length);

        /* If no multibyte characters or ANSI sequences, use ascii_ellipsize_mem for speed. It equates
         * bytes with character cells, which holds for ASCII except for the tab, which
         * unichar_console_width() counts as 8 cells. */
        якщо (!has_ansi_seq && ascii_is_valid_n(s, old_length) && !memchr(s, '\t', old_length))
                повернути ascii_ellipsize_mem(s, old_length, new_length, percent);

        x = (new_length - 1) * percent / 100;
        assert(x <= new_length - 1);

        k = 0;
        для (i = s; i < s + old_length; ) {
                size_t slen = has_ansi_seq ? ansi_sequence_length(i, old_length - (i - s)) : 0;
                якщо (slen > 0) {
                        i += slen;
                        continue;  /* ANSI sequences don't take up any space in output */
                }

                r = utf8_encoded_valid_unichar(i, s + old_length - i);
                якщо (r < 0)
                        повернути NULL;

                char32_t c;
                assert_se(utf8_encoded_to_unichar(i, &c) == r);

                ціле w = unichar_console_width(c);
                якщо (k + w > x)
                        break;

                k += w;
                i += r;
        }

        const char *ansi_start = s + old_length;
        size_t ansi_len = 0;

        для (const char *t = j = s + old_length; t > i && k < new_length; ) {
                char32_t c;
                ціле w;
                const char *tt;

                якщо (has_ansi_seq && ansi_start >= t)
                        /* Figure out the previous ANSI sequence, if any */
                        ansi_len = previous_ansi_sequence(s, t - s, &ansi_start);

                /* If the sequence extends all the way to the current position, skip it. */
                якщо (has_ansi_seq && ansi_len > 0 && ansi_start + ansi_len == t) {
                        t = ansi_start;
                        continue;
                }

                /* Find the previous complete UTF-8 character inside the retained suffix. */
                tt = find_previous_unichar(i, t, &c);
                якщо (!tt)
                        повернути NULL;

                w = unichar_console_width(c);
                якщо (k + w > new_length)
                        break;

                k += w;
                j = t = tt;  /* j should always point to the first "real" character */
        }

        /* We don't actually need to ellipsize */
        якщо (i >= j)
                повернути memdup_suffix0(s, old_length);

        якщо (k >= new_length) {
                /* Make space for ellipsis, if required and possible. We know that the edge character is not
                 * part of an ANSI sequence (because then we'd skip it). If the last character we looked at
                 * was wide, we don't need to make space.
                 * Move the edge by one complete UTF-8 character within the input slice. */
                якщо (j < s + old_length) {
                        r = utf8_encoded_valid_unichar(j, s + old_length - j);
                        якщо (r < 0)
                                повернути NULL;

                        j += r;
                } інакше якщо (i > s) {
                        const char *tt = find_previous_unichar(s, i, NULL);
                        якщо (!tt)
                                повернути NULL;

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
        якщо (!e)
                повернути NULL;

        memcpy_safe(e, s, len);
        write_ellipsis(e + len, /* unicode= */ true);

        char *dst = e + len + 3;

        якщо (has_ansi_seq)
                /* Copy over any ANSI sequences in full */
                для (const char *p = s + len; p < j; ) {
                        size_t slen = ansi_sequence_length(p, j - p);
                        якщо (slen > 0) {
                                dst = mempcpy(dst, p, slen);
                                p += slen;
                        } інакше
                                p = utf8_next_char(p);
                }

        memcpy_safe(dst, j, len2);
        dst[len2] = '\0';

        повернути e;
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
                ціле w;

                якщо (*s == 0) /* terminating NUL detected? then we are done! */
                        goto done;

                w = cescape_char(*s, four);
                якщо (i + w + 1 > len) /* This character doesn't fit into the buffer anymore? In that case let's
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

                якщо (i + 4 <= len) /* nice, we reached our space goal */
                        break;

                k = k == 0 ? 3 : k - 1;
                якщо (last_char_width[k] == 0) /* bummer, we reached the beginning of the strings */
                        break;

                assert(i >= last_char_width[k]);
                i -= last_char_width[k];
        }

        якщо (i + 4 <= len) /* yay, enough space */
                i += write_ellipsis(buf + i, /* unicode= */ false);
        інакше якщо (i + 3 <= len) { /* only space for ".." */
                buf[i++] = '.';
                buf[i++] = '.';
        } інакше якщо (i + 2 <= len) /* only space for a single "." */
                buf[i++] = '.';
        інакше
                assert(i + 1 <= len);

done:
        buf[i] = '\0';
        повернути buf;
}

char* strshorten(char *s, size_t l) {
        assert(s);

        якщо (l >= SIZE_MAX-1) /* Would not change anything */
                повернути s;

        якщо (strnlen(s, l+1) > l)
                s[l] = 0;

        повернути s;
}

ціле strgrowpad0(char **s, size_t l) {
        size_t sz;

        assert(s);

        якщо (*s) {
                sz = strlen(*s) + 1;
                якщо (sz >= l) /* never shrink */
                        повернути 0;
        } інакше
                sz = 0;

        char *q = realloc(*s, l);
        якщо (!q)
                повернути -ENOMEM;

        *s = q;

        memzero(*s + sz, l - sz);
        повернути 0;
}

char* strreplace(const char *text, const char *old_string, const char *new_string) {
        size_t l, old_len, new_len;
        char *t, *ret = NULL;
        const char *f;

        assert(old_string);
        assert(new_string);

        якщо (!text)
                повернути NULL;

        old_len = strlen(old_string);
        new_len = strlen(new_string);

        l = strlen(text);
        якщо (!GREEDY_REALLOC(ret, l+1))
                повернути NULL;

        f = text;
        t = ret;
        поки (*f) {
                size_t d, nl;

                якщо (!startswith(f, old_string)) {
                        *(t++) = *(f++);
                        continue;
                }

                d = t - ret;
                nl = l - old_len + new_len;

                якщо (!GREEDY_REALLOC(ret, nl + 1))
                        повернути mfree(ret);

                l = nl;
                t = ret + d;

                t = stpcpy(t, new_string);
                f += old_len;
        }

        *t = 0;
        повернути ret;
}

static void advance_offsets(
                ssize_t diff,
                size_t offsets[2], /* note: we can't use [static 2] here, since this may be NULL */
                size_t shift[static 2],
                size_t size) {

        якщо (!offsets)
                повернути;

        assert(shift);

        якщо ((size_t) diff < offsets[0])
                shift[0] += size;
        якщо ((size_t) diff < offsets[1])
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

        якщо (!offsets)
                повернути;

        assert(shift);
        assert(begin <= end);

        для (size_t k = 0; k < 2; k++)
                якщо (offsets[k] > begin)
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
        якщо (!f)
                повернути NULL;

        для (const char *i = *ibuf; i < *ibuf + isz; i++) {
                ціле e;

                для (;;) {
                        e = ansi_seq_parser_feed(&parser, *i);
                        якщо (e < 0)
                                повернути NULL;
                        якщо (e != ANSI_SEQ_EVENT_ABORT)
                                break;

                        /* Eat up bogus sequences. The aborted sequence is over (and dropped, up to but
                         * excluding the current character, which is fed again), hence forget about it, so
                         * that the next sequence flushes pending carriage returns again. */
                        якщо (begin) {
                                retract_offsets(begin - *ibuf, i - *ibuf, highlight, shift);
                                begin = NULL;
                        }
                }

                switch (e) {

                case ANSI_SEQ_EVENT_TEXT:
                        якщо (*i == '\r') {
                                n_carriage_returns++;
                                continue;
                        }
                        якщо (*i == '\n') {
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

                        якщо (*i == '\t') {
                                fputs("        ", f);
                                advance_offsets(i - *ibuf, highlight, shift, 7);
                        } інакше
                                fputc(*i, f);

                        break;

                case ANSI_SEQ_EVENT_SEQUENCE:
                        якщо (!begin) {
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
        якщо (begin)
                retract_offsets(begin - *ibuf, isz, highlight, shift);

        /* Trailing carriage returns are dropped, too */
        retract_offsets(isz - n_carriage_returns, isz, highlight, shift);

        char *obuf;
        якщо (memstream_finalize(&m, &obuf, _isz) < 0)
                повернути NULL;

        free_and_replace(*ibuf, obuf);

        якщо (highlight) {
                highlight[0] += shift[0];
                highlight[1] += shift[1];
        }

        повернути *ibuf;
}

char* strextendv_with_separator(char **x, const char *separator, va_list ap) {
        _cleanup_free_ char *buffer = NULL;
        size_t f, l, l_separator;
        bool need_separator;
        char *nr, *p;

        якщо (!x)
                x = &buffer;

        l = f = strlen_ptr(*x);

        need_separator = !isempty(*x);
        l_separator = strlen_ptr(separator);

        va_list aq;
        va_copy(aq, ap);
        для (const char *t;;) {
                size_t n;

                t = va_arg(aq, const char *);
                якщо (!t)
                        break;
                якщо (t == POINTER_MAX)
                        continue;

                n = strlen(t);

                якщо (need_separator)
                        n += l_separator;

                якщо (n >= SIZE_MAX - l) {
                        va_end(aq);
                        повернути NULL;
                }

                l += n;
                need_separator = true;
        }
        va_end(aq);

        need_separator = !isempty(*x);

        nr = realloc(*x, GREEDY_ALLOC_ROUND_UP(l+1));
        якщо (!nr)
                повернути NULL;

        *x = nr;
        p = nr + f;

        для (;;) {
                const char *t;

                t = va_arg(ap, const char *);
                якщо (!t)
                        break;
                якщо (t == POINTER_MAX)
                        continue;

                якщо (need_separator && separator)
                        p = stpcpy(p, separator);

                p = stpcpy(p, t);

                need_separator = true;
        }

        assert(p == nr + l);
        *p = 0;

        /* If no buffer to extend was passed in return the start of the buffer */
        якщо (buffer)
                повернути TAKE_PTR(buffer);

        /* Otherwise we extended the buffer: return the end */
        повернути p;
}

char* strextend_with_separator_internal(char **x, const char *separator, ...) {
        va_list ap;
        char *ret;

        va_start(ap, separator);
        ret = strextendv_with_separator(x, separator, ap);
        va_end(ap);

        повернути ret;
}

ціле strextendf_with_separator(char **x, const char *separator, const char *format, ...) {
        size_t m, a, l_separator;
        va_list ap;
        ціле l;

        /* Appends a formatted string to the specified string. Don't use this in inner loops, since then
         * we'll spend a tonload of time in determining the length of the string passed in, over and over
         * again. */

        assert(x);
        assert(format);

        l_separator = isempty(*x) ? 0 : strlen_ptr(separator);

        /* Let's try to use the allocated buffer, if there's room at the end still. Otherwise let's extend by 64 chars. */
        якщо (*x) {
                m = strlen(*x);
                a = MALLOC_SIZEOF_SAFE(*x);
                assert(a >= m + 1);
        } інакше
                m = a = 0;

        якщо (a - m < 17 + l_separator) { /* if there's less than 16 chars space, then enlarge the buffer first */
                char *n;

                якщо (_unlikely_(l_separator > SIZE_MAX - 64)) /* overflow check #1 */
                        повернути -ENOMEM;
                якщо (_unlikely_(m > SIZE_MAX - 64 - l_separator)) /* overflow check #2 */
                        повернути -ENOMEM;

                n = realloc(*x, m + 64 + l_separator);
                якщо (!n)
                        повернути -ENOMEM;

                *x = n;
                a = MALLOC_SIZEOF_SAFE(*x);
        }

        /* Now, let's try to format the string into it */
        memcpy_safe(*x + m, separator, l_separator);
        va_start(ap, format);
        l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
        va_end(ap);

        assert(l >= 0);

        якщо ((size_t) l < a - m - l_separator) {
                char *n;

                /* Nice! This worked. We are done. But first, let's return the extra space we don't
                 * need. This should be a cheap operation, since we only lower the allocation size here,
                 * never increase. */
                n = realloc(*x, m + (size_t) l + l_separator + 1);
                якщо (n)
                        *x = n;
        } інакше {
                char *n;

                /* Wasn't enough. Then let's allocate exactly what we need. */

                якщо (_unlikely_((size_t) l > SIZE_MAX - (l_separator + 1))) /* overflow check #1 */
                        goto oom;
                якщо (_unlikely_(m > SIZE_MAX - ((size_t) l + l_separator + 1))) /* overflow check #2 */
                        goto oom;

                a = m + (size_t) l + l_separator + 1;
                n = realloc(*x, a);
                якщо (!n)
                        goto oom;
                *x = n;

                va_start(ap, format);
                l = vsnprintf(*x + m + l_separator, a - m - l_separator, format, ap);
                va_end(ap);

                assert((size_t) l < a - m - l_separator);
        }

        повернути 0;

oom:
        /* truncate the bytes added after memcpy_safe() again */
        (*x)[m] = 0;
        повернути -ENOMEM;
}

char* strrep(const char *s, size_t n) {
        char *ret, *p;
        size_t l;

        assert(s);

        l = strlen(s);
        якщо (!MUL_ASSIGN_SAFE(&l, n))
                повернути NULL;
        якщо (!INC_SAFE(&l, 1))
                повернути NULL;

        p = ret = malloc(l);
        якщо (!ret)
                повернути NULL;

        для (size_t i = 0; i < n; i++)
                p = stpcpy(p, s);

        *p = 0;
        повернути ret;
}

ціле split_pair(const char *s, const char *sep, char **ret_first, char **ret_second) {
        assert(s);
        assert(!isempty(sep));

        const char *x = strstr(s, sep);
        якщо (!x)
                повернути -EINVAL;

        _cleanup_free_ char *a = NULL;
        якщо (ret_first) {
                a = strndup(s, x - s);
                якщо (!a)
                        повернути -ENOMEM;
        }

        _cleanup_free_ char *b = NULL;
        якщо (ret_second) {
                b = strdup(x + strlen(sep));
                якщо (!b)
                        повернути -ENOMEM;
        }

        якщо (ret_first)
                *ret_first = TAKE_PTR(a);
        якщо (ret_second)
                *ret_second = TAKE_PTR(b);
        повернути 0;
}

ціле free_and_strdup(char **p, const char *s) {
        char *t;

        assert(p);

        /* Replaces a string pointer with a strdup()ed new string,
         * possibly freeing the old one. */

        якщо (streq_ptr(*p, s))
                повернути 0;

        якщо (s) {
                t = strdup(s);
                якщо (!t)
                        повернути -ENOMEM;
        } інакше
                t = NULL;

        free_and_replace(*p, t);

        повернути 1;
}

ціле free_and_strdup_warn(char **p, const char *s) {
        ціле r;

        r = free_and_strdup(p, s);
        якщо (r < 0)
                повернути log_oom();
        повернути r;
}

ціле free_and_strndup(char **p, const char *s, size_t l) {
        char *t;

        assert(p);
        assert(s || l == 0);

        /* Replaces a string pointer with a strndup()ed new string,
         * freeing the old one. */

        якщо (!*p && !s)
                повернути 0;

        якщо (*p && s && strneq(*p, s, l) && (l > strlen(*p) || (*p)[l] == '\0'))
                повернути 0;

        якщо (s) {
                t = strndup(s, l);
                якщо (!t)
                        повернути -ENOMEM;
        } інакше
                t = NULL;

        free_and_replace(*p, t);
        повернути 1;
}

ціле strdup_to_full(char **ret, const char *src) {
        якщо (!src) {
                якщо (ret)
                        *ret = NULL;

                повернути 0;
        } інакше {
                якщо (ret) {
                        char *t = strdup(src);
                        якщо (!t)
                                повернути -ENOMEM;
                        *ret = t;
                }

                повернути 1;
        }
};

bool string_is_safe(const char *p, StringSafeFlags flags) {

        /* Baseline checks are:
         *   • No control characters (i.e. 0…31 + 127)
         *   • UTF-8 valid (well, technically we skip this test if STRING_ASCII is set, since that is a tighter test)
         */

        якщо (FLAGS_SET(flags, STRING_ALLOW_EMPTY) ? !p : isempty(p))
                повернути false;

        якщо (!FLAGS_SET(flags, STRING_ASCII) && !utf8_is_valid(p))
                повернути false;

        для (const char *t = p; *t; t++) {
                /* never allow control characters, except for new line */
                якщо ((*t > 0 && *t < ' ' && *t != '\n') || *t == 0x7f)
                        повернути false;

                якщо (!FLAGS_SET(flags, STRING_ALLOW_NEWLINES) && *t == '\n')
                        повернути false;

                якщо (!FLAGS_SET(flags, STRING_ALLOW_BACKSLASHES) && *t == '\\')
                        повернути false;

                якщо (!FLAGS_SET(flags, STRING_ALLOW_QUOTES) && strchr(QUOTES, *t))
                        повернути false;

                якщо (!FLAGS_SET(flags, STRING_ALLOW_GLOBS) && strchr(GLOB_CHARS, *t))
                        повернути false;

                якщо (FLAGS_SET(flags, STRING_DISALLOW_WHITESPACE) && strchr(WHITESPACE, *t))
                        повернути false;

                якщо (FLAGS_SET(flags, STRING_ASCII) && (uint8_t) *t >= 0x80)
                        повернути false;
        }

        якщо (FLAGS_SET(flags, STRING_FILENAME) && !filename_is_valid(p))
                повернути false;

        якщо (FLAGS_SET(flags, STRING_FILENAME_PART) && !filename_part_is_valid(p))
                повернути false;

        повернути true;
}

char* str_realloc(char *p) {
        /* Reallocate *p to actual size. Ignore failure, and return the original string on error. */

        якщо (!p)
                повернути NULL;

        повернути realloc(p, strlen(p) + 1) ?: p;
}

char* string_erase(char *x) {
        якщо (!x)
                повернути NULL;

        /* A delicious drop of snake-oil! To be called on memory where we stored passphrases or so, after we
         * used them. */
        explicit_bzero_safe(x, strlen(x));
        повернути x;
}

ціле string_truncate_lines(const char *s, size_t n_lines, char **ret) {
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

                якщо (p[k] == 0) {
                        якщо (k == 0) /* final empty line */
                                break;

                        якщо (n >= n_lines) /* above threshold */
                                break;

                        e = p + k; /* last line to include */
                        break;
                }

                assert(p[k] == '\n');

                якщо (n >= n_lines)
                        break;

                якщо (k > 0)
                        e = p + k;

                p += k + 1;
                n++;
        }

        /* e points after the last character we want to keep */
        якщо (isempty(e))
                copy = strdup(s);
        інакше {
                якщо (!in_charset(e, "\n")) /* We only consider things truncated if we remove something that
                                           * isn't a new-line or a series of them */
                        truncation_applied = true;

                copy = strndup(s, e - s);
        }
        якщо (!copy)
                повернути -ENOMEM;

        *ret = copy;
        повернути truncation_applied;
}

ціле string_extract_line(const char *s, size_t i, char **ret) {
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
                якщо (i == c) {
                        /* The line we are looking for! */

                        якщо (q) {
                                char *m;

                                m = strndup(p, q - p);
                                якщо (!m)
                                        повернути -ENOMEM;

                                *ret = m;
                                повернути !isempty(q + 1); /* More coming? */
                        } інакше
                                /* Tell the caller to use the input string if equal */
                                повернути strdup_to(ret, p != s ? p : NULL);
                }

                якщо (!q)
                        /* No more lines, return empty line */
                        повернути strdup_to(ret, "");

                p = q + 1;
                c++;
        }
}

ціле string_contains_word_strv(const char *string, const char *separators, char * const *words, const char **ret_word) {
        /* In the default mode with no separators specified, we split on whitespace and coalesce separators. */
        const ExtractFlags flags = separators ? EXTRACT_DONT_COALESCE_SEPARATORS : 0;
        const char *found = NULL;
        ціле r;

        для (;;) {
                _cleanup_free_ char *w = NULL;

                r = extract_first_word(&string, &w, separators, flags);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        break;

                found = strv_find(words, w);
                якщо (found)
                        break;
        }

        якщо (ret_word)
                *ret_word = found;
        повернути !!found;
}

bool streq_skip_trailing_chars(const char *s1, const char *s2, const char *ok) {
        якщо (!s1 && !s2)
                повернути true;
        якщо (!s1 || !s2)
                повернути false;

        якщо (!ok)
                ok = WHITESPACE;

        для (; *s1 && *s2; s1++, s2++)
                якщо (*s1 != *s2)
                        break;

        повернути in_charset(s1, ok) && in_charset(s2, ok);
}

char* string_replace_char(char *str, char old_char, char new_char) {
        assert(str);
        assert(old_char != '\0');
        assert(new_char != '\0');
        assert(old_char != new_char);

        для (char *p = strchr(str, old_char); p; p = strchr(p + 1, old_char))
                *p = new_char;

        повернути str;
}

ціле make_cstring(const void *s, size_t n, MakeCStringMode mode, char **ret) {
        char *b;

        assert(s || n == 0);
        assert(mode >= 0);
        assert(mode < _MAKE_CSTRING_MODE_MAX);

        /* Converts a sized character buffer into a NUL-terminated NUL string, refusing if there are embedded
         * NUL bytes. Whether to expect a trailing NUL byte can be specified via 'mode' */

        якщо (n == 0) {
                якщо (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        повернути -EINVAL;

                якщо (!ret)
                        повернути 0;

                b = new0(char, 1);
        } інакше {
                const uint8_t *nul;

                nul = memchr(s, 0, n);
                якщо (nul) {
                        якщо (nul < (const uint8_t*) s + n - 1 || /* embedded NUL? */
                            mode == MAKE_CSTRING_REFUSE_TRAILING_NUL)
                                повернути -EINVAL;

                        n--;
                } інакше якщо (mode == MAKE_CSTRING_REQUIRE_TRAILING_NUL)
                        повернути -EINVAL;

                якщо (!ret)
                        повернути 0;

                b = memdup_suffix0(s, n);
        }
        якщо (!b)
                повернути -ENOMEM;

        *ret = b;
        повернути 0;
}

size_t strspn_from_end(const char *str, const char *accept) {
        size_t n = 0;

        якщо (isempty(str))
                повернути 0;

        якщо (isempty(accept))
                повернути 0;

        для (const char *p = str + strlen(str); p > str && strchr(accept, p[-1]); p--)
                n++;

        повернути n;
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

        повернути i;
}

char* strdupspn(const char *a, const char *accept) {
        якщо (isempty(a) || isempty(accept))
                повернути strdup("");

        повернути strndup(a, strspn(a, accept));
}

char* strdupcspn(const char *a, const char *reject) {
        якщо (isempty(a))
                повернути strdup("");
        якщо (isempty(reject))
                повернути strdup(a);

        повернути strndup(a, strcspn(a, reject));
}

char* find_line_startswith_internal(const char *haystack, const char *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that starts with the specified string. Returns a pointer to the
         * first character after it */

        char *p = (char*) strstr(haystack, needle);
        якщо (!p)
                повернути NULL;

        якщо (p > haystack)
                поки (p[-1] != '\n') {
                        p = strstr(p + 1, needle);
                        якщо (!p)
                                повернути NULL;
                }

        повернути p + strlen(needle);
}

char* find_line_internal(const char *haystack, const char *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * beginning of the line */

        char *p = (char*) find_line_startswith(haystack, needle);
        якщо (!p)
                повернути NULL;

        якщо (*p == 0 || strchr(NEWLINE, *p))
                повернути p - strlen(needle);

        повернути NULL;
}

char* find_line_after_internal(const char *haystack, const char *needle) {
        assert(haystack);
        assert(needle);

        /* Finds the first line in 'haystack' that match the specified string. Returns a pointer to the
         * next line after it */

        char *p = (char*) find_line_startswith(haystack, needle);
        якщо (!p)
                повернути NULL;

        якщо (*p == 0)
                повернути p;
        якщо (strchr(NEWLINE, *p))
                повернути p + 1;

        повернути NULL;
}

bool version_is_valid(const char *s, VersionFlags flags) {

        /* Validates a version string superficially. This does not process the version string in any
         * semantical way, it mostly just validates that its charset is reasonable. */

        якщо (FLAGS_SET(flags, VERSION_ALLOW_EMPTY) ? !s : isempty(s))
                повернути false;

        якщо (!filename_part_is_valid(s))
                повернути false;

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

        якщо (FLAGS_SET(flags, VERSION_ALLOW_UNDERSCORE))
                charset[l++] = '_';
        якщо (FLAGS_SET(flags, VERSION_ALLOW_PLUS))
                charset[l++] = '+';

        повернути in_charset(s, charset);
}

ssize_t strlevenshtein(const char *x, const char *y) {
        _cleanup_free_ size_t *t0 = NULL, *t1 = NULL, *t2 = NULL;
        size_t xl, yl;

        /* This is inspired from the Linux kernel's Levenshtein implementation */

        якщо (streq_ptr(x, y))
                повернути 0;

        xl = strlen_ptr(x);
        якщо (xl > SSIZE_MAX)
                повернути -E2BIG;

        yl = strlen_ptr(y);
        якщо (yl > SSIZE_MAX)
                повернути -E2BIG;

        якщо (isempty(x))
                повернути yl;
        якщо (isempty(y))
                повернути xl;

        t0 = new0(size_t, yl + 1);
        якщо (!t0)
                повернути -ENOMEM;
        t1 = new0(size_t, yl + 1);
        якщо (!t1)
                повернути -ENOMEM;
        t2 = new0(size_t, yl + 1);
        якщо (!t2)
                повернути -ENOMEM;

        для (size_t i = 0; i <= yl; i++)
                t1[i] = i;

        для (size_t i = 0; i < xl; i++) {
                t2[0] = i + 1;

                для (size_t j = 0; j < yl; j++) {
                        /* Substitution */
                        t2[j+1] = t1[j] + (x[i] != y[j]);

                        /* Swap */
                        якщо (i > 0 && j > 0 && x[i-1] == y[j] && x[i] == y[j-1] && t2[j+1] > t0[j-1] + 1)
                                t2[j+1] = t0[j-1] + 1;

                        /* Deletion */
                        якщо (t2[j+1] > t1[j+1] + 1)
                                t2[j+1] = t1[j+1] + 1;

                        /* Insertion */
                        якщо (t2[j+1] > t2[j] + 1)
                                t2[j+1] = t2[j] + 1;
                }

                size_t *dummy = t0;
                t0 = t1;
                t1 = t2;
                t2 = dummy;
        }

        повернути t1[yl];
}

char* strrstr_internal(const char *haystack, const char *needle) {
        /* Like strstr() but returns the last rather than the first occurrence of "needle" in "haystack". */

        якщо (!haystack || !needle)
                повернути NULL;

        /* Special case: for the empty string we return the very last possible occurrence, i.e. *after* the
         * last char, not before. */
        якщо (needle[0] == 0)
                повернути (char*) strchr(haystack, 0);

        /* Special case: for single character strings, just use optimized strrchr() */
        якщо (needle[1] == 0)
                повернути (char*) strrchr(haystack, needle[0]);

        для (const char *p = strstr(haystack, needle), *q; p; p = q) {
                q = strstr(p + 1, needle);
                якщо (!q)
                        повернути (char*) p;
        }
        повернути NULL;
}

char* strrstr_no_case_internal(const char *haystack, const char *needle) {
        якщо (!haystack || !needle)
                повернути NULL;

        для (const char *p = strchr(haystack, 0); p > haystack; p--)
                якщо (startswith_no_case(p, needle))
                        повернути (char*) p;

        повернути startswith_no_case(haystack, needle) ? (char*) haystack : NULL;
}

size_t str_common_prefix(const char *a, const char *b) {
        assert(a);
        assert(b);

        /* Returns the length of the common prefix of the two specified strings, or SIZE_MAX in case the
         * strings are fully identical. */

        для (size_t n = 0;; n++) {
                char c = a[n];
                якщо (c != b[n])
                        повернути n;
                якщо (c == 0)
                        повернути SIZE_MAX;
        }
}
