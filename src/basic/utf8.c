/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#ضمن "alloc-util.h"
#ضمن "gunicode.h"
#ضمن "hexdecoct.h"
#ضمن "string-util.h"
#ضمن "unaligned.h"
#ضمن "utf8.h"

bool unichar_is_valid(char32_t ch) {

        ܐܢ (ch >= 0x110000) /* End of unicode space */
                ܦܢܐ false;
        ܐܢ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ܦܢܐ false;
        ܐܢ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ܦܢܐ false;
        ܐܢ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ܦܢܐ false;

        ܦܢܐ true;
}

ساكن bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ܦܢܐ (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ساكن size_t utf8_encoded_expected_len(uint8_t c) {
        ܐܢ (c < 0x80)
                ܦܢܐ 1;
        ܐܢ ((c & 0xe0) == 0xc0)
                ܦܢܐ 2;
        ܐܢ ((c & 0xf0) == 0xe0)
                ܦܢܐ 3;
        ܐܢ ((c & 0xf8) == 0xf0)
                ܦܢܐ 4;
        ܐܢ ((c & 0xfc) == 0xf8)
                ܦܢܐ 5;
        ܐܢ ((c & 0xfe) == 0xfc)
                ܦܢܐ 6;

        ܦܢܐ 0;
}

/* decode one unicode char */
صحيح utf8_encoded_to_unichar(ثابت حرف *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        أكد(str);
        أكد(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        بدل (len) {
        حالة 1:
                *ret_unichar = (char32_t)str[0];
                ܦܢܐ 1;
        حالة 2:
                unichar = str[0] & 0x1f;
                اكسر;
        حالة 3:
                unichar = (char32_t)str[0] & 0x0f;
                اكسر;
        حالة 4:
                unichar = (char32_t)str[0] & 0x07;
                اكسر;
        حالة 5:
                unichar = (char32_t)str[0] & 0x03;
                اكسر;
        حالة 6:
                unichar = (char32_t)str[0] & 0x01;
                اكسر;
        افتراضي:
                ܦܢܐ -EINVAL;
        }

        لكل (size_t i = 1; i < len; i++) {
                ܐܢ (((char32_t)str[i] & 0xc0) != 0x80)
                        ܦܢܐ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ܦܢܐ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ساكن bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ܦܢܐ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ثابت حرف* str, size_t length, bool allow_newline) {
        أكد(str);

        لكل (ثابت حرف *p = str; length > 0;) {
                صحيح encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ܟܡܐ (length > 0) {
                        uint64_t w;
                        size_t n;

                        ܐܢ (length < حجم(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } ܐܚܪܢܐ {
                                n = حجم(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ܐܢ (!is_printable_fastpath_u64(w))
                                اكسر;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ܐܢ (length == 0)
                        اكسر;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                ܐܢ (encoded_len < 0)
                        ܦܢܐ false;
                أكد(encoded_len > 0 && (size_t) encoded_len <= length);

                ܐܢ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ܦܢܐ false;

                length -= encoded_len;
                p += encoded_len;
        }

        ܦܢܐ true;
}

حرف* utf8_is_valid_n(ثابت حرف *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        أكد(str);

        لكل (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                صحيح len;

                ܐܢ (_unlikely_(str[i] == '\0'))
                        ܦܢܐ عدم; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                ܐܢ (_unlikely_(len < 0))
                        ܦܢܐ عدم; /* invalid character */

                i += len;
        }

        ܦܢܐ (حرف*) str;
}

حرف* utf8_escape_invalid(ثابت حرف *str) {
        حرف *p, *s;

        أكد(str);

        p = s = malloc(strlen(str) * 4 + 1);
        ܐܢ (!p)
                ܦܢܐ عدم;

        ܟܡܐ (*str) {
                صحيح len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ܐܢ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ܐܚܪܢܐ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ܦܢܐ str_realloc(p);
}

صحيح utf8_char_console_width(ثابت حرف *str) {
        char32_t c;
        صحيح r;

        أكد(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ܐܢ (r < 0)
                ܦܢܐ r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ܦܢܐ unichar_console_width(c);
}

صحيح unichar_console_width(char32_t c) {
        ܐܢ (c == '\t')
                ܦܢܐ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ܦܢܐ unichar_iswide(c) ? 2 : 1;
}

حرف* utf8_escape_non_printable_full(ثابت حرف *str, size_t console_width, bool force_ellipsis) {
        حرف *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        أكد(str);

        ܐܢ (console_width == 0)
                ܦܢܐ strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        ܐܢ (!p)
                ܦܢܐ عدم;

        لكل (;;) {
                صحيح len;
                حرف *saved_s = s;

                ܐܢ (!*str) { /* done! */
                        ܐܢ (force_ellipsis)
                                اذهب truncation;
                        ܐܚܪܢܐ
                                اذهب finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ܐܢ (len > 0) {
                        ܐܢ (utf8_is_printable(str, len)) {
                                صحيح w;

                                w = utf8_char_console_width(str);
                                أكد(w >= 0);
                                ܐܢ (n + w > console_width)
                                        اذهب truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ܐܚܪܢܐ {
                                لكل (; len > 0; len--) {
                                        ܐܢ (n + 4 > console_width)
                                                اذهب truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((صحيح) *str >> 4);
                                        *(s++) = hexchar((صحيح) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ܐܚܪܢܐ {
                        ܐܢ (n + 1 > console_width)
                                اذهب truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ܐܢ (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ܦܢܐ str_realloc(p);
}

حرف* ascii_is_valid_n(ثابت حرف *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        أكد(str);

        لكل (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ܐܢ ((unsigned حرف) str[i] >= 128 || str[i] == '\0')
                        ܦܢܐ عدم;

        ܦܢܐ (حرف*) str;
}

صحيح utf8_to_ascii(ثابت حرف *str, حرف replacement_char, حرف **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        أكد(str);
        أكد(ret);

        _cleanup_free_ حرف *ans = new(حرف, strlen(str) + 1);
        ܐܢ (!ans)
                ܦܢܐ -ENOMEM;

        حرف *q = ans;

        لكل (ثابت حرف *p = str; *p; q++) {
                صحيح l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ܐܢ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ܦܢܐ l;

                ܐܢ (l == 1)
                        *q = *p;
                ܐܚܪܢܐ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ܦܢܐ 0;
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
size_t utf8_encode_unichar(حرف *out_utf8, char32_t g) {

        ܐܢ (g < (1 << 7)) {
                ܐܢ (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ܦܢܐ 1;
        } ܐܚܪܢܐ ܐܢ (g < (1 << 11)) {
                ܐܢ (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ܦܢܐ 2;
        } ܐܚܪܢܐ ܐܢ (g < (1 << 16)) {
                ܐܢ (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ܦܢܐ 3;
        } ܐܚܪܢܐ ܐܢ (g < (1 << 21)) {
                ܐܢ (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ܦܢܐ 4;
        }

        ܦܢܐ 0;
}

حرف* utf16_to_utf8(ثابت char16_t *s, size_t length /* bytes! */) {
        ثابت uint8_t *f;
        حرف *r, *t;

        ܐܢ (length == 0)
                ܦܢܐ new0(حرف, 1);

        أكد(s);

        ܐܢ (length == SIZE_MAX) {
                length = char16_strlen(s);

                ܐܢ (length > SIZE_MAX/2)
                        ܦܢܐ عدم; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ܐܢ (length > (SIZE_MAX - 1) / 2)
                ܦܢܐ عدم; /* overflow */

        r = new(حرف, length * 2 + 1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        f = (ثابت uint8_t*) s;
        t = r;

        ܟܡܐ (f + 1 < (ثابت uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ܐܢ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        استمر;
                }

                ܐܢ (utf16_is_trailing_surrogate(w1))
                        استمر; /* spurious trailing surrogate, ignore */

                ܐܢ (f + 1 >= (ثابت uint8_t*) s + length)
                        اكسر;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ܐܢ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        استمر; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ܦܢܐ r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        أكد(out);

        /* Note that this encodes as little-endian. */

        بدل (c) {

        حالة 0 ... 0xd7ffU:
        حالة 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ܦܢܐ 1;

        حالة 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ܦܢܐ 2;

        افتراضي: /* A surrogate (invalid) */
                ܦܢܐ 0;
        }
}

char16_t *utf8_to_utf16(ثابت حرف *s, size_t length) {
        char16_t *n, *p;
        صحيح r;

        ܐܢ (length == 0)
                ܦܢܐ new0(char16_t, 1);

        أكد(s);

        ܐܢ (length == SIZE_MAX)
                length = strlen(s);

        ܐܢ (length > SIZE_MAX - 1)
                ܦܢܐ عدم; /* overflow */

        n = new(char16_t, length + 1);
        ܐܢ (!n)
                ܦܢܐ عدم;

        p = n;

        لكل (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ܐܢ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        اذهب copy;

                ܐܢ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        اذهب copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                ܐܢ (r < 0) /* sequence invalid, then copy as-is */
                        اذهب copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                استمر;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ܦܢܐ n;
}

size_t char16_strlen(ثابت char16_t *s) {
        size_t n = 0;

        أكد(s);

        ܟܡܐ (*s != 0)
                n++, s++;

        ܦܢܐ n;
}

size_t char16_strsize(ثابت char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ܦܢܐ s ? (char16_strlen(s) + 1) * حجم(*s) : 0;
}

/* expected size used to encode one unicode char */
ساكن صحيح utf8_unichar_to_encoded_len(char32_t unichar) {

        ܐܢ (unichar < 0x80)
                ܦܢܐ 1;
        ܐܢ (unichar < 0x800)
                ܦܢܐ 2;
        ܐܢ (unichar < 0x10000)
                ܦܢܐ 3;
        ܐܢ (unichar < 0x200000)
                ܦܢܐ 4;
        ܐܢ (unichar < 0x4000000)
                ܦܢܐ 5;

        ܦܢܐ 6;
}

/* validate one encoded unicode char and return its length */
صحيح utf8_encoded_valid_unichar_full(ثابت حرف *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        صحيح r;

        أكد(str);
        أكد(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ܐܢ (len == 0)
                ܦܢܐ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ܐܢ (len > length)
                ܦܢܐ -EINVAL;

        /* ascii is valid */
        ܐܢ (len == 1) {
                ܐܢ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ܦܢܐ 1;
        }

        /* check if expected encoded chars are available */
        لكل (size_t i = 0; i < len; i++)
                ܐܢ ((str[i] & 0x80) != 0x80)
                        ܦܢܐ -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* check if encoded length matches encoded value */
        ܐܢ (utf8_unichar_to_encoded_len(unichar) != (صحيح) len)
                ܦܢܐ -EINVAL;

        /* check if value has valid range */
        ܐܢ (!unichar_is_valid(unichar))
                ܦܢܐ -EINVAL;

        ܐܢ (ret_unichar)
                *ret_unichar = unichar;

        ܦܢܐ (صحيح) len;
}

size_t utf8_n_codepoints(ثابت حرف *str) {
        size_t n = 0;

        أكد(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ܟܡܐ (*str != 0) {
                صحيح k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ܐܢ (k < 0)
                        ܦܢܐ SIZE_MAX;

                str += k;
                n++;
        }

        ܦܢܐ n;
}

size_t utf8_console_width(ثابت حرف *str) {
        POINTER_MAY_BE_NULL(str);

        ܐܢ (isempty(str))
                ܦܢܐ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ܟܡܐ (*str) {
                صحيح w;

                w = utf8_char_console_width(str);
                ܐܢ (w < 0)
                        ܦܢܐ SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        ܦܢܐ n;
}

size_t utf8_last_length(ثابت حرف *s, size_t n) {
        صحيح r;

        أكد(s);

        ܐܢ (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        لكل (size_t last = 0;;) {
                ܐܢ (n == 0)
                        ܦܢܐ last;

                r = utf8_encoded_valid_unichar(s, n);
                ܐܢ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
