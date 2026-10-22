/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "gunicode.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        ಯದಿ (ch >= 0x110000) /* End of unicode space */
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ಪ್ರತ್ಯಾಗಮ (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ಸ್ಥಿರ size_t utf8_encoded_expected_len(uint8_t c) {
        ಯದಿ (c < 0x80)
                ಪ್ರತ್ಯಾಗಮ 1;
        ಯದಿ ((c & 0xe0) == 0xc0)
                ಪ್ರತ್ಯಾಗಮ 2;
        ಯದಿ ((c & 0xf0) == 0xe0)
                ಪ್ರತ್ಯಾಗಮ 3;
        ಯದಿ ((c & 0xf8) == 0xf0)
                ಪ್ರತ್ಯಾಗಮ 4;
        ಯದಿ ((c & 0xfc) == 0xf8)
                ಪ್ರತ್ಯಾಗಮ 5;
        ಯದಿ ((c & 0xfe) == 0xfc)
                ಪ್ರತ್ಯಾಗಮ 6;

        ಪ್ರತ್ಯಾಗಮ 0;
}

/* decode one unicode char */
ಪೂರ್ಣಾಙ್ಕ utf8_encoded_to_unichar(ಧ್ರುವ ವರ್ಣ *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        ವಿಕಲ್ಪ (len) {
        ಪಕ್ಷ 1:
                *ret_unichar = (char32_t)str[0];
                ಪ್ರತ್ಯಾಗಮ 1;
        ಪಕ್ಷ 2:
                unichar = str[0] & 0x1f;
                ಭಙ್ಗ;
        ಪಕ್ಷ 3:
                unichar = (char32_t)str[0] & 0x0f;
                ಭಙ್ಗ;
        ಪಕ್ಷ 4:
                unichar = (char32_t)str[0] & 0x07;
                ಭಙ್ಗ;
        ಪಕ್ಷ 5:
                unichar = (char32_t)str[0] & 0x03;
                ಭಙ್ಗ;
        ಪಕ್ಷ 6:
                unichar = (char32_t)str[0] & 0x01;
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಪ್ರತಿ (size_t i = 1; i < len; i++) {
                ಯದಿ (((char32_t)str[i] & 0xc0) != 0x80)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ಪ್ರತ್ಯಾಗಮ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ಸ್ಥಿರ bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ಪ್ರತ್ಯಾಗಮ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ಧ್ರುವ ವರ್ಣ* str, size_t length, bool allow_newline) {
        ಪ್ರತಿಜ್ಞಾ(str);

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = str; length > 0;) {
                ಪೂರ್ಣಾಙ್ಕ encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ಯಾವತ್ (length > 0) {
                        uint64_t w;
                        size_t n;

                        ಯದಿ (length < ಪರಿಮಾಣ(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } ಅನ್ಯಥಾ {
                                n = ಪರಿಮಾಣ(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ಯದಿ (!is_printable_fastpath_u64(w))
                                ಭಙ್ಗ;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ಯದಿ (length == 0)
                        ಭಙ್ಗ;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                ಯದಿ (encoded_len < 0)
                        ಪ್ರತ್ಯಾಗಮ false;
                ಪ್ರತಿಜ್ಞಾ(encoded_len > 0 && (size_t) encoded_len <= length);

                ಯದಿ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ಪ್ರತ್ಯಾಗಮ false;

                length -= encoded_len;
                p += encoded_len;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ವರ್ಣ* utf8_is_valid_n(ಧ್ರುವ ವರ್ಣ *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        ಪ್ರತಿಜ್ಞಾ(str);

        ಪ್ರತಿ (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                ಪೂರ್ಣಾಙ್ಕ len;

                ಯದಿ (_unlikely_(str[i] == '\0'))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                ಯದಿ (_unlikely_(len < 0))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* invalid character */

                i += len;
        }

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) str;
}

ವರ್ಣ* utf8_escape_invalid(ಧ್ರುವ ವರ್ಣ *str) {
        ವರ್ಣ *p, *s;

        ಪ್ರತಿಜ್ಞಾ(str);

        p = s = malloc(strlen(str) * 4 + 1);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯಾವತ್ (*str) {
                ಪೂರ್ಣಾಙ್ಕ len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ಯದಿ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ಅನ್ಯಥಾ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ಪ್ರತ್ಯಾಗಮ str_realloc(p);
}

ಪೂರ್ಣಾಙ್ಕ utf8_char_console_width(ಧ್ರುವ ವರ್ಣ *str) {
        char32_t c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ಪ್ರತ್ಯಾಗಮ unichar_console_width(c);
}

ಪೂರ್ಣಾಙ್ಕ unichar_console_width(char32_t c) {
        ಯದಿ (c == '\t')
                ಪ್ರತ್ಯಾಗಮ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ಪ್ರತ್ಯಾಗಮ unichar_iswide(c) ? 2 : 1;
}

ವರ್ಣ* utf8_escape_non_printable_full(ಧ್ರುವ ವರ್ಣ *str, size_t console_width, bool force_ellipsis) {
        ವರ್ಣ *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        ಪ್ರತಿಜ್ಞಾ(str);

        ಯದಿ (console_width == 0)
                ಪ್ರತ್ಯಾಗಮ strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (;;) {
                ಪೂರ್ಣಾಙ್ಕ len;
                ವರ್ಣ *saved_s = s;

                ಯದಿ (!*str) { /* done! */
                        ಯದಿ (force_ellipsis)
                                ಗಚ್ಛ truncation;
                        ಅನ್ಯಥಾ
                                ಗಚ್ಛ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ಯದಿ (len > 0) {
                        ಯದಿ (utf8_is_printable(str, len)) {
                                ಪೂರ್ಣಾಙ್ಕ w;

                                w = utf8_char_console_width(str);
                                ಪ್ರತಿಜ್ಞಾ(w >= 0);
                                ಯದಿ (n + w > console_width)
                                        ಗಚ್ಛ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ಅನ್ಯಥಾ {
                                ಪ್ರತಿ (; len > 0; len--) {
                                        ಯದಿ (n + 4 > console_width)
                                                ಗಚ್ಛ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((ಪೂರ್ಣಾಙ್ಕ) *str >> 4);
                                        *(s++) = hexchar((ಪೂರ್ಣಾಙ್ಕ) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ಅನ್ಯಥಾ {
                        ಯದಿ (n + 1 > console_width)
                                ಗಚ್ಛ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ಯದಿ (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ಪ್ರತ್ಯಾಗಮ str_realloc(p);
}

ವರ್ಣ* ascii_is_valid_n(ಧ್ರುವ ವರ್ಣ *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        ಪ್ರತಿಜ್ಞಾ(str);

        ಪ್ರತಿ (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ಯದಿ ((unsigned ವರ್ಣ) str[i] >= 128 || str[i] == '\0')
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) str;
}

ಪೂರ್ಣಾಙ್ಕ utf8_to_ascii(ಧ್ರುವ ವರ್ಣ *str, ವರ್ಣ replacement_char, ವರ್ಣ **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ವರ್ಣ *ans = new(ವರ್ಣ, strlen(str) + 1);
        ಯದಿ (!ans)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ *q = ans;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = str; *p; q++) {
                ಪೂರ್ಣಾಙ್ಕ l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ಯದಿ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ಪ್ರತ್ಯಾಗಮ l;

                ಯದಿ (l == 1)
                        *q = *p;
                ಅನ್ಯಥಾ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ಪ್ರತ್ಯಾಗಮ 0;
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
size_t utf8_encode_unichar(ವರ್ಣ *out_utf8, char32_t g) {

        ಯದಿ (g < (1 << 7)) {
                ಯದಿ (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ಪ್ರತ್ಯಾಗಮ 1;
        } ಅನ್ಯಥಾ ಯದಿ (g < (1 << 11)) {
                ಯದಿ (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ಪ್ರತ್ಯಾಗಮ 2;
        } ಅನ್ಯಥಾ ಯದಿ (g < (1 << 16)) {
                ಯದಿ (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ಪ್ರತ್ಯಾಗಮ 3;
        } ಅನ್ಯಥಾ ಯದಿ (g < (1 << 21)) {
                ಯದಿ (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ಪ್ರತ್ಯಾಗಮ 4;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* utf16_to_utf8(ಧ್ರುವ char16_t *s, size_t length /* bytes! */) {
        ಧ್ರುವ uint8_t *f;
        ವರ್ಣ *r, *t;

        ಯದಿ (length == 0)
                ಪ್ರತ್ಯಾಗಮ new0(ವರ್ಣ, 1);

        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (length == SIZE_MAX) {
                length = char16_strlen(s);

                ಯದಿ (length > SIZE_MAX/2)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ಯದಿ (length > (SIZE_MAX - 1) / 2)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* overflow */

        r = new(ವರ್ಣ, length * 2 + 1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        f = (ಧ್ರುವ uint8_t*) s;
        t = r;

        ಯಾವತ್ (f + 1 < (ಧ್ರುವ uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ಯದಿ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        ಅನುವರ್ತ;
                }

                ಯದಿ (utf16_is_trailing_surrogate(w1))
                        ಅನುವರ್ತ; /* spurious trailing surrogate, ignore */

                ಯದಿ (f + 1 >= (ಧ್ರುವ uint8_t*) s + length)
                        ಭಙ್ಗ;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ಯದಿ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        ಅನುವರ್ತ; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ಪ್ರತ್ಯಾಗಮ r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        ಪ್ರತಿಜ್ಞಾ(out);

        /* Note that this encodes as little-endian. */

        ವಿಕಲ್ಪ (c) {

        ಪಕ್ಷ 0 ... 0xd7ffU:
        ಪಕ್ಷ 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ಪ್ರತ್ಯಾಗಮ 1;

        ಪಕ್ಷ 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ಪ್ರತ್ಯಾಗಮ 2;

        ಸ್ವಭಾವ: /* A surrogate (invalid) */
                ಪ್ರತ್ಯಾಗಮ 0;
        }
}

char16_t *utf8_to_utf16(ಧ್ರುವ ವರ್ಣ *s, size_t length) {
        char16_t *n, *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (length == 0)
                ಪ್ರತ್ಯಾಗಮ new0(char16_t, 1);

        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (length == SIZE_MAX)
                length = strlen(s);

        ಯದಿ (length > SIZE_MAX - 1)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* overflow */

        n = new(char16_t, length + 1);
        ಯದಿ (!n)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        p = n;

        ಪ್ರತಿ (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ಯದಿ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        ಗಚ್ಛ copy;

                ಯದಿ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        ಗಚ್ಛ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                ಯದಿ (r < 0) /* sequence invalid, then copy as-is */
                        ಗಚ್ಛ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                ಅನುವರ್ತ;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ಪ್ರತ್ಯಾಗಮ n;
}

size_t char16_strlen(ಧ್ರುವ char16_t *s) {
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಯಾವತ್ (*s != 0)
                n++, s++;

        ಪ್ರತ್ಯಾಗಮ n;
}

size_t char16_strsize(ಧ್ರುವ char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ಪ್ರತ್ಯಾಗಮ s ? (char16_strlen(s) + 1) * ಪರಿಮಾಣ(*s) : 0;
}

/* expected size used to encode one unicode char */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ utf8_unichar_to_encoded_len(char32_t unichar) {

        ಯದಿ (unichar < 0x80)
                ಪ್ರತ್ಯಾಗಮ 1;
        ಯದಿ (unichar < 0x800)
                ಪ್ರತ್ಯಾಗಮ 2;
        ಯದಿ (unichar < 0x10000)
                ಪ್ರತ್ಯಾಗಮ 3;
        ಯದಿ (unichar < 0x200000)
                ಪ್ರತ್ಯಾಗಮ 4;
        ಯದಿ (unichar < 0x4000000)
                ಪ್ರತ್ಯಾಗಮ 5;

        ಪ್ರತ್ಯಾಗಮ 6;
}

/* validate one encoded unicode char and return its length */
ಪೂರ್ಣಾಙ್ಕ utf8_encoded_valid_unichar_full(ಧ್ರುವ ವರ್ಣ *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ಯದಿ (len == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ಯದಿ (len > length)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* ascii is valid */
        ಯದಿ (len == 1) {
                ಯದಿ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ಪ್ರತ್ಯಾಗಮ 1;
        }

        /* check if expected encoded chars are available */
        ಪ್ರತಿ (size_t i = 0; i < len; i++)
                ಯದಿ ((str[i] & 0x80) != 0x80)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* check if encoded length matches encoded value */
        ಯದಿ (utf8_unichar_to_encoded_len(unichar) != (ಪೂರ್ಣಾಙ್ಕ) len)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* check if value has valid range */
        ಯದಿ (!unichar_is_valid(unichar))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret_unichar)
                *ret_unichar = unichar;

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) len;
}

size_t utf8_n_codepoints(ಧ್ರುವ ವರ್ಣ *str) {
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ಯಾವತ್ (*str != 0) {
                ಪೂರ್ಣಾಙ್ಕ k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ಯದಿ (k < 0)
                        ಪ್ರತ್ಯಾಗಮ SIZE_MAX;

                str += k;
                n++;
        }

        ಪ್ರತ್ಯಾಗಮ n;
}

size_t utf8_console_width(ಧ್ರುವ ವರ್ಣ *str) {
        POINTER_MAY_BE_NULL(str);

        ಯದಿ (isempty(str))
                ಪ್ರತ್ಯಾಗಮ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ಯಾವತ್ (*str) {
                ಪೂರ್ಣಾಙ್ಕ w;

                w = utf8_char_console_width(str);
                ಯದಿ (w < 0)
                        ಪ್ರತ್ಯಾಗಮ SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        ಪ್ರತ್ಯಾಗಮ n;
}

size_t utf8_last_length(ಧ್ರುವ ವರ್ಣ *s, size_t n) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        ಪ್ರತಿ (size_t last = 0;;) {
                ಯದಿ (n == 0)
                        ಪ್ರತ್ಯಾಗಮ last;

                r = utf8_encoded_valid_unichar(s, n);
                ಯದಿ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
