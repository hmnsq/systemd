/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "gunicode.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unaligned.h"
#अन्तर्भाव "utf8.h"

bool unichar_is_valid(char32_t ch) {

        यदि (ch >= 0x110000) /* End of unicode space */
                फर्काऊ false;
        यदि ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                फर्काऊ false;
        यदि ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                फर्काऊ false;
        यदि ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                फर्काऊ false;

        फर्काऊ true;
}

स्थिर bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        फर्काऊ (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
स्थिर माप_प्रकार utf8_encoded_expected_len(uint8_t c) {
        यदि (c < 0x80)
                फर्काऊ 1;
        यदि ((c & 0xe0) == 0xc0)
                फर्काऊ 2;
        यदि ((c & 0xf0) == 0xe0)
                फर्काऊ 3;
        यदि ((c & 0xf8) == 0xf0)
                फर्काऊ 4;
        यदि ((c & 0xfc) == 0xf8)
                फर्काऊ 5;
        यदि ((c & 0xfe) == 0xfc)
                फर्काऊ 6;

        फर्काऊ 0;
}

/* decode one unicode char */
पूर्णाङ्क utf8_encoded_to_unichar(अचर वर्ण *str, char32_t *ret_unichar) {
        char32_t unichar;
        माप_प्रकार len;

        प्रतिज्ञा(str);
        प्रतिज्ञा(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        विकल्प (len) {
        पक्ष 1:
                *ret_unichar = (char32_t)str[0];
                फर्काऊ 1;
        पक्ष 2:
                unichar = str[0] & 0x1f;
                रोक;
        पक्ष 3:
                unichar = (char32_t)str[0] & 0x0f;
                रोक;
        पक्ष 4:
                unichar = (char32_t)str[0] & 0x07;
                रोक;
        पक्ष 5:
                unichar = (char32_t)str[0] & 0x03;
                रोक;
        पक्ष 6:
                unichar = (char32_t)str[0] & 0x01;
                रोक;
        स्वभाव:
                फर्काऊ -EINVAL;
        }

        लागि (माप_प्रकार i = 1; i < len; i++) {
                यदि (((char32_t)str[i] & 0xc0) != 0x80)
                        फर्काऊ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        फर्काऊ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
स्थिर bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        फर्काऊ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(अचर वर्ण* str, माप_प्रकार length, bool allow_newline) {
        प्रतिज्ञा(str);

        लागि (अचर वर्ण *p = str; length > 0;) {
                पूर्णाङ्क encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                जबसम्म (length > 0) {
                        uint64_t w;
                        माप_प्रकार n;

                        यदि (length < परिमाण(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                स_नकल(&w, p, n);
                        } अन्यथा {
                                n = परिमाण(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        यदि (!is_printable_fastpath_u64(w))
                                रोक;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                यदि (length == 0)
                        रोक;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                यदि (encoded_len < 0)
                        फर्काऊ false;
                प्रतिज्ञा(encoded_len > 0 && (माप_प्रकार) encoded_len <= length);

                यदि (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        फर्काऊ false;

                length -= encoded_len;
                p += encoded_len;
        }

        फर्काऊ true;
}

वर्ण* utf8_is_valid_n(अचर वर्ण *str, माप_प्रकार len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        प्रतिज्ञा(str);

        लागि (माप_प्रकार i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                पूर्णाङ्क len;

                यदि (_unlikely_(str[i] == '\0'))
                        फर्काऊ रिक्त; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                यदि (_unlikely_(len < 0))
                        फर्काऊ रिक्त; /* invalid character */

                i += len;
        }

        फर्काऊ (वर्ण*) str;
}

वर्ण* utf8_escape_invalid(अचर वर्ण *str) {
        वर्ण *p, *s;

        प्रतिज्ञा(str);

        p = s = दो_स्मृति(म_माप(str) * 4 + 1);
        यदि (!p)
                फर्काऊ रिक्त;

        जबसम्म (*str) {
                पूर्णाङ्क len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                यदि (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } अन्यथा {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        फर्काऊ str_realloc(p);
}

पूर्णाङ्क utf8_char_console_width(अचर वर्ण *str) {
        char32_t c;
        पूर्णाङ्क r;

        प्रतिज्ञा(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        यदि (r < 0)
                फर्काऊ r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        फर्काऊ unichar_console_width(c);
}

पूर्णाङ्क unichar_console_width(char32_t c) {
        यदि (c == '\t')
                फर्काऊ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        फर्काऊ unichar_iswide(c) ? 2 : 1;
}

वर्ण* utf8_escape_non_printable_full(अचर वर्ण *str, माप_प्रकार console_width, bool force_ellipsis) {
        वर्ण *p, *s, *prev_s;
        माप_प्रकार n = 0; /* estimated print width */

        प्रतिज्ञा(str);

        यदि (console_width == 0)
                फर्काऊ strdup("");

        माप_प्रकार body = म_माप(str) * 4;
        p = s = prev_s = दो_स्मृति(body + STRLEN("…") + 1);
        यदि (!p)
                फर्काऊ रिक्त;

        लागि (;;) {
                पूर्णाङ्क len;
                वर्ण *saved_s = s;

                यदि (!*str) { /* done! */
                        यदि (force_ellipsis)
                                गच्छ truncation;
                        अन्यथा
                                गच्छ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                यदि (len > 0) {
                        यदि (utf8_is_printable(str, len)) {
                                पूर्णाङ्क w;

                                w = utf8_char_console_width(str);
                                प्रतिज्ञा(w >= 0);
                                यदि (n + w > console_width)
                                        गच्छ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } अन्यथा {
                                लागि (; len > 0; len--) {
                                        यदि (n + 4 > console_width)
                                                गच्छ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((पूर्णाङ्क) *str >> 4);
                                        *(s++) = hexchar((पूर्णाङ्क) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } अन्यथा {
                        यदि (n + 1 > console_width)
                                गच्छ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, म_माप(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        यदि (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", म_माप("…"));

 finish:
        *s = '\0';
        फर्काऊ str_realloc(p);
}

वर्ण* ascii_is_valid_n(अचर वर्ण *str, माप_प्रकार len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        प्रतिज्ञा(str);

        लागि (माप_प्रकार i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                यदि ((अचिन्हित वर्ण) str[i] >= 128 || str[i] == '\0')
                        फर्काऊ रिक्त;

        फर्काऊ (वर्ण*) str;
}

पूर्णाङ्क utf8_to_ascii(अचर वर्ण *str, वर्ण replacement_char, वर्ण **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        प्रतिज्ञा(str);
        प्रतिज्ञा(ret);

        _cleanup_free_ वर्ण *ans = new(वर्ण, म_माप(str) + 1);
        यदि (!ans)
                फर्काऊ -ENOMEM;

        वर्ण *q = ans;

        लागि (अचर वर्ण *p = str; *p; q++) {
                पूर्णाङ्क l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                यदि (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        फर्काऊ l;

                यदि (l == 1)
                        *q = *p;
                अन्यथा
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        फर्काऊ 0;
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
माप_प्रकार utf8_encode_unichar(वर्ण *out_utf8, char32_t g) {

        यदि (g < (1 << 7)) {
                यदि (out_utf8)
                        out_utf8[0] = g & 0x7f;
                फर्काऊ 1;
        } अन्यथा यदि (g < (1 << 11)) {
                यदि (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                फर्काऊ 2;
        } अन्यथा यदि (g < (1 << 16)) {
                यदि (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                फर्काऊ 3;
        } अन्यथा यदि (g < (1 << 21)) {
                यदि (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                फर्काऊ 4;
        }

        फर्काऊ 0;
}

वर्ण* utf16_to_utf8(अचर char16_t *s, माप_प्रकार length /* bytes! */) {
        अचर uint8_t *f;
        वर्ण *r, *t;

        यदि (length == 0)
                फर्काऊ new0(वर्ण, 1);

        प्रतिज्ञा(s);

        यदि (length == SIZE_MAX) {
                length = char16_strlen(s);

                यदि (length > SIZE_MAX/2)
                        फर्काऊ रिक्त; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        यदि (length > (SIZE_MAX - 1) / 2)
                फर्काऊ रिक्त; /* overflow */

        r = new(वर्ण, length * 2 + 1);
        यदि (!r)
                फर्काऊ रिक्त;

        f = (अचर uint8_t*) s;
        t = r;

        जबसम्म (f + 1 < (अचर uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                यदि (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        जारी;
                }

                यदि (utf16_is_trailing_surrogate(w1))
                        जारी; /* spurious trailing surrogate, ignore */

                यदि (f + 1 >= (अचर uint8_t*) s + length)
                        रोक;

                w2 = f[1] << 8 | f[0];
                f += 2;

                यदि (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        जारी; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        फर्काऊ r;
}

माप_प्रकार utf16_encode_unichar(char16_t *out, char32_t c) {
        प्रतिज्ञा(out);

        /* Note that this encodes as little-endian. */

        विकल्प (c) {

        पक्ष 0 ... 0xd7ffU:
        पक्ष 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                फर्काऊ 1;

        पक्ष 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                फर्काऊ 2;

        स्वभाव: /* A surrogate (invalid) */
                फर्काऊ 0;
        }
}

char16_t *utf8_to_utf16(अचर वर्ण *s, माप_प्रकार length) {
        char16_t *n, *p;
        पूर्णाङ्क r;

        यदि (length == 0)
                फर्काऊ new0(char16_t, 1);

        प्रतिज्ञा(s);

        यदि (length == SIZE_MAX)
                length = म_माप(s);

        यदि (length > SIZE_MAX - 1)
                फर्काऊ रिक्त; /* overflow */

        n = new(char16_t, length + 1);
        यदि (!n)
                फर्काऊ रिक्त;

        p = n;

        लागि (माप_प्रकार i = 0; i < length;) {
                char32_t unichar;
                माप_प्रकार e;

                e = utf8_encoded_expected_len(s[i]);
                यदि (e <= 1) /* Invalid and single byte characters are copied as they are */
                        गच्छ copy;

                यदि (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        गच्छ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                यदि (r < 0) /* sequence invalid, then copy as-is */
                        गच्छ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                जारी;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        फर्काऊ n;
}

माप_प्रकार char16_strlen(अचर char16_t *s) {
        माप_प्रकार n = 0;

        प्रतिज्ञा(s);

        जबसम्म (*s != 0)
                n++, s++;

        फर्काऊ n;
}

माप_प्रकार char16_strsize(अचर char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        फर्काऊ s ? (char16_strlen(s) + 1) * परिमाण(*s) : 0;
}

/* expected size used to encode one unicode char */
स्थिर पूर्णाङ्क utf8_unichar_to_encoded_len(char32_t unichar) {

        यदि (unichar < 0x80)
                फर्काऊ 1;
        यदि (unichar < 0x800)
                फर्काऊ 2;
        यदि (unichar < 0x10000)
                फर्काऊ 3;
        यदि (unichar < 0x200000)
                फर्काऊ 4;
        यदि (unichar < 0x4000000)
                फर्काऊ 5;

        फर्काऊ 6;
}

/* validate one encoded unicode char and return its length */
पूर्णाङ्क utf8_encoded_valid_unichar_full(अचर वर्ण *str, माप_प्रकार length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        माप_प्रकार len;
        पूर्णाङ्क r;

        प्रतिज्ञा(str);
        प्रतिज्ञा(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        यदि (len == 0)
                फर्काऊ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        यदि (len > length)
                फर्काऊ -EINVAL;

        /* ascii is valid */
        यदि (len == 1) {
                यदि (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                फर्काऊ 1;
        }

        /* check if expected encoded chars are available */
        लागि (माप_प्रकार i = 0; i < len; i++)
                यदि ((str[i] & 0x80) != 0x80)
                        फर्काऊ -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        यदि (r < 0)
                फर्काऊ r;

        /* check if encoded length matches encoded value */
        यदि (utf8_unichar_to_encoded_len(unichar) != (पूर्णाङ्क) len)
                फर्काऊ -EINVAL;

        /* check if value has valid range */
        यदि (!unichar_is_valid(unichar))
                फर्काऊ -EINVAL;

        यदि (ret_unichar)
                *ret_unichar = unichar;

        फर्काऊ (पूर्णाङ्क) len;
}

माप_प्रकार utf8_n_codepoints(अचर वर्ण *str) {
        माप_प्रकार n = 0;

        प्रतिज्ञा(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        जबसम्म (*str != 0) {
                पूर्णाङ्क k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                यदि (k < 0)
                        फर्काऊ SIZE_MAX;

                str += k;
                n++;
        }

        फर्काऊ n;
}

माप_प्रकार utf8_console_width(अचर वर्ण *str) {
        POINTER_MAY_BE_NULL(str);

        यदि (isempty(str))
                फर्काऊ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        माप_प्रकार n = 0;
        जबसम्म (*str) {
                पूर्णाङ्क w;

                w = utf8_char_console_width(str);
                यदि (w < 0)
                        फर्काऊ SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        फर्काऊ n;
}

माप_प्रकार utf8_last_length(अचर वर्ण *s, माप_प्रकार n) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        यदि (n == SIZE_MAX)
                n = म_माप(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        लागि (माप_प्रकार last = 0;;) {
                यदि (n == 0)
                        फर्काऊ last;

                r = utf8_encoded_valid_unichar(s, n);
                यदि (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
