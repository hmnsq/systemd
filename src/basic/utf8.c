/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "gunicode.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "unaligned.h"
#அந்தர்பாவ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        எனில் (ch >= 0x110000) /* End of unicode space */
                திரும்பு false;
        எனில் ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                திரும்பு false;
        எனில் ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                திரும்பு false;
        எனில் ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                திரும்பு false;

        திரும்பு true;
}

நிலையான bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        திரும்பு (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
நிலையான size_t utf8_encoded_expected_len(uint8_t c) {
        எனில் (c < 0x80)
                திரும்பு 1;
        எனில் ((c & 0xe0) == 0xc0)
                திரும்பு 2;
        எனில் ((c & 0xf0) == 0xe0)
                திரும்பு 3;
        எனில் ((c & 0xf8) == 0xf0)
                திரும்பு 4;
        எனில் ((c & 0xfc) == 0xf8)
                திரும்பு 5;
        எனில் ((c & 0xfe) == 0xfc)
                திரும்பு 6;

        திரும்பு 0;
}

/* decode one unicode char */
முழு utf8_encoded_to_unichar(மாறிலி எழுத்து *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        ப்ரதிஜ்ஞா(str);
        ப்ரதிஜ்ஞா(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        தேர்வு (len) {
        நிலை 1:
                *ret_unichar = (char32_t)str[0];
                திரும்பு 1;
        நிலை 2:
                unichar = str[0] & 0x1f;
                நிறுத்து;
        நிலை 3:
                unichar = (char32_t)str[0] & 0x0f;
                நிறுத்து;
        நிலை 4:
                unichar = (char32_t)str[0] & 0x07;
                நிறுத்து;
        நிலை 5:
                unichar = (char32_t)str[0] & 0x03;
                நிறுத்து;
        நிலை 6:
                unichar = (char32_t)str[0] & 0x01;
                நிறுத்து;
        இயல்பு:
                திரும்பு -EINVAL;
        }

        க்கு (size_t i = 1; i < len; i++) {
                எனில் (((char32_t)str[i] & 0xc0) != 0x80)
                        திரும்பு -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        திரும்பு len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
நிலையான bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        திரும்பு (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(மாறிலி எழுத்து* str, size_t length, bool allow_newline) {
        ப்ரதிஜ்ஞா(str);

        க்கு (மாறிலி எழுத்து *p = str; length > 0;) {
                முழு encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                போது (length > 0) {
                        uint64_t w;
                        size_t n;

                        எனில் (length < அளவு(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } இல்லையெனில் {
                                n = அளவு(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        எனில் (!is_printable_fastpath_u64(w))
                                நிறுத்து;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                எனில் (length == 0)
                        நிறுத்து;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                எனில் (encoded_len < 0)
                        திரும்பு false;
                ப்ரதிஜ்ஞா(encoded_len > 0 && (size_t) encoded_len <= length);

                எனில் (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        திரும்பு false;

                length -= encoded_len;
                p += encoded_len;
        }

        திரும்பு true;
}

எழுத்து* utf8_is_valid_n(மாறிலி எழுத்து *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        ப்ரதிஜ்ஞா(str);

        க்கு (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                முழு len;

                எனில் (_unlikely_(str[i] == '\0'))
                        திரும்பு வெற்று; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                எனில் (_unlikely_(len < 0))
                        திரும்பு வெற்று; /* invalid character */

                i += len;
        }

        திரும்பு (எழுத்து*) str;
}

எழுத்து* utf8_escape_invalid(மாறிலி எழுத்து *str) {
        எழுத்து *p, *s;

        ப்ரதிஜ்ஞா(str);

        p = s = malloc(strlen(str) * 4 + 1);
        எனில் (!p)
                திரும்பு வெற்று;

        போது (*str) {
                முழு len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                எனில் (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } இல்லையெனில் {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        திரும்பு str_realloc(p);
}

முழு utf8_char_console_width(மாறிலி எழுத்து *str) {
        char32_t c;
        முழு r;

        ப்ரதிஜ்ஞா(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        எனில் (r < 0)
                திரும்பு r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        திரும்பு unichar_console_width(c);
}

முழு unichar_console_width(char32_t c) {
        எனில் (c == '\t')
                திரும்பு 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        திரும்பு unichar_iswide(c) ? 2 : 1;
}

எழுத்து* utf8_escape_non_printable_full(மாறிலி எழுத்து *str, size_t console_width, bool force_ellipsis) {
        எழுத்து *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        ப்ரதிஜ்ஞா(str);

        எனில் (console_width == 0)
                திரும்பு strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        எனில் (!p)
                திரும்பு வெற்று;

        க்கு (;;) {
                முழு len;
                எழுத்து *saved_s = s;

                எனில் (!*str) { /* done! */
                        எனில் (force_ellipsis)
                                செல் truncation;
                        இல்லையெனில்
                                செல் finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                எனில் (len > 0) {
                        எனில் (utf8_is_printable(str, len)) {
                                முழு w;

                                w = utf8_char_console_width(str);
                                ப்ரதிஜ்ஞா(w >= 0);
                                எனில் (n + w > console_width)
                                        செல் truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } இல்லையெனில் {
                                க்கு (; len > 0; len--) {
                                        எனில் (n + 4 > console_width)
                                                செல் truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((முழு) *str >> 4);
                                        *(s++) = hexchar((முழு) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } இல்லையெனில் {
                        எனில் (n + 1 > console_width)
                                செல் truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        எனில் (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        திரும்பு str_realloc(p);
}

எழுத்து* ascii_is_valid_n(மாறிலி எழுத்து *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        ப்ரதிஜ்ஞா(str);

        க்கு (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                எனில் ((unsigned எழுத்து) str[i] >= 128 || str[i] == '\0')
                        திரும்பு வெற்று;

        திரும்பு (எழுத்து*) str;
}

முழு utf8_to_ascii(மாறிலி எழுத்து *str, எழுத்து replacement_char, எழுத்து **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        ப்ரதிஜ்ஞா(str);
        ப்ரதிஜ்ஞா(ret);

        _cleanup_free_ எழுத்து *ans = new(எழுத்து, strlen(str) + 1);
        எனில் (!ans)
                திரும்பு -ENOMEM;

        எழுத்து *q = ans;

        க்கு (மாறிலி எழுத்து *p = str; *p; q++) {
                முழு l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                எனில் (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        திரும்பு l;

                எனில் (l == 1)
                        *q = *p;
                இல்லையெனில்
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        திரும்பு 0;
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
size_t utf8_encode_unichar(எழுத்து *out_utf8, char32_t g) {

        எனில் (g < (1 << 7)) {
                எனில் (out_utf8)
                        out_utf8[0] = g & 0x7f;
                திரும்பு 1;
        } இல்லையெனில் எனில் (g < (1 << 11)) {
                எனில் (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                திரும்பு 2;
        } இல்லையெனில் எனில் (g < (1 << 16)) {
                எனில் (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                திரும்பு 3;
        } இல்லையெனில் எனில் (g < (1 << 21)) {
                எனில் (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                திரும்பு 4;
        }

        திரும்பு 0;
}

எழுத்து* utf16_to_utf8(மாறிலி char16_t *s, size_t length /* bytes! */) {
        மாறிலி uint8_t *f;
        எழுத்து *r, *t;

        எனில் (length == 0)
                திரும்பு new0(எழுத்து, 1);

        ப்ரதிஜ்ஞா(s);

        எனில் (length == SIZE_MAX) {
                length = char16_strlen(s);

                எனில் (length > SIZE_MAX/2)
                        திரும்பு வெற்று; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        எனில் (length > (SIZE_MAX - 1) / 2)
                திரும்பு வெற்று; /* overflow */

        r = new(எழுத்து, length * 2 + 1);
        எனில் (!r)
                திரும்பு வெற்று;

        f = (மாறிலி uint8_t*) s;
        t = r;

        போது (f + 1 < (மாறிலி uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                எனில் (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        தொடர்;
                }

                எனில் (utf16_is_trailing_surrogate(w1))
                        தொடர்; /* spurious trailing surrogate, ignore */

                எனில் (f + 1 >= (மாறிலி uint8_t*) s + length)
                        நிறுத்து;

                w2 = f[1] << 8 | f[0];
                f += 2;

                எனில் (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        தொடர்; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        திரும்பு r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        ப்ரதிஜ்ஞா(out);

        /* Note that this encodes as little-endian. */

        தேர்வு (c) {

        நிலை 0 ... 0xd7ffU:
        நிலை 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                திரும்பு 1;

        நிலை 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                திரும்பு 2;

        இயல்பு: /* A surrogate (invalid) */
                திரும்பு 0;
        }
}

char16_t *utf8_to_utf16(மாறிலி எழுத்து *s, size_t length) {
        char16_t *n, *p;
        முழு r;

        எனில் (length == 0)
                திரும்பு new0(char16_t, 1);

        ப்ரதிஜ்ஞா(s);

        எனில் (length == SIZE_MAX)
                length = strlen(s);

        எனில் (length > SIZE_MAX - 1)
                திரும்பு வெற்று; /* overflow */

        n = new(char16_t, length + 1);
        எனில் (!n)
                திரும்பு வெற்று;

        p = n;

        க்கு (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                எனில் (e <= 1) /* Invalid and single byte characters are copied as they are */
                        செல் copy;

                எனில் (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        செல் copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                எனில் (r < 0) /* sequence invalid, then copy as-is */
                        செல் copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                தொடர்;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        திரும்பு n;
}

size_t char16_strlen(மாறிலி char16_t *s) {
        size_t n = 0;

        ப்ரதிஜ்ஞா(s);

        போது (*s != 0)
                n++, s++;

        திரும்பு n;
}

size_t char16_strsize(மாறிலி char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        திரும்பு s ? (char16_strlen(s) + 1) * அளவு(*s) : 0;
}

/* expected size used to encode one unicode char */
நிலையான முழு utf8_unichar_to_encoded_len(char32_t unichar) {

        எனில் (unichar < 0x80)
                திரும்பு 1;
        எனில் (unichar < 0x800)
                திரும்பு 2;
        எனில் (unichar < 0x10000)
                திரும்பு 3;
        எனில் (unichar < 0x200000)
                திரும்பு 4;
        எனில் (unichar < 0x4000000)
                திரும்பு 5;

        திரும்பு 6;
}

/* validate one encoded unicode char and return its length */
முழு utf8_encoded_valid_unichar_full(மாறிலி எழுத்து *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        முழு r;

        ப்ரதிஜ்ஞா(str);
        ப்ரதிஜ்ஞா(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        எனில் (len == 0)
                திரும்பு -EINVAL;

        /* Do we have a truncated multi-byte character? */
        எனில் (len > length)
                திரும்பு -EINVAL;

        /* ascii is valid */
        எனில் (len == 1) {
                எனில் (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                திரும்பு 1;
        }

        /* check if expected encoded chars are available */
        க்கு (size_t i = 0; i < len; i++)
                எனில் ((str[i] & 0x80) != 0x80)
                        திரும்பு -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        எனில் (r < 0)
                திரும்பு r;

        /* check if encoded length matches encoded value */
        எனில் (utf8_unichar_to_encoded_len(unichar) != (முழு) len)
                திரும்பு -EINVAL;

        /* check if value has valid range */
        எனில் (!unichar_is_valid(unichar))
                திரும்பு -EINVAL;

        எனில் (ret_unichar)
                *ret_unichar = unichar;

        திரும்பு (முழு) len;
}

size_t utf8_n_codepoints(மாறிலி எழுத்து *str) {
        size_t n = 0;

        ப்ரதிஜ்ஞா(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        போது (*str != 0) {
                முழு k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                எனில் (k < 0)
                        திரும்பு SIZE_MAX;

                str += k;
                n++;
        }

        திரும்பு n;
}

size_t utf8_console_width(மாறிலி எழுத்து *str) {
        POINTER_MAY_BE_NULL(str);

        எனில் (isempty(str))
                திரும்பு 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        போது (*str) {
                முழு w;

                w = utf8_char_console_width(str);
                எனில் (w < 0)
                        திரும்பு SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        திரும்பு n;
}

size_t utf8_last_length(மாறிலி எழுத்து *s, size_t n) {
        முழு r;

        ப்ரதிஜ்ஞா(s);

        எனில் (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        க்கு (size_t last = 0;;) {
                எனில் (n == 0)
                        திரும்பு last;

                r = utf8_encoded_valid_unichar(s, n);
                எனில் (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
