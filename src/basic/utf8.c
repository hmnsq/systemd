/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#شامل "alloc-util.h"
#شامل "gunicode.h"
#شامل "hexdecoct.h"
#شامل "string-util.h"
#شامل "unaligned.h"
#شامل "utf8.h"

bool unichar_is_valid(char32_t ch) {

        اگر (ch >= 0x110000) /* End of unicode space */
                واپس false;
        اگر ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                واپس false;
        اگر ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                واپس false;
        اگر ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                واپس false;

        واپس true;
}

ساکن bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        واپس (ch < ' ' && !اندر_سیٹ(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ساکن size_t utf8_encoded_expected_len(uint8_t c) {
        اگر (c < 0x80)
                واپس 1;
        اگر ((c & 0xe0) == 0xc0)
                واپس 2;
        اگر ((c & 0xf0) == 0xe0)
                واپس 3;
        اگر ((c & 0xf8) == 0xf0)
                واپس 4;
        اگر ((c & 0xfc) == 0xf8)
                واپس 5;
        اگر ((c & 0xfe) == 0xfc)
                واپس 6;

        واپس 0;
}

/* decode one unicode char */
صحیح utf8_encoded_مقصد_unichar(ثابت حرف *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        أكد(str);
        أكد(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        انتخاب (len) {
        حالت 1:
                *ret_unichar = (char32_t)str[0];
                واپس 1;
        حالت 2:
                unichar = str[0] & 0x1f;
                توڑو;
        حالت 3:
                unichar = (char32_t)str[0] & 0x0f;
                توڑو;
        حالت 4:
                unichar = (char32_t)str[0] & 0x07;
                توڑو;
        حالت 5:
                unichar = (char32_t)str[0] & 0x03;
                توڑو;
        حالت 6:
                unichar = (char32_t)str[0] & 0x01;
                توڑو;
        طےشدہ:
                واپس -EINVAL;
        }

        برائے (size_t i = 1; i < len; i++) {
                اگر (((char32_t)str[i] & 0xc0) != 0x80)
                        واپس -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        واپس len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ساکن bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        واپس (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ثابت حرف* str, size_t length, bool allow_newline) {
        أكد(str);

        برائے (ثابت حرف *p = str; length > 0;) {
                صحیح encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                جب (length > 0) {
                        uint64_t w;
                        size_t n;

                        اگر (length < حجم(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                حافظہ_نقل(&w, p, n);
                        } ورنہ {
                                n = حجم(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        اگر (!is_printable_fastpath_u64(w))
                                توڑو;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                اگر (length == 0)
                        توڑو;

                encoded_len = utf8_encoded_valid_unichar_بھرا(p, length, &val);
                اگر (encoded_len < 0)
                        واپس false;
                أكد(encoded_len > 0 && (size_t) encoded_len <= length);

                اگر (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        واپس false;

                length -= encoded_len;
                p += encoded_len;
        }

        واپس true;
}

حرف* utf8_is_valid_n(ثابت حرف *str, size_t len_بائٹیں) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        أكد(str);

        برائے (size_t i = 0; len_بائٹیں != SIZE_MAX ? i < len_بائٹیں : str[i] != '\0'; ) {
                صحیح len;

                اگر (_unlikely_(str[i] == '\0'))
                        واپس تہی; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_بائٹیں != SIZE_MAX ? len_بائٹیں - i : SIZE_MAX);
                اگر (_unlikely_(len < 0))
                        واپس تہی; /* invalid character */

                i += len;
        }

        واپس (حرف*) str;
}

حرف* utf8_escape_invalid(ثابت حرف *str) {
        حرف *p, *s;

        أكد(str);

        p = s = حافظہ_دو(لڑی_لمبائی(str) * 4 + 1);
        اگر (!p)
                واپس تہی;

        جب (*str) {
                صحیح len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                اگر (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ورنہ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        واپس str_realloc(p);
}

صحیح utf8_char_console_چوڑائی(ثابت حرف *str) {
        char32_t c;
        صحیح r;

        أكد(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        اگر (r < 0)
                واپس r;

        assert_se(utf8_encoded_مقصد_unichar(str, &c) == r);

        واپس unichar_console_چوڑائی(c);
}

صحیح unichar_console_چوڑائی(char32_t c) {
        اگر (c == '\t')
                واپس 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        واپس unichar_iswide(c) ? 2 : 1;
}

حرف* utf8_escape_non_printable_بھرا(ثابت حرف *str, size_t console_چوڑائی, bool force_ellipsis) {
        حرف *p, *s, *پچھلا_s;
        size_t n = 0; /* estimated print width */

        أكد(str);

        اگر (console_چوڑائی == 0)
                واپس strdup("");

        size_t body = لڑی_لمبائی(str) * 4;
        p = s = پچھلا_s = حافظہ_دو(body + STRLEN("…") + 1);
        اگر (!p)
                واپس تہی;

        برائے (;;) {
                صحیح len;
                حرف *saved_s = s;

                اگر (!*str) { /* done! */
                        اگر (force_ellipsis)
                                جاؤ truncation;
                        ورنہ
                                جاؤ اختتام;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                اگر (len > 0) {
                        اگر (utf8_is_printable(str, len)) {
                                صحیح w;

                                w = utf8_char_console_چوڑائی(str);
                                أكد(w >= 0);
                                اگر (n + w > console_چوڑائی)
                                        جاؤ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ورنہ {
                                برائے (; len > 0; len--) {
                                        اگر (n + 4 > console_چوڑائی)
                                                جاؤ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((صحیح) *str >> 4);
                                        *(s++) = hexchar((صحیح) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ورنہ {
                        اگر (n + 1 > console_چوڑائی)
                                جاؤ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, لڑی_لمبائی(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                پچھلا_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        اگر (n + 1 > console_چوڑائی)
                s = پچھلا_s;

        s = mempcpy(s, "…", لڑی_لمبائی("…"));

 اختتام:
        *s = '\0';
        واپس str_realloc(p);
}

حرف* ascii_is_valid_n(ثابت حرف *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        أكد(str);

        برائے (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                اگر ((غیرعلامتی حرف) str[i] >= 128 || str[i] == '\0')
                        واپس تہی;

        واپس (حرف*) str;
}

صحیح utf8_مقصد_ascii(ثابت حرف *str, حرف replacement_char, حرف **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        أكد(str);
        أكد(ret);

        _cleanup_free_ حرف *ans = new(حرف, لڑی_لمبائی(str) + 1);
        اگر (!ans)
                واپس -ENOMEM;

        حرف *q = ans;

        برائے (ثابت حرف *p = str; *p; q++) {
                صحیح l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                اگر (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        واپس l;

                اگر (l == 1)
                        *q = *p;
                ورنہ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        واپس 0;
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
size_t utf8_encode_unichar(حرف *برآمد_utf8, char32_t g) {

        اگر (g < (1 << 7)) {
                اگر (برآمد_utf8)
                        برآمد_utf8[0] = g & 0x7f;
                واپس 1;
        } ورنہ اگر (g < (1 << 11)) {
                اگر (برآمد_utf8) {
                        برآمد_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        برآمد_utf8[1] = 0x80 | (g & 0x3f);
                }
                واپس 2;
        } ورنہ اگر (g < (1 << 16)) {
                اگر (برآمد_utf8) {
                        برآمد_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        برآمد_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        برآمد_utf8[2] = 0x80 | (g & 0x3f);
                }
                واپس 3;
        } ورنہ اگر (g < (1 << 21)) {
                اگر (برآمد_utf8) {
                        برآمد_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        برآمد_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        برآمد_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        برآمد_utf8[3] = 0x80 | (g & 0x3f);
                }
                واپس 4;
        }

        واپس 0;
}

حرف* utf16_مقصد_utf8(ثابت char16_t *s, size_t length /* bytes! */) {
        ثابت uint8_t *f;
        حرف *r, *t;

        اگر (length == 0)
                واپس new0(حرف, 1);

        أكد(s);

        اگر (length == SIZE_MAX) {
                length = char16_strlen(s);

                اگر (length > SIZE_MAX/2)
                        واپس تہی; /*حد_سے_تجاوز*/

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        اگر (length > (SIZE_MAX - 1) / 2)
                واپس تہی; /*حد_سے_تجاوز*/

        r = new(حرف, length * 2 + 1);
        اگر (!r)
                واپس تہی;

        f = (ثابت uint8_t*) s;
        t = r;

        جب (f + 1 < (ثابت uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                اگر (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        جاری;
                }

                اگر (utf16_is_trailing_surrogate(w1))
                        جاری; /* spurious trailing surrogate, ignore */

                اگر (f + 1 >= (ثابت uint8_t*) s + length)
                        توڑو;

                w2 = f[1] << 8 | f[0];
                f += 2;

                اگر (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        جاری; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_مقصد_unichar(w1, w2));
        }

        *t = 0;
        واپس r;
}

size_t utf16_encode_unichar(char16_t *برآمد, char32_t c) {
        أكد(برآمد);

        /* Note that this encodes as little-endian. */

        انتخاب (c) {

        حالت 0 ... 0xd7ffU:
        حالت 0xe000U ... 0xffffU:
                برآمد[0] = htole16(c);
                واپس 1;

        حالت 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                برآمد[0] = htole16((c >> 10) + 0xd800U);
                برآمد[1] = htole16((c & 0x3ffU) + 0xdc00U);
                واپس 2;

        طےشدہ: /* A surrogate (invalid) */
                واپس 0;
        }
}

char16_t *utf8_مقصد_utf16(ثابت حرف *s, size_t length) {
        char16_t *n, *p;
        صحیح r;

        اگر (length == 0)
                واپس new0(char16_t, 1);

        أكد(s);

        اگر (length == SIZE_MAX)
                length = لڑی_لمبائی(s);

        اگر (length > SIZE_MAX - 1)
                واپس تہی; /*حد_سے_تجاوز*/

        n = new(char16_t, length + 1);
        اگر (!n)
                واپس تہی;

        p = n;

        برائے (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                اگر (e <= 1) /* Invalid and single byte characters are copied as they are */
                        جاؤ copy;

                اگر (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        جاؤ copy;

                r = utf8_encoded_مقصد_unichar(s + i, &unichar);
                اگر (r < 0) /* sequence invalid, then copy as-is */
                        جاؤ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                جاری;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        واپس n;
}

size_t char16_strlen(ثابت char16_t *s) {
        size_t n = 0;

        أكد(s);

        جب (*s != 0)
                n++, s++;

        واپس n;
}

size_t char16_strsize(ثابت char16_t *s) {
        اشارہ_گر_MAY_BE_NULL(s);

        واپس s ? (char16_strlen(s) + 1) * حجم(*s) : 0;
}

/* expected size used to encode one unicode char */
ساکن صحیح utf8_unichar_مقصد_encoded_len(char32_t unichar) {

        اگر (unichar < 0x80)
                واپس 1;
        اگر (unichar < 0x800)
                واپس 2;
        اگر (unichar < 0x10000)
                واپس 3;
        اگر (unichar < 0x200000)
                واپس 4;
        اگر (unichar < 0x4000000)
                واپس 5;

        واپس 6;
}

/* validate one encoded unicode char and return its length */
صحیح utf8_encoded_valid_unichar_بھرا(ثابت حرف *str, size_t length /*بائٹیں*/, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        صحیح r;

        أكد(str);
        أكد(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        اگر (len == 0)
                واپس -EINVAL;

        /* Do we have a truncated multi-byte character? */
        اگر (len > length)
                واپس -EINVAL;

        /* ascii is valid */
        اگر (len == 1) {
                اگر (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                واپس 1;
        }

        /* check if expected encoded chars are available */
        برائے (size_t i = 0; i < len; i++)
                اگر ((str[i] & 0x80) != 0x80)
                        واپس -EINVAL;

        r = utf8_encoded_مقصد_unichar(str, &unichar);
        اگر (r < 0)
                واپس r;

        /* check if encoded length matches encoded value */
        اگر (utf8_unichar_مقصد_encoded_len(unichar) != (صحیح) len)
                واپس -EINVAL;

        /* check if value has valid range */
        اگر (!unichar_is_valid(unichar))
                واپس -EINVAL;

        اگر (ret_unichar)
                *ret_unichar = unichar;

        واپس (صحیح) len;
}

size_t utf8_n_codepoints(ثابت حرف *str) {
        size_t n = 0;

        أكد(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        جب (*str != 0) {
                صحیح k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                اگر (k < 0)
                        واپس SIZE_MAX;

                str += k;
                n++;
        }

        واپس n;
}

size_t utf8_console_چوڑائی(ثابت حرف *str) {
        اشارہ_گر_MAY_BE_NULL(str);

        اگر (isempty(str))
                واپس 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        جب (*str) {
                صحیح w;

                w = utf8_char_console_چوڑائی(str);
                اگر (w < 0)
                        واپس SIZE_MAX;

                n += w;
                str = utf8_اگلا_char(str);
        }

        واپس n;
}

size_t utf8_آخری_لمبائی(ثابت حرف *s, size_t n) {
        صحیح r;

        أكد(s);

        اگر (n == SIZE_MAX)
                n = لڑی_لمبائی(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        برائے (size_t آخری_2 = 0;;) {
                اگر (n == 0)
                        واپس آخری_2;

                r = utf8_encoded_valid_unichar(s, n);
                اگر (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                آخری_2 = r;
        }
}
