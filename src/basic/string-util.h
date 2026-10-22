/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <string.h>

#شامل "alloc-util.h"
#شامل "forward.h"

#شامل "../fundamental/string-util.h" /* IWYU pragma: export */

ایستا درونخطی نویسه* strstr_ptr_internal(ثابت نویسه *haystack, ثابت نویسه *needle) {
        اگر (!haystack || !needle)
                بازگشت پوچ;
        بازگشت (نویسه*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

ایستا درونخطی نویسه* strstrafter_internal(ثابت نویسه *haystack, ثابت نویسه *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        نویسه *p = (نویسه*) strstr_ptr(haystack, needle);
        اگر (!p)
                بازگشت پوچ;

        بازگشت p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

ایستا درونخطی ثابت نویسه* strnull(ثابت نویسه *s) {
        بازگشت s ?: "(null)";
}

ایستا درونخطی ثابت نویسه* strna(ثابت نویسه *s) {
        بازگشت s ?: "n/a";
}

ایستا درونخطی ثابت نویسه* true_false(bool b) {
        بازگشت b ? "true" : "false";
}

ایستا درونخطی ثابت نویسه* plus_minus(bool b) {
        بازگشت b ? "+" : "-";
}

ایستا درونخطی ثابت نویسه* one_zero(bool b) {
        بازگشت b ? "1" : "0";
}

ایستا درونخطی ثابت نویسه* enable_disable(bool b) {
        بازگشت b ? "enable" : "disable";
}

ایستا درونخطی ثابت نویسه* enabled_disabled(bool b) {
        بازگشت b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ثابت نویسه *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? پوچ : _p);  \
        })

ایستا درونخطی ثابت نویسه* empty_to_na(ثابت نویسه *p) {
        بازگشت isempty(p) ? "n/a" : p;
}

ایستا درونخطی ثابت نویسه* empty_to_dash(ثابت نویسه *str) {
        بازگشت isempty(str) ? "-" : str;
}

ایستا درونخطی bool empty_or_dash(ثابت نویسه *str) {
        بازگشت !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ایستا درونخطی ثابت نویسه* empty_or_dash_to_null(ثابت نویسه *p) {
        بازگشت empty_or_dash(p) ? پوچ : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ثابت نویسه *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? پوچ : _p);    \
        })

نویسه* first_word(ثابت نویسه *s, ثابت نویسه *word) _pure_;

نویسه* strextendn(نویسه **x, ثابت نویسه *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(پوچ, پوچ, a, __VA_ARGS__, پوچ)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ثابت نویسه *_appendees_[] = { a, __VA_ARGS__ };         \
                نویسه *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                برای (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(نویسه, _len_ + 1);                      \
                برای (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

نویسه* strstrip(نویسه *s);
نویسه* delete_chars(نویسه *s, ثابت نویسه *bad);
نویسه* delete_trailing_chars(نویسه *s, ثابت نویسه *bad);
نویسه* truncate_nl_full(نویسه *s, size_t *ret_len);
ایستا درونخطی نویسه* truncate_nl(نویسه *s) {
        بازگشت truncate_nl_full(s, پوچ);
}

ایستا درونخطی نویسه* skip_leading_chars(ثابت نویسه *s, ثابت نویسه *bad) {
        اگر (!s)
                بازگشت پوچ;

        اگر (!bad)
                bad = WHITESPACE;

        بازگشت (نویسه*) s + strspn(s, bad);
}

نویسه ascii_tolower(نویسه x) _const_;
نویسه* ascii_strlower(نویسه *s);
نویسه* ascii_strlower_n(نویسه *s, size_t n);

نویسه ascii_toupper(نویسه x) _const_;
نویسه* ascii_strupper(نویسه *s);

صحیح ascii_strcasecmp_n(ثابت نویسه *a, ثابت نویسه *b, size_t n);
صحیح ascii_strcasecmp_nn(ثابت نویسه *a, size_t n, ثابت نویسه *b, size_t m);

bool chars_intersect(ثابت نویسه *a, ثابت نویسه *b) _pure_;

ایستا درونخطی bool _pure_ in_charset(ثابت نویسه *s, ثابت نویسه *charset) {
        أكد(s);
        أكد(charset);
        بازگشت s[strspn(s, charset)] == '\0';
}

ایستا درونخطی bool char_is_cc(نویسه p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        بازگشت (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ثابت نویسه *p, ثابت نویسه *ok) _pure_;

نویسه* ellipsize_mem(ثابت نویسه *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
ایستا درونخطی نویسه* ellipsize(ثابت نویسه *s, size_t length, unsigned percent) {
        بازگشت ellipsize_mem(s, strlen(s), length, percent);
}

نویسه* cellescape(نویسه *buf, size_t len, ثابت نویسه *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

نویسه* strshorten(نویسه *s, size_t l);

صحیح strgrowpad0(نویسه **s, size_t l);

نویسه* strreplace(ثابت نویسه *text, ثابت نویسه *old_string, ثابت نویسه *new_string);

نویسه* strip_tab_ansi(نویسه **ibuf, size_t *_isz, size_t highlight[2]);

نویسه* strextendv_with_separator(نویسه **x, ثابت نویسه *separator, va_list ap);
نویسه* strextend_with_separator_internal(نویسه **x, ثابت نویسه *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, پوچ)
#define strextend(x, ...) strextend_with_separator_internal(x, پوچ, __VA_ARGS__, پوچ)

صحیح strextendf_with_separator(نویسه **x, ثابت نویسه *separator, ثابت نویسه *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, پوچ, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                نویسه **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(پوچ, (separator), __VA_ARGS__, empty_to_null(*_p_), پوچ); \
                اگر (_s_) {                                                      \
                        آزاد(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, پوچ, __VA_ARGS__)

نویسه* strrep(ثابت نویسه *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ثابت نویسه *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                نویسه *_d_, *_p_;                                        \
                _p_ = _d_ = newa(نویسه, _len_ + 1);                      \
                برای (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

صحیح split_pair(ثابت نویسه *s, ثابت نویسه *sep, نویسه **ret_first, نویسه **ret_second);

صحیح free_and_strdup(نویسه **p, ثابت نویسه *s);
صحیح free_and_strdup_warn(نویسه **p, ثابت نویسه *s);
صحیح free_and_strndup(نویسه **p, ثابت نویسه *s, size_t l) _nonnull_if_nonzero_(2, 3);

صحیح strdup_to_full(نویسه **ret, ثابت نویسه *src);
ایستا درونخطی صحیح strdup_to(نویسه **ret, ثابت نویسه *src) {
        صحیح r = strdup_to_full(ASSERT_PTR(ret), src);
        بازگشت r < 0 ? r : 0;  /* Suppress return value of 1. */
}

تعریفنوع شمارش StringSafeFlags {
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

bool string_is_safe(ثابت نویسه *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ایستا درونخطی تهی strncpy_exact(نویسه *buf, ثابت نویسه *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ایستا درونخطی تهی* memory_startswith_no_case(ثابت تهی *p, size_t sz, ثابت نویسه *token) {
        أكد(token);

        size_t n = strlen(token);
        اگر (sz < n)
                بازگشت پوچ;

        أكد(p);

        برای (size_t i = 0; i < n; i++)
                اگر (ascii_tolower(((نویسه *)p)[i]) != ascii_tolower(token[i]))
                        بازگشت پوچ;

        بازگشت (uint8_t*) p + n;
}

نویسه* str_realloc(نویسه *p);
نویسه* string_erase(نویسه *x);

صحیح string_truncate_سطرها(ثابت نویسه *s, size_t n_سطرها, نویسه **ret);
صحیح string_extract_سطر(ثابت نویسه *s, size_t i, نویسه **ret);

صحیح string_contains_word_strv(ثابت نویسه *string, ثابت نویسه *separators, نویسه * ثابت *واژگان, ثابت نویسه **ret_word);
ایستا درونخطی صحیح string_contains_word(ثابت نویسه *string, ثابت نویسه *separators, ثابت نویسه *word) {
        بازگشت string_contains_word_strv(string, separators, STRV_MAKE(word), پوچ);
}

bool streq_skip_trailing_chars(ثابت نویسه *s1, ثابت نویسه *s2, ثابت نویسه *ok);

نویسه* string_replace_char(نویسه *str, نویسه old_char, نویسه new_char);

تعریفنوع شمارش MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

صحیح make_cstring(ثابت تهی *s, size_t n, MakeCStringMode mode, نویسه **ret);

size_t strspn_from_end(ثابت نویسه *str, ثابت نویسه *accept) _pure_;
size_t strnspn(ثابت نویسه *str, ثابت نویسه *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

نویسه* strdupspn(ثابت نویسه *a, ثابت نویسه *accept);
نویسه* strdupcspn(ثابت نویسه *a, ثابت نویسه *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ثابت نویسه *_t = (s);                                   \
                (نویسه*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ثابت نویسه *_t = (s);                                   \
                (نویسه*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

نویسه* find_سطر_startswith_internal(ثابت نویسه *haystack, ثابت نویسه *needle);
#define find_سطر_startswith(haystack, needle) \
        const_generic(haystack, find_سطر_startswith_internal(haystack, needle))

نویسه* find_سطر_internal(ثابت نویسه *haystack, ثابت نویسه *needle);
#define find_سطر(haystack, needle) \
        const_generic(haystack, find_سطر_internal(haystack, needle))

نویسه* find_سطر_after_internal(ثابت نویسه *haystack, ثابت نویسه *needle);
#define find_سطر_after(haystack, needle) \
        const_generic(haystack, find_سطر_after_internal(haystack, needle))

تعریفنوع شمارش VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ثابت نویسه *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ثابت نویسه *x, ثابت نویسه *y);

نویسه* strrstr_internal(ثابت نویسه *haystack, ثابت نویسه *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

نویسه* strrstr_no_case_internal(ثابت نویسه *haystack, ثابت نویسه *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ثابت نویسه *a, ثابت نویسه *b) _pure_;
