/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#include "alloc-util.h"
#include "gunicode.h"
#include "hexdecoct.h"
#include "string-util.h"
#include "unaligned.h"
#include "utf8.h"

bool unichar_is_valid(char32_t ch) {

        ᬬᬾᬦ᭄ (ch >= 0x110000) /* End of unicode space */
                ᬯᬮᬶ false;
        ᬬᬾᬦ᭄ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ᬯᬮᬶ false;
        ᬬᬾᬦ᭄ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ᬯᬮᬶ false;
        ᬬᬾᬦ᭄ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

static bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ᬯᬮᬶ (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
static size_t utf8_encoded_expected_len(uint8_t c) {
        ᬬᬾᬦ᭄ (c < 0x80)
                ᬯᬮᬶ 1;
        ᬬᬾᬦ᭄ ((c & 0xe0) == 0xc0)
                ᬯᬮᬶ 2;
        ᬬᬾᬦ᭄ ((c & 0xf0) == 0xe0)
                ᬯᬮᬶ 3;
        ᬬᬾᬦ᭄ ((c & 0xf8) == 0xf0)
                ᬯᬮᬶ 4;
        ᬬᬾᬦ᭄ ((c & 0xfc) == 0xf8)
                ᬯᬮᬶ 5;
        ᬬᬾᬦ᭄ ((c & 0xfe) == 0xfc)
                ᬯᬮᬶ 6;

        ᬯᬮᬶ 0;
}

/* decode one unicode char */
puur_naa_nka utf8_encoded_to_unichar(const var_na *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        assert(str);
        assert(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        switch (len) {
        pak_sa 1:
                *ret_unichar = (char32_t)str[0];
                ᬯᬮᬶ 1;
        pak_sa 2:
                unichar = str[0] & 0x1f;
                bha_nga;
        pak_sa 3:
                unichar = (char32_t)str[0] & 0x0f;
                bha_nga;
        pak_sa 4:
                unichar = (char32_t)str[0] & 0x07;
                bha_nga;
        pak_sa 5:
                unichar = (char32_t)str[0] & 0x03;
                bha_nga;
        pak_sa 6:
                unichar = (char32_t)str[0] & 0x01;
                bha_nga;
        default:
                ᬯᬮᬶ -EINVAL;
        }

        for (size_t i = 1; i < len; i++) {
                ᬬᬾᬦ᭄ (((char32_t)str[i] & 0xc0) != 0x80)
                        ᬯᬮᬶ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ᬯᬮᬶ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
static bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ᬯᬮᬶ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(const var_na* str, size_t length, bool allow_newline) {
        assert(str);

        for (const var_na *p = str; length > 0;) {
                puur_naa_nka encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ᬲᭂᬤᭂᬮ (length > 0) {
                        uint64_t w;
                        size_t n;

                        ᬬᬾᬦ᭄ (length < parimaa_na(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } ᬮᬶᬬ {
                                n = parimaa_na(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ᬬᬾᬦ᭄ (!is_printable_fastpath_u64(w))
                                bha_nga;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ᬬᬾᬦ᭄ (length == 0)
                        bha_nga;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                ᬬᬾᬦ᭄ (encoded_len < 0)
                        ᬯᬮᬶ false;
                assert(encoded_len > 0 && (size_t) encoded_len <= length);

                ᬬᬾᬦ᭄ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ᬯᬮᬶ false;

                length -= encoded_len;
                p += encoded_len;
        }

        ᬯᬮᬶ true;
}

var_na* utf8_is_valid_n(const var_na *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        assert(str);

        for (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                puur_naa_nka len;

                ᬬᬾᬦ᭄ (_unlikely_(str[i] == '\0'))
                        ᬯᬮᬶ NULL; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                ᬬᬾᬦ᭄ (_unlikely_(len < 0))
                        ᬯᬮᬶ NULL; /* invalid character */

                i += len;
        }

        ᬯᬮᬶ (var_na*) str;
}

var_na* utf8_escape_invalid(const var_na *str) {
        var_na *p, *s;

        assert(str);

        p = s = malloc(strlen(str) * 4 + 1);
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ NULL;

        ᬲᭂᬤᭂᬮ (*str) {
                puur_naa_nka len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ᬬᬾᬦ᭄ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ᬮᬶᬬ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ᬯᬮᬶ str_realloc(p);
}

puur_naa_nka utf8_char_console_width(const var_na *str) {
        char32_t c;
        puur_naa_nka r;

        assert(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ᬯᬮᬶ unichar_console_width(c);
}

puur_naa_nka unichar_console_width(char32_t c) {
        ᬬᬾᬦ᭄ (c == '\t')
                ᬯᬮᬶ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ᬯᬮᬶ unichar_iswide(c) ? 2 : 1;
}

var_na* utf8_escape_non_printable_full(const var_na *str, size_t console_width, bool force_ellipsis) {
        var_na *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        assert(str);

        ᬬᬾᬦ᭄ (console_width == 0)
                ᬯᬮᬶ strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ NULL;

        for (;;) {
                puur_naa_nka len;
                var_na *saved_s = s;

                ᬬᬾᬦ᭄ (!*str) { /* done! */
                        ᬬᬾᬦ᭄ (force_ellipsis)
                                goto truncation;
                        ᬮᬶᬬ
                                goto finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ᬬᬾᬦ᭄ (len > 0) {
                        ᬬᬾᬦ᭄ (utf8_is_printable(str, len)) {
                                puur_naa_nka w;

                                w = utf8_char_console_width(str);
                                assert(w >= 0);
                                ᬬᬾᬦ᭄ (n + w > console_width)
                                        goto truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ᬮᬶᬬ {
                                for (; len > 0; len--) {
                                        ᬬᬾᬦ᭄ (n + 4 > console_width)
                                                goto truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((puur_naa_nka) *str >> 4);
                                        *(s++) = hexchar((puur_naa_nka) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ᬮᬶᬬ {
                        ᬬᬾᬦ᭄ (n + 1 > console_width)
                                goto truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ᬬᬾᬦ᭄ (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ᬯᬮᬶ str_realloc(p);
}

var_na* ascii_is_valid_n(const var_na *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        assert(str);

        for (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ᬬᬾᬦ᭄ ((unsigned var_na) str[i] >= 128 || str[i] == '\0')
                        ᬯᬮᬶ NULL;

        ᬯᬮᬶ (var_na*) str;
}

puur_naa_nka utf8_to_ascii(const var_na *str, var_na replacement_char, var_na **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        assert(str);
        assert(ret);

        _cleanup_free_ var_na *ans = new(var_na, strlen(str) + 1);
        ᬬᬾᬦ᭄ (!ans)
                ᬯᬮᬶ -ENOMEM;

        var_na *q = ans;

        for (const var_na *p = str; *p; q++) {
                puur_naa_nka l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ᬬᬾᬦ᭄ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ᬯᬮᬶ l;

                ᬬᬾᬦ᭄ (l == 1)
                        *q = *p;
                ᬮᬶᬬ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ᬯᬮᬶ 0;
}

/**
 * utf8_encode_unichar() - Encode single UCS-4 character as UTF-8
 * @out_utf8: output buffer of at least 4 bytes or NULL
 * @g: UCS-4 character to encode
 *
 * This encodes a single UCS-4 character as UTF-8 and writes it into @out_utf8.
 * The length of the character is returned. It is not zero-terminated! If the
 * output buffer is NULL, only the length is returned.
 *
 * Returns: The length in bytes that the UTF-8 representation does or would
 *          occupy.
 */
size_t utf8_encode_unichar(var_na *out_utf8, char32_t g) {

        ᬬᬾᬦ᭄ (g < (1 << 7)) {
                ᬬᬾᬦ᭄ (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ᬯᬮᬶ 1;
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (g < (1 << 11)) {
                ᬬᬾᬦ᭄ (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ᬯᬮᬶ 2;
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (g < (1 << 16)) {
                ᬬᬾᬦ᭄ (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ᬯᬮᬶ 3;
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (g < (1 << 21)) {
                ᬬᬾᬦ᭄ (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ᬯᬮᬶ 4;
        }

        ᬯᬮᬶ 0;
}

var_na* utf16_to_utf8(const char16_t *s, size_t length /* bytes! */) {
        const uint8_t *f;
        var_na *r, *t;

        ᬬᬾᬦ᭄ (length == 0)
                ᬯᬮᬶ new0(var_na, 1);

        assert(s);

        ᬬᬾᬦ᭄ (length == SIZE_MAX) {
                length = char16_strlen(s);

                ᬬᬾᬦ᭄ (length > SIZE_MAX/2)
                        ᬯᬮᬶ NULL; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ᬬᬾᬦ᭄ (length > (SIZE_MAX - 1) / 2)
                ᬯᬮᬶ NULL; /* overflow */

        r = new(var_na, length * 2 + 1);
        ᬬᬾᬦ᭄ (!r)
                ᬯᬮᬶ NULL;

        f = (const uint8_t*) s;
        t = r;

        ᬲᭂᬤᭂᬮ (f + 1 < (const uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ᬬᬾᬦ᭄ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        continue;
                }

                ᬬᬾᬦ᭄ (utf16_is_trailing_surrogate(w1))
                        continue; /* spurious trailing surrogate, ignore */

                ᬬᬾᬦ᭄ (f + 1 >= (const uint8_t*) s + length)
                        bha_nga;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ᬬᬾᬦ᭄ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        continue; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ᬯᬮᬶ r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        assert(out);

        /* Note that this encodes as little-endian. */

        switch (c) {

        pak_sa 0 ... 0xd7ffU:
        pak_sa 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ᬯᬮᬶ 1;

        pak_sa 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ᬯᬮᬶ 2;

        default: /* A surrogate (invalid) */
                ᬯᬮᬶ 0;
        }
}

char16_t *utf8_to_utf16(const var_na *s, size_t length) {
        char16_t *n, *p;
        puur_naa_nka r;

        ᬬᬾᬦ᭄ (length == 0)
                ᬯᬮᬶ new0(char16_t, 1);

        assert(s);

        ᬬᬾᬦ᭄ (length == SIZE_MAX)
                length = strlen(s);

        ᬬᬾᬦ᭄ (length > SIZE_MAX - 1)
                ᬯᬮᬶ NULL; /* overflow */

        n = new(char16_t, length + 1);
        ᬬᬾᬦ᭄ (!n)
                ᬯᬮᬶ NULL;

        p = n;

        for (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ᬬᬾᬦ᭄ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        goto copy;

                ᬬᬾᬦ᭄ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        goto copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                ᬬᬾᬦ᭄ (r < 0) /* sequence invalid, then copy as-is */
                        goto copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                continue;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ᬯᬮᬶ n;
}

size_t char16_strlen(const char16_t *s) {
        size_t n = 0;

        assert(s);

        ᬲᭂᬤᭂᬮ (*s != 0)
                n++, s++;

        ᬯᬮᬶ n;
}

size_t char16_strsize(const char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ᬯᬮᬶ s ? (char16_strlen(s) + 1) * parimaa_na(*s) : 0;
}

/* expected size used to encode one unicode char */
static puur_naa_nka utf8_unichar_to_encoded_len(char32_t unichar) {

        ᬬᬾᬦ᭄ (unichar < 0x80)
                ᬯᬮᬶ 1;
        ᬬᬾᬦ᭄ (unichar < 0x800)
                ᬯᬮᬶ 2;
        ᬬᬾᬦ᭄ (unichar < 0x10000)
                ᬯᬮᬶ 3;
        ᬬᬾᬦ᭄ (unichar < 0x200000)
                ᬯᬮᬶ 4;
        ᬬᬾᬦ᭄ (unichar < 0x4000000)
                ᬯᬮᬶ 5;

        ᬯᬮᬶ 6;
}

/* validate one encoded unicode char and return its length */
puur_naa_nka utf8_encoded_valid_unichar_full(const var_na *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        puur_naa_nka r;

        assert(str);
        assert(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ᬬᬾᬦ᭄ (len == 0)
                ᬯᬮᬶ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ᬬᬾᬦ᭄ (len > length)
                ᬯᬮᬶ -EINVAL;

        /* ascii is valid */
        ᬬᬾᬦ᭄ (len == 1) {
                ᬬᬾᬦ᭄ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ᬯᬮᬶ 1;
        }

        /* check if expected encoded chars are available */
        for (size_t i = 0; i < len; i++)
                ᬬᬾᬦ᭄ ((str[i] & 0x80) != 0x80)
                        ᬯᬮᬶ -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        /* check if encoded length matches encoded value */
        ᬬᬾᬦ᭄ (utf8_unichar_to_encoded_len(unichar) != (puur_naa_nka) len)
                ᬯᬮᬶ -EINVAL;

        /* check if value has valid range */
        ᬬᬾᬦ᭄ (!unichar_is_valid(unichar))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (ret_unichar)
                *ret_unichar = unichar;

        ᬯᬮᬶ (puur_naa_nka) len;
}

size_t utf8_n_codepoints(const var_na *str) {
        size_t n = 0;

        assert(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ᬲᭂᬤᭂᬮ (*str != 0) {
                puur_naa_nka k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ᬬᬾᬦ᭄ (k < 0)
                        ᬯᬮᬶ SIZE_MAX;

                str += k;
                n++;
        }

        ᬯᬮᬶ n;
}

size_t utf8_console_width(const var_na *str) {
        POINTER_MAY_BE_NULL(str);

        ᬬᬾᬦ᭄ (isempty(str))
                ᬯᬮᬶ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ᬲᭂᬤᭂᬮ (*str) {
                puur_naa_nka w;

                w = utf8_char_console_width(str);
                ᬬᬾᬦ᭄ (w < 0)
                        ᬯᬮᬶ SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        ᬯᬮᬶ n;
}

size_t utf8_last_length(const var_na *s, size_t n) {
        puur_naa_nka r;

        assert(s);

        ᬬᬾᬦ᭄ (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        for (size_t last = 0;;) {
                ᬬᬾᬦ᭄ (n == 0)
                        ᬯᬮᬶ last;

                r = utf8_encoded_valid_unichar(s, n);
                ᬬᬾᬦ᭄ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
