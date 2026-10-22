/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

અક્ષર* utf8_is_valid_n(અચલ અક્ષર *str, size_t len_bytes) _pure_;
સ્થિર અંતઃરેખ અક્ષર* utf8_is_valid(અચલ અક્ષર *str) {
        પાછા utf8_is_valid_n(str, SIZE_MAX);
}

અક્ષર* ascii_is_valid_n(અચલ અક્ષર *str, size_t len) _pure_;
સ્થિર અંતઃરેખ અક્ષર* ascii_is_valid(અચલ અક્ષર *str) {
        પાછા ascii_is_valid_n(str, SIZE_MAX);
}

પૂર્ણાંક utf8_to_ascii(અચલ અક્ષર *str, અક્ષર replacement_char, અક્ષર **ret);

bool utf8_is_printable_newline(અચલ અક્ષર* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

અક્ષર* utf8_escape_invalid(અચલ અક્ષર *s);
અક્ષર* utf8_escape_non_printable_full(અચલ અક્ષર *str, size_t console_width, bool force_ellipsis);
સ્થિર અંતઃરેખ અક્ષર* utf8_escape_non_printable(અચલ અક્ષર *str) {
        પાછા utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(અક્ષર *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

અક્ષર* utf16_to_utf8(અચલ char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(અચલ અક્ષર *s, size_t length);

size_t char16_strlen(અચલ char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(અચલ char16_t *s) _pure_;

પૂર્ણાંક utf8_encoded_valid_unichar_full(અચલ અક્ષર *str, size_t length, char32_t *ret_unichar);
સ્થિર અંતઃરેખ પૂર્ણાંક _pure_ utf8_encoded_valid_unichar(અચલ અક્ષર *str, size_t length) {
        પાછા utf8_encoded_valid_unichar_full(str, length, ખાલી);
}
પૂર્ણાંક utf8_encoded_to_unichar(અચલ અક્ષર *str, char32_t *ret_unichar);

સ્થિર અંતઃરેખ bool utf16_is_surrogate(char16_t c) {
        પાછા c >= 0xd800U && c <= 0xdfffU;
}

સ્થિર અંતઃરેખ bool utf16_is_trailing_surrogate(char16_t c) {
        પાછા c >= 0xdc00U && c <= 0xdfffU;
}

સ્થિર અંતઃરેખ char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        પાછા ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(અચલ અક્ષર *str) _pure_;
પૂર્ણાંક utf8_char_console_width(અચલ અક્ષર *str) _pure_;
પૂર્ણાંક unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(અચલ અક્ષર *str) _pure_;

size_t utf8_last_length(અચલ અક્ષર *s, size_t n) _pure_;
