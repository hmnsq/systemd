/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "gunicode.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "unaligned.h"
#અન્તર્ભાવ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        જો (ch >= 0x110000) /* End of unicode space */
                પાછા false;
        જો ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                પાછા false;
        જો ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                પાછા false;
        જો ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                પાછા false;

        પાછા true;
}

સ્થિર bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        પાછા (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
સ્થિર size_t utf8_encoded_expected_len(uint8_t c) {
        જો (c < 0x80)
                પાછા 1;
        જો ((c & 0xe0) == 0xc0)
                પાછા 2;
        જો ((c & 0xf0) == 0xe0)
                પાછા 3;
        જો ((c & 0xf8) == 0xf0)
                પાછા 4;
        જો ((c & 0xfc) == 0xf8)
                પાછા 5;
        જો ((c & 0xfe) == 0xfc)
                પાછા 6;

        પાછા 0;
}

/* decode one unicode char */
પૂર્ણાંક utf8_encoded_to_unichar(અચલ અક્ષર *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        પ્રતિજ્ઞા(str);
        પ્રતિજ્ઞા(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        પસંદગી (len) {
        સ્થિતિ 1:
                *ret_unichar = (char32_t)str[0];
                પાછા 1;
        સ્થિતિ 2:
                unichar = str[0] & 0x1f;
                તોડો;
        સ્થિતિ 3:
                unichar = (char32_t)str[0] & 0x0f;
                તોડો;
        સ્થિતિ 4:
                unichar = (char32_t)str[0] & 0x07;
                તોડો;
        સ્થિતિ 5:
                unichar = (char32_t)str[0] & 0x03;
                તોડો;
        સ્થિતિ 6:
                unichar = (char32_t)str[0] & 0x01;
                તોડો;
        મૂળભૂત:
                પાછા -EINVAL;
        }

        માટે (size_t i = 1; i < len; i++) {
                જો (((char32_t)str[i] & 0xc0) != 0x80)
                        પાછા -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        પાછા len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
સ્થિર bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        પાછા (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(અચલ અક્ષર* str, size_t length, bool allow_newline) {
        પ્રતિજ્ઞા(str);

        માટે (અચલ અક્ષર *p = str; length > 0;) {
                પૂર્ણાંક encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                જ્યારે (length > 0) {
                        uint64_t w;
                        size_t n;

                        જો (length < કદ(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } નહીંતર {
                                n = કદ(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        જો (!is_printable_fastpath_u64(w))
                                તોડો;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                જો (length == 0)
                        તોડો;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                જો (encoded_len < 0)
                        પાછા false;
                પ્રતિજ્ઞા(encoded_len > 0 && (size_t) encoded_len <= length);

                જો (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        પાછા false;

                length -= encoded_len;
                p += encoded_len;
        }

        પાછા true;
}

અક્ષર* utf8_is_valid_n(અચલ અક્ષર *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        પ્રતિજ્ઞા(str);

        માટે (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                પૂર્ણાંક len;

                જો (_unlikely_(str[i] == '\0'))
                        પાછા ખાલી; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                જો (_unlikely_(len < 0))
                        પાછા ખાલી; /* invalid character */

                i += len;
        }

        પાછા (અક્ષર*) str;
}

અક્ષર* utf8_escape_invalid(અચલ અક્ષર *str) {
        અક્ષર *p, *s;

        પ્રતિજ્ઞા(str);

        p = s = malloc(strlen(str) * 4 + 1);
        જો (!p)
                પાછા ખાલી;

        જ્યારે (*str) {
                પૂર્ણાંક len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                જો (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } નહીંતર {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        પાછા str_realloc(p);
}

પૂર્ણાંક utf8_char_console_width(અચલ અક્ષર *str) {
        char32_t c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        જો (r < 0)
                પાછા r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        પાછા unichar_console_width(c);
}

પૂર્ણાંક unichar_console_width(char32_t c) {
        જો (c == '\t')
                પાછા 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        પાછા unichar_iswide(c) ? 2 : 1;
}

અક્ષર* utf8_escape_non_printable_full(અચલ અક્ષર *str, size_t console_width, bool force_ellipsis) {
        અક્ષર *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        પ્રતિજ્ઞા(str);

        જો (console_width == 0)
                પાછા strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        જો (!p)
                પાછા ખાલી;

        માટે (;;) {
                પૂર્ણાંક len;
                અક્ષર *saved_s = s;

                જો (!*str) { /* done! */
                        જો (force_ellipsis)
                                જાઓ truncation;
                        નહીંતર
                                જાઓ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                જો (len > 0) {
                        જો (utf8_is_printable(str, len)) {
                                પૂર્ણાંક w;

                                w = utf8_char_console_width(str);
                                પ્રતિજ્ઞા(w >= 0);
                                જો (n + w > console_width)
                                        જાઓ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } નહીંતર {
                                માટે (; len > 0; len--) {
                                        જો (n + 4 > console_width)
                                                જાઓ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((પૂર્ણાંક) *str >> 4);
                                        *(s++) = hexchar((પૂર્ણાંક) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } નહીંતર {
                        જો (n + 1 > console_width)
                                જાઓ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        જો (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        પાછા str_realloc(p);
}

અક્ષર* ascii_is_valid_n(અચલ અક્ષર *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        પ્રતિજ્ઞા(str);

        માટે (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                જો ((unsigned અક્ષર) str[i] >= 128 || str[i] == '\0')
                        પાછા ખાલી;

        પાછા (અક્ષર*) str;
}

પૂર્ણાંક utf8_to_ascii(અચલ અક્ષર *str, અક્ષર replacement_char, અક્ષર **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        પ્રતિજ્ઞા(str);
        પ્રતિજ્ઞા(ret);

        _cleanup_free_ અક્ષર *ans = new(અક્ષર, strlen(str) + 1);
        જો (!ans)
                પાછા -ENOMEM;

        અક્ષર *q = ans;

        માટે (અચલ અક્ષર *p = str; *p; q++) {
                પૂર્ણાંક l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                જો (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        પાછા l;

                જો (l == 1)
                        *q = *p;
                નહીંતર
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        પાછા 0;
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
size_t utf8_encode_unichar(અક્ષર *out_utf8, char32_t g) {

        જો (g < (1 << 7)) {
                જો (out_utf8)
                        out_utf8[0] = g & 0x7f;
                પાછા 1;
        } નહીંતર જો (g < (1 << 11)) {
                જો (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                પાછા 2;
        } નહીંતર જો (g < (1 << 16)) {
                જો (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                પાછા 3;
        } નહીંતર જો (g < (1 << 21)) {
                જો (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                પાછા 4;
        }

        પાછા 0;
}

અક્ષર* utf16_to_utf8(અચલ char16_t *s, size_t length /* bytes! */) {
        અચલ uint8_t *f;
        અક્ષર *r, *t;

        જો (length == 0)
                પાછા new0(અક્ષર, 1);

        પ્રતિજ્ઞા(s);

        જો (length == SIZE_MAX) {
                length = char16_strlen(s);

                જો (length > SIZE_MAX/2)
                        પાછા ખાલી; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        જો (length > (SIZE_MAX - 1) / 2)
                પાછા ખાલી; /* overflow */

        r = new(અક્ષર, length * 2 + 1);
        જો (!r)
                પાછા ખાલી;

        f = (અચલ uint8_t*) s;
        t = r;

        જ્યારે (f + 1 < (અચલ uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                જો (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        ચાલુ;
                }

                જો (utf16_is_trailing_surrogate(w1))
                        ચાલુ; /* spurious trailing surrogate, ignore */

                જો (f + 1 >= (અચલ uint8_t*) s + length)
                        તોડો;

                w2 = f[1] << 8 | f[0];
                f += 2;

                જો (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        ચાલુ; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        પાછા r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        પ્રતિજ્ઞા(out);

        /* Note that this encodes as little-endian. */

        પસંદગી (c) {

        સ્થિતિ 0 ... 0xd7ffU:
        સ્થિતિ 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                પાછા 1;

        સ્થિતિ 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                પાછા 2;

        મૂળભૂત: /* A surrogate (invalid) */
                પાછા 0;
        }
}

char16_t *utf8_to_utf16(અચલ અક્ષર *s, size_t length) {
        char16_t *n, *p;
        પૂર્ણાંક r;

        જો (length == 0)
                પાછા new0(char16_t, 1);

        પ્રતિજ્ઞા(s);

        જો (length == SIZE_MAX)
                length = strlen(s);

        જો (length > SIZE_MAX - 1)
                પાછા ખાલી; /* overflow */

        n = new(char16_t, length + 1);
        જો (!n)
                પાછા ખાલી;

        p = n;

        માટે (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                જો (e <= 1) /* Invalid and single byte characters are copied as they are */
                        જાઓ copy;

                જો (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        જાઓ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                જો (r < 0) /* sequence invalid, then copy as-is */
                        જાઓ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                ચાલુ;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        પાછા n;
}

size_t char16_strlen(અચલ char16_t *s) {
        size_t n = 0;

        પ્રતિજ્ઞા(s);

        જ્યારે (*s != 0)
                n++, s++;

        પાછા n;
}

size_t char16_strsize(અચલ char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        પાછા s ? (char16_strlen(s) + 1) * કદ(*s) : 0;
}

/* expected size used to encode one unicode char */
સ્થિર પૂર્ણાંક utf8_unichar_to_encoded_len(char32_t unichar) {

        જો (unichar < 0x80)
                પાછા 1;
        જો (unichar < 0x800)
                પાછા 2;
        જો (unichar < 0x10000)
                પાછા 3;
        જો (unichar < 0x200000)
                પાછા 4;
        જો (unichar < 0x4000000)
                પાછા 5;

        પાછા 6;
}

/* validate one encoded unicode char and return its length */
પૂર્ણાંક utf8_encoded_valid_unichar_full(અચલ અક્ષર *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(str);
        પ્રતિજ્ઞા(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        જો (len == 0)
                પાછા -EINVAL;

        /* Do we have a truncated multi-byte character? */
        જો (len > length)
                પાછા -EINVAL;

        /* ascii is valid */
        જો (len == 1) {
                જો (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                પાછા 1;
        }

        /* check if expected encoded chars are available */
        માટે (size_t i = 0; i < len; i++)
                જો ((str[i] & 0x80) != 0x80)
                        પાછા -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        જો (r < 0)
                પાછા r;

        /* check if encoded length matches encoded value */
        જો (utf8_unichar_to_encoded_len(unichar) != (પૂર્ણાંક) len)
                પાછા -EINVAL;

        /* check if value has valid range */
        જો (!unichar_is_valid(unichar))
                પાછા -EINVAL;

        જો (ret_unichar)
                *ret_unichar = unichar;

        પાછા (પૂર્ણાંક) len;
}

size_t utf8_n_codepoints(અચલ અક્ષર *str) {
        size_t n = 0;

        પ્રતિજ્ઞા(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        જ્યારે (*str != 0) {
                પૂર્ણાંક k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                જો (k < 0)
                        પાછા SIZE_MAX;

                str += k;
                n++;
        }

        પાછા n;
}

size_t utf8_console_width(અચલ અક્ષર *str) {
        POINTER_MAY_BE_NULL(str);

        જો (isempty(str))
                પાછા 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        જ્યારે (*str) {
                પૂર્ણાંક w;

                w = utf8_char_console_width(str);
                જો (w < 0)
                        પાછા SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        પાછા n;
}

size_t utf8_last_length(અચલ અક્ષર *s, size_t n) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        જો (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        માટે (size_t last = 0;;) {
                જો (n == 0)
                        પાછા last;

                r = utf8_encoded_valid_unichar(s, n);
                જો (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
