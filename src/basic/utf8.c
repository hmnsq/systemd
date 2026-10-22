/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gunicode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        ਜੇ (ch >= 0x110000) /* End of unicode space */
                ਵਾਪਸ false;
        ਜੇ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ਵਾਪਸ false;
        ਜੇ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ਵਾਪਸ false;
        ਜੇ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਸਥਿਰ bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ਵਾਪਸ (ch < ' ' && !ਵਿੱਚ_ਸੈੱਟ(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ਸਥਿਰ size_t utf8_encoded_expected_len(uint8_t c) {
        ਜੇ (c < 0x80)
                ਵਾਪਸ 1;
        ਜੇ ((c & 0xe0) == 0xc0)
                ਵਾਪਸ 2;
        ਜੇ ((c & 0xf0) == 0xe0)
                ਵਾਪਸ 3;
        ਜੇ ((c & 0xf8) == 0xf0)
                ਵਾਪਸ 4;
        ਜੇ ((c & 0xfc) == 0xf8)
                ਵਾਪਸ 5;
        ਜੇ ((c & 0xfe) == 0xfc)
                ਵਾਪਸ 6;

        ਵਾਪਸ 0;
}

/* decode one unicode char */
ਪੂਰਨ utf8_encoded_ਮੰਜ਼ਿਲ_unichar(ਅਚਲ ਅੱਖਰ *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        ਪ੍ਰਤਿਜ੍ਞਾ(str);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        ਚੋਣ (len) {
        ਹਾਲਤ 1:
                *ret_unichar = (char32_t)str[0];
                ਵਾਪਸ 1;
        ਹਾਲਤ 2:
                unichar = str[0] & 0x1f;
                ਰੋਕੋ;
        ਹਾਲਤ 3:
                unichar = (char32_t)str[0] & 0x0f;
                ਰੋਕੋ;
        ਹਾਲਤ 4:
                unichar = (char32_t)str[0] & 0x07;
                ਰੋਕੋ;
        ਹਾਲਤ 5:
                unichar = (char32_t)str[0] & 0x03;
                ਰੋਕੋ;
        ਹਾਲਤ 6:
                unichar = (char32_t)str[0] & 0x01;
                ਰੋਕੋ;
        ਮੂਲ:
                ਵਾਪਸ -EINVAL;
        }

        ਲਈ (size_t i = 1; i < len; i++) {
                ਜੇ (((char32_t)str[i] & 0xc0) != 0x80)
                        ਵਾਪਸ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ਵਾਪਸ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ਸਥਿਰ bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ਵਾਪਸ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ਅਚਲ ਅੱਖਰ* str, size_t length, bool allow_newline) {
        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = str; length > 0;) {
                ਪੂਰਨ encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ਜਦੋਂ (length > 0) {
                        uint64_t w;
                        size_t n;

                        ਜੇ (length < ਆਕਾਰ(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                ਸਮ੍ਰਿਤੀ_ਨਕਲ(&w, p, n);
                        } ਨਹੀਂਤਾਂ {
                                n = ਆਕਾਰ(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ਜੇ (!is_printable_fastpath_u64(w))
                                ਰੋਕੋ;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ਜੇ (length == 0)
                        ਰੋਕੋ;

                encoded_len = utf8_encoded_valid_unichar_ਭਰਿਆ(p, length, &val);
                ਜੇ (encoded_len < 0)
                        ਵਾਪਸ false;
                ਪ੍ਰਤਿਜ੍ਞਾ(encoded_len > 0 && (size_t) encoded_len <= length);

                ਜੇ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ਵਾਪਸ false;

                length -= encoded_len;
                p += encoded_len;
        }

        ਵਾਪਸ true;
}

ਅੱਖਰ* utf8_is_valid_n(ਅਚਲ ਅੱਖਰ *str, size_t len_ਬਾਈਟਾਂ) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        ਲਈ (size_t i = 0; len_ਬਾਈਟਾਂ != SIZE_MAX ? i < len_ਬਾਈਟਾਂ : str[i] != '\0'; ) {
                ਪੂਰਨ len;

                ਜੇ (_unlikely_(str[i] == '\0'))
                        ਵਾਪਸ ਨਿਰਾ; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_ਬਾਈਟਾਂ != SIZE_MAX ? len_ਬਾਈਟਾਂ - i : SIZE_MAX);
                ਜੇ (_unlikely_(len < 0))
                        ਵਾਪਸ ਨਿਰਾ; /* invalid character */

                i += len;
        }

        ਵਾਪਸ (ਅੱਖਰ*) str;
}

ਅੱਖਰ* utf8_escape_invalid(ਅਚਲ ਅੱਖਰ *str) {
        ਅੱਖਰ *p, *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        p = s = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਲੜੀ_ਲੰਬਾਈ(str) * 4 + 1);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਜਦੋਂ (*str) {
                ਪੂਰਨ len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ਜੇ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ਨਹੀਂਤਾਂ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ਵਾਪਸ str_realloc(p);
}

ਪੂਰਨ utf8_char_console_ਚੌੜਾਈ(ਅਚਲ ਅੱਖਰ *str) {
        char32_t c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        assert_se(utf8_encoded_ਮੰਜ਼ਿਲ_unichar(str, &c) == r);

        ਵਾਪਸ unichar_console_ਚੌੜਾਈ(c);
}

ਪੂਰਨ unichar_console_ਚੌੜਾਈ(char32_t c) {
        ਜੇ (c == '\t')
                ਵਾਪਸ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ਵਾਪਸ unichar_iswide(c) ? 2 : 1;
}

ਅੱਖਰ* utf8_escape_non_printable_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *str, size_t console_ਚੌੜਾਈ, bool force_ellipsis) {
        ਅੱਖਰ *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        ਜੇ (console_ਚੌੜਾਈ == 0)
                ਵਾਪਸ strdup("");

        size_t body = ਲੜੀ_ਲੰਬਾਈ(str) * 4;
        p = s = prev_s = ਸਮ੍ਰਿਤੀ_ਦਿਓ(body + STRLEN("…") + 1);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (;;) {
                ਪੂਰਨ len;
                ਅੱਖਰ *saved_s = s;

                ਜੇ (!*str) { /* done! */
                        ਜੇ (force_ellipsis)
                                ਜਾਓ truncation;
                        ਨਹੀਂਤਾਂ
                                ਜਾਓ ਅੰਤ;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ਜੇ (len > 0) {
                        ਜੇ (utf8_is_printable(str, len)) {
                                ਪੂਰਨ w;

                                w = utf8_char_console_ਚੌੜਾਈ(str);
                                ਪ੍ਰਤਿਜ੍ਞਾ(w >= 0);
                                ਜੇ (n + w > console_ਚੌੜਾਈ)
                                        ਜਾਓ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ਨਹੀਂਤਾਂ {
                                ਲਈ (; len > 0; len--) {
                                        ਜੇ (n + 4 > console_ਚੌੜਾਈ)
                                                ਜਾਓ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((ਪੂਰਨ) *str >> 4);
                                        *(s++) = hexchar((ਪੂਰਨ) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ਨਹੀਂਤਾਂ {
                        ਜੇ (n + 1 > console_ਚੌੜਾਈ)
                                ਜਾਓ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, ਲੜੀ_ਲੰਬਾਈ(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ਜੇ (n + 1 > console_ਚੌੜਾਈ)
                s = prev_s;

        s = mempcpy(s, "…", ਲੜੀ_ਲੰਬਾਈ("…"));

 ਅੰਤ:
        *s = '\0';
        ਵਾਪਸ str_realloc(p);
}

ਅੱਖਰ* ascii_is_valid_n(ਅਚਲ ਅੱਖਰ *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        ਲਈ (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) str[i] >= 128 || str[i] == '\0')
                        ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ (ਅੱਖਰ*) str;
}

ਪੂਰਨ utf8_ਮੰਜ਼ਿਲ_ascii(ਅਚਲ ਅੱਖਰ *str, ਅੱਖਰ replacement_char, ਅੱਖਰ **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        ਪ੍ਰਤਿਜ੍ਞਾ(str);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        _cleanup_free_ ਅੱਖਰ *ans = new(ਅੱਖਰ, ਲੜੀ_ਲੰਬਾਈ(str) + 1);
        ਜੇ (!ans)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ *q = ans;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = str; *p; q++) {
                ਪੂਰਨ l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ਜੇ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ਵਾਪਸ l;

                ਜੇ (l == 1)
                        *q = *p;
                ਨਹੀਂਤਾਂ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ਵਾਪਸ 0;
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
size_t utf8_encode_unichar(ਅੱਖਰ *ਬਾਹਰਲਾ_utf8, char32_t g) {

        ਜੇ (g < (1 << 7)) {
                ਜੇ (ਬਾਹਰਲਾ_utf8)
                        ਬਾਹਰਲਾ_utf8[0] = g & 0x7f;
                ਵਾਪਸ 1;
        } ਨਹੀਂਤਾਂ ਜੇ (g < (1 << 11)) {
                ਜੇ (ਬਾਹਰਲਾ_utf8) {
                        ਬਾਹਰਲਾ_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        ਬਾਹਰਲਾ_utf8[1] = 0x80 | (g & 0x3f);
                }
                ਵਾਪਸ 2;
        } ਨਹੀਂਤਾਂ ਜੇ (g < (1 << 16)) {
                ਜੇ (ਬਾਹਰਲਾ_utf8) {
                        ਬਾਹਰਲਾ_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        ਬਾਹਰਲਾ_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        ਬਾਹਰਲਾ_utf8[2] = 0x80 | (g & 0x3f);
                }
                ਵਾਪਸ 3;
        } ਨਹੀਂਤਾਂ ਜੇ (g < (1 << 21)) {
                ਜੇ (ਬਾਹਰਲਾ_utf8) {
                        ਬਾਹਰਲਾ_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        ਬਾਹਰਲਾ_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        ਬਾਹਰਲਾ_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        ਬਾਹਰਲਾ_utf8[3] = 0x80 | (g & 0x3f);
                }
                ਵਾਪਸ 4;
        }

        ਵਾਪਸ 0;
}

ਅੱਖਰ* utf16_ਮੰਜ਼ਿਲ_utf8(ਅਚਲ char16_t *s, size_t length /* bytes! */) {
        ਅਚਲ uint8_t *f;
        ਅੱਖਰ *r, *t;

        ਜੇ (length == 0)
                ਵਾਪਸ new0(ਅੱਖਰ, 1);

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (length == SIZE_MAX) {
                length = char16_strlen(s);

                ਜੇ (length > SIZE_MAX/2)
                        ਵਾਪਸ ਨਿਰਾ; /*ਉਛਾਲ*/

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ਜੇ (length > (SIZE_MAX - 1) / 2)
                ਵਾਪਸ ਨਿਰਾ; /*ਉਛਾਲ*/

        r = new(ਅੱਖਰ, length * 2 + 1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        f = (ਅਚਲ uint8_t*) s;
        t = r;

        ਜਦੋਂ (f + 1 < (ਅਚਲ uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ਜੇ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        ਜਾਰੀ;
                }

                ਜੇ (utf16_is_trailing_surrogate(w1))
                        ਜਾਰੀ; /* spurious trailing surrogate, ignore */

                ਜੇ (f + 1 >= (ਅਚਲ uint8_t*) s + length)
                        ਰੋਕੋ;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ਜੇ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        ਜਾਰੀ; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_ਮੰਜ਼ਿਲ_unichar(w1, w2));
        }

        *t = 0;
        ਵਾਪਸ r;
}

size_t utf16_encode_unichar(char16_t *ਬਾਹਰਲਾ, char32_t c) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਾਹਰਲਾ);

        /* Note that this encodes as little-endian. */

        ਚੋਣ (c) {

        ਹਾਲਤ 0 ... 0xd7ffU:
        ਹਾਲਤ 0xe000U ... 0xffffU:
                ਬਾਹਰਲਾ[0] = htole16(c);
                ਵਾਪਸ 1;

        ਹਾਲਤ 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                ਬਾਹਰਲਾ[0] = htole16((c >> 10) + 0xd800U);
                ਬਾਹਰਲਾ[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ਵਾਪਸ 2;

        ਮੂਲ: /* A surrogate (invalid) */
                ਵਾਪਸ 0;
        }
}

char16_t *utf8_ਮੰਜ਼ਿਲ_utf16(ਅਚਲ ਅੱਖਰ *s, size_t length) {
        char16_t *n, *p;
        ਪੂਰਨ r;

        ਜੇ (length == 0)
                ਵਾਪਸ new0(char16_t, 1);

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (length == SIZE_MAX)
                length = ਲੜੀ_ਲੰਬਾਈ(s);

        ਜੇ (length > SIZE_MAX - 1)
                ਵਾਪਸ ਨਿਰਾ; /*ਉਛਾਲ*/

        n = new(char16_t, length + 1);
        ਜੇ (!n)
                ਵਾਪਸ ਨਿਰਾ;

        p = n;

        ਲਈ (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ਜੇ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        ਜਾਓ copy;

                ਜੇ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        ਜਾਓ copy;

                r = utf8_encoded_ਮੰਜ਼ਿਲ_unichar(s + i, &unichar);
                ਜੇ (r < 0) /* sequence invalid, then copy as-is */
                        ਜਾਓ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                ਜਾਰੀ;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ਵਾਪਸ n;
}

size_t char16_strlen(ਅਚਲ char16_t *s) {
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜਦੋਂ (*s != 0)
                n++, s++;

        ਵਾਪਸ n;
}

size_t char16_strsize(ਅਚਲ char16_t *s) {
        ਸੂਚਕ_MAY_BE_NULL(s);

        ਵਾਪਸ s ? (char16_strlen(s) + 1) * ਆਕਾਰ(*s) : 0;
}

/* expected size used to encode one unicode char */
ਸਥਿਰ ਪੂਰਨ utf8_unichar_ਮੰਜ਼ਿਲ_encoded_len(char32_t unichar) {

        ਜੇ (unichar < 0x80)
                ਵਾਪਸ 1;
        ਜੇ (unichar < 0x800)
                ਵਾਪਸ 2;
        ਜੇ (unichar < 0x10000)
                ਵਾਪਸ 3;
        ਜੇ (unichar < 0x200000)
                ਵਾਪਸ 4;
        ਜੇ (unichar < 0x4000000)
                ਵਾਪਸ 5;

        ਵਾਪਸ 6;
}

/* validate one encoded unicode char and return its length */
ਪੂਰਨ utf8_encoded_valid_unichar_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *str, size_t length /*ਬਾਈਟਾਂ*/, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(str);
        ਪ੍ਰਤਿਜ੍ਞਾ(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ਜੇ (len == 0)
                ਵਾਪਸ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ਜੇ (len > length)
                ਵਾਪਸ -EINVAL;

        /* ascii is valid */
        ਜੇ (len == 1) {
                ਜੇ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ਵਾਪਸ 1;
        }

        /* check if expected encoded chars are available */
        ਲਈ (size_t i = 0; i < len; i++)
                ਜੇ ((str[i] & 0x80) != 0x80)
                        ਵਾਪਸ -EINVAL;

        r = utf8_encoded_ਮੰਜ਼ਿਲ_unichar(str, &unichar);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* check if encoded length matches encoded value */
        ਜੇ (utf8_unichar_ਮੰਜ਼ਿਲ_encoded_len(unichar) != (ਪੂਰਨ) len)
                ਵਾਪਸ -EINVAL;

        /* check if value has valid range */
        ਜੇ (!unichar_is_valid(unichar))
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret_unichar)
                *ret_unichar = unichar;

        ਵਾਪਸ (ਪੂਰਨ) len;
}

size_t utf8_n_codepoints(ਅਚਲ ਅੱਖਰ *str) {
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ਜਦੋਂ (*str != 0) {
                ਪੂਰਨ k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ਜੇ (k < 0)
                        ਵਾਪਸ SIZE_MAX;

                str += k;
                n++;
        }

        ਵਾਪਸ n;
}

size_t utf8_console_ਚੌੜਾਈ(ਅਚਲ ਅੱਖਰ *str) {
        ਸੂਚਕ_MAY_BE_NULL(str);

        ਜੇ (isempty(str))
                ਵਾਪਸ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ਜਦੋਂ (*str) {
                ਪੂਰਨ w;

                w = utf8_char_console_ਚੌੜਾਈ(str);
                ਜੇ (w < 0)
                        ਵਾਪਸ SIZE_MAX;

                n += w;
                str = utf8_ਅਗਲਾ_char(str);
        }

        ਵਾਪਸ n;
}

size_t utf8_ਆਖਰੀ_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *s, size_t n) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (n == SIZE_MAX)
                n = ਲੜੀ_ਲੰਬਾਈ(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        ਲਈ (size_t ਆਖਰੀ_2 = 0;;) {
                ਜੇ (n == 0)
                        ਵਾਪਸ ਆਖਰੀ_2;

                r = utf8_encoded_valid_unichar(s, n);
                ਜੇ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                ਆਖਰੀ_2 = r;
        }
}
