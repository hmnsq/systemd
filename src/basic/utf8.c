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

        ако (ch >= 0x110000) /* End of unicode space */
                върни false;
        ако ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                върни false;
        ако ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                върни false;
        ако ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                върни false;

        върни true;
}

static bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        върни (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
static size_t utf8_encoded_expected_len(uint8_t c) {
        ако (c < 0x80)
                върни 1;
        ако ((c & 0xe0) == 0xc0)
                върни 2;
        ако ((c & 0xf0) == 0xe0)
                върни 3;
        ако ((c & 0xf8) == 0xf0)
                върни 4;
        ако ((c & 0xfc) == 0xf8)
                върни 5;
        ако ((c & 0xfe) == 0xfc)
                върни 6;

        върни 0;
}

/* decode one unicode char */
цяло utf8_encoded_to_unichar(const char *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        assert(str);
        assert(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        switch (len) {
        case 1:
                *ret_unichar = (char32_t)str[0];
                върни 1;
        case 2:
                unichar = str[0] & 0x1f;
                break;
        case 3:
                unichar = (char32_t)str[0] & 0x0f;
                break;
        case 4:
                unichar = (char32_t)str[0] & 0x07;
                break;
        case 5:
                unichar = (char32_t)str[0] & 0x03;
                break;
        case 6:
                unichar = (char32_t)str[0] & 0x01;
                break;
        default:
                върни -EINVAL;
        }

        за (size_t i = 1; i < len; i++) {
                ако (((char32_t)str[i] & 0xc0) != 0x80)
                        върни -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        върни len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
static bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        върни (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(const char* str, size_t length, bool allow_newline) {
        assert(str);

        за (const char *p = str; length > 0;) {
                цяло encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                докато (length > 0) {
                        uint64_t w;
                        size_t n;

                        ако (length < sizeof(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } иначе {
                                n = sizeof(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ако (!is_printable_fastpath_u64(w))
                                break;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ако (length == 0)
                        break;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                ако (encoded_len < 0)
                        върни false;
                assert(encoded_len > 0 && (size_t) encoded_len <= length);

                ако (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        върни false;

                length -= encoded_len;
                p += encoded_len;
        }

        върни true;
}

char* utf8_is_valid_n(const char *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        assert(str);

        за (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                цяло len;

                ако (_unlikely_(str[i] == '\0'))
                        върни NULL; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                ако (_unlikely_(len < 0))
                        върни NULL; /* invalid character */

                i += len;
        }

        върни (char*) str;
}

char* utf8_escape_invalid(const char *str) {
        char *p, *s;

        assert(str);

        p = s = malloc(strlen(str) * 4 + 1);
        ако (!p)
                върни NULL;

        докато (*str) {
                цяло len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ако (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } иначе {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        върни str_realloc(p);
}

цяло utf8_char_console_width(const char *str) {
        char32_t c;
        цяло r;

        assert(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ако (r < 0)
                върни r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        върни unichar_console_width(c);
}

цяло unichar_console_width(char32_t c) {
        ако (c == '\t')
                върни 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        върни unichar_iswide(c) ? 2 : 1;
}

char* utf8_escape_non_printable_full(const char *str, size_t console_width, bool force_ellipsis) {
        char *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        assert(str);

        ако (console_width == 0)
                върни strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        ако (!p)
                върни NULL;

        за (;;) {
                цяло len;
                char *saved_s = s;

                ако (!*str) { /* done! */
                        ако (force_ellipsis)
                                goto truncation;
                        иначе
                                goto finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ако (len > 0) {
                        ако (utf8_is_printable(str, len)) {
                                цяло w;

                                w = utf8_char_console_width(str);
                                assert(w >= 0);
                                ако (n + w > console_width)
                                        goto truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } иначе {
                                за (; len > 0; len--) {
                                        ако (n + 4 > console_width)
                                                goto truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((цяло) *str >> 4);
                                        *(s++) = hexchar((цяло) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } иначе {
                        ако (n + 1 > console_width)
                                goto truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ако (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        върни str_realloc(p);
}

char* ascii_is_valid_n(const char *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        assert(str);

        за (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ако ((unsigned char) str[i] >= 128 || str[i] == '\0')
                        върни NULL;

        върни (char*) str;
}

цяло utf8_to_ascii(const char *str, char replacement_char, char **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        assert(str);
        assert(ret);

        _cleanup_free_ char *ans = new(char, strlen(str) + 1);
        ако (!ans)
                върни -ENOMEM;

        char *q = ans;

        за (const char *p = str; *p; q++) {
                цяло l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ако (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        върни l;

                ако (l == 1)
                        *q = *p;
                иначе
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        върни 0;
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
size_t utf8_encode_unichar(char *out_utf8, char32_t g) {

        ако (g < (1 << 7)) {
                ако (out_utf8)
                        out_utf8[0] = g & 0x7f;
                върни 1;
        } иначе ако (g < (1 << 11)) {
                ако (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                върни 2;
        } иначе ако (g < (1 << 16)) {
                ако (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                върни 3;
        } иначе ако (g < (1 << 21)) {
                ако (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                върни 4;
        }

        върни 0;
}

char* utf16_to_utf8(const char16_t *s, size_t length /* bytes! */) {
        const uint8_t *f;
        char *r, *t;

        ако (length == 0)
                върни new0(char, 1);

        assert(s);

        ако (length == SIZE_MAX) {
                length = char16_strlen(s);

                ако (length > SIZE_MAX/2)
                        върни NULL; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ако (length > (SIZE_MAX - 1) / 2)
                върни NULL; /* overflow */

        r = new(char, length * 2 + 1);
        ако (!r)
                върни NULL;

        f = (const uint8_t*) s;
        t = r;

        докато (f + 1 < (const uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ако (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        continue;
                }

                ако (utf16_is_trailing_surrogate(w1))
                        continue; /* spurious trailing surrogate, ignore */

                ако (f + 1 >= (const uint8_t*) s + length)
                        break;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ако (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        continue; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        върни r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        assert(out);

        /* Note that this encodes as little-endian. */

        switch (c) {

        case 0 ... 0xd7ffU:
        case 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                върни 1;

        case 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                върни 2;

        default: /* A surrogate (invalid) */
                върни 0;
        }
}

char16_t *utf8_to_utf16(const char *s, size_t length) {
        char16_t *n, *p;
        цяло r;

        ако (length == 0)
                върни new0(char16_t, 1);

        assert(s);

        ако (length == SIZE_MAX)
                length = strlen(s);

        ако (length > SIZE_MAX - 1)
                върни NULL; /* overflow */

        n = new(char16_t, length + 1);
        ако (!n)
                върни NULL;

        p = n;

        за (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ако (e <= 1) /* Invalid and single byte characters are copied as they are */
                        goto copy;

                ако (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        goto copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                ако (r < 0) /* sequence invalid, then copy as-is */
                        goto copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                continue;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        върни n;
}

size_t char16_strlen(const char16_t *s) {
        size_t n = 0;

        assert(s);

        докато (*s != 0)
                n++, s++;

        върни n;
}

size_t char16_strsize(const char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        върни s ? (char16_strlen(s) + 1) * sizeof(*s) : 0;
}

/* expected size used to encode one unicode char */
static цяло utf8_unichar_to_encoded_len(char32_t unichar) {

        ако (unichar < 0x80)
                върни 1;
        ако (unichar < 0x800)
                върни 2;
        ако (unichar < 0x10000)
                върни 3;
        ако (unichar < 0x200000)
                върни 4;
        ако (unichar < 0x4000000)
                върни 5;

        върни 6;
}

/* validate one encoded unicode char and return its length */
цяло utf8_encoded_valid_unichar_full(const char *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        цяло r;

        assert(str);
        assert(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ако (len == 0)
                върни -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ако (len > length)
                върни -EINVAL;

        /* ascii is valid */
        ако (len == 1) {
                ако (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                върни 1;
        }

        /* check if expected encoded chars are available */
        за (size_t i = 0; i < len; i++)
                ако ((str[i] & 0x80) != 0x80)
                        върни -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        ако (r < 0)
                върни r;

        /* check if encoded length matches encoded value */
        ако (utf8_unichar_to_encoded_len(unichar) != (цяло) len)
                върни -EINVAL;

        /* check if value has valid range */
        ако (!unichar_is_valid(unichar))
                върни -EINVAL;

        ако (ret_unichar)
                *ret_unichar = unichar;

        върни (цяло) len;
}

size_t utf8_n_codepoints(const char *str) {
        size_t n = 0;

        assert(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        докато (*str != 0) {
                цяло k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ако (k < 0)
                        върни SIZE_MAX;

                str += k;
                n++;
        }

        върни n;
}

size_t utf8_console_width(const char *str) {
        POINTER_MAY_BE_NULL(str);

        ако (isempty(str))
                върни 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        докато (*str) {
                цяло w;

                w = utf8_char_console_width(str);
                ако (w < 0)
                        върни SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        върни n;
}

size_t utf8_last_length(const char *s, size_t n) {
        цяло r;

        assert(s);

        ако (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        за (size_t last = 0;;) {
                ако (n == 0)
                        върни last;

                r = utf8_encoded_valid_unichar(s, n);
                ако (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
