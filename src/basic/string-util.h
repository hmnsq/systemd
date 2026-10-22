/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/string-util.h" /* IWYU pragma: export */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* strstr_ptr_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಆದರೆ (!haystack || !needle)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(haystack, needle);
}

#ವ್ಯಾಖ್ಯಾನಿಸು strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_ಆಂತರಿಕ(haystack, needle))

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* strstrafter_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        ಅಕ್ಷರ *p = (ಅಕ್ಷರ*) strstr_ptr(haystack, needle);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ p + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(needle);
}

#ವ್ಯಾಖ್ಯಾನಿಸು strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_ಆಂತರಿಕ(haystack, needle))

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* strnull(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ s ?: "(null)";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* strna(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ s ?: "n/a";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* true_false(bool b) {
        ಹಿಂತಿರುಗಿ b ? "true" : "false";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* plus_minus(bool b) {
        ಹಿಂತಿರುಗಿ b ? "+" : "-";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* one_zero(bool b) {
        ಹಿಂತಿರುಗಿ b ? "1" : "0";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* enable_disable(bool b) {
        ಹಿಂತಿರುಗಿ b ? "enable" : "disable";
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* enabled_disabled(bool b) {
        ಹಿಂತಿರುಗಿ b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಲಿಗಳು_ಗಮ್ಯ_null(p)                                \
        ({                                              \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ಖಾಲಿ : _p);  \
        })

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಖಾಲಿಗಳು_ಗಮ್ಯ_na(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ isempty(p) ? "n/a" : p;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಖಾಲಿಗಳು_ಗಮ್ಯ_dash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಹಿಂತಿರುಗಿ isempty(str) ? "-" : str;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಖಾಲಿಗಳು_or_dash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str) {
        ಹಿಂತಿರುಗಿ !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಖಾಲಿಗಳು_or_dash_ಗಮ್ಯ_null(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_or_dash(p) ? ಖಾಲಿ : p;
}
#ವ್ಯಾಖ್ಯಾನಿಸು ಖಾಲಿಗಳು_or_dash_ಗಮ್ಯ_null(p)                                \
        ({                                                      \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_p = (p);                           \
                (typeof(p)) (ಖಾಲಿಗಳು_or_dash(_p) ? ಖಾಲಿ : _p);    \
        })

ಅಕ್ಷರ* ಮೊದಲ_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ) ಶುದ್ಧ;

ಅಕ್ಷರ* strextendn(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l) _nonnull_if_nonzero_(2, 3);

#ವ್ಯಾಖ್ಯಾನಿಸು strjoin(a, ...) strextend_with_separator_ಆಂತರಿಕ(ಖಾಲಿ, ಖಾಲಿ, a, __VA_ARGS__, ಖಾಲಿ)

#ವ್ಯಾಖ್ಯಾನಿಸು strjoina(a, ...)                                                \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_appendees_[] = { a, __VA_ARGS__ };         \
                ಅಕ್ಷರ *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                ಗಾಗಿ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(_appendees_[_i_]);              \
                _p_ = _d_ = newa(ಅಕ್ಷರ, _len_ + 1);                      \
                ಗಾಗಿ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ಅಕ್ಷರ* strstrip(ಅಕ್ಷರ *s);
ಅಕ್ಷರ* delete_chars(ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad);
ಅಕ್ಷರ* delete_trailing_chars(ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad);
ಅಕ್ಷರ* truncate_nl_ತುಂಬಿದ(ಅಕ್ಷರ *s, size_t *ret_len);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* truncate_nl(ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ truncate_nl_ತುಂಬಿದ(s, ಖಾಲಿ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* skip_leading_chars(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (!bad)
                bad = WHITESPACE;

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) s + strspn(s, bad);
}

ಅಕ್ಷರ ascii_tolower(ಅಕ್ಷರ x) _const_;
ಅಕ್ಷರ* ascii_strlower(ಅಕ್ಷರ *s);
ಅಕ್ಷರ* ascii_strlower_n(ಅಕ್ಷರ *s, size_t n);

ಅಕ್ಷರ ascii_toupper(ಅಕ್ಷರ x) _const_;
ಅಕ್ಷರ* ascii_strupper(ಅಕ್ಷರ *s);

ಪೂರ್ಣಾಂಕ ascii_strcasecmp_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, size_t n);
ಪೂರ್ಣಾಂಕ ascii_strcasecmp_nn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, size_t n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, size_t m);

bool chars_intersect(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) ಶುದ್ಧ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಶುದ್ಧ ಒಳ_charset(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *charset) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(charset);
        ಹಿಂತಿರುಗಿ s[strspn(s, charset)] == '\0';
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool char_is_cc(ಅಕ್ಷರ p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ಹಿಂತಿರುಗಿ (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ok) ಶುದ್ಧ;

ಅಕ್ಷರ* ellipsize_mem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t old_ಉದ್ದಳತೆ_ಬೈಟುಗಳು, size_t new_ಉದ್ದಳತೆ_columns, ಚಿಹ್ನೆರಹಿತ percent);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* ellipsize(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t length, ಚಿಹ್ನೆರಹಿತ percent) {
        ಹಿಂತಿರುಗಿ ellipsize_mem(s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s), length, percent);
}

ಅಕ್ಷರ* cellescape(ಅಕ್ಷರ *buf, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#ವ್ಯಾಖ್ಯಾನಿಸು CELLESCAPE_DEFAULT_ಉದ್ದಳತೆ 64

ಅಕ್ಷರ* strshorten(ಅಕ್ಷರ *s, size_t l);

ಪೂರ್ಣಾಂಕ strgrowpad0(ಅಕ್ಷರ **s, size_t l);

ಅಕ್ಷರ* strreplace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *old_string, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *new_string);

ಅಕ್ಷರ* strip_tab_ansi(ಅಕ್ಷರ **ibuf, size_t *_isz, size_t highlight[2]);

ಅಕ್ಷರ* strextendv_with_separator(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, va_list ap);
ಅಕ್ಷರ* strextend_with_separator_ಆಂತರಿಕ(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು strextend_with_separator(x, separator, ...) strextend_with_separator_ಆಂತರಿಕ(x, separator, __VA_ARGS__, ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು strextend(x, ...) strextend_with_separator_ಆಂತರಿಕ(x, ಖಾಲಿ, __VA_ARGS__, ಖಾಲಿ)

ಪೂರ್ಣಾಂಕ strextendf_with_separator(ಅಕ್ಷರ **x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(3,4);
#ವ್ಯಾಖ್ಯಾನಿಸು strextendf(x, ...) strextendf_with_separator(x, ಖಾಲಿ, __VA_ARGS__)

#ವ್ಯಾಖ್ಯಾನಿಸು strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                ಅಕ್ಷರ **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_ಆಂತರಿಕ(ಖಾಲಿ, (separator), __VA_ARGS__, ಖಾಲಿಗಳು_ಗಮ್ಯ_null(*_p_), ಖಾಲಿ); \
                ಆದರೆ (_s_) {                                                      \
                        ಬಿಡುಗಡೆ(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#ವ್ಯಾಖ್ಯಾನಿಸು strprepend(x, ...) strprepend_with_separator(x, ಖಾಲಿ, __VA_ARGS__)

ಅಕ್ಷರ* strrep(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n);

#ವ್ಯಾಖ್ಯಾನಿಸು strrepa(s, n)                                                   \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(_sss_);              \
                assert_se(MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&_len_, _nnn_));              \
                ಅಕ್ಷರ *_d_, *_p_;                                        \
                _p_ = _d_ = newa(ಅಕ್ಷರ, _len_ + 1);                      \
                ಗಾಗಿ (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು_pair(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep, ಅಕ್ಷರ **ret_ಮೊದಲ, ಅಕ್ಷರ **ret_ಎರಡನೇ);

ಪೂರ್ಣಾಂಕ free_and_strdup(ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ free_and_strdup_warn(ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ free_and_strndup(ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l) _nonnull_if_nonzero_(2, 3);

ಪೂರ್ಣಾಂಕ strdup_ಗಮ್ಯ_ತುಂಬಿದ(ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strdup_ಗಮ್ಯ(ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src) {
        ಪೂರ್ಣಾಂಕ r = strdup_ಗಮ್ಯ_ತುಂಬಿದ(ASSERT_PTR(ret), src);
        ಹಿಂತಿರುಗಿ r < 0 ? r : 0;  /* Suppress return value of 1. */
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ String_ಸುರಕ್ಷಿತ_Flags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_ಖಾಲಿಗಳು         = 1 << 1, /* Allow empty strings */
        STRING_ALLOW_NEWLINES      = 1 << 2, /* Allow newlines (\n) */
        STRING_ALLOW_BACKSLASHES   = 1 << 3, /* Allow backslashes (\) */
        STRING_ALLOW_QUOTES        = 1 << 4, /* Allow quotes (" or ') */
        STRING_ALLOW_GLOBS         = 1 << 5, /* Allow globs (?, * or [) */
        STRING_FILENAME            = 1 << 6, /* Verify the string is valid as regular filename */
        STRING_FILENAME_PART       = 1 << 7, /* Verify the string is valid as part of a regular filename */
        STRING_DISALLOW_WHITESPACE = 1 << 8, /* Refuse whitespace (space, tab, newline, …) */
} String_ಸುರಕ್ಷಿತ_Flags;

bool string_is_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, String_ಸುರಕ್ಷಿತ_Flags flags) ಶುದ್ಧ;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ strncpy_exact(ಅಕ್ಷರ *buf, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ಸ್ಮರಣೆ_startswith_no_case(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t sz, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token) {
        ಪ್ರತಿಜ್ಞಾ(token);

        size_t n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(token);
        ಆದರೆ (sz < n)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಗಾಗಿ (size_t i = 0; i < n; i++)
                ಆದರೆ (ascii_tolower(((ಅಕ್ಷರ *)p)[i]) != ascii_tolower(token[i]))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ (uint8_t*) p + n;
}

ಅಕ್ಷರ* str_realloc(ಅಕ್ಷರ *p);
ಅಕ್ಷರ* string_erase(ಅಕ್ಷರ *x);

ಪೂರ್ಣಾಂಕ string_truncate_ಸಾಲುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n_ಸಾಲುಗಳು, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ string_ಹೊರತೆಗೆ_ಸಾಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t i, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ string_ಒಳಗೊಂಡಿದೆ_ಶಬ್ದ_strv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಪದಗಳು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret_ಶಬ್ದ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ string_ಒಳಗೊಂಡಿದೆ_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ) {
        ಹಿಂತಿರುಗಿ string_ಒಳಗೊಂಡಿದೆ_ಶಬ್ದ_strv(string, separators, STRV_MAKE(ಶಬ್ದ), ಖಾಲಿ);
}

bool streq_skip_trailing_chars(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ok);

ಅಕ್ಷರ* string_replace_char(ಅಕ್ಷರ *str, ಅಕ್ಷರ old_char, ಅಕ್ಷರ new_char);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Make_CString_ಬಹುಲಕ {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        MAKE_CSTRING_ಬಹುಲಕ_MAX,
        MAKE_CSTRING_ಬಹುಲಕ_INVALID = -1,
} Make_CString_ಬಹುಲಕ;

ಪೂರ್ಣಾಂಕ make_cstring(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s, size_t n, Make_CString_ಬಹುಲಕ ಬಹುಲಕ, ಅಕ್ಷರ **ret);

size_t strspn_from_ಕೊನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept) ಶುದ್ಧ;
size_t strnspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept, size_t n) ಶುದ್ಧ _nonnull_if_nonzero_(1, 3);

ಅಕ್ಷರ* strdupspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accept);
ಅಕ್ಷರ* strdupcspn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#ವ್ಯಾಖ್ಯಾನಿಸು strdupa_ಸುರಕ್ಷಿತ(s)                                                 \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_t = (s);                                   \
                (ಅಕ್ಷರ*) memdupa_suffix0(_t, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(_t));                \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು strndupa_ಸುರಕ್ಷಿತ(s, n)                                             \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_t = (s);                                   \
                (ಅಕ್ಷರ*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಸಾಲು_startswith_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle);
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_ಸಾಲು_startswith(haystack, needle) \
        const_generic(haystack, ಕಂಡುಹಿಡಿ_ಸಾಲು_startswith_ಆಂತರಿಕ(haystack, needle))

ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಸಾಲು_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle);
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_ಸಾಲು(haystack, needle) \
        const_generic(haystack, ಕಂಡುಹಿಡಿ_ಸಾಲು_ಆಂತರಿಕ(haystack, needle))

ಅಕ್ಷರ* ಕಂಡುಹಿಡಿ_ಸಾಲು_after_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle);
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_ಸಾಲು_after(haystack, needle) \
        const_generic(haystack, ಕಂಡುಹಿಡಿ_ಸಾಲು_after_ಆಂತರಿಕ(haystack, needle))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಆವೃತ್ತಿ_Flags {
        ಆವೃತ್ತಿ_ALLOW_ಖಾಲಿಗಳು      = 1 << 0,
        ಆವೃತ್ತಿ_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        ಆವೃತ್ತಿ_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} ಆವೃತ್ತಿ_Flags;

bool ಆವೃತ್ತಿ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಆವೃತ್ತಿ_Flags flags) ಶುದ್ಧ;

ssize_t strlevenshtein(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *y);

ಅಕ್ಷರ* strrstr_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) ಶುದ್ಧ;
#ವ್ಯಾಖ್ಯಾನಿಸು strrstr(haystack, needle) \
        const_generic(haystack, strrstr_ಆಂತರಿಕ(haystack, needle))

ಅಕ್ಷರ* strrstr_no_case_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *haystack, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) ಶುದ್ಧ;
#ವ್ಯಾಖ್ಯಾನಿಸು strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_ಆಂತರಿಕ(haystack, needle))

size_t str_common_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) ಶುದ್ಧ;
