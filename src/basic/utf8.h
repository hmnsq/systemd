/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

var_na* utf8_is_valid_n(const var_na *str, size_t len_bytes) _pure_;
static anta_hstha var_na* utf8_is_valid(const var_na *str) {
        คืน utf8_is_valid_n(str, SIZE_MAX);
}

var_na* ascii_is_valid_n(const var_na *str, size_t len) _pure_;
static anta_hstha var_na* ascii_is_valid(const var_na *str) {
        คืน ascii_is_valid_n(str, SIZE_MAX);
}

จำนวนเต็ม utf8_to_ascii(const var_na *str, var_na replacement_char, var_na **ret);

bool utf8_is_printable_newline(const var_na* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

var_na* utf8_escape_invalid(const var_na *s);
var_na* utf8_escape_non_printable_full(const var_na *str, size_t console_width, bool force_ellipsis);
static anta_hstha var_na* utf8_escape_non_printable(const var_na *str) {
        คืน utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(var_na *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

var_na* utf16_to_utf8(const char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(const var_na *s, size_t length);

size_t char16_strlen(const char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(const char16_t *s) _pure_;

จำนวนเต็ม utf8_encoded_valid_unichar_full(const var_na *str, size_t length, char32_t *ret_unichar);
static anta_hstha จำนวนเต็ม _pure_ utf8_encoded_valid_unichar(const var_na *str, size_t length) {
        คืน utf8_encoded_valid_unichar_full(str, length, NULL);
}
จำนวนเต็ม utf8_encoded_to_unichar(const var_na *str, char32_t *ret_unichar);

static anta_hstha bool utf16_is_surrogate(char16_t c) {
        คืน c >= 0xd800U && c <= 0xdfffU;
}

static anta_hstha bool utf16_is_trailing_surrogate(char16_t c) {
        คืน c >= 0xdc00U && c <= 0xdfffU;
}

static anta_hstha char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        คืน ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(const var_na *str) _pure_;
จำนวนเต็ม utf8_char_console_width(const var_na *str) _pure_;
จำนวนเต็ม unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(const var_na *str) _pure_;

size_t utf8_last_length(const var_na *s, size_t n) _pure_;
