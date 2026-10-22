/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#నిర్వచించు UTF8_BYTE_క్రమం_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

అక్షరం* utf8_is_valid_n(స్థిరరాశి అక్షరం *str, size_t len_బైట్లు) శుద్ధ;
స్థిర అంతఃవరుస అక్షరం* utf8_is_valid(స్థిరరాశి అక్షరం *str) {
        తిరిగి utf8_is_valid_n(str, SIZE_MAX);
}

అక్షరం* ascii_is_valid_n(స్థిరరాశి అక్షరం *str, size_t len) శుద్ధ;
స్థిర అంతఃవరుస అక్షరం* ascii_is_valid(స్థిరరాశి అక్షరం *str) {
        తిరిగి ascii_is_valid_n(str, SIZE_MAX);
}

పూర్ణాంకం utf8_గమ్యం_ascii(స్థిరరాశి అక్షరం *str, అక్షరం replacement_char, అక్షరం **ret);

bool utf8_is_printable_newline(స్థిరరాశి అక్షరం* str, size_t length, bool allow_newline) శుద్ధ;
#నిర్వచించు utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

అక్షరం* utf8_escape_invalid(స్థిరరాశి అక్షరం *s);
అక్షరం* utf8_escape_non_printable_నిండిన(స్థిరరాశి అక్షరం *str, size_t console_వెడల్పు, bool force_ellipsis);
స్థిర అంతఃవరుస అక్షరం* utf8_escape_non_printable(స్థిరరాశి అక్షరం *str) {
        తిరిగి utf8_escape_non_printable_నిండిన(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(అక్షరం *బహిర్గతం_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *బహిర్గతం, char32_t c);

అక్షరం* utf16_గమ్యం_utf8(స్థిరరాశి char16_t *s, size_t length /* bytes! */);
char16_t *utf8_గమ్యం_utf16(స్థిరరాశి అక్షరం *s, size_t length);

size_t char16_strlen(స్థిరరాశి char16_t *s) శుద్ధ; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(స్థిరరాశి char16_t *s) శుద్ధ;

పూర్ణాంకం utf8_encoded_valid_unichar_నిండిన(స్థిరరాశి అక్షరం *str, size_t length, char32_t *ret_unichar);
స్థిర అంతఃవరుస పూర్ణాంకం శుద్ధ utf8_encoded_valid_unichar(స్థిరరాశి అక్షరం *str, size_t length) {
        తిరిగి utf8_encoded_valid_unichar_నిండిన(str, length, ఖాళీ);
}
పూర్ణాంకం utf8_encoded_గమ్యం_unichar(స్థిరరాశి అక్షరం *str, char32_t *ret_unichar);

స్థిర అంతఃవరుస bool utf16_is_surrogate(char16_t c) {
        తిరిగి c >= 0xd800U && c <= 0xdfffU;
}

స్థిర అంతఃవరుస bool utf16_is_trailing_surrogate(char16_t c) {
        తిరిగి c >= 0xdc00U && c <= 0xdfffU;
}

స్థిర అంతఃవరుస char32_t utf16_surrogate_pair_గమ్యం_unichar(char16_t lead, char16_t trail) {
        తిరిగి ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(స్థిరరాశి అక్షరం *str) శుద్ధ;
పూర్ణాంకం utf8_char_console_వెడల్పు(స్థిరరాశి అక్షరం *str) శుద్ధ;
పూర్ణాంకం unichar_console_వెడల్పు(char32_t c) శుద్ధ;
size_t utf8_console_వెడల్పు(స్థిరరాశి అక్షరం *str) శుద్ధ;

size_t utf8_చివరి_నిడివి(స్థిరరాశి అక్షరం *s, size_t n) శుద్ధ;
