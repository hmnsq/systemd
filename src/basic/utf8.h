/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF8_BYTE_ਕ੍ਰਮ_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

ਅੱਖਰ* utf8_is_valid_n(ਅਚਲ ਅੱਖਰ *str, size_t len_ਬਾਈਟਾਂ) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* utf8_is_valid(ਅਚਲ ਅੱਖਰ *str) {
        ਵਾਪਸ utf8_is_valid_n(str, SIZE_MAX);
}

ਅੱਖਰ* ascii_is_valid_n(ਅਚਲ ਅੱਖਰ *str, size_t len) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* ascii_is_valid(ਅਚਲ ਅੱਖਰ *str) {
        ਵਾਪਸ ascii_is_valid_n(str, SIZE_MAX);
}

ਪੂਰਨ utf8_ਮੰਜ਼ਿਲ_ascii(ਅਚਲ ਅੱਖਰ *str, ਅੱਖਰ replacement_char, ਅੱਖਰ **ret);

bool utf8_is_printable_newline(ਅਚਲ ਅੱਖਰ* str, size_t length, bool allow_newline) ਸ਼ੁੱਧ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

ਅੱਖਰ* utf8_escape_invalid(ਅਚਲ ਅੱਖਰ *s);
ਅੱਖਰ* utf8_escape_non_printable_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *str, size_t console_ਚੌੜਾਈ, bool force_ellipsis);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* utf8_escape_non_printable(ਅਚਲ ਅੱਖਰ *str) {
        ਵਾਪਸ utf8_escape_non_printable_ਭਰਿਆ(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(ਅੱਖਰ *ਬਾਹਰਲਾ_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *ਬਾਹਰਲਾ, char32_t c);

ਅੱਖਰ* utf16_ਮੰਜ਼ਿਲ_utf8(ਅਚਲ char16_t *s, size_t length /* bytes! */);
char16_t *utf8_ਮੰਜ਼ਿਲ_utf16(ਅਚਲ ਅੱਖਰ *s, size_t length);

size_t char16_strlen(ਅਚਲ char16_t *s) ਸ਼ੁੱਧ; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ਅਚਲ char16_t *s) ਸ਼ੁੱਧ;

ਪੂਰਨ utf8_encoded_valid_unichar_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *str, size_t length, char32_t *ret_unichar);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸ਼ੁੱਧ utf8_encoded_valid_unichar(ਅਚਲ ਅੱਖਰ *str, size_t length) {
        ਵਾਪਸ utf8_encoded_valid_unichar_ਭਰਿਆ(str, length, ਨਿਰਾ);
}
ਪੂਰਨ utf8_encoded_ਮੰਜ਼ਿਲ_unichar(ਅਚਲ ਅੱਖਰ *str, char32_t *ret_unichar);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool utf16_is_surrogate(char16_t c) {
        ਵਾਪਸ c >= 0xd800U && c <= 0xdfffU;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool utf16_is_trailing_surrogate(char16_t c) {
        ਵਾਪਸ c >= 0xdc00U && c <= 0xdfffU;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ char32_t utf16_surrogate_pair_ਮੰਜ਼ਿਲ_unichar(char16_t lead, char16_t trail) {
        ਵਾਪਸ ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ਅਚਲ ਅੱਖਰ *str) ਸ਼ੁੱਧ;
ਪੂਰਨ utf8_char_console_ਚੌੜਾਈ(ਅਚਲ ਅੱਖਰ *str) ਸ਼ੁੱਧ;
ਪੂਰਨ unichar_console_ਚੌੜਾਈ(char32_t c) ਸ਼ੁੱਧ;
size_t utf8_console_ਚੌੜਾਈ(ਅਚਲ ਅੱਖਰ *str) ਸ਼ੁੱਧ;

size_t utf8_ਆਖਰੀ_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *s, size_t n) ਸ਼ੁੱਧ;
