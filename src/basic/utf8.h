/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#define UTF8_BYTE_ORDER_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

எழுத்து* utf8_is_valid_n(மாறிலி எழுத்து *str, size_t len_bytes) _pure_;
நிலையான உள்வரி எழுத்து* utf8_is_valid(மாறிலி எழுத்து *str) {
        திரும்பு utf8_is_valid_n(str, SIZE_MAX);
}

எழுத்து* ascii_is_valid_n(மாறிலி எழுத்து *str, size_t len) _pure_;
நிலையான உள்வரி எழுத்து* ascii_is_valid(மாறிலி எழுத்து *str) {
        திரும்பு ascii_is_valid_n(str, SIZE_MAX);
}

முழு utf8_to_ascii(மாறிலி எழுத்து *str, எழுத்து replacement_char, எழுத்து **ret);

bool utf8_is_printable_newline(மாறிலி எழுத்து* str, size_t length, bool allow_newline) _pure_;
#define utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

எழுத்து* utf8_escape_invalid(மாறிலி எழுத்து *s);
எழுத்து* utf8_escape_non_printable_full(மாறிலி எழுத்து *str, size_t console_width, bool force_ellipsis);
நிலையான உள்வரி எழுத்து* utf8_escape_non_printable(மாறிலி எழுத்து *str) {
        திரும்பு utf8_escape_non_printable_full(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(எழுத்து *out_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *out, char32_t c);

எழுத்து* utf16_to_utf8(மாறிலி char16_t *s, size_t length /* bytes! */);
char16_t *utf8_to_utf16(மாறிலி எழுத்து *s, size_t length);

size_t char16_strlen(மாறிலி char16_t *s) _pure_; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(மாறிலி char16_t *s) _pure_;

முழு utf8_encoded_valid_unichar_full(மாறிலி எழுத்து *str, size_t length, char32_t *ret_unichar);
நிலையான உள்வரி முழு _pure_ utf8_encoded_valid_unichar(மாறிலி எழுத்து *str, size_t length) {
        திரும்பு utf8_encoded_valid_unichar_full(str, length, வெற்று);
}
முழு utf8_encoded_to_unichar(மாறிலி எழுத்து *str, char32_t *ret_unichar);

நிலையான உள்வரி bool utf16_is_surrogate(char16_t c) {
        திரும்பு c >= 0xd800U && c <= 0xdfffU;
}

நிலையான உள்வரி bool utf16_is_trailing_surrogate(char16_t c) {
        திரும்பு c >= 0xdc00U && c <= 0xdfffU;
}

நிலையான உள்வரி char32_t utf16_surrogate_pair_to_unichar(char16_t lead, char16_t trail) {
        திரும்பு ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(மாறிலி எழுத்து *str) _pure_;
முழு utf8_char_console_width(மாறிலி எழுத்து *str) _pure_;
முழு unichar_console_width(char32_t c) _pure_;
size_t utf8_console_width(மாறிலி எழுத்து *str) _pure_;

size_t utf8_last_length(மாறிலி எழுத்து *s, size_t n) _pure_;
