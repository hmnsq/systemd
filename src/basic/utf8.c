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

        ಆದರೆ (ch >= 0x110000) /* End of unicode space */
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ಹಿಂತಿರುಗಿ (ch < ' ' && !ಒಳ_ಗಣ(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ಸ್ಥಿರ size_t utf8_encoded_expected_len(uint8_t c) {
        ಆದರೆ (c < 0x80)
                ಹಿಂತಿರುಗಿ 1;
        ಆದರೆ ((c & 0xe0) == 0xc0)
                ಹಿಂತಿರುಗಿ 2;
        ಆದರೆ ((c & 0xf0) == 0xe0)
                ಹಿಂತಿರುಗಿ 3;
        ಆದರೆ ((c & 0xf8) == 0xf0)
                ಹಿಂತಿರುಗಿ 4;
        ಆದರೆ ((c & 0xfc) == 0xf8)
                ಹಿಂತಿರುಗಿ 5;
        ಆದರೆ ((c & 0xfe) == 0xfc)
                ಹಿಂತಿರುಗಿ 6;

        ಹಿಂತಿರುಗಿ 0;
}

/* decode one unicode char */
ಪೂರ್ಣಾಂಕ utf8_encoded_ಗಮ್ಯ_unichar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        ಆಯ್ಕೆ (len) {
        ಸ್ಥಿತಿ 1:
                *ret_unichar = (char32_t)str[0];
                ಹಿಂತಿರುಗಿ 1;
        ಸ್ಥಿತಿ 2:
                unichar = str[0] & 0x1f;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 3:
                unichar = (char32_t)str[0] & 0x0f;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 4:
                unichar = (char32_t)str[0] & 0x07;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 5:
                unichar = (char32_t)str[0] & 0x03;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 6:
                unichar = (char32_t)str[0] & 0x01;
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಗಾಗಿ (size_t i = 1; i < len; i++) {
                ಆದರೆ (((char32_t)str[i] & 0xc0) != 0x80)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ಹಿಂತಿರುಗಿ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ಸ್ಥಿರ bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ಹಿಂತಿರುಗಿ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str, size_t length, bool allow_newline) {
        ಪ್ರತಿಜ್ಞಾ(str);

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = str; length > 0;) {
                ಪೂರ್ಣಾಂಕ encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ಆಗ (length > 0) {
                        uint64_t w;
                        size_t n;

                        ಆದರೆ (length < ಗಾತ್ರ(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                ಸ್ಮರಣೆ_ನಕಲು(&w, p, n);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                n = ಗಾತ್ರ(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ಆದರೆ (!is_printable_fastpath_u64(w))
                                ನಿಲ್ಲಿಸು;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ಆದರೆ (length == 0)
                        ನಿಲ್ಲಿಸು;

                encoded_len = utf8_encoded_valid_unichar_ತುಂಬಿದ(p, length, &val);
                ಆದರೆ (encoded_len < 0)
                        ಹಿಂತಿರುಗಿ false;
                ಪ್ರತಿಜ್ಞಾ(encoded_len > 0 && (size_t) encoded_len <= length);

                ಆದರೆ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ಹಿಂತಿರುಗಿ false;

                length -= encoded_len;
                p += encoded_len;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಅಕ್ಷರ* utf8_is_valid_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len_ಬೈಟುಗಳು) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        ಪ್ರತಿಜ್ಞಾ(str);

        ಗಾಗಿ (size_t i = 0; len_ಬೈಟುಗಳು != SIZE_MAX ? i < len_ಬೈಟುಗಳು : str[i] != '\0'; ) {
                ಪೂರ್ಣಾಂಕ len;

                ಆದರೆ (_unlikely_(str[i] == '\0'))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_ಬೈಟುಗಳು != SIZE_MAX ? len_ಬೈಟುಗಳು - i : SIZE_MAX);
                ಆದರೆ (_unlikely_(len < 0))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* invalid character */

                i += len;
        }

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) str;
}

ಅಕ್ಷರ* utf8_escape_invalid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಅಕ್ಷರ *p, *s;

        ಪ್ರತಿಜ್ಞಾ(str);

        p = s = ಸ್ಮರಣೆ_ಕೊಡು(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) * 4 + 1);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆಗ (*str) {
                ಪೂರ್ಣಾಂಕ len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ಆದರೆ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ಹಿಂತಿರುಗಿ str_realloc(p);
}

ಪೂರ್ಣಾಂಕ utf8_char_console_ಅಗಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        char32_t c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        assert_se(utf8_encoded_ಗಮ್ಯ_unichar(str, &c) == r);

        ಹಿಂತಿರುಗಿ unichar_console_ಅಗಲ(c);
}

ಪೂರ್ಣಾಂಕ unichar_console_ಅಗಲ(char32_t c) {
        ಆದರೆ (c == '\t')
                ಹಿಂತಿರುಗಿ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ಹಿಂತಿರುಗಿ unichar_iswide(c) ? 2 : 1;
}

ಅಕ್ಷರ* utf8_escape_non_printable_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t console_ಅಗಲ, bool force_ellipsis) {
        ಅಕ್ಷರ *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        ಪ್ರತಿಜ್ಞಾ(str);

        ಆದರೆ (console_ಅಗಲ == 0)
                ಹಿಂತಿರುಗಿ strdup("");

        size_t body = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) * 4;
        p = s = prev_s = ಸ್ಮರಣೆ_ಕೊಡು(body + STRLEN("…") + 1);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (;;) {
                ಪೂರ್ಣಾಂಕ len;
                ಅಕ್ಷರ *saved_s = s;

                ಆದರೆ (!*str) { /* done! */
                        ಆದರೆ (force_ellipsis)
                                ಹೋಗು truncation;
                        ಇಲ್ಲದಿದ್ದರೆ
                                ಹೋಗು ಮುಕ್ತಾಯ;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ಆದರೆ (len > 0) {
                        ಆದರೆ (utf8_is_printable(str, len)) {
                                ಪೂರ್ಣಾಂಕ w;

                                w = utf8_char_console_ಅಗಲ(str);
                                ಪ್ರತಿಜ್ಞಾ(w >= 0);
                                ಆದರೆ (n + w > console_ಅಗಲ)
                                        ಹೋಗು truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಗಾಗಿ (; len > 0; len--) {
                                        ಆದರೆ (n + 4 > console_ಅಗಲ)
                                                ಹೋಗು truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((ಪೂರ್ಣಾಂಕ) *str >> 4);
                                        *(s++) = hexchar((ಪೂರ್ಣಾಂಕ) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (n + 1 > console_ಅಗಲ)
                                ಹೋಗು truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ಆದರೆ (n + 1 > console_ಅಗಲ)
                s = prev_s;

        s = mempcpy(s, "…", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ("…"));

 ಮುಕ್ತಾಯ:
        *s = '\0';
        ಹಿಂತಿರುಗಿ str_realloc(p);
}

ಅಕ್ಷರ* ascii_is_valid_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        ಪ್ರತಿಜ್ಞಾ(str);

        ಗಾಗಿ (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) str[i] >= 128 || str[i] == '\0')
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) str;
}

ಪೂರ್ಣಾಂಕ utf8_ಗಮ್ಯ_ascii(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಅಕ್ಷರ replacement_char, ಅಕ್ಷರ **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ಅಕ್ಷರ *ans = new(ಅಕ್ಷರ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) + 1);
        ಆದರೆ (!ans)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ *q = ans;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = str; *p; q++) {
                ಪೂರ್ಣಾಂಕ l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ಆದರೆ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ಹಿಂತಿರುಗಿ l;

                ಆದರೆ (l == 1)
                        *q = *p;
                ಇಲ್ಲದಿದ್ದರೆ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ಹಿಂತಿರುಗಿ 0;
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
size_t utf8_encode_unichar(ಅಕ್ಷರ *ಹೊರಹೋಗು_utf8, char32_t g) {

        ಆದರೆ (g < (1 << 7)) {
                ಆದರೆ (ಹೊರಹೋಗು_utf8)
                        ಹೊರಹೋಗು_utf8[0] = g & 0x7f;
                ಹಿಂತಿರುಗಿ 1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g < (1 << 11)) {
                ಆದರೆ (ಹೊರಹೋಗು_utf8) {
                        ಹೊರಹೋಗು_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        ಹೊರಹೋಗು_utf8[1] = 0x80 | (g & 0x3f);
                }
                ಹಿಂತಿರುಗಿ 2;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g < (1 << 16)) {
                ಆದರೆ (ಹೊರಹೋಗು_utf8) {
                        ಹೊರಹೋಗು_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        ಹೊರಹೋಗು_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        ಹೊರಹೋಗು_utf8[2] = 0x80 | (g & 0x3f);
                }
                ಹಿಂತಿರುಗಿ 3;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (g < (1 << 21)) {
                ಆದರೆ (ಹೊರಹೋಗು_utf8) {
                        ಹೊರಹೋಗು_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        ಹೊರಹೋಗು_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        ಹೊರಹೋಗು_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        ಹೊರಹೋಗು_utf8[3] = 0x80 | (g & 0x3f);
                }
                ಹಿಂತಿರುಗಿ 4;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* utf16_ಗಮ್ಯ_utf8(ಸ್ಥಿರಾಂಕ char16_t *s, size_t length /* bytes! */) {
        ಸ್ಥಿರಾಂಕ uint8_t *f;
        ಅಕ್ಷರ *r, *t;

        ಆದರೆ (length == 0)
                ಹಿಂತಿರುಗಿ new0(ಅಕ್ಷರ, 1);

        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (length == SIZE_MAX) {
                length = char16_strlen(s);

                ಆದರೆ (length > SIZE_MAX/2)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /*ಅತಿಪ್ರವಾಹ*/

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ಆದರೆ (length > (SIZE_MAX - 1) / 2)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ; /*ಅತಿಪ್ರವಾಹ*/

        r = new(ಅಕ್ಷರ, length * 2 + 1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        f = (ಸ್ಥಿರಾಂಕ uint8_t*) s;
        t = r;

        ಆಗ (f + 1 < (ಸ್ಥಿರಾಂಕ uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ಆದರೆ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (utf16_is_trailing_surrogate(w1))
                        ಮುಂದುವರಿ; /* spurious trailing surrogate, ignore */

                ಆದರೆ (f + 1 >= (ಸ್ಥಿರಾಂಕ uint8_t*) s + length)
                        ನಿಲ್ಲಿಸು;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ಆದರೆ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        ಮುಂದುವರಿ; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_ಗಮ್ಯ_unichar(w1, w2));
        }

        *t = 0;
        ಹಿಂತಿರುಗಿ r;
}

size_t utf16_encode_unichar(char16_t *ಹೊರಹೋಗು, char32_t c) {
        ಪ್ರತಿಜ್ಞಾ(ಹೊರಹೋಗು);

        /* Note that this encodes as little-endian. */

        ಆಯ್ಕೆ (c) {

        ಸ್ಥಿತಿ 0 ... 0xd7ffU:
        ಸ್ಥಿತಿ 0xe000U ... 0xffffU:
                ಹೊರಹೋಗು[0] = htole16(c);
                ಹಿಂತಿರುಗಿ 1;

        ಸ್ಥಿತಿ 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                ಹೊರಹೋಗು[0] = htole16((c >> 10) + 0xd800U);
                ಹೊರಹೋಗು[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ಹಿಂತಿರುಗಿ 2;

        ಪೂರ್ವನಿಯೋಜಿತ: /* A surrogate (invalid) */
                ಹಿಂತಿರುಗಿ 0;
        }
}

char16_t *utf8_ಗಮ್ಯ_utf16(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length) {
        char16_t *n, *p;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (length == 0)
                ಹಿಂತಿರುಗಿ new0(char16_t, 1);

        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (length == SIZE_MAX)
                length = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        ಆದರೆ (length > SIZE_MAX - 1)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ; /*ಅತಿಪ್ರವಾಹ*/

        n = new(char16_t, length + 1);
        ಆದರೆ (!n)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        p = n;

        ಗಾಗಿ (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ಆದರೆ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        ಹೋಗು copy;

                ಆದರೆ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        ಹೋಗು copy;

                r = utf8_encoded_ಗಮ್ಯ_unichar(s + i, &unichar);
                ಆದರೆ (r < 0) /* sequence invalid, then copy as-is */
                        ಹೋಗು copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                ಮುಂದುವರಿ;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ಹಿಂತಿರುಗಿ n;
}

size_t char16_strlen(ಸ್ಥಿರಾಂಕ char16_t *s) {
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಆಗ (*s != 0)
                n++, s++;

        ಹಿಂತಿರುಗಿ n;
}

size_t char16_strsize(ಸ್ಥಿರಾಂಕ char16_t *s) {
        ಸೂಚಕ_MAY_BE_NULL(s);

        ಹಿಂತಿರುಗಿ s ? (char16_strlen(s) + 1) * ಗಾತ್ರ(*s) : 0;
}

/* expected size used to encode one unicode char */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ utf8_unichar_ಗಮ್ಯ_encoded_len(char32_t unichar) {

        ಆದರೆ (unichar < 0x80)
                ಹಿಂತಿರುಗಿ 1;
        ಆದರೆ (unichar < 0x800)
                ಹಿಂತಿರುಗಿ 2;
        ಆದರೆ (unichar < 0x10000)
                ಹಿಂತಿರುಗಿ 3;
        ಆದರೆ (unichar < 0x200000)
                ಹಿಂತಿರುಗಿ 4;
        ಆದರೆ (unichar < 0x4000000)
                ಹಿಂತಿರುಗಿ 5;

        ಹಿಂತಿರುಗಿ 6;
}

/* validate one encoded unicode char and return its length */
ಪೂರ್ಣಾಂಕ utf8_encoded_valid_unichar_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t length /*ಬೈಟುಗಳು*/, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(str);
        ಪ್ರತಿಜ್ಞಾ(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ಆದರೆ (len == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ಆದರೆ (len > length)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* ascii is valid */
        ಆದರೆ (len == 1) {
                ಆದರೆ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ಹಿಂತಿರುಗಿ 1;
        }

        /* check if expected encoded chars are available */
        ಗಾಗಿ (size_t i = 0; i < len; i++)
                ಆದರೆ ((str[i] & 0x80) != 0x80)
                        ಹಿಂತಿರುಗಿ -EINVAL;

        r = utf8_encoded_ಗಮ್ಯ_unichar(str, &unichar);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* check if encoded length matches encoded value */
        ಆದರೆ (utf8_unichar_ಗಮ್ಯ_encoded_len(unichar) != (ಪೂರ್ಣಾಂಕ) len)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* check if value has valid range */
        ಆದರೆ (!unichar_is_valid(unichar))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret_unichar)
                *ret_unichar = unichar;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) len;
}

size_t utf8_n_codepoints(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ಆಗ (*str != 0) {
                ಪೂರ್ಣಾಂಕ k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ಆದರೆ (k < 0)
                        ಹಿಂತಿರುಗಿ SIZE_MAX;

                str += k;
                n++;
        }

        ಹಿಂತಿರುಗಿ n;
}

size_t utf8_console_ಅಗಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಸೂಚಕ_MAY_BE_NULL(str);

        ಆದರೆ (isempty(str))
                ಹಿಂತಿರುಗಿ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ಆಗ (*str) {
                ಪೂರ್ಣಾಂಕ w;

                w = utf8_char_console_ಅಗಲ(str);
                ಆದರೆ (w < 0)
                        ಹಿಂತಿರುಗಿ SIZE_MAX;

                n += w;
                str = utf8_ಮುಂದಿನ_char(str);
        }

        ಹಿಂತಿರುಗಿ n;
}

size_t utf8_ಕೊನೆಯ_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (n == SIZE_MAX)
                n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        ಗಾಗಿ (size_t ಕೊನೆಯ_2 = 0;;) {
                ಆದರೆ (n == 0)
                        ಹಿಂತಿರುಗಿ ಕೊನೆಯ_2;

                r = utf8_encoded_valid_unichar(s, n);
                ಆದರೆ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                ಕೊನೆಯ_2 = r;
        }
}
