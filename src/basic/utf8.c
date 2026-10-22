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
                بازگشت false;
        اگر ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                بازگشت false;
        اگر ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                بازگشت false;
        اگر ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                بازگشت false;

        بازگشت true;
}

ایستا bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        بازگشت (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ایستا size_t utf8_encoded_expected_len(uint8_t c) {
        اگر (c < 0x80)
                بازگشت 1;
        اگر ((c & 0xe0) == 0xc0)
                بازگشت 2;
        اگر ((c & 0xf0) == 0xe0)
                بازگشت 3;
        اگر ((c & 0xf8) == 0xf0)
                بازگشت 4;
        اگر ((c & 0xfc) == 0xf8)
                بازگشت 5;
        اگر ((c & 0xfe) == 0xfc)
                بازگشت 6;

        بازگشت 0;
}

/* decode one unicode char */
صحیح utf8_encoded_to_unichar(ثابت نویسه *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        أكد(str);
        أكد(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        انتخاب (len) {
        حالت 1:
                *ret_unichar = (char32_t)str[0];
                بازگشت 1;
        حالت 2:
                unichar = str[0] & 0x1f;
                بشکن;
        حالت 3:
                unichar = (char32_t)str[0] & 0x0f;
                بشکن;
        حالت 4:
                unichar = (char32_t)str[0] & 0x07;
                بشکن;
        حالت 5:
                unichar = (char32_t)str[0] & 0x03;
                بشکن;
        حالت 6:
                unichar = (char32_t)str[0] & 0x01;
                بشکن;
        پیشفرض:
                بازگشت -EINVAL;
        }

        برای (size_t i = 1; i < len; i++) {
                اگر (((char32_t)str[i] & 0xc0) != 0x80)
                        بازگشت -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        بازگشت len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ایستا bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        بازگشت (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ثابت نویسه* str, size_t length, bool allow_newline) {
        أكد(str);

        برای (ثابت نویسه *p = str; length > 0;) {
                صحیح encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                وقتی (length > 0) {
                        uint64_t w;
                        size_t n;

                        اگر (length < اندازه(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } وگرنه {
                                n = اندازه(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        اگر (!is_printable_fastpath_u64(w))
                                بشکن;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                اگر (length == 0)
                        بشکن;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                اگر (encoded_len < 0)
                        بازگشت false;
                أكد(encoded_len > 0 && (size_t) encoded_len <= length);

                اگر (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        بازگشت false;

                length -= encoded_len;
                p += encoded_len;
        }

        بازگشت true;
}

نویسه* utf8_is_valid_n(ثابت نویسه *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        أكد(str);

        برای (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                صحیح len;

                اگر (_unlikely_(str[i] == '\0'))
                        بازگشت پوچ; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                اگر (_unlikely_(len < 0))
                        بازگشت پوچ; /* invalid character */

                i += len;
        }

        بازگشت (نویسه*) str;
}

نویسه* utf8_escape_invalid(ثابت نویسه *str) {
        نویسه *p, *s;

        أكد(str);

        p = s = malloc(strlen(str) * 4 + 1);
        اگر (!p)
                بازگشت پوچ;

        وقتی (*str) {
                صحیح len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                اگر (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } وگرنه {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        بازگشت str_realloc(p);
}

صحیح utf8_char_console_width(ثابت نویسه *str) {
        char32_t c;
        صحیح r;

        أكد(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        اگر (r < 0)
                بازگشت r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        بازگشت unichar_console_width(c);
}

صحیح unichar_console_width(char32_t c) {
        اگر (c == '\t')
                بازگشت 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        بازگشت unichar_iswide(c) ? 2 : 1;
}

نویسه* utf8_escape_non_printable_full(ثابت نویسه *str, size_t console_width, bool force_ellipsis) {
        نویسه *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        أكد(str);

        اگر (console_width == 0)
                بازگشت strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        اگر (!p)
                بازگشت پوچ;

        برای (;;) {
                صحیح len;
                نویسه *saved_s = s;

                اگر (!*str) { /* done! */
                        اگر (force_ellipsis)
                                برو truncation;
                        وگرنه
                                برو finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                اگر (len > 0) {
                        اگر (utf8_is_printable(str, len)) {
                                صحیح w;

                                w = utf8_char_console_width(str);
                                أكد(w >= 0);
                                اگر (n + w > console_width)
                                        برو truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } وگرنه {
                                برای (; len > 0; len--) {
                                        اگر (n + 4 > console_width)
                                                برو truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((صحیح) *str >> 4);
                                        *(s++) = hexchar((صحیح) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } وگرنه {
                        اگر (n + 1 > console_width)
                                برو truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        اگر (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        بازگشت str_realloc(p);
}

نویسه* ascii_is_valid_n(ثابت نویسه *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        أكد(str);

        برای (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                اگر ((unsigned نویسه) str[i] >= 128 || str[i] == '\0')
                        بازگشت پوچ;

        بازگشت (نویسه*) str;
}

صحیح utf8_to_ascii(ثابت نویسه *str, نویسه replacement_char, نویسه **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        أكد(str);
        أكد(ret);

        _cleanup_free_ نویسه *ans = new(نویسه, strlen(str) + 1);
        اگر (!ans)
                بازگشت -ENOMEM;

        نویسه *q = ans;

        برای (ثابت نویسه *p = str; *p; q++) {
                صحیح l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                اگر (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        بازگشت l;

                اگر (l == 1)
                        *q = *p;
                وگرنه
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        بازگشت 0;
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
size_t utf8_encode_unichar(نویسه *out_utf8, char32_t g) {

        اگر (g < (1 << 7)) {
                اگر (out_utf8)
                        out_utf8[0] = g & 0x7f;
                بازگشت 1;
        } وگرنه اگر (g < (1 << 11)) {
                اگر (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                بازگشت 2;
        } وگرنه اگر (g < (1 << 16)) {
                اگر (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                بازگشت 3;
        } وگرنه اگر (g < (1 << 21)) {
                اگر (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                بازگشت 4;
        }

        بازگشت 0;
}

نویسه* utf16_to_utf8(ثابت char16_t *s, size_t length /* bytes! */) {
        ثابت uint8_t *f;
        نویسه *r, *t;

        اگر (length == 0)
                بازگشت new0(نویسه, 1);

        أكد(s);

        اگر (length == SIZE_MAX) {
                length = char16_strlen(s);

                اگر (length > SIZE_MAX/2)
                        بازگشت پوچ; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        اگر (length > (SIZE_MAX - 1) / 2)
                بازگشت پوچ; /* overflow */

        r = new(نویسه, length * 2 + 1);
        اگر (!r)
                بازگشت پوچ;

        f = (ثابت uint8_t*) s;
        t = r;

        وقتی (f + 1 < (ثابت uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                اگر (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        ادامه;
                }

                اگر (utf16_is_trailing_surrogate(w1))
                        ادامه; /* spurious trailing surrogate, ignore */

                اگر (f + 1 >= (ثابت uint8_t*) s + length)
                        بشکن;

                w2 = f[1] << 8 | f[0];
                f += 2;

                اگر (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        ادامه; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        بازگشت r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        أكد(out);

        /* Note that this encodes as little-endian. */

        انتخاب (c) {

        حالت 0 ... 0xd7ffU:
        حالت 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                بازگشت 1;

        حالت 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                بازگشت 2;

        پیشفرض: /* A surrogate (invalid) */
                بازگشت 0;
        }
}

char16_t *utf8_to_utf16(ثابت نویسه *s, size_t length) {
        char16_t *n, *p;
        صحیح r;

        اگر (length == 0)
                بازگشت new0(char16_t, 1);

        أكد(s);

        اگر (length == SIZE_MAX)
                length = strlen(s);

        اگر (length > SIZE_MAX - 1)
                بازگشت پوچ; /* overflow */

        n = new(char16_t, length + 1);
        اگر (!n)
                بازگشت پوچ;

        p = n;

        برای (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                اگر (e <= 1) /* Invalid and single byte characters are copied as they are */
                        برو copy;

                اگر (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        برو copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                اگر (r < 0) /* sequence invalid, then copy as-is */
                        برو copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                ادامه;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        بازگشت n;
}

size_t char16_strlen(ثابت char16_t *s) {
        size_t n = 0;

        أكد(s);

        وقتی (*s != 0)
                n++, s++;

        بازگشت n;
}

size_t char16_strsize(ثابت char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        بازگشت s ? (char16_strlen(s) + 1) * اندازه(*s) : 0;
}

/* expected size used to encode one unicode char */
ایستا صحیح utf8_unichar_to_encoded_len(char32_t unichar) {

        اگر (unichar < 0x80)
                بازگشت 1;
        اگر (unichar < 0x800)
                بازگشت 2;
        اگر (unichar < 0x10000)
                بازگشت 3;
        اگر (unichar < 0x200000)
                بازگشت 4;
        اگر (unichar < 0x4000000)
                بازگشت 5;

        بازگشت 6;
}

/* validate one encoded unicode char and return its length */
صحیح utf8_encoded_valid_unichar_full(ثابت نویسه *str, size_t length /* bytes */, char32_t *ret_unichar) {
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
                بازگشت -EINVAL;

        /* Do we have a truncated multi-byte character? */
        اگر (len > length)
                بازگشت -EINVAL;

        /* ascii is valid */
        اگر (len == 1) {
                اگر (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                بازگشت 1;
        }

        /* check if expected encoded chars are available */
        برای (size_t i = 0; i < len; i++)
                اگر ((str[i] & 0x80) != 0x80)
                        بازگشت -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        اگر (r < 0)
                بازگشت r;

        /* check if encoded length matches encoded value */
        اگر (utf8_unichar_to_encoded_len(unichar) != (صحیح) len)
                بازگشت -EINVAL;

        /* check if value has valid range */
        اگر (!unichar_is_valid(unichar))
                بازگشت -EINVAL;

        اگر (ret_unichar)
                *ret_unichar = unichar;

        بازگشت (صحیح) len;
}

size_t utf8_n_codepoints(ثابت نویسه *str) {
        size_t n = 0;

        أكد(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        وقتی (*str != 0) {
                صحیح k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                اگر (k < 0)
                        بازگشت SIZE_MAX;

                str += k;
                n++;
        }

        بازگشت n;
}

size_t utf8_console_width(ثابت نویسه *str) {
        POINTER_MAY_BE_NULL(str);

        اگر (isempty(str))
                بازگشت 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        وقتی (*str) {
                صحیح w;

                w = utf8_char_console_width(str);
                اگر (w < 0)
                        بازگشت SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        بازگشت n;
}

size_t utf8_last_length(ثابت نویسه *s, size_t n) {
        صحیح r;

        أكد(s);

        اگر (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        برای (size_t last = 0;;) {
                اگر (n == 0)
                        بازگشت last;

                r = utf8_encoded_valid_unichar(s, n);
                اگر (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
