/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/string-util.h" /* IWYU pragma: export */

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* strstr_ptr_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        ಯದಿ (!haystack || !needle)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* strstrafter_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        ವರ್ಣ *p = (ವರ್ಣ*) strstr_ptr(haystack, needle);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* strnull(ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ s ?: "(null)";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* strna(ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ s ?: "n/a";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* true_false(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? "true" : "false";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* plus_minus(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? "+" : "-";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* one_zero(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? "1" : "0";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* enable_disable(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? "enable" : "disable";
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* enabled_disabled(bool b) {
        ಪ್ರತ್ಯಾಗಮ b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ಧ್ರುವ ವರ್ಣ *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ರಿಕ್ತ : _p);  \
        })

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* empty_to_na(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ isempty(p) ? "n/a" : p;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* empty_to_dash(ಧ್ರುವ ವರ್ಣ *str) {
        ಪ್ರತ್ಯಾಗಮ isempty(str) ? "-" : str;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool empty_or_dash(ಧ್ರುವ ವರ್ಣ *str) {
        ಪ್ರತ್ಯಾಗಮ !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* empty_or_dash_to_null(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ empty_or_dash(p) ? ರಿಕ್ತ : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ಧ್ರುವ ವರ್ಣ *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? ರಿಕ್ತ : _p);    \
        })

ವರ್ಣ* first_word(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *word) _pure_;

ವರ್ಣ* strextendn(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(ರಿಕ್ತ, ರಿಕ್ತ, a, __VA_ARGS__, ರಿಕ್ತ)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ಧ್ರುವ ವರ್ಣ *_appendees_[] = { a, __VA_ARGS__ };         \
                ವರ್ಣ *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                ಪ್ರತಿ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(ವರ್ಣ, _len_ + 1);                      \
                ಪ್ರತಿ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ವರ್ಣ* strstrip(ವರ್ಣ *s);
ವರ್ಣ* delete_chars(ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad);
ವರ್ಣ* delete_trailing_chars(ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad);
ವರ್ಣ* truncate_nl_full(ವರ್ಣ *s, size_t *ret_len);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* truncate_nl(ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ truncate_nl_full(s, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* skip_leading_chars(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *bad) {
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (!bad)
                bad = WHITESPACE;

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) s + strspn(s, bad);
}

ವರ್ಣ ascii_tolower(ವರ್ಣ x) _const_;
ವರ್ಣ* ascii_strlower(ವರ್ಣ *s);
ವರ್ಣ* ascii_strlower_n(ವರ್ಣ *s, size_t n);

ವರ್ಣ ascii_toupper(ವರ್ಣ x) _const_;
ವರ್ಣ* ascii_strupper(ವರ್ಣ *s);

ಪೂರ್ಣಾಙ್ಕ ascii_strcasecmp_n(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b, size_t n);
ಪೂರ್ಣಾಙ್ಕ ascii_strcasecmp_nn(ಧ್ರುವ ವರ್ಣ *a, size_t n, ಧ್ರುವ ವರ್ಣ *b, size_t m);

bool chars_intersect(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) _pure_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool _pure_ in_charset(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *charset) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(charset);
        ಪ್ರತ್ಯಾಗಮ s[strspn(s, charset)] == '\0';
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool char_is_cc(ವರ್ಣ p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ಪ್ರತ್ಯಾಗಮ (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *ok) _pure_;

ವರ್ಣ* ellipsize_mem(ಧ್ರುವ ವರ್ಣ *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* ellipsize(ಧ್ರುವ ವರ್ಣ *s, size_t length, unsigned percent) {
        ಪ್ರತ್ಯಾಗಮ ellipsize_mem(s, strlen(s), length, percent);
}

ವರ್ಣ* cellescape(ವರ್ಣ *buf, size_t len, ಧ್ರುವ ವರ್ಣ *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

ವರ್ಣ* strshorten(ವರ್ಣ *s, size_t l);

ಪೂರ್ಣಾಙ್ಕ strgrowpad0(ವರ್ಣ **s, size_t l);

ವರ್ಣ* strreplace(ಧ್ರುವ ವರ್ಣ *text, ಧ್ರುವ ವರ್ಣ *old_string, ಧ್ರುವ ವರ್ಣ *new_string);

ವರ್ಣ* strip_tab_ansi(ವರ್ಣ **ibuf, size_t *_isz, size_t highlight[2]);

ವರ್ಣ* strextendv_with_separator(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *separator, va_list ap);
ವರ್ಣ* strextend_with_separator_internal(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, ರಿಕ್ತ)
#define strextend(x, ...) strextend_with_separator_internal(x, ರಿಕ್ತ, __VA_ARGS__, ರಿಕ್ತ)

ಪೂರ್ಣಾಙ್ಕ strextendf_with_separator(ವರ್ಣ **x, ಧ್ರುವ ವರ್ಣ *separator, ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, ರಿಕ್ತ, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                ವರ್ಣ **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(ರಿಕ್ತ, (separator), __VA_ARGS__, empty_to_null(*_p_), ರಿಕ್ತ); \
                ಯದಿ (_s_) {                                                      \
                        ಮುಕ್ತ(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, ರಿಕ್ತ, __VA_ARGS__)

ವರ್ಣ* strrep(ಧ್ರುವ ವರ್ಣ *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ಧ್ರುವ ವರ್ಣ *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                ವರ್ಣ *_d_, *_p_;                                        \
                _p_ = _d_ = newa(ವರ್ಣ, _len_ + 1);                      \
                ಪ್ರತಿ (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ಪೂರ್ಣಾಙ್ಕ split_pair(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *sep, ವರ್ಣ **ret_first, ವರ್ಣ **ret_second);

ಪೂರ್ಣಾಙ್ಕ free_and_strdup(ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ free_and_strdup_warn(ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ free_and_strndup(ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *s, size_t l) _nonnull_if_nonzero_(2, 3);

ಪೂರ್ಣಾಙ್ಕ strdup_to_full(ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *src);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strdup_to(ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *src) {
        ಪೂರ್ಣಾಙ್ಕ r = strdup_to_full(ASSERT_PTR(ret), src);
        ಪ್ರತ್ಯಾಗಮ r < 0 ? r : 0;  /* Suppress return value of 1. */
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ StringSafeFlags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_EMPTY         = 1 << 1, /* Allow empty strings */
        STRING_ALLOW_NEWLINES      = 1 << 2, /* Allow newlines (\n) */
        STRING_ALLOW_BACKSLASHES   = 1 << 3, /* Allow backslashes (\) */
        STRING_ALLOW_QUOTES        = 1 << 4, /* Allow quotes (" or ') */
        STRING_ALLOW_GLOBS         = 1 << 5, /* Allow globs (?, * or [) */
        STRING_FILENAME            = 1 << 6, /* Verify the string is valid as regular filename */
        STRING_FILENAME_PART       = 1 << 7, /* Verify the string is valid as part of a regular filename */
        STRING_DISALLOW_WHITESPACE = 1 << 8, /* Refuse whitespace (space, tab, newline, …) */
} StringSafeFlags;

bool string_is_safe(ಧ್ರುವ ವರ್ಣ *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ strncpy_exact(ವರ್ಣ *buf, ಧ್ರುವ ವರ್ಣ *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* memory_startswith_no_case(ಧ್ರುವ ಶೂನ್ಯ *p, size_t sz, ಧ್ರುವ ವರ್ಣ *token) {
        ಪ್ರತಿಜ್ಞಾ(token);

        size_t n = strlen(token);
        ಯದಿ (sz < n)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಪ್ರತಿ (size_t i = 0; i < n; i++)
                ಯದಿ (ascii_tolower(((ವರ್ಣ *)p)[i]) != ascii_tolower(token[i]))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ (uint8_t*) p + n;
}

ವರ್ಣ* str_realloc(ವರ್ಣ *p);
ವರ್ಣ* string_erase(ವರ್ಣ *x);

ಪೂರ್ಣಾಙ್ಕ string_truncate_ಸಾಲುಗಳು(ಧ್ರುವ ವರ್ಣ *s, size_t n_ಸಾಲುಗಳು, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ string_extract_ಸಾಲು(ಧ್ರುವ ವರ್ಣ *s, size_t i, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ string_contains_word_strv(ಧ್ರುವ ವರ್ಣ *string, ಧ್ರುವ ವರ್ಣ *separators, ವರ್ಣ * ಧ್ರುವ *ಪದಗಳು, ಧ್ರುವ ವರ್ಣ **ret_word);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ string_contains_word(ಧ್ರುವ ವರ್ಣ *string, ಧ್ರುವ ವರ್ಣ *separators, ಧ್ರುವ ವರ್ಣ *word) {
        ಪ್ರತ್ಯಾಗಮ string_contains_word_strv(string, separators, STRV_MAKE(word), ರಿಕ್ತ);
}

bool streq_skip_trailing_chars(ಧ್ರುವ ವರ್ಣ *s1, ಧ್ರುವ ವರ್ಣ *s2, ಧ್ರುವ ವರ್ಣ *ok);

ವರ್ಣ* string_replace_char(ವರ್ಣ *str, ವರ್ಣ old_char, ವರ್ಣ new_char);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

ಪೂರ್ಣಾಙ್ಕ make_cstring(ಧ್ರುವ ಶೂನ್ಯ *s, size_t n, MakeCStringMode mode, ವರ್ಣ **ret);

size_t strspn_from_end(ಧ್ರುವ ವರ್ಣ *str, ಧ್ರುವ ವರ್ಣ *accept) _pure_;
size_t strnspn(ಧ್ರುವ ವರ್ಣ *str, ಧ್ರುವ ವರ್ಣ *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

ವರ್ಣ* strdupspn(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *accept);
ವರ್ಣ* strdupcspn(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ಧ್ರುವ ವರ್ಣ *_t = (s);                                   \
                (ವರ್ಣ*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ಧ್ರುವ ವರ್ಣ *_t = (s);                                   \
                (ವರ್ಣ*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

ವರ್ಣ* find_ಸಾಲು_startswith_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle);
#define find_ಸಾಲು_startswith(haystack, needle) \
        const_generic(haystack, find_ಸಾಲು_startswith_internal(haystack, needle))

ವರ್ಣ* find_ಸಾಲು_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle);
#define find_ಸಾಲು(haystack, needle) \
        const_generic(haystack, find_ಸಾಲು_internal(haystack, needle))

ವರ್ಣ* find_ಸಾಲು_after_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle);
#define find_ಸಾಲು_after(haystack, needle) \
        const_generic(haystack, find_ಸಾಲು_after_internal(haystack, needle))

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ಧ್ರುವ ವರ್ಣ *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ಧ್ರುವ ವರ್ಣ *x, ಧ್ರುವ ವರ್ಣ *y);

ವರ್ಣ* strrstr_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

ವರ್ಣ* strrstr_no_case_internal(ಧ್ರುವ ವರ್ಣ *haystack, ಧ್ರುವ ವರ್ಣ *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) _pure_;
