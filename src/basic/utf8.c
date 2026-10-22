/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "gunicode.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "unaligned.h"
#অন্তর্ভাব "utf8.h"

bool unichar_is_valid(char32_t ch) {

        যদি (ch >= 0x110000) /* End of unicode space */
                ফেরত false;
        যদি ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ফেরত false;
        যদি ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ফেরত false;
        যদি ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ফেরত false;

        ফেরত true;
}

স্থির bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ফেরত (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
স্থির size_t utf8_encoded_expected_len(uint8_t c) {
        যদি (c < 0x80)
                ফেরত 1;
        যদি ((c & 0xe0) == 0xc0)
                ফেরত 2;
        যদি ((c & 0xf0) == 0xe0)
                ফেরত 3;
        যদি ((c & 0xf8) == 0xf0)
                ফেরত 4;
        যদি ((c & 0xfc) == 0xf8)
                ফেরত 5;
        যদি ((c & 0xfe) == 0xfc)
                ফেরত 6;

        ফেরত 0;
}

/* decode one unicode char */
পূর্ণসংখ্যা utf8_encoded_to_unichar(ধ্রুবক অক্ষর *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        নির্বাচন (len) {
        ক্ষেত্র 1:
                *ret_unichar = (char32_t)str[0];
                ফেরত 1;
        ক্ষেত্র 2:
                unichar = str[0] & 0x1f;
                বিরতি;
        ক্ষেত্র 3:
                unichar = (char32_t)str[0] & 0x0f;
                বিরতি;
        ক্ষেত্র 4:
                unichar = (char32_t)str[0] & 0x07;
                বিরতি;
        ক্ষেত্র 5:
                unichar = (char32_t)str[0] & 0x03;
                বিরতি;
        ক্ষেত্র 6:
                unichar = (char32_t)str[0] & 0x01;
                বিরতি;
        পূর্বনির্ধারিত:
                ফেরত -EINVAL;
        }

        জন্য (size_t i = 1; i < len; i++) {
                যদি (((char32_t)str[i] & 0xc0) != 0x80)
                        ফেরত -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ফেরত len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
স্থির bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ফেরত (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ধ্রুবক অক্ষর* str, size_t length, bool allow_newline) {
        প্রতিজ্ঞা(str);

        জন্য (ধ্রুবক অক্ষর *p = str; length > 0;) {
                পূর্ণসংখ্যা encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                যখন (length > 0) {
                        uint64_t w;
                        size_t n;

                        যদি (length < আকার(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } অন্যথা {
                                n = আকার(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        যদি (!is_printable_fastpath_u64(w))
                                বিরতি;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                যদি (length == 0)
                        বিরতি;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                যদি (encoded_len < 0)
                        ফেরত false;
                প্রতিজ্ঞা(encoded_len > 0 && (size_t) encoded_len <= length);

                যদি (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ফেরত false;

                length -= encoded_len;
                p += encoded_len;
        }

        ফেরত true;
}

অক্ষর* utf8_is_valid_n(ধ্রুবক অক্ষর *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        প্রতিজ্ঞা(str);

        জন্য (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                পূর্ণসংখ্যা len;

                যদি (_unlikely_(str[i] == '\0'))
                        ফেরত ফাঁকা; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                যদি (_unlikely_(len < 0))
                        ফেরত ফাঁকা; /* invalid character */

                i += len;
        }

        ফেরত (অক্ষর*) str;
}

অক্ষর* utf8_escape_invalid(ধ্রুবক অক্ষর *str) {
        অক্ষর *p, *s;

        প্রতিজ্ঞা(str);

        p = s = malloc(strlen(str) * 4 + 1);
        যদি (!p)
                ফেরত ফাঁকা;

        যখন (*str) {
                পূর্ণসংখ্যা len;

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
        ফেরত str_realloc(p);
}

পূর্ণসংখ্যা utf8_char_console_width(ধ্রুবক অক্ষর *str) {
        char32_t c;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        যদি (r < 0)
                ফেরত r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ফেরত unichar_console_width(c);
}

পূর্ণসংখ্যা unichar_console_width(char32_t c) {
        যদি (c == '\t')
                ফেরত 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ফেরত unichar_iswide(c) ? 2 : 1;
}

অক্ষর* utf8_escape_non_printable_full(ধ্রুবক অক্ষর *str, size_t console_width, bool force_ellipsis) {
        অক্ষর *p, *s, *পূর্ব_s;
        size_t n = 0; /* estimated print width */

        প্রতিজ্ঞা(str);

        যদি (console_width == 0)
                ফেরত strdup("");

        size_t body = strlen(str) * 4;
        p = s = পূর্ব_s = malloc(body + STRLEN("…") + 1);
        যদি (!p)
                ফেরত ফাঁকা;

        জন্য (;;) {
                পূর্ণসংখ্যা len;
                অক্ষর *saved_s = s;

                যদি (!*str) { /* done! */
                        যদি (force_ellipsis)
                                যাও truncation;
                        অন্যথা
                                যাও finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                যদি (len > 0) {
                        যদি (utf8_is_printable(str, len)) {
                                পূর্ণসংখ্যা w;

                                w = utf8_char_console_width(str);
                                প্রতিজ্ঞা(w >= 0);
                                যদি (n + w > console_width)
                                        যাও truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } অন্যথা {
                                জন্য (; len > 0; len--) {
                                        যদি (n + 4 > console_width)
                                                যাও truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((পূর্ণসংখ্যা) *str >> 4);
                                        *(s++) = hexchar((পূর্ণসংখ্যা) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } অন্যথা {
                        যদি (n + 1 > console_width)
                                যাও truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                পূর্ব_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        যদি (n + 1 > console_width)
                s = পূর্ব_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ফেরত str_realloc(p);
}

অক্ষর* ascii_is_valid_n(ধ্রুবক অক্ষর *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        প্রতিজ্ঞা(str);

        জন্য (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                যদি ((unsigned অক্ষর) str[i] >= 128 || str[i] == '\0')
                        ফেরত ফাঁকা;

        ফেরত (অক্ষর*) str;
}

পূর্ণসংখ্যা utf8_to_ascii(ধ্রুবক অক্ষর *str, অক্ষর replacement_char, অক্ষর **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(ret);

        _cleanup_free_ অক্ষর *ans = new(অক্ষর, strlen(str) + 1);
        যদি (!ans)
                ফেরত -ENOMEM;

        অক্ষর *q = ans;

        জন্য (ধ্রুবক অক্ষর *p = str; *p; q++) {
                পূর্ণসংখ্যা l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                যদি (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ফেরত l;

                যদি (l == 1)
                        *q = *p;
                অন্যথা
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ফেরত 0;
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
size_t utf8_encode_unichar(অক্ষর *out_utf8, char32_t g) {

        যদি (g < (1 << 7)) {
                যদি (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ফেরত 1;
        } অন্যথা যদি (g < (1 << 11)) {
                যদি (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ফেরত 2;
        } অন্যথা যদি (g < (1 << 16)) {
                যদি (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ফেরত 3;
        } অন্যথা যদি (g < (1 << 21)) {
                যদি (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ফেরত 4;
        }

        ফেরত 0;
}

অক্ষর* utf16_to_utf8(ধ্রুবক char16_t *s, size_t length /* bytes! */) {
        ধ্রুবক uint8_t *f;
        অক্ষর *r, *t;

        যদি (length == 0)
                ফেরত new0(অক্ষর, 1);

        প্রতিজ্ঞা(s);

        যদি (length == SIZE_MAX) {
                length = char16_strlen(s);

                যদি (length > SIZE_MAX/2)
                        ফেরত ফাঁকা; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        যদি (length > (SIZE_MAX - 1) / 2)
                ফেরত ফাঁকা; /* overflow */

        r = new(অক্ষর, length * 2 + 1);
        যদি (!r)
                ফেরত ফাঁকা;

        f = (ধ্রুবক uint8_t*) s;
        t = r;

        যখন (f + 1 < (ধ্রুবক uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                যদি (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        চালু;
                }

                যদি (utf16_is_trailing_surrogate(w1))
                        চালু; /* spurious trailing surrogate, ignore */

                যদি (f + 1 >= (ধ্রুবক uint8_t*) s + length)
                        বিরতি;

                w2 = f[1] << 8 | f[0];
                f += 2;

                যদি (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        চালু; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ফেরত r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        প্রতিজ্ঞা(out);

        /* Note that this encodes as little-endian. */

        নির্বাচন (c) {

        ক্ষেত্র 0 ... 0xd7ffU:
        ক্ষেত্র 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ফেরত 1;

        ক্ষেত্র 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ফেরত 2;

        পূর্বনির্ধারিত: /* A surrogate (invalid) */
                ফেরত 0;
        }
}

char16_t *utf8_to_utf16(ধ্রুবক অক্ষর *s, size_t length) {
        char16_t *n, *p;
        পূর্ণসংখ্যা r;

        যদি (length == 0)
                ফেরত new0(char16_t, 1);

        প্রতিজ্ঞা(s);

        যদি (length == SIZE_MAX)
                length = strlen(s);

        যদি (length > SIZE_MAX - 1)
                ফেরত ফাঁকা; /* overflow */

        n = new(char16_t, length + 1);
        যদি (!n)
                ফেরত ফাঁকা;

        p = n;

        জন্য (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                যদি (e <= 1) /* Invalid and single byte characters are copied as they are */
                        যাও copy;

                যদি (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        যাও copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                যদি (r < 0) /* sequence invalid, then copy as-is */
                        যাও copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                চালু;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ফেরত n;
}

size_t char16_strlen(ধ্রুবক char16_t *s) {
        size_t n = 0;

        প্রতিজ্ঞা(s);

        যখন (*s != 0)
                n++, s++;

        ফেরত n;
}

size_t char16_strsize(ধ্রুবক char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ফেরত s ? (char16_strlen(s) + 1) * আকার(*s) : 0;
}

/* expected size used to encode one unicode char */
স্থির পূর্ণসংখ্যা utf8_unichar_to_encoded_len(char32_t unichar) {

        যদি (unichar < 0x80)
                ফেরত 1;
        যদি (unichar < 0x800)
                ফেরত 2;
        যদি (unichar < 0x10000)
                ফেরত 3;
        যদি (unichar < 0x200000)
                ফেরত 4;
        যদি (unichar < 0x4000000)
                ফেরত 5;

        ফেরত 6;
}

/* validate one encoded unicode char and return its length */
পূর্ণসংখ্যা utf8_encoded_valid_unichar_full(ধ্রুবক অক্ষর *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(str);
        প্রতিজ্ঞা(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        যদি (len == 0)
                ফেরত -EINVAL;

        /* Do we have a truncated multi-byte character? */
        যদি (len > length)
                ফেরত -EINVAL;

        /* ascii is valid */
        যদি (len == 1) {
                যদি (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ফেরত 1;
        }

        /* check if expected encoded chars are available */
        জন্য (size_t i = 0; i < len; i++)
                যদি ((str[i] & 0x80) != 0x80)
                        ফেরত -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        যদি (r < 0)
                ফেরত r;

        /* check if encoded length matches encoded value */
        যদি (utf8_unichar_to_encoded_len(unichar) != (পূর্ণসংখ্যা) len)
                ফেরত -EINVAL;

        /* check if value has valid range */
        যদি (!unichar_is_valid(unichar))
                ফেরত -EINVAL;

        যদি (ret_unichar)
                *ret_unichar = unichar;

        ফেরত (পূর্ণসংখ্যা) len;
}

size_t utf8_n_codepoints(ধ্রুবক অক্ষর *str) {
        size_t n = 0;

        প্রতিজ্ঞা(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        যখন (*str != 0) {
                পূর্ণসংখ্যা k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                যদি (k < 0)
                        ফেরত SIZE_MAX;

                str += k;
                n++;
        }

        ফেরত n;
}

size_t utf8_console_width(ধ্রুবক অক্ষর *str) {
        POINTER_MAY_BE_NULL(str);

        যদি (isempty(str))
                ফেরত 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        যখন (*str) {
                পূর্ণসংখ্যা w;

                w = utf8_char_console_width(str);
                যদি (w < 0)
                        ফেরত SIZE_MAX;

                n += w;
                str = utf8_পরবর্তী_char(str);
        }

        ফেরত n;
}

size_t utf8_last_দৈর্ঘ্য(ধ্রুবক অক্ষর *s, size_t n) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        যদি (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        জন্য (size_t last = 0;;) {
                যদি (n == 0)
                        ফেরত last;

                r = utf8_encoded_valid_unichar(s, n);
                যদি (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
