/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

বর্ণ* utf8_is_valid_n(ধ্রুব বর্ণ *str, size_t len_bytes) _pure_;
স্থির অন্তঃস্থ বর্ণ* utf8_is_valid(ধ্রুব বর্ণ *str) {
        প্রত্যাগম utf8_is_valid_n(str, SIZE_MAX);
}

বর্ণ* ascii_is_valid_n(ধ্রুব বর্ণ *str, size_t len) _pure_;
স্থির অন্তঃস্থ বর্ণ* ascii_is_valid(ধ্রুব বর্ণ *str) {
        প্রত্যাগম ascii_is_valid_n(str, SIZE_MAX);
}

পূর্ণাঙ্ক utf8_to_ascii(ধ্রুব বর্ণ *str, বর্ণ replacement_char, বর্ণ **ret);

bool utf8_is_printable_newline(ধ্রুব বর্ণ* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

বর্ণ* utf8_escape_invalid(ধ্রুব বর্ণ *s);
বর্ণ* utf8_escape_non_printable_full(ধ্রুব বর্ণ *str, size_t console_width, bool force_ellipsis);
স্থির অন্তঃস্থ বর্ণ* utf8_escape_non_printable(ধ্রুব বর্ণ *str) {
        প্রত্যাগম utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(বর্ণ *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

বর্ণ* utf16_to_utf8(ধ্রুব char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(ধ্রুব বর্ণ *s, size_t length);

size_t char16_strlen(ধ্রুব char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ধ্রুব char16_t *s) _pure_;

পূর্ণাঙ্ক utf8_encoded_valid_unichar_full(ধ্রুব বর্ণ *str, size_t length, char32_t *ret_unichar);
স্থির অন্তঃস্থ পূর্ণাঙ্ক _pure_ utf8_encoded_valid_unichar(ধ্রুব বর্ণ *str, size_t length) {
        প্রত্যাগম utf8_encoded_valid_unichar_full(str, length, রিক্ত);
}
পূর্ণাঙ্ক utf8_encoded_to_unichar(ধ্রুব বর্ণ *str, char32_t *ret_unichar);

স্থির অন্তঃস্থ bool utf16_is_surrogate(char16_t c) {
        প্রত্যাগম c >= 0xd800U && c <= 0xdfffU;
}

স্থির অন্তঃস্থ bool utf16_is_trailing_surrogate(char16_t c) {
        প্রত্যাগম c >= 0xdc00U && c <= 0xdfffU;
}

স্থির অন্তঃস্থ char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        প্রত্যাগম ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ধ্রুব বর্ণ *str) _pure_;
পূর্ণাঙ্ক utf8_char_console_width(ধ্রুব বর্ণ *str) _pure_;
পূর্ণাঙ্ক unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(ধ্রুব বর্ণ *str) _pure_;

size_t utf8_last_length(ধ্রুব বর্ণ *s, size_t n) _pure_;
