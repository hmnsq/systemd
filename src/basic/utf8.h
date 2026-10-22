/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

অক্ষর* utf8_is_valid_n(ধ্রুবক অক্ষর *str, size_t len_bytes) _pure_;
স্থির অন্তঃরেখ অক্ষর* utf8_is_valid(ধ্রুবক অক্ষর *str) {
        ফেরত utf8_is_valid_n(str, SIZE_MAX);
}

অক্ষর* ascii_is_valid_n(ধ্রুবক অক্ষর *str, size_t len) _pure_;
স্থির অন্তঃরেখ অক্ষর* ascii_is_valid(ধ্রুবক অক্ষর *str) {
        ফেরত ascii_is_valid_n(str, SIZE_MAX);
}

পূর্ণসংখ্যা utf8_to_ascii(ধ্রুবক অক্ষর *str, অক্ষর replacement_char, অক্ষর **ret);

bool utf8_is_printable_newline(ধ্রুবক অক্ষর* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

অক্ষর* utf8_escape_invalid(ধ্রুবক অক্ষর *s);
অক্ষর* utf8_escape_non_printable_full(ধ্রুবক অক্ষর *str, size_t console_width, bool force_ellipsis);
স্থির অন্তঃরেখ অক্ষর* utf8_escape_non_printable(ধ্রুবক অক্ষর *str) {
        ফেরত utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(অক্ষর *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

অক্ষর* utf16_to_utf8(ধ্রুবক char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(ধ্রুবক অক্ষর *s, size_t length);

size_t char16_strlen(ধ্রুবক char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ধ্রুবক char16_t *s) _pure_;

পূর্ণসংখ্যা utf8_encoded_valid_unichar_full(ধ্রুবক অক্ষর *str, size_t length, char32_t *ret_unichar);
স্থির অন্তঃরেখ পূর্ণসংখ্যা _pure_ utf8_encoded_valid_unichar(ধ্রুবক অক্ষর *str, size_t length) {
        ফেরত utf8_encoded_valid_unichar_full(str, length, ফাঁকা);
}
পূর্ণসংখ্যা utf8_encoded_to_unichar(ধ্রুবক অক্ষর *str, char32_t *ret_unichar);

স্থির অন্তঃরেখ bool utf16_is_surrogate(char16_t c) {
        ফেরত c >= 0xd800U && c <= 0xdfffU;
}

স্থির অন্তঃরেখ bool utf16_is_trailing_surrogate(char16_t c) {
        ফেরত c >= 0xdc00U && c <= 0xdfffU;
}

স্থির অন্তঃরেখ char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        ফেরত ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ধ্রুবক অক্ষর *str) _pure_;
পূর্ণসংখ্যা utf8_char_console_width(ধ্রুবক অক্ষর *str) _pure_;
পূর্ণসংখ্যা unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(ধ্রুবক অক্ষর *str) _pure_;

size_t utf8_last_দৈর্ঘ্য(ধ্রুবক অক্ষর *s, size_t n) _pure_;
