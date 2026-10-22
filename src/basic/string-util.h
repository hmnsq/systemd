/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <string.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/string-util.h" /* IWYU pragma: export */

स्थिर अन्तःस्थ वर्ण* strstr_ptr_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        जइ (!haystack || !needle)
                प्रत्यागम रिक्त;
        प्रत्यागम (वर्ण*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

स्थिर अन्तःस्थ वर्ण* strstrafter_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        वर्ण *p = (वर्ण*) strstr_ptr(haystack, needle);
        जइ (!p)
                प्रत्यागम रिक्त;

        प्रत्यागम p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

स्थिर अन्तःस्थ ध्रुव वर्ण* strnull(ध्रुव वर्ण *s) {
        प्रत्यागम s ?: "(null)";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* strna(ध्रुव वर्ण *s) {
        प्रत्यागम s ?: "n/a";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* true_false(bool b) {
        प्रत्यागम b ? "true" : "false";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* plus_minus(bool b) {
        प्रत्यागम b ? "+" : "-";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* one_zero(bool b) {
        प्रत्यागम b ? "1" : "0";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* enable_disable(bool b) {
        प्रत्यागम b ? "enable" : "disable";
}

स्थिर अन्तःस्थ ध्रुव वर्ण* enabled_disabled(bool b) {
        प्रत्यागम b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ध्रुव वर्ण *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? रिक्त : _p);  \
        })

स्थिर अन्तःस्थ ध्रुव वर्ण* empty_to_na(ध्रुव वर्ण *p) {
        प्रत्यागम isempty(p) ? "n/a" : p;
}

स्थिर अन्तःस्थ ध्रुव वर्ण* empty_to_dash(ध्रुव वर्ण *str) {
        प्रत्यागम isempty(str) ? "-" : str;
}

स्थिर अन्तःस्थ bool empty_or_dash(ध्रुव वर्ण *str) {
        प्रत्यागम !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

स्थिर अन्तःस्थ ध्रुव वर्ण* empty_or_dash_to_null(ध्रुव वर्ण *p) {
        प्रत्यागम empty_or_dash(p) ? रिक्त : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ध्रुव वर्ण *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? रिक्त : _p);    \
        })

वर्ण* first_word(ध्रुव वर्ण *s, ध्रुव वर्ण *word) _pure_;

वर्ण* strextendn(वर्ण **x, ध्रुव वर्ण *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(रिक्त, रिक्त, a, __VA_ARGS__, रिक्त)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ध्रुव वर्ण *_appendees_[] = { a, __VA_ARGS__ };         \
                वर्ण *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                प्रति (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(वर्ण, _len_ + 1);                      \
                प्रति (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

वर्ण* strstrip(वर्ण *s);
वर्ण* delete_chars(वर्ण *s, ध्रुव वर्ण *bad);
वर्ण* delete_trailing_chars(वर्ण *s, ध्रुव वर्ण *bad);
वर्ण* truncate_nl_full(वर्ण *s, size_t *ret_len);
स्थिर अन्तःस्थ वर्ण* truncate_nl(वर्ण *s) {
        प्रत्यागम truncate_nl_full(s, रिक्त);
}

स्थिर अन्तःस्थ वर्ण* skip_leading_chars(ध्रुव वर्ण *s, ध्रुव वर्ण *bad) {
        जइ (!s)
                प्रत्यागम रिक्त;

        जइ (!bad)
                bad = WHITESPACE;

        प्रत्यागम (वर्ण*) s + strspn(s, bad);
}

वर्ण ascii_tolower(वर्ण x) _const_;
वर्ण* ascii_strlower(वर्ण *s);
वर्ण* ascii_strlower_n(वर्ण *s, size_t n);

वर्ण ascii_toupper(वर्ण x) _const_;
वर्ण* ascii_strupper(वर्ण *s);

पूर्णाङ्क ascii_strcasecmp_n(ध्रुव वर्ण *a, ध्रुव वर्ण *b, size_t n);
पूर्णाङ्क ascii_strcasecmp_nn(ध्रुव वर्ण *a, size_t n, ध्रुव वर्ण *b, size_t m);

bool chars_intersect(ध्रुव वर्ण *a, ध्रुव वर्ण *b) _pure_;

स्थिर अन्तःस्थ bool _pure_ in_charset(ध्रुव वर्ण *s, ध्रुव वर्ण *charset) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(charset);
        प्रत्यागम s[strspn(s, charset)] == '\0';
}

स्थिर अन्तःस्थ bool char_is_cc(वर्ण p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        प्रत्यागम (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ध्रुव वर्ण *p, ध्रुव वर्ण *ok) _pure_;

वर्ण* ellipsize_mem(ध्रुव वर्ण *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
स्थिर अन्तःस्थ वर्ण* ellipsize(ध्रुव वर्ण *s, size_t length, unsigned percent) {
        प्रत्यागम ellipsize_mem(s, strlen(s), length, percent);
}

वर्ण* cellescape(वर्ण *buf, size_t len, ध्रुव वर्ण *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

वर्ण* strshorten(वर्ण *s, size_t l);

पूर्णाङ्क strgrowpad0(वर्ण **s, size_t l);

वर्ण* strreplace(ध्रुव वर्ण *text, ध्रुव वर्ण *old_string, ध्रुव वर्ण *new_string);

वर्ण* strip_tab_ansi(वर्ण **ibuf, size_t *_isz, size_t highlight[2]);

वर्ण* strextendv_with_separator(वर्ण **x, ध्रुव वर्ण *separator, va_list ap);
वर्ण* strextend_with_separator_internal(वर्ण **x, ध्रुव वर्ण *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, रिक्त)
#define strextend(x, ...) strextend_with_separator_internal(x, रिक्त, __VA_ARGS__, रिक्त)

पूर्णाङ्क strextendf_with_separator(वर्ण **x, ध्रुव वर्ण *separator, ध्रुव वर्ण *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, रिक्त, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                वर्ण **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(रिक्त, (separator), __VA_ARGS__, empty_to_null(*_p_), रिक्त); \
                जइ (_s_) {                                                      \
                        मुक्त(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, रिक्त, __VA_ARGS__)

वर्ण* strrep(ध्रुव वर्ण *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ध्रुव वर्ण *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                वर्ण *_d_, *_p_;                                        \
                _p_ = _d_ = newa(वर्ण, _len_ + 1);                      \
                प्रति (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

पूर्णाङ्क split_pair(ध्रुव वर्ण *s, ध्रुव वर्ण *sep, वर्ण **ret_first, वर्ण **ret_second);

पूर्णाङ्क free_and_strdup(वर्ण **p, ध्रुव वर्ण *s);
पूर्णाङ्क free_and_strdup_warn(वर्ण **p, ध्रुव वर्ण *s);
पूर्णाङ्क free_and_strndup(वर्ण **p, ध्रुव वर्ण *s, size_t l) _nonnull_if_nonzero_(2, 3);

पूर्णाङ्क strdup_to_full(वर्ण **ret, ध्रुव वर्ण *src);
स्थिर अन्तःस्थ पूर्णाङ्क strdup_to(वर्ण **ret, ध्रुव वर्ण *src) {
        पूर्णाङ्क r = strdup_to_full(ASSERT_PTR(ret), src);
        प्रत्यागम r < 0 ? r : 0;  /* Suppress return value of 1. */
}

प्रकारनिर्देश गणना StringSafeFlags {
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

bool string_is_safe(ध्रुव वर्ण *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
स्थिर अन्तःस्थ शून्य strncpy_exact(वर्ण *buf, ध्रुव वर्ण *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
स्थिर अन्तःस्थ शून्य* memory_startswith_no_case(ध्रुव शून्य *p, size_t sz, ध्रुव वर्ण *token) {
        प्रतिज्ञा(token);

        size_t n = strlen(token);
        जइ (sz < n)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(p);

        प्रति (size_t i = 0; i < n; i++)
                जइ (ascii_tolower(((वर्ण *)p)[i]) != ascii_tolower(token[i]))
                        प्रत्यागम रिक्त;

        प्रत्यागम (uint8_t*) p + n;
}

वर्ण* str_realloc(वर्ण *p);
वर्ण* string_erase(वर्ण *x);

पूर्णाङ्क string_truncate_पङ्क्तयः(ध्रुव वर्ण *s, size_t n_पङ्क्तयः, वर्ण **ret);
पूर्णाङ्क string_extract_पङ्क्तिः(ध्रुव वर्ण *s, size_t i, वर्ण **ret);

पूर्णाङ्क string_contains_word_strv(ध्रुव वर्ण *string, ध्रुव वर्ण *separators, वर्ण * ध्रुव *शब्दाः, ध्रुव वर्ण **ret_word);
स्थिर अन्तःस्थ पूर्णाङ्क string_contains_word(ध्रुव वर्ण *string, ध्रुव वर्ण *separators, ध्रुव वर्ण *word) {
        प्रत्यागम string_contains_word_strv(string, separators, STRV_MAKE(word), रिक्त);
}

bool streq_skip_trailing_chars(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, ध्रुव वर्ण *ok);

वर्ण* string_replace_char(वर्ण *str, वर्ण old_char, वर्ण new_char);

प्रकारनिर्देश गणना MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

पूर्णाङ्क make_cstring(ध्रुव शून्य *s, size_t n, MakeCStringMode mode, वर्ण **ret);

size_t strspn_from_end(ध्रुव वर्ण *str, ध्रुव वर्ण *accept) _pure_;
size_t strnspn(ध्रुव वर्ण *str, ध्रुव वर्ण *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

वर्ण* strdupspn(ध्रुव वर्ण *a, ध्रुव वर्ण *accept);
वर्ण* strdupcspn(ध्रुव वर्ण *a, ध्रुव वर्ण *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ध्रुव वर्ण *_t = (s);                                   \
                (वर्ण*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ध्रुव वर्ण *_t = (s);                                   \
                (वर्ण*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

वर्ण* find_पङ्क्तिः_startswith_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle);
#define find_पङ्क्तिः_startswith(haystack, needle) \
        const_generic(haystack, find_पङ्क्तिः_startswith_internal(haystack, needle))

वर्ण* find_पङ्क्तिः_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle);
#define find_पङ्क्तिः(haystack, needle) \
        const_generic(haystack, find_पङ्क्तिः_internal(haystack, needle))

वर्ण* find_पङ्क्तिः_after_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle);
#define find_पङ्क्तिः_after(haystack, needle) \
        const_generic(haystack, find_पङ्क्तिः_after_internal(haystack, needle))

प्रकारनिर्देश गणना VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ध्रुव वर्ण *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ध्रुव वर्ण *x, ध्रुव वर्ण *y);

वर्ण* strrstr_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

वर्ण* strrstr_no_case_internal(ध्रुव वर्ण *haystack, ध्रुव वर्ण *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ध्रुव वर्ण *a, ध्रुव वर्ण *b) _pure_;
