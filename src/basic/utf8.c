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

        जइ (ch >= 0x110000) /* End of unicode space */
                प्रत्यागम false;
        जइ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                प्रत्यागम false;
        जइ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                प्रत्यागम false;
        जइ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                प्रत्यागम false;

        प्रत्यागम true;
}

स्थिर bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        प्रत्यागम (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
स्थिर size_t utf8_encoded_expected_len(uint8_t c) {
        जइ (c < 0x80)
                प्रत्यागम 1;
        जइ ((c & 0xe0) == 0xc0)
                प्रत्यागम 2;
        जइ ((c & 0xf0) == 0xe0)
                प्रत्यागम 3;
        जइ ((c & 0xf8) == 0xf0)
                प्रत्यागम 4;
        जइ ((c & 0xfc) == 0xf8)
                प्रत्यागम 5;
        जइ ((c & 0xfe) == 0xfc)
                प्रत्यागम 6;

        प्रत्यागम 0;
}

/* decode one unicode char */
पूर्णाङ्क utf8_encoded_to_unichar(ध्रुव वर्ण *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        प्रतिज्ञा(str);
        प्रतिज्ञा(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        विकल्प (len) {
        पक्ष 1:
                *ret_unichar = (char32_t)str[0];
                प्रत्यागम 1;
        पक्ष 2:
                unichar = str[0] & 0x1f;
                भङ्ग;
        पक्ष 3:
                unichar = (char32_t)str[0] & 0x0f;
                भङ्ग;
        पक्ष 4:
                unichar = (char32_t)str[0] & 0x07;
                भङ्ग;
        पक्ष 5:
                unichar = (char32_t)str[0] & 0x03;
                भङ्ग;
        पक्ष 6:
                unichar = (char32_t)str[0] & 0x01;
                भङ्ग;
        स्वभाव:
                प्रत्यागम -EINVAL;
        }

        प्रति (size_t i = 1; i < len; i++) {
                जइ (((char32_t)str[i] & 0xc0) != 0x80)
                        प्रत्यागम -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        प्रत्यागम len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
स्थिर bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        प्रत्यागम (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ध्रुव वर्ण* str, size_t length, bool allow_newline) {
        प्रतिज्ञा(str);

        प्रति (ध्रुव वर्ण *p = str; length > 0;) {
                पूर्णाङ्क encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                जाव (length > 0) {
                        uint64_t w;
                        size_t n;

                        जइ (length < परिमाण(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } अण्णहा {
                                n = परिमाण(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        जइ (!is_printable_fastpath_u64(w))
                                भङ्ग;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                जइ (length == 0)
                        भङ्ग;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                जइ (encoded_len < 0)
                        प्रत्यागम false;
                प्रतिज्ञा(encoded_len > 0 && (size_t) encoded_len <= length);

                जइ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        प्रत्यागम false;

                length -= encoded_len;
                p += encoded_len;
        }

        प्रत्यागम true;
}

वर्ण* utf8_is_valid_n(ध्रुव वर्ण *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        प्रतिज्ञा(str);

        प्रति (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                पूर्णाङ्क len;

                जइ (_unlikely_(str[i] == '\0'))
                        प्रत्यागम रिक्त; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                जइ (_unlikely_(len < 0))
                        प्रत्यागम रिक्त; /* invalid character */

                i += len;
        }

        प्रत्यागम (वर्ण*) str;
}

वर्ण* utf8_escape_invalid(ध्रुव वर्ण *str) {
        वर्ण *p, *s;

        प्रतिज्ञा(str);

        p = s = malloc(strlen(str) * 4 + 1);
        जइ (!p)
                प्रत्यागम रिक्त;

        जाव (*str) {
                पूर्णाङ्क len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                जइ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } अण्णहा {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        प्रत्यागम str_realloc(p);
}

पूर्णाङ्क utf8_char_console_width(ध्रुव वर्ण *str) {
        char32_t c;
        पूर्णाङ्क r;

        प्रतिज्ञा(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        जइ (r < 0)
                प्रत्यागम r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        प्रत्यागम unichar_console_width(c);
}

पूर्णाङ्क unichar_console_width(char32_t c) {
        जइ (c == '\t')
                प्रत्यागम 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        प्रत्यागम unichar_iswide(c) ? 2 : 1;
}

वर्ण* utf8_escape_non_printable_full(ध्रुव वर्ण *str, size_t console_width, bool force_ellipsis) {
        वर्ण *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        प्रतिज्ञा(str);

        जइ (console_width == 0)
                प्रत्यागम strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        जइ (!p)
                प्रत्यागम रिक्त;

        प्रति (;;) {
                पूर्णाङ्क len;
                वर्ण *saved_s = s;

                जइ (!*str) { /* done! */
                        जइ (force_ellipsis)
                                गच्छ truncation;
                        अण्णहा
                                गच्छ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                जइ (len > 0) {
                        जइ (utf8_is_printable(str, len)) {
                                पूर्णाङ्क w;

                                w = utf8_char_console_width(str);
                                प्रतिज्ञा(w >= 0);
                                जइ (n + w > console_width)
                                        गच्छ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } अण्णहा {
                                प्रति (; len > 0; len--) {
                                        जइ (n + 4 > console_width)
                                                गच्छ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((पूर्णाङ्क) *str >> 4);
                                        *(s++) = hexchar((पूर्णाङ्क) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } अण्णहा {
                        जइ (n + 1 > console_width)
                                गच्छ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        जइ (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        प्रत्यागम str_realloc(p);
}

वर्ण* ascii_is_valid_n(ध्रुव वर्ण *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        प्रतिज्ञा(str);

        प्रति (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                जइ ((unsigned वर्ण) str[i] >= 128 || str[i] == '\0')
                        प्रत्यागम रिक्त;

        प्रत्यागम (वर्ण*) str;
}

पूर्णाङ्क utf8_to_ascii(ध्रुव वर्ण *str, वर्ण replacement_char, वर्ण **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        प्रतिज्ञा(str);
        प्रतिज्ञा(ret);

        _cleanup_free_ वर्ण *ans = new(वर्ण, strlen(str) + 1);
        जइ (!ans)
                प्रत्यागम -ENOMEM;

        वर्ण *q = ans;

        प्रति (ध्रुव वर्ण *p = str; *p; q++) {
                पूर्णाङ्क l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                जइ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        प्रत्यागम l;

                जइ (l == 1)
                        *q = *p;
                अण्णहा
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        प्रत्यागम 0;
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
size_t utf8_encode_unichar(वर्ण *out_utf8, char32_t g) {

        जइ (g < (1 << 7)) {
                जइ (out_utf8)
                        out_utf8[0] = g & 0x7f;
                प्रत्यागम 1;
        } अण्णहा जइ (g < (1 << 11)) {
                जइ (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                प्रत्यागम 2;
        } अण्णहा जइ (g < (1 << 16)) {
                जइ (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                प्रत्यागम 3;
        } अण्णहा जइ (g < (1 << 21)) {
                जइ (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                प्रत्यागम 4;
        }

        प्रत्यागम 0;
}

वर्ण* utf16_to_utf8(ध्रुव char16_t *s, size_t length /* bytes! */) {
        ध्रुव uint8_t *f;
        वर्ण *r, *t;

        जइ (length == 0)
                प्रत्यागम new0(वर्ण, 1);

        प्रतिज्ञा(s);

        जइ (length == SIZE_MAX) {
                length = char16_strlen(s);

                जइ (length > SIZE_MAX/2)
                        प्रत्यागम रिक्त; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        जइ (length > (SIZE_MAX - 1) / 2)
                प्रत्यागम रिक्त; /* overflow */

        r = new(वर्ण, length * 2 + 1);
        जइ (!r)
                प्रत्यागम रिक्त;

        f = (ध्रुव uint8_t*) s;
        t = r;

        जाव (f + 1 < (ध्रुव uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                जइ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        अनुवर्त;
                }

                जइ (utf16_is_trailing_surrogate(w1))
                        अनुवर्त; /* spurious trailing surrogate, ignore */

                जइ (f + 1 >= (ध्रुव uint8_t*) s + length)
                        भङ्ग;

                w2 = f[1] << 8 | f[0];
                f += 2;

                जइ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        अनुवर्त; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        प्रत्यागम r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        प्रतिज्ञा(out);

        /* Note that this encodes as little-endian. */

        विकल्प (c) {

        पक्ष 0 ... 0xd7ffU:
        पक्ष 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                प्रत्यागम 1;

        पक्ष 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                प्रत्यागम 2;

        स्वभाव: /* A surrogate (invalid) */
                प्रत्यागम 0;
        }
}

char16_t *utf8_to_utf16(ध्रुव वर्ण *s, size_t length) {
        char16_t *n, *p;
        पूर्णाङ्क r;

        जइ (length == 0)
                प्रत्यागम new0(char16_t, 1);

        प्रतिज्ञा(s);

        जइ (length == SIZE_MAX)
                length = strlen(s);

        जइ (length > SIZE_MAX - 1)
                प्रत्यागम रिक्त; /* overflow */

        n = new(char16_t, length + 1);
        जइ (!n)
                प्रत्यागम रिक्त;

        p = n;

        प्रति (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                जइ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        गच्छ copy;

                जइ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        गच्छ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                जइ (r < 0) /* sequence invalid, then copy as-is */
                        गच्छ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                अनुवर्त;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        प्रत्यागम n;
}

size_t char16_strlen(ध्रुव char16_t *s) {
        size_t n = 0;

        प्रतिज्ञा(s);

        जाव (*s != 0)
                n++, s++;

        प्रत्यागम n;
}

size_t char16_strsize(ध्रुव char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        प्रत्यागम s ? (char16_strlen(s) + 1) * परिमाण(*s) : 0;
}

/* expected size used to encode one unicode char */
स्थिर पूर्णाङ्क utf8_unichar_to_encoded_len(char32_t unichar) {

        जइ (unichar < 0x80)
                प्रत्यागम 1;
        जइ (unichar < 0x800)
                प्रत्यागम 2;
        जइ (unichar < 0x10000)
                प्रत्यागम 3;
        जइ (unichar < 0x200000)
                प्रत्यागम 4;
        जइ (unichar < 0x4000000)
                प्रत्यागम 5;

        प्रत्यागम 6;
}

/* validate one encoded unicode char and return its length */
पूर्णाङ्क utf8_encoded_valid_unichar_full(ध्रुव वर्ण *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        पूर्णाङ्क r;

        प्रतिज्ञा(str);
        प्रतिज्ञा(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        जइ (len == 0)
                प्रत्यागम -EINVAL;

        /* Do we have a truncated multi-byte character? */
        जइ (len > length)
                प्रत्यागम -EINVAL;

        /* ascii is valid */
        जइ (len == 1) {
                जइ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                प्रत्यागम 1;
        }

        /* check if expected encoded chars are available */
        प्रति (size_t i = 0; i < len; i++)
                जइ ((str[i] & 0x80) != 0x80)
                        प्रत्यागम -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        जइ (r < 0)
                प्रत्यागम r;

        /* check if encoded length matches encoded value */
        जइ (utf8_unichar_to_encoded_len(unichar) != (पूर्णाङ्क) len)
                प्रत्यागम -EINVAL;

        /* check if value has valid range */
        जइ (!unichar_is_valid(unichar))
                प्रत्यागम -EINVAL;

        जइ (ret_unichar)
                *ret_unichar = unichar;

        प्रत्यागम (पूर्णाङ्क) len;
}

size_t utf8_n_codepoints(ध्रुव वर्ण *str) {
        size_t n = 0;

        प्रतिज्ञा(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        जाव (*str != 0) {
                पूर्णाङ्क k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                जइ (k < 0)
                        प्रत्यागम SIZE_MAX;

                str += k;
                n++;
        }

        प्रत्यागम n;
}

size_t utf8_console_width(ध्रुव वर्ण *str) {
        POINTER_MAY_BE_NULL(str);

        जइ (isempty(str))
                प्रत्यागम 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        जाव (*str) {
                पूर्णाङ्क w;

                w = utf8_char_console_width(str);
                जइ (w < 0)
                        प्रत्यागम SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        प्रत्यागम n;
}

size_t utf8_last_length(ध्रुव वर्ण *s, size_t n) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        जइ (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        प्रति (size_t last = 0;;) {
                जइ (n == 0)
                        प्रत्यागम last;

                r = utf8_encoded_valid_unichar(s, n);
                जइ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
