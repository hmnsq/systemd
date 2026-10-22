/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

حرف* utf8_is_valid_n(ثابت حرف *str, size_t len_bytes) _pure_;
ساكن مضمن حرف* utf8_is_valid(ثابت حرف *str) {
        إرجاع utf8_is_valid_n(str, SIZE_MAX);
}

حرف* ascii_is_valid_n(ثابت حرف *str, size_t len) _pure_;
ساكن مضمن حرف* ascii_is_valid(ثابت حرف *str) {
        إرجاع ascii_is_valid_n(str, SIZE_MAX);
}

صحيح utf8_to_ascii(ثابت حرف *str, حرف replacement_char, حرف **ret);

bool utf8_is_printable_newline(ثابت حرف* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

حرف* utf8_escape_invalid(ثابت حرف *s);
حرف* utf8_escape_non_printable_full(ثابت حرف *str, size_t console_width, bool force_ellipsis);
ساكن مضمن حرف* utf8_escape_non_printable(ثابت حرف *str) {
        إرجاع utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(حرف *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

حرف* utf16_to_utf8(ثابت char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(ثابت حرف *s, size_t length);

size_t char16_strlen(ثابت char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ثابت char16_t *s) _pure_;

صحيح utf8_encoded_valid_unichar_full(ثابت حرف *str, size_t length, char32_t *ret_unichar);
ساكن مضمن صحيح _pure_ utf8_encoded_valid_unichar(ثابت حرف *str, size_t length) {
        إرجاع utf8_encoded_valid_unichar_full(str, length, عدم);
}
صحيح utf8_encoded_to_unichar(ثابت حرف *str, char32_t *ret_unichar);

ساكن مضمن bool utf16_is_surrogate(char16_t c) {
        إرجاع c >= 0xd800U && c <= 0xdfffU;
}

ساكن مضمن bool utf16_is_trailing_surrogate(char16_t c) {
        إرجاع c >= 0xdc00U && c <= 0xdfffU;
}

ساكن مضمن char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        إرجاع ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ثابت حرف *str) _pure_;
صحيح utf8_char_console_width(ثابت حرف *str) _pure_;
صحيح unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(ثابت حرف *str) _pure_;

size_t utf8_last_length(ثابت حرف *s, size_t n) _pure_;
