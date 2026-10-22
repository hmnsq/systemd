/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "gunicode.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        ଯଦି (ch >= 0x110000) /* End of unicode space */
                ଫେରନ୍ତୁ false;
        ଯଦି ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                ଫେରନ୍ତୁ false;
        ଯଦି ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                ଫେରନ୍ତୁ false;
        ଯଦି ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        ଫେରନ୍ତୁ (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
ସ୍ଥିର size_t utf8_encoded_expected_len(uint8_t c) {
        ଯଦି (c < 0x80)
                ଫେରନ୍ତୁ 1;
        ଯଦି ((c & 0xe0) == 0xc0)
                ଫେରନ୍ତୁ 2;
        ଯଦି ((c & 0xf0) == 0xe0)
                ଫେରନ୍ତୁ 3;
        ଯଦି ((c & 0xf8) == 0xf0)
                ଫେରନ୍ତୁ 4;
        ଯଦି ((c & 0xfc) == 0xf8)
                ଫେରନ୍ତୁ 5;
        ଯଦି ((c & 0xfe) == 0xfc)
                ଫେରନ୍ତୁ 6;

        ଫେରନ୍ତୁ 0;
}

/* decode one unicode char */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_encoded_to_unichar(ଅଚଳ ଅକ୍ଷର *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        ପ୍ରତିଜ୍ଞା(str);
        ପ୍ରତିଜ୍ଞା(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        ବାଛ (len) {
        ସ୍ଥିତି 1:
                *ret_unichar = (char32_t)str[0];
                ଫେରନ୍ତୁ 1;
        ସ୍ଥିତି 2:
                unichar = str[0] & 0x1f;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 3:
                unichar = (char32_t)str[0] & 0x0f;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 4:
                unichar = (char32_t)str[0] & 0x07;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 5:
                unichar = (char32_t)str[0] & 0x03;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି 6:
                unichar = (char32_t)str[0] & 0x01;
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EINVAL;
        }

        ପାଇଁ (size_t i = 1; i < len; i++) {
                ଯଦି (((char32_t)str[i] & 0xc0) != 0x80)
                        ଫେରନ୍ତୁ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        ଫେରନ୍ତୁ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
ସ୍ଥିର bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        ଫେରନ୍ତୁ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(ଅଚଳ ଅକ୍ଷର* str, size_t length, bool allow_newline) {
        ପ୍ରତିଜ୍ଞା(str);

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = str; length > 0;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ଯେତେବେଳେ (length > 0) {
                        uint64_t w;
                        size_t n;

                        ଯଦି (length < ଆକାର(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } ଅନ୍ୟଥା {
                                n = ଆକାର(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ଯଦି (!is_printable_fastpath_u64(w))
                                ଭାଙ୍ଗ;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ଯଦି (length == 0)
                        ଭାଙ୍ଗ;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                ଯଦି (encoded_len < 0)
                        ଫେରନ୍ତୁ false;
                ପ୍ରତିଜ୍ଞା(encoded_len > 0 && (size_t) encoded_len <= length);

                ଯଦି (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        ଫେରନ୍ତୁ false;

                length -= encoded_len;
                p += encoded_len;
        }

        ଫେରନ୍ତୁ true;
}

ଅକ୍ଷର* utf8_is_valid_n(ଅଚଳ ଅକ୍ଷର *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        ପ୍ରତିଜ୍ଞା(str);

        ପାଇଁ (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

                ଯଦି (_unlikely_(str[i] == '\0'))
                        ଫେରନ୍ତୁ ଖାଲି; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                ଯଦି (_unlikely_(len < 0))
                        ଫେରନ୍ତୁ ଖାଲି; /* invalid character */

                i += len;
        }

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) str;
}

ଅକ୍ଷର* utf8_escape_invalid(ଅଚଳ ଅକ୍ଷର *str) {
        ଅକ୍ଷର *p, *s;

        ପ୍ରତିଜ୍ଞା(str);

        p = s = malloc(strlen(str) * 4 + 1);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯେତେବେଳେ (*str) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ଯଦି (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } ଅନ୍ୟଥା {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        ଫେରନ୍ତୁ str_realloc(p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_char_console_width(ଅଚଳ ଅକ୍ଷର *str) {
        char32_t c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        ଫେରନ୍ତୁ unichar_console_width(c);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unichar_console_width(char32_t c) {
        ଯଦି (c == '\t')
                ଫେରନ୍ତୁ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        ଫେରନ୍ତୁ unichar_iswide(c) ? 2 : 1;
}

ଅକ୍ଷର* utf8_escape_non_printable_full(ଅଚଳ ଅକ୍ଷର *str, size_t console_width, bool force_ellipsis) {
        ଅକ୍ଷର *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        ପ୍ରତିଜ୍ଞା(str);

        ଯଦି (console_width == 0)
                ଫେରନ୍ତୁ strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (;;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
                ଅକ୍ଷର *saved_s = s;

                ଯଦି (!*str) { /* done! */
                        ଯଦି (force_ellipsis)
                                ଯାଅ truncation;
                        ଅନ୍ୟଥା
                                ଯାଅ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ଯଦି (len > 0) {
                        ଯଦି (utf8_is_printable(str, len)) {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା w;

                                w = utf8_char_console_width(str);
                                ପ୍ରତିଜ୍ଞା(w >= 0);
                                ଯଦି (n + w > console_width)
                                        ଯାଅ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } ଅନ୍ୟଥା {
                                ପାଇଁ (; len > 0; len--) {
                                        ଯଦି (n + 4 > console_width)
                                                ଯାଅ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) *str >> 4);
                                        *(s++) = hexchar((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } ଅନ୍ୟଥା {
                        ଯଦି (n + 1 > console_width)
                                ଯାଅ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ଯଦି (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        ଫେରନ୍ତୁ str_realloc(p);
}

ଅକ୍ଷର* ascii_is_valid_n(ଅଚଳ ଅକ୍ଷର *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        ପ୍ରତିଜ୍ଞା(str);

        ପାଇଁ (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ଯଦି ((unsigned ଅକ୍ଷର) str[i] >= 128 || str[i] == '\0')
                        ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) str;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_to_ascii(ଅଚଳ ଅକ୍ଷର *str, ଅକ୍ଷର replacement_char, ଅକ୍ଷର **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        ପ୍ରତିଜ୍ଞା(str);
        ପ୍ରତିଜ୍ଞା(ret);

        _cleanup_free_ ଅକ୍ଷର *ans = new(ଅକ୍ଷର, strlen(str) + 1);
        ଯଦି (!ans)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର *q = ans;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = str; *p; q++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ଯଦି (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        ଫେରନ୍ତୁ l;

                ଯଦି (l == 1)
                        *q = *p;
                ଅନ୍ୟଥା
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        ଫେରନ୍ତୁ 0;
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
size_t utf8_encode_unichar(ଅକ୍ଷର *out_utf8, char32_t g) {

        ଯଦି (g < (1 << 7)) {
                ଯଦି (out_utf8)
                        out_utf8[0] = g & 0x7f;
                ଫେରନ୍ତୁ 1;
        } ଅନ୍ୟଥା ଯଦି (g < (1 << 11)) {
                ଯଦି (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                ଫେରନ୍ତୁ 2;
        } ଅନ୍ୟଥା ଯଦି (g < (1 << 16)) {
                ଯଦି (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                ଫେରନ୍ତୁ 3;
        } ଅନ୍ୟଥା ଯଦି (g < (1 << 21)) {
                ଯଦି (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                ଫେରନ୍ତୁ 4;
        }

        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* utf16_to_utf8(ଅଚଳ char16_t *s, size_t length /* bytes! */) {
        ଅଚଳ uint8_t *f;
        ଅକ୍ଷର *r, *t;

        ଯଦି (length == 0)
                ଫେରନ୍ତୁ new0(ଅକ୍ଷର, 1);

        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (length == SIZE_MAX) {
                length = char16_strlen(s);

                ଯଦି (length > SIZE_MAX/2)
                        ଫେରନ୍ତୁ ଖାଲି; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ଯଦି (length > (SIZE_MAX - 1) / 2)
                ଫେରନ୍ତୁ ଖାଲି; /* overflow */

        r = new(ଅକ୍ଷର, length * 2 + 1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        f = (ଅଚଳ uint8_t*) s;
        t = r;

        ଯେତେବେଳେ (f + 1 < (ଅଚଳ uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ଯଦି (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        ଜାରି;
                }

                ଯଦି (utf16_is_trailing_surrogate(w1))
                        ଜାରି; /* spurious trailing surrogate, ignore */

                ଯଦି (f + 1 >= (ଅଚଳ uint8_t*) s + length)
                        ଭାଙ୍ଗ;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ଯଦି (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        ଜାରି; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        ଫେରନ୍ତୁ r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        ପ୍ରତିଜ୍ଞା(out);

        /* Note that this encodes as little-endian. */

        ବାଛ (c) {

        ସ୍ଥିତି 0 ... 0xd7ffU:
        ସ୍ଥିତି 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                ଫେରନ୍ତୁ 1;

        ସ୍ଥିତି 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                ଫେରନ୍ତୁ 2;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* A surrogate (invalid) */
                ଫେରନ୍ତୁ 0;
        }
}

char16_t *utf8_to_utf16(ଅଚଳ ଅକ୍ଷର *s, size_t length) {
        char16_t *n, *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (length == 0)
                ଫେରନ୍ତୁ new0(char16_t, 1);

        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (length == SIZE_MAX)
                length = strlen(s);

        ଯଦି (length > SIZE_MAX - 1)
                ଫେରନ୍ତୁ ଖାଲି; /* overflow */

        n = new(char16_t, length + 1);
        ଯଦି (!n)
                ଫେରନ୍ତୁ ଖାଲି;

        p = n;

        ପାଇଁ (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ଯଦି (e <= 1) /* Invalid and single byte characters are copied as they are */
                        ଯାଅ copy;

                ଯଦି (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        ଯାଅ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                ଯଦି (r < 0) /* sequence invalid, then copy as-is */
                        ଯାଅ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                ଜାରି;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        ଫେରନ୍ତୁ n;
}

size_t char16_strlen(ଅଚଳ char16_t *s) {
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(s);

        ଯେତେବେଳେ (*s != 0)
                n++, s++;

        ଫେରନ୍ତୁ n;
}

size_t char16_strsize(ଅଚଳ char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        ଫେରନ୍ତୁ s ? (char16_strlen(s) + 1) * ଆକାର(*s) : 0;
}

/* expected size used to encode one unicode char */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_unichar_to_encoded_len(char32_t unichar) {

        ଯଦି (unichar < 0x80)
                ଫେରନ୍ତୁ 1;
        ଯଦି (unichar < 0x800)
                ଫେରନ୍ତୁ 2;
        ଯଦି (unichar < 0x10000)
                ଫେରନ୍ତୁ 3;
        ଯଦି (unichar < 0x200000)
                ଫେରନ୍ତୁ 4;
        ଯଦି (unichar < 0x4000000)
                ଫେରନ୍ତୁ 5;

        ଫେରନ୍ତୁ 6;
}

/* validate one encoded unicode char and return its length */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8_encoded_valid_unichar_full(ଅଚଳ ଅକ୍ଷର *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(str);
        ପ୍ରତିଜ୍ଞା(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ଯଦି (len == 0)
                ଫେରନ୍ତୁ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ଯଦି (len > length)
                ଫେରନ୍ତୁ -EINVAL;

        /* ascii is valid */
        ଯଦି (len == 1) {
                ଯଦି (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                ଫେରନ୍ତୁ 1;
        }

        /* check if expected encoded chars are available */
        ପାଇଁ (size_t i = 0; i < len; i++)
                ଯଦି ((str[i] & 0x80) != 0x80)
                        ଫେରନ୍ତୁ -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* check if encoded length matches encoded value */
        ଯଦି (utf8_unichar_to_encoded_len(unichar) != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) len)
                ଫେରନ୍ତୁ -EINVAL;

        /* check if value has valid range */
        ଯଦି (!unichar_is_valid(unichar))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret_unichar)
                *ret_unichar = unichar;

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) len;
}

size_t utf8_n_codepoints(ଅଚଳ ଅକ୍ଷର *str) {
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ଯେତେବେଳେ (*str != 0) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ଯଦି (k < 0)
                        ଫେରନ୍ତୁ SIZE_MAX;

                str += k;
                n++;
        }

        ଫେରନ୍ତୁ n;
}

size_t utf8_console_width(ଅଚଳ ଅକ୍ଷର *str) {
        POINTER_MAY_BE_NULL(str);

        ଯଦି (isempty(str))
                ଫେରନ୍ତୁ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ଯେତେବେଳେ (*str) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା w;

                w = utf8_char_console_width(str);
                ଯଦି (w < 0)
                        ଫେରନ୍ତୁ SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        ଫେରନ୍ତୁ n;
}

size_t utf8_last_length(ଅଚଳ ଅକ୍ଷର *s, size_t n) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        ପାଇଁ (size_t last = 0;;) {
                ଯଦି (n == 0)
                        ଫେରନ୍ତୁ last;

                r = utf8_encoded_valid_unichar(s, n);
                ଯଦି (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
