/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "gunicode.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "unaligned.h"
#অন্তর্ভাৱ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        যদি (ch >= 0x110000) /* End of unicode space */
                ঘূৰাই false;
        যদি ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ঘূৰাই false;
        যদি ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ঘূৰাই false;
        যদি ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ঘূৰাই false;

        ঘূৰাই true;
}

স্থির bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ঘূৰাই (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
স্থির size_t utf8_encoded_expected_len(uint8_t c) {
        যদি (c < 0x80)
                ঘূৰাই 1;
        যদি ((c & 0xe0) == 0xc0)
                ঘূৰাই 2;
        যদি ((c & 0xf0) == 0xe0)
                ঘূৰাই 3;
        যদি ((c & 0xf8) == 0xf0)
                ঘূৰাই 4;
        যদি ((c & 0xfc) == 0xf8)
                ঘূৰাই 5;
        যদি ((c & 0xfe) == 0xfc)
                ঘূৰাই 6;

        ঘূৰাই 0;
}

/* decode one unicode char */
পূর্ণাঙ্ক utf8_encoded_to_unichar(ধ্রুৱ ৱর্ণ *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        ৱিকল্প (len) {
        পক্ষ 1:
                *ret_unichar = (char32_t)str[0];
                ঘূৰাই 1;
        পক্ষ 2:
                unichar = str[0] & 0x1f;
                ভঙ্গ;
        পক্ষ 3:
                unichar = (char32_t)str[0] & 0x0f;
                ভঙ্গ;
        পক্ষ 4:
                unichar = (char32_t)str[0] & 0x07;
                ভঙ্গ;
        পক্ষ 5:
                unichar = (char32_t)str[0] & 0x03;
                ভঙ্গ;
        পক্ষ 6:
                unichar = (char32_t)str[0] & 0x01;
                ভঙ্গ;
        স্ৱভাৱ:
                ঘূৰাই -EINVAL;
        }

        বাবে (size_t i = 1; i < len; i++) {
                যদি (((char32_t)str[i] & 0xc0) != 0x80)
                        ঘূৰাই -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ঘূৰাই len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
স্থির bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ঘূৰাই (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ধ্রুৱ ৱর্ণ* str, size_t length, bool allow_newline) {
        প্রতিজ্ঞা(str);

        বাবে (ধ্রুৱ ৱর্ণ *p = str; length > 0;) {
                পূর্ণাঙ্ক encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                যেতিয়ালৈ (length > 0) {
                        uint64_t w;
                        size_t n;

                        যদি (length < পরিমাণ(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } অন্যথা {
                                n = পরিমাণ(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        যদি (!is_printable_fastpath_u64(w))
                                ভঙ্গ;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                যদি (length == 0)
                        ভঙ্গ;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                যদি (encoded_len < 0)
                        ঘূৰাই false;
                প্রতিজ্ঞা(encoded_len > 0 && (size_t) encoded_len <= length);

                যদি (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ঘূৰাই false;

                length -= encoded_len;
                p += encoded_len;
        }

        ঘূৰাই true;
}

ৱর্ণ* utf8_is_valid_n(ধ্রুৱ ৱর্ণ *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        প্রতিজ্ঞা(str);

        বাবে (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                পূর্ণাঙ্ক len;

                যদি (_unlikely_(str[i] == '\0'))
                        ঘূৰাই রিক্ত; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                যদি (_unlikely_(len < 0))
                        ঘূৰাই রিক্ত; /* invalid character */

                i += len;
        }

        ঘূৰাই (ৱর্ণ*) str;
}

ৱর্ণ* utf8_escape_invalid(ধ্রুৱ ৱর্ণ *str) {
        ৱর্ণ *p, *s;

        প্রতিজ্ঞা(str);

        p = s = malloc(strlen(str) * 4 + 1);
        যদি (!p)
                ঘূৰাই রিক্ত;

        যেতিয়ালৈ (*str) {
                পূর্ণাঙ্ক len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                যদি (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } অন্যথা {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ঘূৰাই str_realloc(p);
}

পূর্ণাঙ্ক utf8_char_console_width(ধ্রুৱ ৱর্ণ *str) {
        char32_t c;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        যদি (r < 0)
                ঘূৰাই r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ঘূৰাই unichar_console_width(c);
}

পূর্ণাঙ্ক unichar_console_width(char32_t c) {
        যদি (c == '\t')
                ঘূৰাই 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ঘূৰাই unichar_iswide(c) ? 2 : 1;
}

ৱর্ণ* utf8_escape_non_printable_full(ধ্রুৱ ৱর্ণ *str, size_t console_width, bool force_ellipsis) {
        ৱর্ণ *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        প্রতিজ্ঞা(str);

        যদি (console_width == 0)
                ঘূৰাই strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        যদি (!p)
                ঘূৰাই রিক্ত;

        বাবে (;;) {
                পূর্ণাঙ্ক len;
                ৱর্ণ *saved_s = s;

                যদি (!*str) { /* done! */
                        যদি (force_ellipsis)
                                গচ্ছ truncation;
                        অন্যথা
                                গচ্ছ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                যদি (len > 0) {
                        যদি (utf8_is_printable(str, len)) {
                                পূর্ণাঙ্ক w;

                                w = utf8_char_console_width(str);
                                প্রতিজ্ঞা(w >= 0);
                                যদি (n + w > console_width)
                                        গচ্ছ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } অন্যথা {
                                বাবে (; len > 0; len--) {
                                        যদি (n + 4 > console_width)
                                                গচ্ছ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((পূর্ণাঙ্ক) *str >> 4);
                                        *(s++) = hexchar((পূর্ণাঙ্ক) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } অন্যথা {
                        যদি (n + 1 > console_width)
                                গচ্ছ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        যদি (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ঘূৰাই str_realloc(p);
}

ৱর্ণ* ascii_is_valid_n(ধ্রুৱ ৱর্ণ *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        প্রতিজ্ঞা(str);

        বাবে (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                যদি ((unsigned ৱর্ণ) str[i] >= 128 || str[i] == '\0')
                        ঘূৰাই রিক্ত;

        ঘূৰাই (ৱর্ণ*) str;
}

পূর্ণাঙ্ক utf8_to_ascii(ধ্রুৱ ৱর্ণ *str, ৱর্ণ replacement_char, ৱর্ণ **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(ret);

        _cleanup_free_ ৱর্ণ *ans = new(ৱর্ণ, strlen(str) + 1);
        যদি (!ans)
                ঘূৰাই -ENOMEM;

        ৱর্ণ *q = ans;

        বাবে (ধ্রুৱ ৱর্ণ *p = str; *p; q++) {
                পূর্ণাঙ্ক l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                যদি (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ঘূৰাই l;

                যদি (l == 1)
                        *q = *p;
                অন্যথা
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ঘূৰাই 0;
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
size_t utf8_encode_unichar(ৱর্ণ *out_utf8, char32_t g) {

        যদি (g < (1 << 7)) {
                যদি (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ঘূৰাই 1;
        } অন্যথা যদি (g < (1 << 11)) {
                যদি (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ঘূৰাই 2;
        } অন্যথা যদি (g < (1 << 16)) {
                যদি (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ঘূৰাই 3;
        } অন্যথা যদি (g < (1 << 21)) {
                যদি (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ঘূৰাই 4;
        }

        ঘূৰাই 0;
}

ৱর্ণ* utf16_to_utf8(ধ্রুৱ char16_t *s, size_t length /* bytes! */) {
        ধ্রুৱ uint8_t *f;
        ৱর্ণ *r, *t;

        যদি (length == 0)
                ঘূৰাই new0(ৱর্ণ, 1);

        প্রতিজ্ঞা(s);

        যদি (length == SIZE_MAX) {
                length = char16_strlen(s);

                যদি (length > SIZE_MAX/2)
                        ঘূৰাই রিক্ত; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        যদি (length > (SIZE_MAX - 1) / 2)
                ঘূৰাই রিক্ত; /* overflow */

        r = new(ৱর্ণ, length * 2 + 1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        f = (ধ্রুৱ uint8_t*) s;
        t = r;

        যেতিয়ালৈ (f + 1 < (ধ্রুৱ uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                যদি (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        অনুৱর্ত;
                }

                যদি (utf16_is_trailing_surrogate(w1))
                        অনুৱর্ত; /* spurious trailing surrogate, ignore */

                যদি (f + 1 >= (ধ্রুৱ uint8_t*) s + length)
                        ভঙ্গ;

                w2 = f[1] << 8 | f[0];
                f += 2;

                যদি (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        অনুৱর্ত; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ঘূৰাই r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        প্রতিজ্ঞা(out);

        /* Note that this encodes as little-endian. */

        ৱিকল্প (c) {

        পক্ষ 0 ... 0xd7ffU:
        পক্ষ 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ঘূৰাই 1;

        পক্ষ 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ঘূৰাই 2;

        স্ৱভাৱ: /* A surrogate (invalid) */
                ঘূৰাই 0;
        }
}

char16_t *utf8_to_utf16(ধ্রুৱ ৱর্ণ *s, size_t length) {
        char16_t *n, *p;
        পূর্ণাঙ্ক r;

        যদি (length == 0)
                ঘূৰাই new0(char16_t, 1);

        প্রতিজ্ঞা(s);

        যদি (length == SIZE_MAX)
                length = strlen(s);

        যদি (length > SIZE_MAX - 1)
                ঘূৰাই রিক্ত; /* overflow */

        n = new(char16_t, length + 1);
        যদি (!n)
                ঘূৰাই রিক্ত;

        p = n;

        বাবে (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                যদি (e <= 1) /* Invalid and single byte characters are copied as they are */
                        গচ্ছ copy;

                যদি (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        গচ্ছ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                যদি (r < 0) /* sequence invalid, then copy as-is */
                        গচ্ছ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                অনুৱর্ত;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ঘূৰাই n;
}

size_t char16_strlen(ধ্রুৱ char16_t *s) {
        size_t n = 0;

        প্রতিজ্ঞা(s);

        যেতিয়ালৈ (*s != 0)
                n++, s++;

        ঘূৰাই n;
}

size_t char16_strsize(ধ্রুৱ char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ঘূৰাই s ? (char16_strlen(s) + 1) * পরিমাণ(*s) : 0;
}

/* expected size used to encode one unicode char */
স্থির পূর্ণাঙ্ক utf8_unichar_to_encoded_len(char32_t unichar) {

        যদি (unichar < 0x80)
                ঘূৰাই 1;
        যদি (unichar < 0x800)
                ঘূৰাই 2;
        যদি (unichar < 0x10000)
                ঘূৰাই 3;
        যদি (unichar < 0x200000)
                ঘূৰাই 4;
        যদি (unichar < 0x4000000)
                ঘূৰাই 5;

        ঘূৰাই 6;
}

/* validate one encoded unicode char and return its length */
পূর্ণাঙ্ক utf8_encoded_valid_unichar_full(ধ্রুৱ ৱর্ণ *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        যদি (len == 0)
                ঘূৰাই -EINVAL;

        /* Do we have a truncated multi-byte character? */
        যদি (len > length)
                ঘূৰাই -EINVAL;

        /* ascii is valid */
        যদি (len == 1) {
                যদি (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ঘূৰাই 1;
        }

        /* check if expected encoded chars are available */
        বাবে (size_t i = 0; i < len; i++)
                যদি ((str[i] & 0x80) != 0x80)
                        ঘূৰাই -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        যদি (r < 0)
                ঘূৰাই r;

        /* check if encoded length matches encoded value */
        যদি (utf8_unichar_to_encoded_len(unichar) != (পূর্ণাঙ্ক) len)
                ঘূৰাই -EINVAL;

        /* check if value has valid range */
        যদি (!unichar_is_valid(unichar))
                ঘূৰাই -EINVAL;

        যদি (ret_unichar)
                *ret_unichar = unichar;

        ঘূৰাই (পূর্ণাঙ্ক) len;
}

size_t utf8_n_codepoints(ধ্রুৱ ৱর্ণ *str) {
        size_t n = 0;

        প্রতিজ্ঞা(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        যেতিয়ালৈ (*str != 0) {
                পূর্ণাঙ্ক k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                যদি (k < 0)
                        ঘূৰাই SIZE_MAX;

                str += k;
                n++;
        }

        ঘূৰাই n;
}

size_t utf8_console_width(ধ্রুৱ ৱর্ণ *str) {
        POINTER_MAY_BE_NULL(str);

        যদি (isempty(str))
                ঘূৰাই 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        যেতিয়ালৈ (*str) {
                পূর্ণাঙ্ক w;

                w = utf8_char_console_width(str);
                যদি (w < 0)
                        ঘূৰাই SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        ঘূৰাই n;
}

size_t utf8_last_length(ধ্রুৱ ৱর্ণ *s, size_t n) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        যদি (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        বাবে (size_t last = 0;;) {
                যদি (n == 0)
                        ঘূৰাই last;

                r = utf8_encoded_valid_unichar(s, n);
                যদি (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
