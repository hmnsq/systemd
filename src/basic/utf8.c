/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "gunicode.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "unaligned.h"
#అన్తర్భావ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        ఒకవేళ (ch >= 0x110000) /* End of unicode space */
                తిరిగి false;
        ఒకవేళ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                తిరిగి false;
        ఒకవేళ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                తిరిగి false;
        ఒకవేళ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                తిరిగి false;

        తిరిగి true;
}

స్థిర bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        తిరిగి (ch < ' ' && !లోపల_సమితి(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
స్థిర size_t utf8_encoded_expected_len(uint8_t c) {
        ఒకవేళ (c < 0x80)
                తిరిగి 1;
        ఒకవేళ ((c & 0xe0) == 0xc0)
                తిరిగి 2;
        ఒకవేళ ((c & 0xf0) == 0xe0)
                తిరిగి 3;
        ఒకవేళ ((c & 0xf8) == 0xf0)
                తిరిగి 4;
        ఒకవేళ ((c & 0xfc) == 0xf8)
                తిరిగి 5;
        ఒకవేళ ((c & 0xfe) == 0xfc)
                తిరిగి 6;

        తిరిగి 0;
}

/* decode one unicode char */
పూర్ణాంకం utf8_encoded_గమ్యం_unichar(స్థిరరాశి అక్షరం *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        ప్రతిజ్ఞా(str);
        ప్రతిజ్ఞా(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        ఎంపిక (len) {
        స్థితి 1:
                *ret_unichar = (char32_t)str[0];
                తిరిగి 1;
        స్థితి 2:
                unichar = str[0] & 0x1f;
                ఆపు;
        స్థితి 3:
                unichar = (char32_t)str[0] & 0x0f;
                ఆపు;
        స్థితి 4:
                unichar = (char32_t)str[0] & 0x07;
                ఆపు;
        స్థితి 5:
                unichar = (char32_t)str[0] & 0x03;
                ఆపు;
        స్థితి 6:
                unichar = (char32_t)str[0] & 0x01;
                ఆపు;
        అప్రమేయ:
                తిరిగి -EINVAL;
        }

        కొరకు (size_t i = 1; i < len; i++) {
                ఒకవేళ (((char32_t)str[i] & 0xc0) != 0x80)
                        తిరిగి -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        తిరిగి len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
స్థిర bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        తిరిగి (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(స్థిరరాశి అక్షరం* str, size_t length, bool allow_newline) {
        ప్రతిజ్ఞా(str);

        కొరకు (స్థిరరాశి అక్షరం *p = str; length > 0;) {
                పూర్ణాంకం encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                అయితే (length > 0) {
                        uint64_t w;
                        size_t n;

                        ఒకవేళ (length < పరిమాణం(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                జ్ఞాపకం_నకలు(&w, p, n);
                        } లేకపోతే {
                                n = పరిమాణం(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        ఒకవేళ (!is_printable_fastpath_u64(w))
                                ఆపు;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                ఒకవేళ (length == 0)
                        ఆపు;

                encoded_len = utf8_encoded_valid_unichar_నిండిన(p, length, &val);
                ఒకవేళ (encoded_len < 0)
                        తిరిగి false;
                ప్రతిజ్ఞా(encoded_len > 0 && (size_t) encoded_len <= length);

                ఒకవేళ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        తిరిగి false;

                length -= encoded_len;
                p += encoded_len;
        }

        తిరిగి true;
}

అక్షరం* utf8_is_valid_n(స్థిరరాశి అక్షరం *str, size_t len_బైట్లు) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        ప్రతిజ్ఞా(str);

        కొరకు (size_t i = 0; len_బైట్లు != SIZE_MAX ? i < len_బైట్లు : str[i] != '\0'; ) {
                పూర్ణాంకం len;

                ఒకవేళ (_unlikely_(str[i] == '\0'))
                        తిరిగి ఖాళీ; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_బైట్లు != SIZE_MAX ? len_బైట్లు - i : SIZE_MAX);
                ఒకవేళ (_unlikely_(len < 0))
                        తిరిగి ఖాళీ; /* invalid character */

                i += len;
        }

        తిరిగి (అక్షరం*) str;
}

అక్షరం* utf8_escape_invalid(స్థిరరాశి అక్షరం *str) {
        అక్షరం *p, *s;

        ప్రతిజ్ఞా(str);

        p = s = జ్ఞాపకం_ఇవ్వు(అక్షరశ్రేణి_పొడవు(str) * 4 + 1);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        అయితే (*str) {
                పూర్ణాంకం len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ఒకవేళ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } లేకపోతే {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        తిరిగి str_realloc(p);
}

పూర్ణాంకం utf8_char_console_వెడల్పు(స్థిరరాశి అక్షరం *str) {
        char32_t c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        ఒకవేళ (r < 0)
                తిరిగి r;

        assert_se(utf8_encoded_గమ్యం_unichar(str, &c) == r);

        తిరిగి unichar_console_వెడల్పు(c);
}

పూర్ణాంకం unichar_console_వెడల్పు(char32_t c) {
        ఒకవేళ (c == '\t')
                తిరిగి 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        తిరిగి unichar_iswide(c) ? 2 : 1;
}

అక్షరం* utf8_escape_non_printable_నిండిన(స్థిరరాశి అక్షరం *str, size_t console_వెడల్పు, bool force_ellipsis) {
        అక్షరం *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        ప్రతిజ్ఞా(str);

        ఒకవేళ (console_వెడల్పు == 0)
                తిరిగి strdup("");

        size_t body = అక్షరశ్రేణి_పొడవు(str) * 4;
        p = s = prev_s = జ్ఞాపకం_ఇవ్వు(body + STRLEN("…") + 1);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        కొరకు (;;) {
                పూర్ణాంకం len;
                అక్షరం *saved_s = s;

                ఒకవేళ (!*str) { /* done! */
                        ఒకవేళ (force_ellipsis)
                                వెళ్ళు truncation;
                        లేకపోతే
                                వెళ్ళు ముగింపు;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ఒకవేళ (len > 0) {
                        ఒకవేళ (utf8_is_printable(str, len)) {
                                పూర్ణాంకం w;

                                w = utf8_char_console_వెడల్పు(str);
                                ప్రతిజ్ఞా(w >= 0);
                                ఒకవేళ (n + w > console_వెడల్పు)
                                        వెళ్ళు truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } లేకపోతే {
                                కొరకు (; len > 0; len--) {
                                        ఒకవేళ (n + 4 > console_వెడల్పు)
                                                వెళ్ళు truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((పూర్ణాంకం) *str >> 4);
                                        *(s++) = hexchar((పూర్ణాంకం) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } లేకపోతే {
                        ఒకవేళ (n + 1 > console_వెడల్పు)
                                వెళ్ళు truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, అక్షరశ్రేణి_పొడవు(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        ఒకవేళ (n + 1 > console_వెడల్పు)
                s = prev_s;

        s = mempcpy(s, "…", అక్షరశ్రేణి_పొడవు("…"));

 ముగింపు:
        *s = '\0';
        తిరిగి str_realloc(p);
}

అక్షరం* ascii_is_valid_n(స్థిరరాశి అక్షరం *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        ప్రతిజ్ఞా(str);

        కొరకు (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                ఒకవేళ ((గుర్తులేని అక్షరం) str[i] >= 128 || str[i] == '\0')
                        తిరిగి ఖాళీ;

        తిరిగి (అక్షరం*) str;
}

పూర్ణాంకం utf8_గమ్యం_ascii(స్థిరరాశి అక్షరం *str, అక్షరం replacement_char, అక్షరం **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        ప్రతిజ్ఞా(str);
        ప్రతిజ్ఞా(ret);

        _cleanup_free_ అక్షరం *ans = new(అక్షరం, అక్షరశ్రేణి_పొడవు(str) + 1);
        ఒకవేళ (!ans)
                తిరిగి -ENOMEM;

        అక్షరం *q = ans;

        కొరకు (స్థిరరాశి అక్షరం *p = str; *p; q++) {
                పూర్ణాంకం l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                ఒకవేళ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        తిరిగి l;

                ఒకవేళ (l == 1)
                        *q = *p;
                లేకపోతే
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        తిరిగి 0;
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
size_t utf8_encode_unichar(అక్షరం *బహిర్గతం_utf8, char32_t g) {

        ఒకవేళ (g < (1 << 7)) {
                ఒకవేళ (బహిర్గతం_utf8)
                        బహిర్గతం_utf8[0] = g & 0x7f;
                తిరిగి 1;
        } లేకపోతే ఒకవేళ (g < (1 << 11)) {
                ఒకవేళ (బహిర్గతం_utf8) {
                        బహిర్గతం_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        బహిర్గతం_utf8[1] = 0x80 | (g & 0x3f);
                }
                తిరిగి 2;
        } లేకపోతే ఒకవేళ (g < (1 << 16)) {
                ఒకవేళ (బహిర్గతం_utf8) {
                        బహిర్గతం_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        బహిర్గతం_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        బహిర్గతం_utf8[2] = 0x80 | (g & 0x3f);
                }
                తిరిగి 3;
        } లేకపోతే ఒకవేళ (g < (1 << 21)) {
                ఒకవేళ (బహిర్గతం_utf8) {
                        బహిర్గతం_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        బహిర్గతం_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        బహిర్గతం_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        బహిర్గతం_utf8[3] = 0x80 | (g & 0x3f);
                }
                తిరిగి 4;
        }

        తిరిగి 0;
}

అక్షరం* utf16_గమ్యం_utf8(స్థిరరాశి char16_t *s, size_t length /* bytes! */) {
        స్థిరరాశి uint8_t *f;
        అక్షరం *r, *t;

        ఒకవేళ (length == 0)
                తిరిగి new0(అక్షరం, 1);

        ప్రతిజ్ఞా(s);

        ఒకవేళ (length == SIZE_MAX) {
                length = char16_strlen(s);

                ఒకవేళ (length > SIZE_MAX/2)
                        తిరిగి ఖాళీ; /*అధికప్రవాహం*/

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        ఒకవేళ (length > (SIZE_MAX - 1) / 2)
                తిరిగి ఖాళీ; /*అధికప్రవాహం*/

        r = new(అక్షరం, length * 2 + 1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        f = (స్థిరరాశి uint8_t*) s;
        t = r;

        అయితే (f + 1 < (స్థిరరాశి uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                ఒకవేళ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        కొనసాగు;
                }

                ఒకవేళ (utf16_is_trailing_surrogate(w1))
                        కొనసాగు; /* spurious trailing surrogate, ignore */

                ఒకవేళ (f + 1 >= (స్థిరరాశి uint8_t*) s + length)
                        ఆపు;

                w2 = f[1] << 8 | f[0];
                f += 2;

                ఒకవేళ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        కొనసాగు; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_గమ్యం_unichar(w1, w2));
        }

        *t = 0;
        తిరిగి r;
}

size_t utf16_encode_unichar(char16_t *బహిర్గతం, char32_t c) {
        ప్రతిజ్ఞా(బహిర్గతం);

        /* Note that this encodes as little-endian. */

        ఎంపిక (c) {

        స్థితి 0 ... 0xd7ffU:
        స్థితి 0xe000U ... 0xffffU:
                బహిర్గతం[0] = htole16(c);
                తిరిగి 1;

        స్థితి 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                బహిర్గతం[0] = htole16((c >> 10) + 0xd800U);
                బహిర్గతం[1] = htole16((c & 0x3ffU) + 0xdc00U);
                తిరిగి 2;

        అప్రమేయ: /* A surrogate (invalid) */
                తిరిగి 0;
        }
}

char16_t *utf8_గమ్యం_utf16(స్థిరరాశి అక్షరం *s, size_t length) {
        char16_t *n, *p;
        పూర్ణాంకం r;

        ఒకవేళ (length == 0)
                తిరిగి new0(char16_t, 1);

        ప్రతిజ్ఞా(s);

        ఒకవేళ (length == SIZE_MAX)
                length = అక్షరశ్రేణి_పొడవు(s);

        ఒకవేళ (length > SIZE_MAX - 1)
                తిరిగి ఖాళీ; /*అధికప్రవాహం*/

        n = new(char16_t, length + 1);
        ఒకవేళ (!n)
                తిరిగి ఖాళీ;

        p = n;

        కొరకు (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                ఒకవేళ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        వెళ్ళు copy;

                ఒకవేళ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        వెళ్ళు copy;

                r = utf8_encoded_గమ్యం_unichar(s + i, &unichar);
                ఒకవేళ (r < 0) /* sequence invalid, then copy as-is */
                        వెళ్ళు copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                కొనసాగు;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        తిరిగి n;
}

size_t char16_strlen(స్థిరరాశి char16_t *s) {
        size_t n = 0;

        ప్రతిజ్ఞా(s);

        అయితే (*s != 0)
                n++, s++;

        తిరిగి n;
}

size_t char16_strsize(స్థిరరాశి char16_t *s) {
        సూచిక_MAY_BE_NULL(s);

        తిరిగి s ? (char16_strlen(s) + 1) * పరిమాణం(*s) : 0;
}

/* expected size used to encode one unicode char */
స్థిర పూర్ణాంకం utf8_unichar_గమ్యం_encoded_len(char32_t unichar) {

        ఒకవేళ (unichar < 0x80)
                తిరిగి 1;
        ఒకవేళ (unichar < 0x800)
                తిరిగి 2;
        ఒకవేళ (unichar < 0x10000)
                తిరిగి 3;
        ఒకవేళ (unichar < 0x200000)
                తిరిగి 4;
        ఒకవేళ (unichar < 0x4000000)
                తిరిగి 5;

        తిరిగి 6;
}

/* validate one encoded unicode char and return its length */
పూర్ణాంకం utf8_encoded_valid_unichar_నిండిన(స్థిరరాశి అక్షరం *str, size_t length /*బైట్లు*/, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(str);
        ప్రతిజ్ఞా(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        ఒకవేళ (len == 0)
                తిరిగి -EINVAL;

        /* Do we have a truncated multi-byte character? */
        ఒకవేళ (len > length)
                తిరిగి -EINVAL;

        /* ascii is valid */
        ఒకవేళ (len == 1) {
                ఒకవేళ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                తిరిగి 1;
        }

        /* check if expected encoded chars are available */
        కొరకు (size_t i = 0; i < len; i++)
                ఒకవేళ ((str[i] & 0x80) != 0x80)
                        తిరిగి -EINVAL;

        r = utf8_encoded_గమ్యం_unichar(str, &unichar);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* check if encoded length matches encoded value */
        ఒకవేళ (utf8_unichar_గమ్యం_encoded_len(unichar) != (పూర్ణాంకం) len)
                తిరిగి -EINVAL;

        /* check if value has valid range */
        ఒకవేళ (!unichar_is_valid(unichar))
                తిరిగి -EINVAL;

        ఒకవేళ (ret_unichar)
                *ret_unichar = unichar;

        తిరిగి (పూర్ణాంకం) len;
}

size_t utf8_n_codepoints(స్థిరరాశి అక్షరం *str) {
        size_t n = 0;

        ప్రతిజ్ఞా(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        అయితే (*str != 0) {
                పూర్ణాంకం k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                ఒకవేళ (k < 0)
                        తిరిగి SIZE_MAX;

                str += k;
                n++;
        }

        తిరిగి n;
}

size_t utf8_console_వెడల్పు(స్థిరరాశి అక్షరం *str) {
        సూచిక_MAY_BE_NULL(str);

        ఒకవేళ (isempty(str))
                తిరిగి 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        అయితే (*str) {
                పూర్ణాంకం w;

                w = utf8_char_console_వెడల్పు(str);
                ఒకవేళ (w < 0)
                        తిరిగి SIZE_MAX;

                n += w;
                str = utf8_తదుపరి_char(str);
        }

        తిరిగి n;
}

size_t utf8_చివరి_నిడివి(స్థిరరాశి అక్షరం *s, size_t n) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        ఒకవేళ (n == SIZE_MAX)
                n = అక్షరశ్రేణి_పొడవు(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        కొరకు (size_t చివరి_2 = 0;;) {
                ఒకవేళ (n == 0)
                        తిరిగి చివరి_2;

                r = utf8_encoded_valid_unichar(s, n);
                ఒకవేళ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                చివరి_2 = r;
        }
}
