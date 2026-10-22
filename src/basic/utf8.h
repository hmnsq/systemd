/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#घोषणा UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

अक्षर* utf8_is_valid_n(स्थिर अक्षर *str, माप_प्रकार len_bytes) _pure_;
अटल अंतरभूत अक्षर* utf8_is_valid(स्थिर अक्षर *str) {
        वापस utf8_is_valid_n(str, SIZE_MAX);
}

अक्षर* ascii_is_valid_n(स्थिर अक्षर *str, माप_प्रकार len) _pure_;
अटल अंतरभूत अक्षर* ascii_is_valid(स्थिर अक्षर *str) {
        वापस ascii_is_valid_n(str, SIZE_MAX);
}

पूर्णांक utf8_to_ascii(स्थिर अक्षर *str, अक्षर replacement_char, अक्षर **ret);

bool utf8_is_printable_newline(स्थिर अक्षर* str, माप_प्रकार length, bool allow_newline) _pure_;
#घोषणा utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

अक्षर* utf8_escape_invalid(स्थिर अक्षर *s);
अक्षर* utf8_escape_non_printable_full(स्थिर अक्षर *str, माप_प्रकार console_width, bool force_ellipsis);
अटल अंतरभूत अक्षर* utf8_escape_non_printable(स्थिर अक्षर *str) {
        वापस utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

माप_प्रकार utf8_encode_unichar(अक्षर *out_utf8, char32_t g);
माप_प्रकार utf16_encode_unichar(char16_t *out, char32_t c);

अक्षर* utf16_to_utf8(स्थिर char16_t *s, माप_प्रकार length /* bytes! */);
char16_t *utf8_to_utf16(स्थिर अक्षर *s, माप_प्रकार length);

माप_प्रकार char16_strlen(स्थिर char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
माप_प्रकार char16_strsize(स्थिर char16_t *s) _pure_;

पूर्णांक utf8_encoded_valid_unichar_full(स्थिर अक्षर *str, माप_प्रकार length, char32_t *ret_unichar);
अटल अंतरभूत पूर्णांक _pure_ utf8_encoded_valid_unichar(स्थिर अक्षर *str, माप_प्रकार length) {
        वापस utf8_encoded_valid_unichar_full(str, length, शून्य);
}
पूर्णांक utf8_encoded_to_unichar(स्थिर अक्षर *str, char32_t *ret_unichar);

अटल अंतरभूत bool utf16_is_surrogate(char16_t c) {
        वापस c >= 0xd800U && c <= 0xdfffU;
}

अटल अंतरभूत bool utf16_is_trailing_surrogate(char16_t c) {
        वापस c >= 0xdc00U && c <= 0xdfffU;
}

अटल अंतरभूत char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        वापस ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

माप_प्रकार utf8_n_codepoints(स्थिर अक्षर *str) _pure_;
पूर्णांक utf8_char_console_width(स्थिर अक्षर *str) _pure_;
पूर्णांक unichar_console_width(char32_t c) _pure_;
माप_प्रकार utf8_console_width(स्थिर अक्षर *str) _pure_;

माप_प्रकार utf8_last_लंबाई(स्थिर अक्षर *s, माप_प्रकार n) _pure_;
