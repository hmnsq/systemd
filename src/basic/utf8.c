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

        אם (ch >= 0x110000) /* End of unicode space */
                החזר false;
        אם ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                החזר false;
        אם ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                החזר false;
        אם ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                החזר false;

        החזר true;
}

ساكن bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        החזר (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ساكن size_t utf8_encoded_expected_len(uint8_t c) {
        אם (c < 0x80)
                החזר 1;
        אם ((c & 0xe0) == 0xc0)
                החזר 2;
        אם ((c & 0xf0) == 0xe0)
                החזר 3;
        אם ((c & 0xf8) == 0xf0)
                החזר 4;
        אם ((c & 0xfc) == 0xf8)
                החזר 5;
        אם ((c & 0xfe) == 0xfc)
                החזר 6;

        החזר 0;
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
                החזר 1;
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
                החזר -EINVAL;
        }

        עבור (size_t i = 1; i < len; i++) {
                אם (((char32_t)str[i] & 0xc0) != 0x80)
                        החזר -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        החזר len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ساكن bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        החזר (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ثابت حرف* str, size_t length, bool allow_newline) {
        أكد(str);

        עבור (ثابت حرف *p = str; length > 0;) {
                صحيح encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                כלעוד (length > 0) {
                        uint64_t w;
                        size_t n;

                        אם (length < حجم(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } אחרת {
                                n = حجم(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        אם (!is_printable_fastpath_u64(w))
                                اكسر;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                אם (length == 0)
                        اكسر;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                אם (encoded_len < 0)
                        החזר false;
                أكد(encoded_len > 0 && (size_t) encoded_len <= length);

                אם (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        החזר false;

                length -= encoded_len;
                p += encoded_len;
        }

        החזר true;
}

حرف* utf8_is_valid_n(ثابت حرف *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        أكد(str);

        עבור (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                صحيح len;

                אם (_unlikely_(str[i] == '\0'))
                        החזר عدم; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                אם (_unlikely_(len < 0))
                        החזר عدم; /* invalid character */

                i += len;
        }

        החזר (حرف*) str;
}

حرف* utf8_escape_invalid(ثابت حرف *str) {
        حرف *p, *s;

        أكد(str);

        p = s = malloc(strlen(str) * 4 + 1);
        אם (!p)
                החזר عدم;

        כלעוד (*str) {
                صحيح len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                אם (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } אחרת {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        החזר str_realloc(p);
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
        אם (r < 0)
                החזר r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        החזר unichar_console_width(c);
}

صحيح unichar_console_width(char32_t c) {
        אם (c == '\t')
                החזר 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        החזר unichar_iswide(c) ? 2 : 1;
}

حرف* utf8_escape_non_printable_full(ثابت حرف *str, size_t console_width, bool force_ellipsis) {
        حرف *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        أكد(str);

        אם (console_width == 0)
                החזר strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        אם (!p)
                החזר عدم;

        עבור (;;) {
                صحيح len;
                حرف *saved_s = s;

                אם (!*str) { /* done! */
                        אם (force_ellipsis)
                                اذهب truncation;
                        אחרת
                                اذهب finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                אם (len > 0) {
                        אם (utf8_is_printable(str, len)) {
                                صحيح w;

                                w = utf8_char_console_width(str);
                                أكد(w >= 0);
                                אם (n + w > console_width)
                                        اذهب truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } אחרת {
                                עבור (; len > 0; len--) {
                                        אם (n + 4 > console_width)
                                                اذهب truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((صحيح) *str >> 4);
                                        *(s++) = hexchar((صحيح) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } אחרת {
                        אם (n + 1 > console_width)
                                اذهب truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        אם (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        החזר str_realloc(p);
}

حرف* ascii_is_valid_n(ثابت حرف *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        أكد(str);

        עבור (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                אם ((unsigned حرف) str[i] >= 128 || str[i] == '\0')
                        החזר عدم;

        החזר (حرف*) str;
}

صحيح utf8_to_ascii(ثابت حرف *str, حرف replacement_char, حرف **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        أكد(str);
        أكد(ret);

        _cleanup_free_ حرف *ans = new(حرف, strlen(str) + 1);
        אם (!ans)
                החזר -ENOMEM;

        حرف *q = ans;

        עבור (ثابت حرف *p = str; *p; q++) {
                صحيح l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                אם (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        החזר l;

                אם (l == 1)
                        *q = *p;
                אחרת
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        החזר 0;
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

        אם (g < (1 << 7)) {
                אם (out_utf8)
                        out_utf8[0] = g & 0x7f;
                החזר 1;
        } אחרת אם (g < (1 << 11)) {
                אם (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                החזר 2;
        } אחרת אם (g < (1 << 16)) {
                אם (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                החזר 3;
        } אחרת אם (g < (1 << 21)) {
                אם (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                החזר 4;
        }

        החזר 0;
}

حرف* utf16_to_utf8(ثابت char16_t *s, size_t length /* bytes! */) {
        ثابت uint8_t *f;
        حرف *r, *t;

        אם (length == 0)
                החזר new0(حرف, 1);

        أكد(s);

        אם (length == SIZE_MAX) {
                length = char16_strlen(s);

                אם (length > SIZE_MAX/2)
                        החזר عدم; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        אם (length > (SIZE_MAX - 1) / 2)
                החזר عدم; /* overflow */

        r = new(حرف, length * 2 + 1);
        אם (!r)
                החזר عدم;

        f = (ثابت uint8_t*) s;
        t = r;

        כלעוד (f + 1 < (ثابت uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                אם (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        استمر;
                }

                אם (utf16_is_trailing_surrogate(w1))
                        استمر; /* spurious trailing surrogate, ignore */

                אם (f + 1 >= (ثابت uint8_t*) s + length)
                        اكسر;

                w2 = f[1] << 8 | f[0];
                f += 2;

                אם (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        استمر; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        החזר r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        أكد(out);

        /* Note that this encodes as little-endian. */

        بدل (c) {

        حالة 0 ... 0xd7ffU:
        حالة 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                החזר 1;

        حالة 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                החזר 2;

        افتراضي: /* A surrogate (invalid) */
                החזר 0;
        }
}

char16_t *utf8_to_utf16(ثابت حرف *s, size_t length) {
        char16_t *n, *p;
        صحيح r;

        אם (length == 0)
                החזר new0(char16_t, 1);

        أكد(s);

        אם (length == SIZE_MAX)
                length = strlen(s);

        אם (length > SIZE_MAX - 1)
                החזר عدم; /* overflow */

        n = new(char16_t, length + 1);
        אם (!n)
                החזר عدم;

        p = n;

        עבור (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                אם (e <= 1) /* Invalid and single byte characters are copied as they are */
                        اذهب copy;

                אם (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        اذهب copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                אם (r < 0) /* sequence invalid, then copy as-is */
                        اذهب copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                استمر;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        החזר n;
}

size_t char16_strlen(ثابت char16_t *s) {
        size_t n = 0;

        أكد(s);

        כלעוד (*s != 0)
                n++, s++;

        החזר n;
}

size_t char16_strsize(ثابت char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        החזר s ? (char16_strlen(s) + 1) * حجم(*s) : 0;
}

/* expected size used to encode one unicode char */
ساكن صحيح utf8_unichar_to_encoded_len(char32_t unichar) {

        אם (unichar < 0x80)
                החזר 1;
        אם (unichar < 0x800)
                החזר 2;
        אם (unichar < 0x10000)
                החזר 3;
        אם (unichar < 0x200000)
                החזר 4;
        אם (unichar < 0x4000000)
                החזר 5;

        החזר 6;
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
        אם (len == 0)
                החזר -EINVAL;

        /* Do we have a truncated multi-byte character? */
        אם (len > length)
                החזר -EINVAL;

        /* ascii is valid */
        אם (len == 1) {
                אם (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                החזר 1;
        }

        /* check if expected encoded chars are available */
        עבור (size_t i = 0; i < len; i++)
                אם ((str[i] & 0x80) != 0x80)
                        החזר -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        אם (r < 0)
                החזר r;

        /* check if encoded length matches encoded value */
        אם (utf8_unichar_to_encoded_len(unichar) != (صحيح) len)
                החזר -EINVAL;

        /* check if value has valid range */
        אם (!unichar_is_valid(unichar))
                החזר -EINVAL;

        אם (ret_unichar)
                *ret_unichar = unichar;

        החזר (صحيح) len;
}

size_t utf8_n_codepoints(ثابت حرف *str) {
        size_t n = 0;

        أكد(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        כלעוד (*str != 0) {
                صحيح k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                אם (k < 0)
                        החזר SIZE_MAX;

                str += k;
                n++;
        }

        החזר n;
}

size_t utf8_console_width(ثابت حرف *str) {
        POINTER_MAY_BE_NULL(str);

        אם (isempty(str))
                החזר 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        כלעוד (*str) {
                صحيح w;

                w = utf8_char_console_width(str);
                אם (w < 0)
                        החזר SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        החזר n;
}

size_t utf8_last_length(ثابت حرف *s, size_t n) {
        صحيح r;

        أكد(s);

        אם (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        עבור (size_t last = 0;;) {
                אם (n == 0)
                        החזר last;

                r = utf8_encoded_valid_unichar(s, n);
                אם (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
