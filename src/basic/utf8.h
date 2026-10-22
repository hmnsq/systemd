/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UTF8_REPLACEMENT_CHARACTER "\xef\xbf\xbd"
#ವ್ಯಾಖ್ಯಾನಿಸು UTF8_BYTE_ಕ್ರಮ_MARK "\xef\xbb\xbf"

bool unichar_is_valid(char32_t c) _const_;

ಅಕ್ಷರ* utf8_is_valid_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len_ಬೈಟುಗಳು) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* utf8_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಹಿಂತಿರುಗಿ utf8_is_valid_n(str, SIZE_MAX);
}

ಅಕ್ಷರ* ascii_is_valid_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t len) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* ascii_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಹಿಂತಿರುಗಿ ascii_is_valid_n(str, SIZE_MAX);
}

ಪೂರ್ಣಾಂಕ utf8_ಗಮ್ಯ_ascii(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಅಕ್ಷರ replacement_char, ಅಕ್ಷರ **ret);

bool utf8_is_printable_newline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str, size_t length, bool allow_newline) ಶುದ್ಧ;
#ವ್ಯಾಖ್ಯಾನಿಸು utf8_is_printable(str, length) utf8_is_printable_newline(str, length, true)

ಅಕ್ಷರ* utf8_escape_invalid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಅಕ್ಷರ* utf8_escape_non_printable_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t console_ಅಗಲ, bool force_ellipsis);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* utf8_escape_non_printable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಹಿಂತಿರುಗಿ utf8_escape_non_printable_ತುಂಬಿದ(str, SIZE_MAX, false);
}

size_t utf8_encode_unichar(ಅಕ್ಷರ *ಹೊರಹೋಗು_utf8, char32_t g);
size_t utf16_encode_unichar(char16_t *ಹೊರಹೋಗು, char32_t c);

ಅಕ್ಷರ* utf16_ಗಮ್ಯ_utf8(ಸ್ಥಿರಾಂಕ char16_t *s, size_t length /* bytes! */);
char16_t *utf8_ಗಮ್ಯ_utf16(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length);

size_t char16_strlen(ಸ್ಥಿರಾಂಕ char16_t *s) ಶುದ್ಧ; /* returns the number of 16-bit words in the string (not bytes!) */
size_t char16_strsize(ಸ್ಥಿರಾಂಕ char16_t *s) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ utf8_encoded_valid_unichar_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t length, char32_t *ret_unichar);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಶುದ್ಧ utf8_encoded_valid_unichar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, size_t length) {
        ಹಿಂತಿರುಗಿ utf8_encoded_valid_unichar_ತುಂಬಿದ(str, length, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ utf8_encoded_ಗಮ್ಯ_unichar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, char32_t *ret_unichar);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool utf16_is_surrogate(char16_t c) {
        ಹಿಂತಿರುಗಿ c >= 0xd800U && c <= 0xdfffU;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool utf16_is_trailing_surrogate(char16_t c) {
        ಹಿಂತಿರುಗಿ c >= 0xdc00U && c <= 0xdfffU;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ char32_t utf16_surrogate_pair_ಗಮ್ಯ_unichar(char16_t lead, char16_t trail) {
        ಹಿಂತಿರುಗಿ ((((char32_t) lead - 0xd800U) << 10) + ((char32_t) trail - 0xdc00U) + 0x10000U);
}

size_t utf8_n_codepoints(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ utf8_char_console_ಅಗಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ unichar_console_ಅಗಲ(char32_t c) ಶುದ್ಧ;
size_t utf8_console_ಅಗಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) ಶುದ್ಧ;

size_t utf8_ಕೊನೆಯ_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n) ಶುದ್ಧ;
