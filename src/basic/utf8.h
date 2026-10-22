/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

#تعریف_کن UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#تعریف_کن UTF8_BYTE_ترتیب_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

حرف* utf8_is_valid_n(ثابت حرف *str, size_t len_بایتها) خالص;
ساكن مضمن حرف* utf8_is_valid(ثابت حرف *str) {
        أرجع utf8_is_valid_n(str, SIZE_MAX);
}

حرف* ascii_is_valid_n(ثابت حرف *str, size_t len) خالص;
ساكن مضمن حرف* ascii_is_valid(ثابت حرف *str) {
        أرجع ascii_is_valid_n(str, SIZE_MAX);
}

صحيح utf8_مقصد_ascii(ثابت حرف *str, حرف replacement_char, حرف **ret);

bool utf8_is_printable_newline(ثابت حرف* str, size_t length, bool allow_newline) خالص;
#تعریف_کن utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

حرف* utf8_escape_invalid(ثابت حرف *s);
حرف* utf8_escape_non_printable_پر(ثابت حرف *str, size_t console_عرض, bool force_ellipsis);
ساكن مضمن حرف* utf8_escape_non_printable(ثابت حرف *str) {
        أرجع utf8_escape_non_printable_پر(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(حرف *بیرون_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *بیرون, char32_t c);

حرف* utf16_مقصد_utf8(ثابت char16_t *s, size_t length /* bytes! */);
char16_t *utf8_مقصد_utf16(ثابت حرف *s, size_t length);

size_t char16_strlen(ثابت char16_t *s) خالص; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ثابت char16_t *s) خالص;

صحيح utf8_encoded_valid_unichar_پر(ثابت حرف *str, size_t length, char32_t *ret_unichar);
ساكن مضمن صحيح خالص utf8_encoded_valid_unichar(ثابت حرف *str, size_t length) {
        أرجع utf8_encoded_valid_unichar_پر(str, length, عدم);
}
صحيح utf8_encoded_مقصد_unichar(ثابت حرف *str, char32_t *ret_unichar);

ساكن مضمن bool utf16_is_surrogate(char16_t c) {
        أرجع c >= 0xd800U && c <= 0xdfffU;
}

ساكن مضمن bool utf16_is_trailing_surrogate(char16_t c) {
        أرجع c >= 0xdc00U && c <= 0xdfffU;
}

ساكن مضمن char32_t utf16_surrogate_pair_مقصد_unichar(char16_t lead, char16_t trail) {
        أرجع ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ثابت حرف *str) خالص;
صحيح utf8_char_console_عرض(ثابت حرف *str) خالص;
صحيح unichar_console_عرض(char32_t c) خالص;
size_t utf8_console_عرض(ثابت حرف *str) خالص;

size_t utf8_آخرین_درازی(ثابت حرف *s, size_t n) خالص;
