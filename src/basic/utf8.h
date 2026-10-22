/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

അക്ഷരം* utf8_is_valid_n(സ്ഥിരം അക്ഷരം *str, size_t len_bytes) _pure_;
സ്ഥിതം വരിയിൽ അക്ഷരം* utf8_is_valid(സ്ഥിരം അക്ഷരം *str) {
        തിരികെ utf8_is_valid_n(str, SIZE_MAX);
}

അക്ഷരം* ascii_is_valid_n(സ്ഥിരം അക്ഷരം *str, size_t len) _pure_;
സ്ഥിതം വരിയിൽ അക്ഷരം* ascii_is_valid(സ്ഥിരം അക്ഷരം *str) {
        തിരികെ ascii_is_valid_n(str, SIZE_MAX);
}

പൂർണ്ണസംഖ്യ utf8_to_ascii(സ്ഥിരം അക്ഷരം *str, അക്ഷരം replacement_char, അക്ഷരം **ret);

bool utf8_is_printable_newline(സ്ഥിരം അക്ഷരം* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

അക്ഷരം* utf8_escape_invalid(സ്ഥിരം അക്ഷരം *s);
അക്ഷരം* utf8_escape_non_printable_full(സ്ഥിരം അക്ഷരം *str, size_t console_width, bool force_ellipsis);
സ്ഥിതം വരിയിൽ അക്ഷരം* utf8_escape_non_printable(സ്ഥിരം അക്ഷരം *str) {
        തിരികെ utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(അക്ഷരം *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

അക്ഷരം* utf16_to_utf8(സ്ഥിരം char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(സ്ഥിരം അക്ഷരം *s, size_t length);

size_t char16_strlen(സ്ഥിരം char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(സ്ഥിരം char16_t *s) _pure_;

പൂർണ്ണസംഖ്യ utf8_encoded_valid_unichar_full(സ്ഥിരം അക്ഷരം *str, size_t length, char32_t *ret_unichar);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ _pure_ utf8_encoded_valid_unichar(സ്ഥിരം അക്ഷരം *str, size_t length) {
        തിരികെ utf8_encoded_valid_unichar_full(str, length, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ utf8_encoded_to_unichar(സ്ഥിരം അക്ഷരം *str, char32_t *ret_unichar);

സ്ഥിതം വരിയിൽ bool utf16_is_surrogate(char16_t c) {
        തിരികെ c >= 0xd800U && c <= 0xdfffU;
}

സ്ഥിതം വരിയിൽ bool utf16_is_trailing_surrogate(char16_t c) {
        തിരികെ c >= 0xdc00U && c <= 0xdfffU;
}

സ്ഥിതം വരിയിൽ char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        തിരികെ ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(സ്ഥിരം അക്ഷരം *str) _pure_;
പൂർണ്ണസംഖ്യ utf8_char_console_width(സ്ഥിരം അക്ഷരം *str) _pure_;
പൂർണ്ണസംഖ്യ unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(സ്ഥിരം അക്ഷരം *str) _pure_;

size_t utf8_last_length(സ്ഥിരം അക്ഷരം *s, size_t n) _pure_;
