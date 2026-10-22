/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

ವರ್ಣ* utf8_is_valid_n(ಧ್ರುವ ವರ್ಣ *str, size_t len_bytes) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* utf8_is_valid(ಧ್ರುವ ವರ್ಣ *str) {
        ಪ್ರತ್ಯಾಗಮ utf8_is_valid_n(str, SIZE_MAX);
}

ವರ್ಣ* ascii_is_valid_n(ಧ್ರುವ ವರ್ಣ *str, size_t len) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* ascii_is_valid(ಧ್ರುವ ವರ್ಣ *str) {
        ಪ್ರತ್ಯಾಗಮ ascii_is_valid_n(str, SIZE_MAX);
}

ಪೂರ್ಣಾಙ್ಕ utf8_to_ascii(ಧ್ರುವ ವರ್ಣ *str, ವರ್ಣ replacement_char, ವರ್ಣ **ret);

bool utf8_is_printable_newline(ಧ್ರುವ ವರ್ಣ* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

ವರ್ಣ* utf8_escape_invalid(ಧ್ರುವ ವರ್ಣ *s);
ವರ್ಣ* utf8_escape_non_printable_full(ಧ್ರುವ ವರ್ಣ *str, size_t console_width, bool force_ellipsis);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* utf8_escape_non_printable(ಧ್ರುವ ವರ್ಣ *str) {
        ಪ್ರತ್ಯಾಗಮ utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(ವರ್ಣ *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

ವರ್ಣ* utf16_to_utf8(ಧ್ರುವ char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(ಧ್ರುವ ವರ್ಣ *s, size_t length);

size_t char16_strlen(ಧ್ರುವ char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ಧ್ರುವ char16_t *s) _pure_;

ಪೂರ್ಣಾಙ್ಕ utf8_encoded_valid_unichar_full(ಧ್ರುವ ವರ್ಣ *str, size_t length, char32_t *ret_unichar);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ _pure_ utf8_encoded_valid_unichar(ಧ್ರುವ ವರ್ಣ *str, size_t length) {
        ಪ್ರತ್ಯಾಗಮ utf8_encoded_valid_unichar_full(str, length, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ utf8_encoded_to_unichar(ಧ್ರುವ ವರ್ಣ *str, char32_t *ret_unichar);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool utf16_is_surrogate(char16_t c) {
        ಪ್ರತ್ಯಾಗಮ c >= 0xd800U && c <= 0xdfffU;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool utf16_is_trailing_surrogate(char16_t c) {
        ಪ್ರತ್ಯಾಗಮ c >= 0xdc00U && c <= 0xdfffU;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        ಪ್ರತ್ಯಾಗಮ ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ಧ್ರುವ ವರ್ಣ *str) _pure_;
ಪೂರ್ಣಾಙ್ಕ utf8_char_console_width(ಧ್ರುವ ವರ್ಣ *str) _pure_;
ಪೂರ್ಣಾಙ್ಕ unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(ಧ್ರುವ ವರ್ಣ *str) _pure_;

size_t utf8_last_length(ಧ್ರುವ ವರ್ಣ *s, size_t n) _pure_;
