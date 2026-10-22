/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

वर्ण* utf8_is_valid_n(ध्रुव वर्ण *str, माप_प्रकार len_bytes) _pure_;
स्थिर अन्तःस्थ वर्ण* utf8_is_valid(ध्रुव वर्ण *str) {
        प्रत्यागम utf8_is_valid_n(str, SIZE_MAX);
}

वर्ण* ascii_is_valid_n(ध्रुव वर्ण *str, माप_प्रकार len) _pure_;
स्थिर अन्तःस्थ वर्ण* ascii_is_valid(ध्रुव वर्ण *str) {
        प्रत्यागम ascii_is_valid_n(str, SIZE_MAX);
}

पूर्णाङ्क utf8_to_ascii(ध्रुव वर्ण *str, वर्ण replacement_char, वर्ण **ret);

bool utf8_is_printable_newline(ध्रुव वर्ण* str, माप_प्रकार length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

वर्ण* utf8_escape_invalid(ध्रुव वर्ण *s);
वर्ण* utf8_escape_non_printable_full(ध्रुव वर्ण *str, माप_प्रकार console_width, bool force_ellipsis);
स्थिर अन्तःस्थ वर्ण* utf8_escape_non_printable(ध्रुव वर्ण *str) {
        प्रत्यागम utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

माप_प्रकार utf8_encode_unichar(वर्ण *out_utf8, char32_t g);
माप_प्रकार utf16_encode_unichar(char16_t *out, char32_t c);

वर्ण* utf16_to_utf8(ध्रुव char16_t *s, माप_प्रकार length /* bytes! */);
char16_t *utf8_to_utf16(ध्रुव वर्ण *s, माप_प्रकार length);

माप_प्रकार char16_strlen(ध्रुव char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
माप_प्रकार char16_strsize(ध्रुव char16_t *s) _pure_;

पूर्णाङ्क utf8_encoded_valid_unichar_full(ध्रुव वर्ण *str, माप_प्रकार length, char32_t *ret_unichar);
स्थिर अन्तःस्थ पूर्णाङ्क _pure_ utf8_encoded_valid_unichar(ध्रुव वर्ण *str, माप_प्रकार length) {
        प्रत्यागम utf8_encoded_valid_unichar_full(str, length, रिक्त);
}
पूर्णाङ्क utf8_encoded_to_unichar(ध्रुव वर्ण *str, char32_t *ret_unichar);

स्थिर अन्तःस्थ bool utf16_is_surrogate(char16_t c) {
        प्रत्यागम c >= 0xd800U && c <= 0xdfffU;
}

स्थिर अन्तःस्थ bool utf16_is_trailing_surrogate(char16_t c) {
        प्रत्यागम c >= 0xdc00U && c <= 0xdfffU;
}

स्थिर अन्तःस्थ char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        प्रत्यागम ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

माप_प्रकार utf8_n_codepoints(ध्रुव वर्ण *str) _pure_;
पूर्णाङ्क utf8_char_console_width(ध्रुव वर्ण *str) _pure_;
पूर्णाङ्क unichar_console_width(char32_t c) _pure_;
माप_प्रकार utf8_console_width(ध्रुव वर्ण *str) _pure_;

माप_प्रकार utf8_last_length(ध्रुव वर्ण *s, माप_प्रकार n) _pure_;
