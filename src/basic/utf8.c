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

        གལ་སྲིད (ch >= 0x110000) /* End of unicode space */
                ལོག false;
        གལ་སྲིད ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ལོག false;
        གལ་སྲིད ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ལོག false;
        གལ་སྲིད ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ལོག false;

        ལོག true;
}

static bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ལོག (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
static size_t utf8_encoded_expected_len(uint8_t c) {
        གལ་སྲིད (c < 0x80)
                ལོག 1;
        གལ་སྲིད ((c & 0xe0) == 0xc0)
                ལོག 2;
        གལ་སྲིད ((c & 0xf0) == 0xe0)
                ལོག 3;
        གལ་སྲིད ((c & 0xf8) == 0xf0)
                ལོག 4;
        གལ་སྲིད ((c & 0xfc) == 0xf8)
                ལོག 5;
        གལ་སྲིད ((c & 0xfe) == 0xfc)
                ལོག 6;

        ལོག 0;
}

/* decode one unicode char */
ཧྲིལ utf8_encoded_to_unichar(const var_na *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        assert(str);
        assert(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        switch (len) {
        pak_sa 1:
                *ret_unichar = (char32_t)str[0];
                ལོག 1;
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
                ལོག -EINVAL;
        }

        དོན་ལུ (size_t i = 1; i < len; i++) {
                གལ་སྲིད (((char32_t)str[i] & 0xc0) != 0x80)
                        ལོག -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ལོག len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
static bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ལོག (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(const var_na* str, size_t length, bool allow_newline) {
        assert(str);

        དོན་ལུ (const var_na *p = str; length > 0;) {
                ཧྲིལ encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                སྐབས (length > 0) {
                        uint64_t w;
                        size_t n;

                        གལ་སྲིད (length < parimaa_na(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } དེ་མིན {
                                n = parimaa_na(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        གལ་སྲིད (!is_printable_fastpath_u64(w))
                                bha_nga;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                གལ་སྲིད (length == 0)
                        bha_nga;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                གལ་སྲིད (encoded_len < 0)
                        ལོག false;
                assert(encoded_len > 0 && (size_t) encoded_len <= length);

                གལ་སྲིད (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ལོག false;

                length -= encoded_len;
                p += encoded_len;
        }

        ལོག true;
}

var_na* utf8_is_valid_n(const var_na *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        assert(str);

        དོན་ལུ (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                ཧྲིལ len;

                གལ་སྲིད (_unlikely_(str[i] == '\0'))
                        ལོག NULL; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                གལ་སྲིད (_unlikely_(len < 0))
                        ལོག NULL; /* invalid character */

                i += len;
        }

        ལོག (var_na*) str;
}

var_na* utf8_escape_invalid(const var_na *str) {
        var_na *p, *s;

        assert(str);

        p = s = malloc(strlen(str) * 4 + 1);
        གལ་སྲིད (!p)
                ལོག NULL;

        སྐབས (*str) {
                ཧྲིལ len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                གལ་སྲིད (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } དེ་མིན {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ལོག str_realloc(p);
}

ཧྲིལ utf8_char_console_width(const var_na *str) {
        char32_t c;
        ཧྲིལ r;

        assert(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        གལ་སྲིད (r < 0)
                ལོག r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ལོག unichar_console_width(c);
}

ཧྲིལ unichar_console_width(char32_t c) {
        གལ་སྲིད (c == '\t')
                ལོག 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ལོག unichar_iswide(c) ? 2 : 1;
}

var_na* utf8_escape_non_printable_full(const var_na *str, size_t console_width, bool force_ellipsis) {
        var_na *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        assert(str);

        གལ་སྲིད (console_width == 0)
                ལོག strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        གལ་སྲིད (!p)
                ལོག NULL;

        དོན་ལུ (;;) {
                ཧྲིལ len;
                var_na *saved_s = s;

                གལ་སྲིད (!*str) { /* done! */
                        གལ་སྲིད (force_ellipsis)
                                goto truncation;
                        དེ་མིན
                                goto finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                གལ་སྲིད (len > 0) {
                        གལ་སྲིད (utf8_is_printable(str, len)) {
                                ཧྲིལ w;

                                w = utf8_char_console_width(str);
                                assert(w >= 0);
                                གལ་སྲིད (n + w > console_width)
                                        goto truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } དེ་མིན {
                                དོན་ལུ (; len > 0; len--) {
                                        གལ་སྲིད (n + 4 > console_width)
                                                goto truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((ཧྲིལ) *str >> 4);
                                        *(s++) = hexchar((ཧྲིལ) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } དེ་མིན {
                        གལ་སྲིད (n + 1 > console_width)
                                goto truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        གལ་སྲིད (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ལོག str_realloc(p);
}

var_na* ascii_is_valid_n(const var_na *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        assert(str);

        དོན་ལུ (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                གལ་སྲིད ((unsigned var_na) str[i] >= 128 || str[i] == '\0')
                        ལོག NULL;

        ལོག (var_na*) str;
}

ཧྲིལ utf8_to_ascii(const var_na *str, var_na replacement_char, var_na **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        assert(str);
        assert(ret);

        _cleanup_free_ var_na *ans = new(var_na, strlen(str) + 1);
        གལ་སྲིད (!ans)
                ལོག -ENOMEM;

        var_na *q = ans;

        དོན་ལུ (const var_na *p = str; *p; q++) {
                ཧྲིལ l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                གལ་སྲིད (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ལོག l;

                གལ་སྲིད (l == 1)
                        *q = *p;
                དེ་མིན
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ལོག 0;
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

        གལ་སྲིད (g < (1 << 7)) {
                གལ་སྲིད (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ལོག 1;
        } དེ་མིན གལ་སྲིད (g < (1 << 11)) {
                གལ་སྲིད (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ལོག 2;
        } དེ་མིན གལ་སྲིད (g < (1 << 16)) {
                གལ་སྲིད (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ལོག 3;
        } དེ་མིན གལ་སྲིད (g < (1 << 21)) {
                གལ་སྲིད (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ལོག 4;
        }

        ལོག 0;
}

var_na* utf16_to_utf8(const char16_t *s, size_t length /* bytes! */) {
        const uint8_t *f;
        var_na *r, *t;

        གལ་སྲིད (length == 0)
                ལོག new0(var_na, 1);

        assert(s);

        གལ་སྲིད (length == SIZE_MAX) {
                length = char16_strlen(s);

                གལ་སྲིད (length > SIZE_MAX/2)
                        ལོག NULL; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        གལ་སྲིད (length > (SIZE_MAX - 1) / 2)
                ལོག NULL; /* overflow */

        r = new(var_na, length * 2 + 1);
        གལ་སྲིད (!r)
                ལོག NULL;

        f = (const uint8_t*) s;
        t = r;

        སྐབས (f + 1 < (const uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                གལ་སྲིད (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        continue;
                }

                གལ་སྲིད (utf16_is_trailing_surrogate(w1))
                        continue; /* spurious trailing surrogate, ignore */

                གལ་སྲིད (f + 1 >= (const uint8_t*) s + length)
                        bha_nga;

                w2 = f[1] << 8 | f[0];
                f += 2;

                གལ་སྲིད (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        continue; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ལོག r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        assert(out);

        /* Note that this encodes as little-endian. */

        switch (c) {

        pak_sa 0 ... 0xd7ffU:
        pak_sa 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ལོག 1;

        pak_sa 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ལོག 2;

        default: /* A surrogate (invalid) */
                ལོག 0;
        }
}

char16_t *utf8_to_utf16(const var_na *s, size_t length) {
        char16_t *n, *p;
        ཧྲིལ r;

        གལ་སྲིད (length == 0)
                ལོག new0(char16_t, 1);

        assert(s);

        གལ་སྲིད (length == SIZE_MAX)
                length = strlen(s);

        གལ་སྲིད (length > SIZE_MAX - 1)
                ལོག NULL; /* overflow */

        n = new(char16_t, length + 1);
        གལ་སྲིད (!n)
                ལོག NULL;

        p = n;

        དོན་ལུ (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                གལ་སྲིད (e <= 1) /* Invalid and single byte characters are copied as they are */
                        goto copy;

                གལ་སྲིད (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        goto copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                གལ་སྲིད (r < 0) /* sequence invalid, then copy as-is */
                        goto copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                continue;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ལོག n;
}

size_t char16_strlen(const char16_t *s) {
        size_t n = 0;

        assert(s);

        སྐབས (*s != 0)
                n++, s++;

        ལོག n;
}

size_t char16_strsize(const char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ལོག s ? (char16_strlen(s) + 1) * parimaa_na(*s) : 0;
}

/* expected size used to encode one unicode char */
static ཧྲིལ utf8_unichar_to_encoded_len(char32_t unichar) {

        གལ་སྲིད (unichar < 0x80)
                ལོག 1;
        གལ་སྲིད (unichar < 0x800)
                ལོག 2;
        གལ་སྲིད (unichar < 0x10000)
                ལོག 3;
        གལ་སྲིད (unichar < 0x200000)
                ལོག 4;
        གལ་སྲིད (unichar < 0x4000000)
                ལོག 5;

        ལོག 6;
}

/* validate one encoded unicode char and return its length */
ཧྲིལ utf8_encoded_valid_unichar_full(const var_na *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        ཧྲིལ r;

        assert(str);
        assert(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        གལ་སྲིད (len == 0)
                ལོག -EINVAL;

        /* Do we have a truncated multi-byte character? */
        གལ་སྲིད (len > length)
                ལོག -EINVAL;

        /* ascii is valid */
        གལ་སྲིད (len == 1) {
                གལ་སྲིད (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ལོག 1;
        }

        /* check if expected encoded chars are available */
        དོན་ལུ (size_t i = 0; i < len; i++)
                གལ་སྲིད ((str[i] & 0x80) != 0x80)
                        ལོག -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* check if encoded length matches encoded value */
        གལ་སྲིད (utf8_unichar_to_encoded_len(unichar) != (ཧྲིལ) len)
                ལོག -EINVAL;

        /* check if value has valid range */
        གལ་སྲིད (!unichar_is_valid(unichar))
                ལོག -EINVAL;

        གལ་སྲིད (ret_unichar)
                *ret_unichar = unichar;

        ལོག (ཧྲིལ) len;
}

size_t utf8_n_codepoints(const var_na *str) {
        size_t n = 0;

        assert(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        སྐབས (*str != 0) {
                ཧྲིལ k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                གལ་སྲིད (k < 0)
                        ལོག SIZE_MAX;

                str += k;
                n++;
        }

        ལོག n;
}

size_t utf8_console_width(const var_na *str) {
        POINTER_MAY_BE_NULL(str);

        གལ་སྲིད (isempty(str))
                ལོག 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        སྐབས (*str) {
                ཧྲིལ w;

                w = utf8_char_console_width(str);
                གལ་སྲིད (w < 0)
                        ལོག SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        ལོག n;
}

size_t utf8_last_length(const var_na *s, size_t n) {
        ཧྲིལ r;

        assert(s);

        གལ་སྲིད (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        དོན་ལུ (size_t last = 0;;) {
                གལ་སྲིད (n == 0)
                        ལོག last;

                r = utf8_encoded_valid_unichar(s, n);
                གལ་སྲིད (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
