/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "forward.h"

#ضمن "../fundamental/string-util.h" /* IWYU pragma: export */

ساكن مضمن حرف* strstr_ptr_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        إذا (!haystack || !needle)
                أرجع عدم;
        أرجع (حرف*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

ساكن مضمن حرف* strstrafter_internal(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        حرف *p = (حرف*) strstr_ptr(haystack, needle);
        إذا (!p)
                أرجع عدم;

        أرجع p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

ساكن مضمن ثابت حرف* strnull(ثابت حرف *s) {
        أرجع s ?: "(null)";
}

ساكن مضمن ثابت حرف* strna(ثابت حرف *s) {
        أرجع s ?: "n/a";
}

ساكن مضمن ثابت حرف* true_false(bool b) {
        أرجع b ? "true" : "false";
}

ساكن مضمن ثابت حرف* plus_minus(bool b) {
        أرجع b ? "+" : "-";
}

ساكن مضمن ثابت حرف* one_zero(bool b) {
        أرجع b ? "1" : "0";
}

ساكن مضمن ثابت حرف* enable_disable(bool b) {
        أرجع b ? "enable" : "disable";
}

ساكن مضمن ثابت حرف* enabled_disabled(bool b) {
        أرجع b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ثابت حرف *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? عدم : _p);  \
        })

ساكن مضمن ثابت حرف* empty_to_na(ثابت حرف *p) {
        أرجع isempty(p) ? "n/a" : p;
}

ساكن مضمن ثابت حرف* empty_to_dash(ثابت حرف *str) {
        أرجع isempty(str) ? "-" : str;
}

ساكن مضمن bool empty_or_dash(ثابت حرف *str) {
        أرجع !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ساكن مضمن ثابت حرف* empty_or_dash_to_null(ثابت حرف *p) {
        أرجع empty_or_dash(p) ? عدم : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ثابت حرف *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? عدم : _p);    \
        })

حرف* first_word(ثابت حرف *s, ثابت حرف *word) _pure_;

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(عدم, عدم, a, __VA_ARGS__, عدم)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ثابت حرف *_appendees_[] = { a, __VA_ARGS__ };         \
                حرف *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                لكل (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                لكل (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

حرف* strstrip(حرف *s);
حرف* delete_chars(حرف *s, ثابت حرف *bad);
حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad);
حرف* truncate_nl_full(حرف *s, size_t *ret_len);
ساكن مضمن حرف* truncate_nl(حرف *s) {
        أرجع truncate_nl_full(s, عدم);
}

ساكن مضمن حرف* skip_leading_chars(ثابت حرف *s, ثابت حرف *bad) {
        إذا (!s)
                أرجع عدم;

        إذا (!bad)
                bad = WHITESPACE;

        أرجع (حرف*) s + strspn(s, bad);
}

حرف ascii_tolower(حرف x) _const_;
حرف* ascii_strlower(حرف *s);
حرف* ascii_strlower_n(حرف *s, size_t n);

حرف ascii_toupper(حرف x) _const_;
حرف* ascii_strupper(حرف *s);

صحيح ascii_strcasecmp_n(ثابت حرف *a, ثابت حرف *b, size_t n);
صحيح ascii_strcasecmp_nn(ثابت حرف *a, size_t n, ثابت حرف *b, size_t m);

bool chars_intersect(ثابت حرف *a, ثابت حرف *b) _pure_;

ساكن مضمن bool _pure_ in_charset(ثابت حرف *s, ثابت حرف *charset) {
        أكد(s);
        أكد(charset);
        أرجع s[strspn(s, charset)] == '\0';
}

ساكن مضمن bool char_is_cc(حرف p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        أرجع (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ثابت حرف *p, ثابت حرف *ok) _pure_;

حرف* ellipsize_mem(ثابت حرف *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
ساكن مضمن حرف* ellipsize(ثابت حرف *s, size_t length, unsigned percent) {
        أرجع ellipsize_mem(s, strlen(s), length, percent);
}

حرف* cellescape(حرف *buf, size_t len, ثابت حرف *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

حرف* strshorten(حرف *s, size_t l);

صحيح strgrowpad0(حرف **s, size_t l);

حرف* strreplace(ثابت حرف *text, ثابت حرف *old_string, ثابت حرف *new_string);

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]);

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap);
حرف* strextend_with_separator_internal(حرف **x, ثابت حرف *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, عدم)
#define strextend(x, ...) strextend_with_separator_internal(x, عدم, __VA_ARGS__, عدم)

صحيح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, عدم, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                حرف **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(عدم, (separator), __VA_ARGS__, empty_to_null(*_p_), عدم); \
                إذا (_s_) {                                                      \
                        حرر(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, عدم, __VA_ARGS__)

حرف* strrep(ثابت حرف *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ثابت حرف *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                حرف *_d_, *_p_;                                        \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                لكل (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

صحيح split_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_first, حرف **ret_second);

صحيح free_and_strdup(حرف **p, ثابت حرف *s);
صحيح free_and_strdup_warn(حرف **p, ثابت حرف *s);
صحيح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

صحيح strdup_to_full(حرف **ret, ثابت حرف *src);
ساكن مضمن صحيح strdup_to(حرف **ret, ثابت حرف *src) {
        صحيح r = strdup_to_full(ASSERT_PTR(ret), src);
        أرجع r < 0 ? r : 0;  /* Suppress return value of 1. */
}

تعريفنوع تعداد StringSafeFlags {
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

bool string_is_safe(ثابت حرف *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ساكن مضمن فراغ strncpy_exact(حرف *buf, ثابت حرف *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ساكن مضمن فراغ* memory_startswith_no_case(ثابت فراغ *p, size_t sz, ثابت حرف *token) {
        أكد(token);

        size_t n = strlen(token);
        إذا (sz < n)
                أرجع عدم;

        أكد(p);

        لكل (size_t i = 0; i < n; i++)
                إذا (ascii_tolower(((حرف *)p)[i]) != ascii_tolower(token[i]))
                        أرجع عدم;

        أرجع (uint8_t*) p + n;
}

حرف* str_realloc(حرف *p);
حرف* string_erase(حرف *x);

صحيح string_truncate_قۇرلار(ثابت حرف *s, size_t n_قۇرلار, حرف **ret);
صحيح string_extract_قۇر(ثابت حرف *s, size_t i, حرف **ret);

صحيح string_contains_word_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *سۆزلەر, ثابت حرف **ret_word);
ساكن مضمن صحيح string_contains_word(ثابت حرف *string, ثابت حرف *separators, ثابت حرف *word) {
        أرجع string_contains_word_strv(string, separators, STRV_MAKE(word), عدم);
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok);

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char);

تعريفنوع تعداد MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

صحيح make_cstring(ثابت فراغ *s, size_t n, MakeCStringMode mode, حرف **ret);

size_t strspn_from_end(ثابت حرف *str, ثابت حرف *accept) _pure_;
size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept);
حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

حرف* find_قۇر_startswith_internal(ثابت حرف *haystack, ثابت حرف *needle);
#define find_قۇر_startswith(haystack, needle) \
        const_generic(haystack, find_قۇر_startswith_internal(haystack, needle))

حرف* find_قۇر_internal(ثابت حرف *haystack, ثابت حرف *needle);
#define find_قۇر(haystack, needle) \
        const_generic(haystack, find_قۇر_internal(haystack, needle))

حرف* find_قۇر_after_internal(ثابت حرف *haystack, ثابت حرف *needle);
#define find_قۇر_after(haystack, needle) \
        const_generic(haystack, find_قۇر_after_internal(haystack, needle))

تعريفنوع تعداد VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ثابت حرف *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y);

حرف* strrstr_internal(ثابت حرف *haystack, ثابت حرف *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

حرف* strrstr_no_case_internal(ثابت حرف *haystack, ثابت حرف *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ثابت حرف *a, ثابت حرف *b) _pure_;
