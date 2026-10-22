/* SPDX-License-Identifier: LGPL-2.0-or-later */

/* Parts of this file are based on the GLIB utf8 validation functions. The original copyright follows.
 *
 * gutf8.c - Operations on UTF-8 strings.
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 */

#समावेश "alloc-util.h"
#समावेश "gunicode.h"
#समावेश "hexdecoct.h"
#समावेश "string-util.h"
#समावेश "unaligned.h"
#समावेश "utf8.h"

bool unichar_is_valid(char32_t ch) {

        अगर (ch >= 0x110000) /* End of unicode space */
                वापस false;
        अगर ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                वापस false;
        अगर ((ch >= 0xFDD0) && (ch <= 0xFDEF)) /* Reserved */
                वापस false;
        अगर ((ch & 0xFFFE) == 0xFFFE) /* BOM (Byte Order Mark) */
                वापस false;

        वापस true;
}

अटल bool unichar_is_control(char32_t ch) {

        /*
          0 to ' '-1 is the C0 range.
          DEL=0x7F, and DEL+1 to 0x9F is C1 range.
          '\t' is in C0 range, but more or less harmless and commonly used.
        */

        वापस (ch < ' ' && !IN_SET(ch, '\t', '\n')) ||
                (0x7F <= ch && ch <= 0x9F);
}

/* count of characters used to encode one unicode char */
अटल माप_प्रकार utf8_encoded_expected_len(uint8_t c) {
        अगर (c < 0x80)
                वापस 1;
        अगर ((c & 0xe0) == 0xc0)
                वापस 2;
        अगर ((c & 0xf0) == 0xe0)
                वापस 3;
        अगर ((c & 0xf8) == 0xf0)
                वापस 4;
        अगर ((c & 0xfc) == 0xf8)
                वापस 5;
        अगर ((c & 0xfe) == 0xfc)
                वापस 6;

        वापस 0;
}

/* decode one unicode char */
पूर्णांक utf8_encoded_to_unichar(स्थिर अक्षर *str, char32_t *ret_unichar) {
        char32_t unichar;
        माप_प्रकार len;

        निश्चित(str);
        निश्चित(ret_unichar);

        len = utf8_encoded_expected_len(str[0]);

        चयन (len) {
        हाल 1:
                *ret_unichar = (char32_t)str[0];
                वापस 1;
        हाल 2:
                unichar = str[0] & 0x1f;
                अवरोध;
        हाल 3:
                unichar = (char32_t)str[0] & 0x0f;
                अवरोध;
        हाल 4:
                unichar = (char32_t)str[0] & 0x07;
                अवरोध;
        हाल 5:
                unichar = (char32_t)str[0] & 0x03;
                अवरोध;
        हाल 6:
                unichar = (char32_t)str[0] & 0x01;
                अवरोध;
        शेष:
                वापस -EINVAL;
        }

        क्रम (माप_प्रकार i = 1; i < len; i++) {
                अगर (((char32_t)str[i] & 0xc0) != 0x80)
                        वापस -EINVAL;

                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;
        वापस len;
}

/* Tests whether all eight bytes of a word are plain printable ASCII, i.e. within 0x20…0x7e. The three
 * terms below cover the three ways a byte can fail. A byte that fails is always flagged in its own
 * lane, but the converse does not hold: the two subtractions are whole-word ones, so a borrow out of
 * a failing lane also flags bytes above it. Only the aggregate test is meaningful, the mask is not a
 * per-byte map of which bytes failed. */
अटल bool is_printable_fastpath_u64(uint64_t w) {
        uint64_t x = w ^ UINT64_C(0x7f7f7f7f7f7f7f7f);

        वापस (((w & UINT64_C(0x8080808080808080)) |                                  /* >= 0x80 */
                 ((w - UINT64_C(0x2020202020202020)) & ~w) |                           /* < 0x20 */
                 ((x - UINT64_C(0x0101010101010101)) & ~x)) &                          /* == 0x7f */
                UINT64_C(0x8080808080808080)) == 0;
}

bool utf8_is_printable_newline(स्थिर अक्षर* str, माप_प्रकार length, bool allow_newline) {
        निश्चित(str);

        क्रम (स्थिर अक्षर *p = str; length > 0;) {
                पूर्णांक encoded_len;
                char32_t val;

                /* Bulk fast path: almost everything we are asked about is plain printable ASCII, so
                 * test eight bytes of it at once. Anything else falls through to the per-character
                 * path below, which handles it as before. A remainder of less than eight bytes is
                 * copied into a word padded with spaces, so that it can be tested the same way. */
                जबतक (length > 0) {
                        uint64_t w;
                        माप_प्रकार n;

                        अगर (length < माप(uint64_t)) {
                                n = length;
                                w = UINT64_C(0x2020202020202020);
                                स_नकल(&w, p, n);
                        } अन्यथा {
                                n = माप(uint64_t);
                                w = unaligned_read_ne64(p);
                        }

                        अगर (!is_printable_fastpath_u64(w))
                                अवरोध;

                        length -= n;
                        p += n;
                }

                /* Note that this is not redundant with the loop condition above: the fast path may
                 * have consumed the remainder of the string, and then there's nothing left to decode. */
                अगर (length == 0)
                        अवरोध;

                encoded_len = utf8_encoded_valid_unichar_full(p, length, &val);
                अगर (encoded_len < 0)
                        वापस false;
                निश्चित(encoded_len > 0 && (माप_प्रकार) encoded_len <= length);

                अगर (unichar_is_control(val) || (!allow_newline && val == '\n'))
                        वापस false;

                length -= encoded_len;
                p += encoded_len;
        }

        वापस true;
}

अक्षर* utf8_is_valid_n(स्थिर अक्षर *str, माप_प्रकार len_bytes) {
        /* Check if the string is composed of valid utf8 characters. If length len_bytes is given, stop after
         * len_bytes. Otherwise, stop at NUL. */

        निश्चित(str);

        क्रम (माप_प्रकार i = 0; len_bytes != SIZE_MAX ? i < len_bytes : str[i] != '\0'; ) {
                पूर्णांक len;

                अगर (_unlikely_(str[i] == '\0'))
                        वापस शून्य; /* embedded NUL */

                len = utf8_encoded_valid_unichar(str + i,
                                                 len_bytes != SIZE_MAX ? len_bytes - i : SIZE_MAX);
                अगर (_unlikely_(len < 0))
                        वापस शून्य; /* invalid character */

                i += len;
        }

        वापस (अक्षर*) str;
}

अक्षर* utf8_escape_invalid(स्थिर अक्षर *str) {
        अक्षर *p, *s;

        निश्चित(str);

        p = s = दो_स्मृति(म_माप(str) * 4 + 1);
        अगर (!p)
                वापस शून्य;

        जबतक (*str) {
                पूर्णांक len;

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                अगर (len > 0) {
                        s = mempcpy(s, str, len);
                        str += len;
                } अन्यथा {
                        s = stpcpy(s, UTF8_REPLACEMENT_CHARACTER);
                        str += 1;
                }
        }

        *s = '\0';
        वापस str_realloc(p);
}

पूर्णांक utf8_char_console_width(स्थिर अक्षर *str) {
        char32_t c;
        पूर्णांक r;

        निश्चित(str);

        /* Validate the same way utf8_is_valid() does. utf8_encoded_to_unichar() on its own accepts
         * overlong encodings, surrogates, noncharacters and the obsolete 5 and 6 byte forms, which would
         * make our idea of how wide a string is disagree with our idea of whether it is valid UTF-8 in
         * the first place. */
        r = utf8_encoded_valid_unichar(str, SIZE_MAX);
        अगर (r < 0)
                वापस r;

        assert_se(utf8_encoded_to_unichar(str, &c) == r);

        वापस unichar_console_width(c);
}

पूर्णांक unichar_console_width(char32_t c) {
        अगर (c == '\t')
                वापस 8; /* Assume a tab width of 8 */

        /* TODO: we should detect combining characters */

        वापस unichar_iswide(c) ? 2 : 1;
}

अक्षर* utf8_escape_non_printable_full(स्थिर अक्षर *str, माप_प्रकार console_width, bool force_ellipsis) {
        अक्षर *p, *s, *पूर्व_s;
        माप_प्रकार n = 0; /* estimated print width */

        निश्चित(str);

        अगर (console_width == 0)
                वापस strdup("");

        माप_प्रकार body = म_माप(str) * 4;
        p = s = पूर्व_s = दो_स्मृति(body + STRLEN("…") + 1);
        अगर (!p)
                वापस शून्य;

        क्रम (;;) {
                पूर्णांक len;
                अक्षर *saved_s = s;

                अगर (!*str) { /* done! */
                        अगर (force_ellipsis)
                                जाओ truncation;
                        अन्यथा
                                जाओ finish;
                }

                len = utf8_encoded_valid_unichar(str, SIZE_MAX);
                अगर (len > 0) {
                        अगर (utf8_is_printable(str, len)) {
                                पूर्णांक w;

                                w = utf8_char_console_width(str);
                                निश्चित(w >= 0);
                                अगर (n + w > console_width)
                                        जाओ truncation;

                                s = mempcpy(s, str, len);
                                str += len;
                                n += w;

                        } अन्यथा {
                                क्रम (; len > 0; len--) {
                                        अगर (n + 4 > console_width)
                                                जाओ truncation;

                                        *(s++) = '\\';
                                        *(s++) = 'x';
                                        *(s++) = hexchar((पूर्णांक) *str >> 4);
                                        *(s++) = hexchar((पूर्णांक) *str);

                                        str += 1;
                                        n += 4;
                                }
                        }
                } अन्यथा {
                        अगर (n + 1 > console_width)
                                जाओ truncation;

                        s = mempcpy(s, UTF8_REPLACEMENT_CHARACTER, म_माप(UTF8_REPLACEMENT_CHARACTER));
                        str += 1;
                        n += 1;
                }

                पूर्व_s = saved_s;
        }

 truncation:
        /* Try to go back one if we don't have enough space for the ellipsis */
        अगर (n + 1 > console_width)
                s = पूर्व_s;

        s = mempcpy(s, "…", म_माप("…"));

 finish:
        *s = '\0';
        वापस str_realloc(p);
}

अक्षर* ascii_is_valid_n(स्थिर अक्षर *str, माप_प्रकार len) {
        /* Check whether the string consists of valid ASCII bytes, i.e values between 1 and 127, inclusive.
         * Stops at len, or NUL byte if len is SIZE_MAX. */

        निश्चित(str);

        क्रम (माप_प्रकार i = 0; len != SIZE_MAX ? i < len : str[i] != '\0'; i++)
                अगर ((अचिन्हित अक्षर) str[i] >= 128 || str[i] == '\0')
                        वापस शून्य;

        वापस (अक्षर*) str;
}

पूर्णांक utf8_to_ascii(स्थिर अक्षर *str, अक्षर replacement_char, अक्षर **ret) {
        /* Convert to a string that has only ASCII chars, replacing anything that is not ASCII
         * by replacement_char. */

        निश्चित(str);
        निश्चित(ret);

        _cleanup_free_ अक्षर *ans = new(अक्षर, म_माप(str) + 1);
        अगर (!ans)
                वापस -ENOMEM;

        अक्षर *q = ans;

        क्रम (स्थिर अक्षर *p = str; *p; q++) {
                पूर्णांक l;

                l = utf8_encoded_valid_unichar(p, SIZE_MAX);
                अगर (l < 0)  /* Non-UTF-8, let's not even try to propagate the garbage */
                        वापस l;

                अगर (l == 1)
                        *q = *p;
                अन्यथा
                        /* non-ASCII, we need to replace it */
                        *q = replacement_char;

                p += l;
        }
        *q = '\0';

        *ret = TAKE_PTR(ans);
        वापस 0;
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
माप_प्रकार utf8_encode_unichar(अक्षर *out_utf8, char32_t g) {

        अगर (g < (1 << 7)) {
                अगर (out_utf8)
                        out_utf8[0] = g & 0x7f;
                वापस 1;
        } अन्यथा अगर (g < (1 << 11)) {
                अगर (out_utf8) {
                        out_utf8[0] = 0xc0 | ((g >> 6) & 0x1f);
                        out_utf8[1] = 0x80 | (g & 0x3f);
                }
                वापस 2;
        } अन्यथा अगर (g < (1 << 16)) {
                अगर (out_utf8) {
                        out_utf8[0] = 0xe0 | ((g >> 12) & 0x0f);
                        out_utf8[1] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[2] = 0x80 | (g & 0x3f);
                }
                वापस 3;
        } अन्यथा अगर (g < (1 << 21)) {
                अगर (out_utf8) {
                        out_utf8[0] = 0xf0 | ((g >> 18) & 0x07);
                        out_utf8[1] = 0x80 | ((g >> 12) & 0x3f);
                        out_utf8[2] = 0x80 | ((g >> 6) & 0x3f);
                        out_utf8[3] = 0x80 | (g & 0x3f);
                }
                वापस 4;
        }

        वापस 0;
}

अक्षर* utf16_to_utf8(स्थिर char16_t *s, माप_प्रकार length /* bytes! */) {
        स्थिर uint8_t *f;
        अक्षर *r, *t;

        अगर (length == 0)
                वापस new0(अक्षर, 1);

        निश्चित(s);

        अगर (length == SIZE_MAX) {
                length = char16_strlen(s);

                अगर (length > SIZE_MAX/2)
                        वापस शून्य; /* overflow */

                length *= 2;
        }

        /* Input length is in bytes, i.e. the shortest possible character takes 2 bytes. Each unicode character may
         * take up to 4 bytes in UTF-8. Let's also account for a trailing NUL byte. */
        अगर (length > (SIZE_MAX - 1) / 2)
                वापस शून्य; /* overflow */

        r = new(अक्षर, length * 2 + 1);
        अगर (!r)
                वापस शून्य;

        f = (स्थिर uint8_t*) s;
        t = r;

        जबतक (f + 1 < (स्थिर uint8_t*) s + length) {
                char16_t w1, w2;

                /* see RFC 2781 section 2.2 */

                w1 = f[1] << 8 | f[0];
                f += 2;

                अगर (!utf16_is_surrogate(w1)) {
                        t += utf8_encode_unichar(t, w1);
                        जारी;
                }

                अगर (utf16_is_trailing_surrogate(w1))
                        जारी; /* spurious trailing surrogate, ignore */

                अगर (f + 1 >= (स्थिर uint8_t*) s + length)
                        अवरोध;

                w2 = f[1] << 8 | f[0];
                f += 2;

                अगर (!utf16_is_trailing_surrogate(w2)) {
                        f -= 2;
                        जारी; /* surrogate missing its trailing surrogate, ignore */
                }

                t += utf8_encode_unichar(t, utf16_surrogate_pair_to_unichar(w1, w2));
        }

        *t = 0;
        वापस r;
}

माप_प्रकार utf16_encode_unichar(char16_t *out, char32_t c) {
        निश्चित(out);

        /* Note that this encodes as little-endian. */

        चयन (c) {

        हाल 0 ... 0xd7ffU:
        हाल 0xe000U ... 0xffffU:
                out[0] = htole16(c);
                वापस 1;

        हाल 0x10000U ... 0x10ffffU:
                c -= 0x10000U;
                out[0] = htole16((c >> 10) + 0xd800U);
                out[1] = htole16((c & 0x3ffU) + 0xdc00U);
                वापस 2;

        शेष: /* A surrogate (invalid) */
                वापस 0;
        }
}

char16_t *utf8_to_utf16(स्थिर अक्षर *s, माप_प्रकार length) {
        char16_t *n, *p;
        पूर्णांक r;

        अगर (length == 0)
                वापस new0(char16_t, 1);

        निश्चित(s);

        अगर (length == SIZE_MAX)
                length = म_माप(s);

        अगर (length > SIZE_MAX - 1)
                वापस शून्य; /* overflow */

        n = new(char16_t, length + 1);
        अगर (!n)
                वापस शून्य;

        p = n;

        क्रम (माप_प्रकार i = 0; i < length;) {
                char32_t unichar;
                माप_प्रकार e;

                e = utf8_encoded_expected_len(s[i]);
                अगर (e <= 1) /* Invalid and single byte characters are copied as they are */
                        जाओ copy;

                अगर (i + e > length) /* sequence longer than input buffer, then copy as-is */
                        जाओ copy;

                r = utf8_encoded_to_unichar(s + i, &unichar);
                अगर (r < 0) /* sequence invalid, then copy as-is */
                        जाओ copy;

                p += utf16_encode_unichar(p, unichar);
                i += e;
                जारी;

        copy:
                *(p++) = htole16(s[i++]);
        }

        *p = 0;
        वापस n;
}

माप_प्रकार char16_strlen(स्थिर char16_t *s) {
        माप_प्रकार n = 0;

        निश्चित(s);

        जबतक (*s != 0)
                n++, s++;

        वापस n;
}

माप_प्रकार char16_strsize(स्थिर char16_t *s) {
        POINTER_MAY_BE_NULL(s);

        वापस s ? (char16_strlen(s) + 1) * माप(*s) : 0;
}

/* expected size used to encode one unicode char */
अटल पूर्णांक utf8_unichar_to_encoded_len(char32_t unichar) {

        अगर (unichar < 0x80)
                वापस 1;
        अगर (unichar < 0x800)
                वापस 2;
        अगर (unichar < 0x10000)
                वापस 3;
        अगर (unichar < 0x200000)
                वापस 4;
        अगर (unichar < 0x4000000)
                वापस 5;

        वापस 6;
}

/* validate one encoded unicode char and return its length */
पूर्णांक utf8_encoded_valid_unichar_full(स्थिर अक्षर *str, माप_प्रकार length /* bytes */, char32_t *ret_unichar) {
        char32_t unichar;
        माप_प्रकार len;
        पूर्णांक r;

        निश्चित(str);
        निश्चित(length > 0);

        /* We read until NUL, at most length bytes. SIZE_MAX may be used to disable the length check.
         * If ret_unichar is non-NULL the decoded character is returned in it, so that callers
         * interested in both validity and value needn't decode the character a second time. */

        len = utf8_encoded_expected_len(str[0]);
        अगर (len == 0)
                वापस -EINVAL;

        /* Do we have a truncated multi-byte character? */
        अगर (len > length)
                वापस -EINVAL;

        /* ascii is valid */
        अगर (len == 1) {
                अगर (ret_unichar)
                        *ret_unichar = (char32_t) (uint8_t) str[0];
                वापस 1;
        }

        /* check if expected encoded chars are available */
        क्रम (माप_प्रकार i = 0; i < len; i++)
                अगर ((str[i] & 0x80) != 0x80)
                        वापस -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        अगर (r < 0)
                वापस r;

        /* check if encoded length matches encoded value */
        अगर (utf8_unichar_to_encoded_len(unichar) != (पूर्णांक) len)
                वापस -EINVAL;

        /* check if value has valid range */
        अगर (!unichar_is_valid(unichar))
                वापस -EINVAL;

        अगर (ret_unichar)
                *ret_unichar = unichar;

        वापस (पूर्णांक) len;
}

माप_प्रकार utf8_n_codepoints(स्थिर अक्षर *str) {
        माप_प्रकार n = 0;

        निश्चित(str);

        /* Returns the number of UTF-8 codepoints in this string, or SIZE_MAX if the string is not valid UTF-8. */

        जबतक (*str != 0) {
                पूर्णांक k;

                k = utf8_encoded_valid_unichar(str, SIZE_MAX);
                अगर (k < 0)
                        वापस SIZE_MAX;

                str += k;
                n++;
        }

        वापस n;
}

माप_प्रकार utf8_console_width(स्थिर अक्षर *str) {
        POINTER_MAY_BE_NULL(str);

        अगर (isempty(str))
                वापस 0;

        /* Returns the approximate width a string will take on screen when printed on a character cell
         * terminal/console. */

        माप_प्रकार n = 0;
        जबतक (*str) {
                पूर्णांक w;

                w = utf8_char_console_width(str);
                अगर (w < 0)
                        वापस SIZE_MAX;

                n += w;
                str = utf8_अग्र_char(str);
        }

        वापस n;
}

माप_प्रकार utf8_last_लंबाई(स्थिर अक्षर *s, माप_प्रकार n) {
        पूर्णांक r;

        निश्चित(s);

        अगर (n == SIZE_MAX)
                n = म_माप(s);

        /* Determines length in bytes of last UTF-8 codepoint in string. If the string is empty, returns
         * zero. Treats invalid UTF-8 codepoints as 1 sized ones. */

        क्रम (माप_प्रकार last = 0;;) {
                अगर (n == 0)
                        वापस last;

                r = utf8_encoded_valid_unichar(s, n);
                अगर (r <= 0)
                        r = 1; /* treat invalid UTF-8 as byte-wide */

                s += r;
                n -= r;
                last = r;
        }
}
