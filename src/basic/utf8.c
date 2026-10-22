/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "gunicode.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "unaligned.h"
#അന്തര്ഭാവ "utf8.h"

bool unichar_is_valid(char32_t ch) {

        എങ്കിൽ (ch >= 0x110000) /* End of unicode space */
                തിരികെ false;
        എങ്കിൽ ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                തിരികെ false;
        എങ്കിൽ ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                തിരികെ false;
        എങ്കിൽ ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                തിരികെ false;

        തിരികെ true;
}

സ്ഥിതം bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        തിരികെ (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
സ്ഥിതം size_t utf8_encoded_expected_len(uint8_t c) {
        എങ്കിൽ (c < 0x80)
                തിരികെ 1;
        എങ്കിൽ ((c & 0xe0) == 0xc0)
                തിരികെ 2;
        എങ്കിൽ ((c & 0xf0) == 0xe0)
                തിരികെ 3;
        എങ്കിൽ ((c & 0xf8) == 0xf0)
                തിരികെ 4;
        എങ്കിൽ ((c & 0xfc) == 0xf8)
                തിരികെ 5;
        എങ്കിൽ ((c & 0xfe) == 0xfc)
                തിരികെ 6;

        തിരികെ 0;
}

/* decode one unicode char */
പൂർണ്ണസംഖ്യ utf8_encoded_to_unichar(സ്ഥിരം അക്ഷരം *str, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;

        പ്രതിജ്ഞാ(str);
        പ്രതിജ്ഞാ(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        തിരഞ്ഞെടുപ്പ് (len) {
        അവസ്ഥ 1:
                *ret_unichar = (char32_t)str[0];
                തിരികെ 1;
        അവസ്ഥ 2:
                unichar = str[0] & 0x1f;
                നിർത്തുക;
        അവസ്ഥ 3:
                unichar = (char32_t)str[0] & 0x0f;
                നിർത്തുക;
        അവസ്ഥ 4:
                unichar = (char32_t)str[0] & 0x07;
                നിർത്തുക;
        അവസ്ഥ 5:
                unichar = (char32_t)str[0] & 0x03;
                നിർത്തുക;
        അവസ്ഥ 6:
                unichar = (char32_t)str[0] & 0x01;
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EINVAL;
        }

        വേണ്ടി (size_t i = 1; i < len; i++) {
                എങ്കിൽ (((char32_t)str[i] & 0xc0) != 0x80)
                        തിരികെ -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        തിരികെ len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
സ്ഥിതം bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        തിരികെ (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(സ്ഥിരം അക്ഷരം* str, size_t length, bool allow_newline) {
        പ്രതിജ്ഞാ(str);

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = str; length > 0;) {
                പൂർണ്ണസംഖ്യ encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                ആകുമ്പോൾ (length > 0) {
                        uint64_t w;
                        size_t n;

                        എങ്കിൽ (length < വലുപ്പം(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                memcpy(&w, p, n);
                        } അല്ലെങ്കിൽ {
                                n = വലുപ്പം(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        എങ്കിൽ (!is_printable_fastpath_u64(w))
                                നിർത്തുക;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                എങ്കിൽ (length == 0)
                        നിർത്തുക;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                എങ്കിൽ (encoded_len < 0)
                        തിരികെ false;
                പ്രതിജ്ഞാ(encoded_len > 0 && (size_t) encoded_len <= length);

                എങ്കിൽ (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        തിരികെ false;

                length -= encoded_len;
                p += encoded_len;
        }

        തിരികെ true;
}

അക്ഷരം* utf8_is_valid_n(സ്ഥിരം അക്ഷരം *str, size_t len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        പ്രതിജ്ഞാ(str);

        വേണ്ടി (size_t i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                പൂർണ്ണസംഖ്യ len;

                എങ്കിൽ (_unlikely_(str[i] == '\0'))
                        തിരികെ ശൂന്യ; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                എങ്കിൽ (_unlikely_(len < 0))
                        തിരികെ ശൂന്യ; /* invalid character */

                i += len;
        }

        തിരികെ (അക്ഷരം*) str;
}

അക്ഷരം* utf8_escape_invalid(സ്ഥിരം അക്ഷരം *str) {
        അക്ഷരം *p, *s;

        പ്രതിജ്ഞാ(str);

        p = s = malloc(strlen(str) * 4 + 1);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        ആകുമ്പോൾ (*str) {
                പൂർണ്ണസംഖ്യ len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                എങ്കിൽ (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } അല്ലെങ്കിൽ {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        തിരികെ str_realloc(p);
}

പൂർണ്ണസംഖ്യ utf8_char_console_width(സ്ഥിരം അക്ഷരം *str) {
        char32_t c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        തിരികെ unichar_console_width(c);
}

പൂർണ്ണസംഖ്യ unichar_console_width(char32_t c) {
        എങ്കിൽ (c == '\t')
                തിരികെ 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        തിരികെ unichar_iswide(c) ? 2 : 1;
}

അക്ഷരം* utf8_escape_non_printable_full(സ്ഥിരം അക്ഷരം *str, size_t console_width, bool force_ellipsis) {
        അക്ഷരം *p, *s, *prev_s;
        size_t n = 0; /* estimated print width */

        പ്രതിജ്ഞാ(str);

        എങ്കിൽ (console_width == 0)
                തിരികെ strdup("");

        size_t body = strlen(str) * 4;
        p = s = prev_s = malloc(body + STRLEN("…") + 1);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        വേണ്ടി (;;) {
                പൂർണ്ണസംഖ്യ len;
                അക്ഷരം *saved_s = s;

                എങ്കിൽ (!*str) { /* done! */
                        എങ്കിൽ (force_ellipsis)
                                പോകുക truncation;
                        അല്ലെങ്കിൽ
                                പോകുക finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                എങ്കിൽ (len > 0) {
                        എങ്കിൽ (utf8_is_printable(str, len)) {
                                പൂർണ്ണസംഖ്യ w;

                                w = utf8_char_console_width(str);
                                പ്രതിജ്ഞാ(w >= 0);
                                എങ്കിൽ (n + w > console_width)
                                        പോകുക truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } അല്ലെങ്കിൽ {
                                വേണ്ടി (; len > 0; len--) {
                                        എങ്കിൽ (n + 4 > console_width)
                                                പോകുക truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((പൂർണ്ണസംഖ്യ) *str >> 4);
                                        *(s++) = hexchar((പൂർണ്ണസംഖ്യ) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } അല്ലെങ്കിൽ {
                        എങ്കിൽ (n + 1 > console_width)
                                പോകുക truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, strlen(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                prev_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        എങ്കിൽ (n + 1 > console_width)
                s = prev_s;

        s = mempcpy(s, "…", strlen("…"));

 finish:
        *s = '\0';
        തിരികെ str_realloc(p);
}

അക്ഷരം* ascii_is_valid_n(സ്ഥിരം അക്ഷരം *str, size_t len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        പ്രതിജ്ഞാ(str);

        വേണ്ടി (size_t i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                എങ്കിൽ ((unsigned അക്ഷരം) str[i] >= 128 || str[i] == '\0')
                        തിരികെ ശൂന്യ;

        തിരികെ (അക്ഷരം*) str;
}

പൂർണ്ണസംഖ്യ utf8_to_ascii(സ്ഥിരം അക്ഷരം *str, അക്ഷരം replacement_char, അക്ഷരം **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        പ്രതിജ്ഞാ(str);
        പ്രതിജ്ഞാ(ret);

        _cleanup_free_ അക്ഷരം *ans = new(അക്ഷരം, strlen(str) + 1);
        എങ്കിൽ (!ans)
                തിരികെ -ENOMEM;

        അക്ഷരം *q = ans;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = str; *p; q++) {
                പൂർണ്ണസംഖ്യ l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                എങ്കിൽ (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        തിരികെ l;

                എങ്കിൽ (l == 1)
                        *q = *p;
                അല്ലെങ്കിൽ
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        തിരികെ 0;
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
size_t utf8_encode_unichar(അക്ഷരം *out_utf8, char32_t g) {

        എങ്കിൽ (g < (1 << 7)) {
                എങ്കിൽ (out_utf8)
                        out_utf8[0] = g & 0x7f;
                തിരികെ 1;
        } അല്ലെങ്കിൽ എങ്കിൽ (g < (1 << 11)) {
                എങ്കിൽ (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                തിരികെ 2;
        } അല്ലെങ്കിൽ എങ്കിൽ (g < (1 << 16)) {
                എങ്കിൽ (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                തിരികെ 3;
        } അല്ലെങ്കിൽ എങ്കിൽ (g < (1 << 21)) {
                എങ്കിൽ (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                തിരികെ 4;
        }

        തിരികെ 0;
}

അക്ഷരം* utf16_to_utf8(സ്ഥിരം char16_t *s, size_t length /* bytes! */) {
        സ്ഥിരം uint8_t *f;
        അക്ഷരം *r, *t;

        എങ്കിൽ (length == 0)
                തിരികെ new0(അക്ഷരം, 1);

        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (length == SIZE_MAX) {
                length = char16_strlen(s);

                എങ്കിൽ (length > SIZE_MAX/2)
                        തിരികെ ശൂന്യ; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        എങ്കിൽ (length > (SIZE_MAX - 1) / 2)
                തിരികെ ശൂന്യ; /* overflow */

        r = new(അക്ഷരം, length * 2 + 1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        f = (സ്ഥിരം uint8_t*) s;
        t = r;

        ആകുമ്പോൾ (f + 1 < (സ്ഥിരം uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                എങ്കിൽ (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        തുടരുക;
                }

                എങ്കിൽ (utf16_is_trailing_surrogate(w1))
                        തുടരുക; /* spurious trailing surrogate, ignore */

                എങ്കിൽ (f + 1 >= (സ്ഥിരം uint8_t*) s + length)
                        നിർത്തുക;

                w2 = f[1] << 8 | f[0];
                f += 2;

                എങ്കിൽ (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        തുടരുക; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        തിരികെ r;
}

size_t utf16_encode_unichar(char16_t *out, char32_t c) {
        പ്രതിജ്ഞാ(out);

        /* Note that this encodes as little-endian. */

        തിരഞ്ഞെടുപ്പ് (c) {

        അവസ്ഥ 0 ... 0xd7ffU:
        അവസ്ഥ 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                തിരികെ 1;

        അവസ്ഥ 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                തിരികെ 2;

        സ്ഥിരസ്ഥിതി: /* A surrogate (invalid) */
                തിരികെ 0;
        }
}

char16_t *utf8_to_utf16(സ്ഥിരം അക്ഷരം *s, size_t length) {
        char16_t *n, *p;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (length == 0)
                തിരികെ new0(char16_t, 1);

        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (length == SIZE_MAX)
                length = strlen(s);

        എങ്കിൽ (length > SIZE_MAX - 1)
                തിരികെ ശൂന്യ; /* overflow */

        n = new(char16_t, length + 1);
        എങ്കിൽ (!n)
                തിരികെ ശൂന്യ;

        p = n;

        വേണ്ടി (size_t i = 0; i < length;) {
                char32_t unichar;
                size_t e;

                e = utf8_encoded_expected_len(s[i]);
                എങ്കിൽ (e <= 1) /* Invalid and single byte characters are copied as they are */
                        പോകുക copy;

                എങ്കിൽ (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        പോകുക copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                എങ്കിൽ (r < 0) /* sequence invalid, then copy as-is */
                        പോകുക copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                തുടരുക;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        തിരികെ n;
}

size_t char16_strlen(സ്ഥിരം char16_t *s) {
        size_t n = 0;

        പ്രതിജ്ഞാ(s);

        ആകുമ്പോൾ (*s != 0)
                n++, s++;

        തിരികെ n;
}

size_t char16_strsize(സ്ഥിരം char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        തിരികെ s ? (char16_strlen(s) + 1) * വലുപ്പം(*s) : 0;
}

/* expected size used to encode one unicode char */
സ്ഥിതം പൂർണ്ണസംഖ്യ utf8_unichar_to_encoded_len(char32_t unichar) {

        എങ്കിൽ (unichar < 0x80)
                തിരികെ 1;
        എങ്കിൽ (unichar < 0x800)
                തിരികെ 2;
        എങ്കിൽ (unichar < 0x10000)
                തിരികെ 3;
        എങ്കിൽ (unichar < 0x200000)
                തിരികെ 4;
        എങ്കിൽ (unichar < 0x4000000)
                തിരികെ 5;

        തിരികെ 6;
}

/* validate one encoded unicode char and return its length */
പൂർണ്ണസംഖ്യ utf8_encoded_valid_unichar_full(സ്ഥിരം അക്ഷരം *str, size_t length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        size_t len;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(str);
        പ്രതിജ്ഞാ(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        എങ്കിൽ (len == 0)
                തിരികെ -EINVAL;

        /* Do we have a truncated multi-byte character? */
        എങ്കിൽ (len > length)
                തിരികെ -EINVAL;

        /* ascii is valid */
        എങ്കിൽ (len == 1) {
                എങ്കിൽ (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                തിരികെ 1;
        }

        /* check if expected encoded chars are available */
        വേണ്ടി (size_t i = 0; i < len; i++)
                എങ്കിൽ ((str[i] & 0x80) != 0x80)
                        തിരികെ -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* check if encoded length matches encoded value */
        എങ്കിൽ (utf8_unichar_to_encoded_len(unichar) != (പൂർണ്ണസംഖ്യ) len)
                തിരികെ -EINVAL;

        /* check if value has valid range */
        എങ്കിൽ (!unichar_is_valid(unichar))
                തിരികെ -EINVAL;

        എങ്കിൽ (ret_unichar)
                *ret_unichar = unichar;

        തിരികെ (പൂർണ്ണസംഖ്യ) len;
}

size_t utf8_n_codepoints(സ്ഥിരം അക്ഷരം *str) {
        size_t n = 0;

        പ്രതിജ്ഞാ(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        ആകുമ്പോൾ (*str != 0) {
                പൂർണ്ണസംഖ്യ k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                എങ്കിൽ (k < 0)
                        തിരികെ SIZE_MAX;

                str += k;
                n++;
        }

        തിരികെ n;
}

size_t utf8_console_width(സ്ഥിരം അക്ഷരം *str) {
        POINTER_MAY_BE_NULL(str);

        എങ്കിൽ (isempty(str))
                തിരികെ 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        size_t n = 0;
        ആകുമ്പോൾ (*str) {
                പൂർണ്ണസംഖ്യ w;

                w = utf8_char_console_width(str);
                എങ്കിൽ (w < 0)
                        തിരികെ SIZE_MAX;

                n += w;
                str = utf8_next_char(str);
        }

        തിരികെ n;
}

size_t utf8_last_length(സ്ഥിരം അക്ഷരം *s, size_t n) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (n == SIZE_MAX)
                n = strlen(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        വേണ്ടി (size_t last = 0;;) {
                എങ്കിൽ (n == 0)
                        തിരികെ last;

                r = utf8_encoded_valid_unichar(s, n);
                എങ്കിൽ (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
