/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#परिभाषय UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#परिभाषय UTF8_BYTE_क्रमः_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

वर्ण* utf8_is_valid_n(ध्रुव वर्ण *str, size_t len_अष्टकानि) विशुद्धम्;
स्थिर अन्तःस्थ वर्ण* utf8_is_valid(ध्रुव वर्ण *str) {
        प्रत्यागम utf8_is_valid_n(str, SIZE_MAX);
}

वर्ण* ascii_is_valid_n(ध्रुव वर्ण *str, size_t len) विशुद्धम्;
स्थिर अन्तःस्थ वर्ण* ascii_is_valid(ध्रुव वर्ण *str) {
        प्रत्यागम ascii_is_valid_n(str, SIZE_MAX);
}

पूर्णाङ्क utf8_गन्तव्यम्_ascii(ध्रुव वर्ण *str, वर्ण replacement_char, वर्ण **ret);

bool utf8_is_printable_newline(ध्रुव वर्ण* str, size_t length, bool allow_newline) विशुद्धम्;
#परिभाषय utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

वर्ण* utf8_escape_invalid(ध्रुव वर्ण *s);
वर्ण* utf8_escape_non_printable_पूर्णम्(ध्रुव वर्ण *str, size_t console_विस्तारः, bool force_ellipsis);
स्थिर अन्तःस्थ वर्ण* utf8_escape_non_printable(ध्रुव वर्ण *str) {
        प्रत्यागम utf8_escape_non_printable_पूर्णम्(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(वर्ण *बहिर्गमनम्_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *बहिर्गमनम्, char32_t c);

वर्ण* utf16_गन्तव्यम्_utf8(ध्रुव char16_t *s, size_t length /* bytes! */);
char16_t *utf8_गन्तव्यम्_utf16(ध्रुव वर्ण *s, size_t length);

size_t char16_strlen(ध्रुव char16_t *s) विशुद्धम्; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ध्रुव char16_t *s) विशुद्धम्;

पूर्णाङ्क utf8_encoded_valid_unichar_पूर्णम्(ध्रुव वर्ण *str, size_t length, char32_t *ret_unichar);
स्थिर अन्तःस्थ पूर्णाङ्क विशुद्धम् utf8_encoded_valid_unichar(ध्रुव वर्ण *str, size_t length) {
        प्रत्यागम utf8_encoded_valid_unichar_पूर्णम्(str, length, रिक्त);
}
पूर्णाङ्क utf8_encoded_गन्तव्यम्_unichar(ध्रुव वर्ण *str, char32_t *ret_unichar);

स्थिर अन्तःस्थ bool utf16_is_surrogate(char16_t c) {
        प्रत्यागम c >= 0xd800U && c <= 0xdfffU;
}

स्थिर अन्तःस्थ bool utf16_is_trailing_surrogate(char16_t c) {
        प्रत्यागम c >= 0xdc00U && c <= 0xdfffU;
}

स्थिर अन्तःस्थ char32_t utf16_surrogate_pair_गन्तव्यम्_unichar(char16_t lead, char16_t trail) {
        प्रत्यागम ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ध्रुव वर्ण *str) विशुद्धम्;
पूर्णाङ्क utf8_char_console_विस्तारः(ध्रुव वर्ण *str) विशुद्धम्;
पूर्णाङ्क unichar_console_विस्तारः(char32_t c) विशुद्धम्;
size_t utf8_console_विस्तारः(ध्रुव वर्ण *str) विशुद्धम्;

size_t utf8_चरमः_दीर्घता(ध्रुव वर्ण *s, size_t n) विशुद्धम्;
