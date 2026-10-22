/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/string-util.h" /* IWYU pragma: export */

স্থির অন্তঃরেখ অক্ষর* strstr_ptr_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        যদি (!haystack || !needle)
                ফেরত ফাঁকা;
        ফেরত (অক্ষর*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

স্থির অন্তঃরেখ অক্ষর* strstrafter_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        অক্ষর *p = (অক্ষর*) strstr_ptr(haystack, needle);
        যদি (!p)
                ফেরত ফাঁকা;

        ফেরত p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* strnull(ধ্রুবক অক্ষর *s) {
        ফেরত s ?: "(null)";
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* strna(ধ্রুবক অক্ষর *s) {
        ফেরত s ?: "n/a";
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* true_false(bool b) {
        ফেরত b ? "true" : "false";
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* plus_minus(bool b) {
        ফেরত b ? "+" : "-";
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* one_zero(bool b) {
        ফেরত b ? "1" : "0";
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* enable_disable(bool b) {
        ফেরত b ? "enable" : "disable";
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* enabled_disabled(bool b) {
        ফেরত b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ধ্রুবক অক্ষর *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ফাঁকা : _p);  \
        })

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* empty_to_na(ধ্রুবক অক্ষর *p) {
        ফেরত isempty(p) ? "n/a" : p;
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* empty_to_dash(ধ্রুবক অক্ষর *str) {
        ফেরত isempty(str) ? "-" : str;
}

স্থির অন্তঃরেখ bool empty_or_dash(ধ্রুবক অক্ষর *str) {
        ফেরত !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* empty_or_dash_to_null(ধ্রুবক অক্ষর *p) {
        ফেরত empty_or_dash(p) ? ফাঁকা : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ধ্রুবক অক্ষর *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? ফাঁকা : _p);    \
        })

অক্ষর* first_word(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *word) _pure_;

অক্ষর* strextendn(অক্ষর **x, ধ্রুবক অক্ষর *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(ফাঁকা, ফাঁকা, a, __VA_ARGS__, ফাঁকা)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ধ্রুবক অক্ষর *_appendees_[] = { a, __VA_ARGS__ };         \
                অক্ষর *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                জন্য (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(অক্ষর, _len_ + 1);                      \
                জন্য (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

অক্ষর* strstrip(অক্ষর *s);
অক্ষর* delete_chars(অক্ষর *s, ধ্রুবক অক্ষর *bad);
অক্ষর* delete_trailing_chars(অক্ষর *s, ধ্রুবক অক্ষর *bad);
অক্ষর* truncate_nl_full(অক্ষর *s, size_t *ret_len);
স্থির অন্তঃরেখ অক্ষর* truncate_nl(অক্ষর *s) {
        ফেরত truncate_nl_full(s, ফাঁকা);
}

স্থির অন্তঃরেখ অক্ষর* skip_leading_chars(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *bad) {
        যদি (!s)
                ফেরত ফাঁকা;

        যদি (!bad)
                bad = WHITESPACE;

        ফেরত (অক্ষর*) s + strspn(s, bad);
}

অক্ষর ascii_tolower(অক্ষর x) _const_;
অক্ষর* ascii_strlower(অক্ষর *s);
অক্ষর* ascii_strlower_n(অক্ষর *s, size_t n);

অক্ষর ascii_toupper(অক্ষর x) _const_;
অক্ষর* ascii_strupper(অক্ষর *s);

পূর্ণসংখ্যা ascii_strcasecmp_n(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b, size_t n);
পূর্ণসংখ্যা ascii_strcasecmp_nn(ধ্রুবক অক্ষর *a, size_t n, ধ্রুবক অক্ষর *b, size_t m);

bool chars_intersect(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) _pure_;

স্থির অন্তঃরেখ bool _pure_ in_charset(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *charset) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(charset);
        ফেরত s[strspn(s, charset)] == '\0';
}

স্থির অন্তঃরেখ bool char_is_cc(অক্ষর p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ফেরত (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *ok) _pure_;

অক্ষর* ellipsize_mem(ধ্রুবক অক্ষর *s, size_t old_দৈর্ঘ্য_bytes, size_t new_দৈর্ঘ্য_columns, unsigned percent);
স্থির অন্তঃরেখ অক্ষর* ellipsize(ধ্রুবক অক্ষর *s, size_t length, unsigned percent) {
        ফেরত ellipsize_mem(s, strlen(s), length, percent);
}

অক্ষর* cellescape(অক্ষর *buf, size_t len, ধ্রুবক অক্ষর *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_দৈর্ঘ্য 64

অক্ষর* strshorten(অক্ষর *s, size_t l);

পূর্ণসংখ্যা strgrowpad0(অক্ষর **s, size_t l);

অক্ষর* strreplace(ধ্রুবক অক্ষর *text, ধ্রুবক অক্ষর *old_string, ধ্রুবক অক্ষর *new_string);

অক্ষর* strip_tab_ansi(অক্ষর **ibuf, size_t *_isz, size_t highlight[2]);

অক্ষর* strextendv_with_separator(অক্ষর **x, ধ্রুবক অক্ষর *separator, va_list ap);
অক্ষর* strextend_with_separator_internal(অক্ষর **x, ধ্রুবক অক্ষর *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, ফাঁকা)
#define strextend(x, ...) strextend_with_separator_internal(x, ফাঁকা, __VA_ARGS__, ফাঁকা)

পূর্ণসংখ্যা strextendf_with_separator(অক্ষর **x, ধ্রুবক অক্ষর *separator, ধ্রুবক অক্ষর *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, ফাঁকা, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                অক্ষর **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(ফাঁকা, (separator), __VA_ARGS__, empty_to_null(*_p_), ফাঁকা); \
                যদি (_s_) {                                                      \
                        মুক্ত(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, ফাঁকা, __VA_ARGS__)

অক্ষর* strrep(ধ্রুবক অক্ষর *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ধ্রুবক অক্ষর *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                অক্ষর *_d_, *_p_;                                        \
                _p_ = _d_ = newa(অক্ষর, _len_ + 1);                      \
                জন্য (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

পূর্ণসংখ্যা split_pair(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *sep, অক্ষর **ret_first, অক্ষর **ret_second);

পূর্ণসংখ্যা free_and_strdup(অক্ষর **p, ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা free_and_strdup_warn(অক্ষর **p, ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা free_and_strndup(অক্ষর **p, ধ্রুবক অক্ষর *s, size_t l) _nonnull_if_nonzero_(2, 3);

পূর্ণসংখ্যা strdup_to_full(অক্ষর **ret, ধ্রুবক অক্ষর *src);
স্থির অন্তঃরেখ পূর্ণসংখ্যা strdup_to(অক্ষর **ret, ধ্রুবক অক্ষর *src) {
        পূর্ণসংখ্যা r = strdup_to_full(ASSERT_PTR(ret), src);
        ফেরত r < 0 ? r : 0;  /* Suppress return value of 1. */
}

প্রকারলেখ গণনা StringSafeFlags {
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

bool string_is_safe(ধ্রুবক অক্ষর *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
স্থির অন্তঃরেখ শূন্য strncpy_exact(অক্ষর *buf, ধ্রুবক অক্ষর *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
স্থির অন্তঃরেখ শূন্য* memory_startswith_no_case(ধ্রুবক শূন্য *p, size_t sz, ধ্রুবক অক্ষর *token) {
        প্রতিজ্ঞা(token);

        size_t n = strlen(token);
        যদি (sz < n)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(p);

        জন্য (size_t i = 0; i < n; i++)
                যদি (ascii_tolower(((অক্ষর *)p)[i]) != ascii_tolower(token[i]))
                        ফেরত ফাঁকা;

        ফেরত (uint8_t*) p + n;
}

অক্ষর* str_realloc(অক্ষর *p);
অক্ষর* string_erase(অক্ষর *x);

পূর্ণসংখ্যা string_truncate_সারিগুলি(ধ্রুবক অক্ষর *s, size_t n_সারিগুলি, অক্ষর **ret);
পূর্ণসংখ্যা string_extract_সারি(ধ্রুবক অক্ষর *s, size_t i, অক্ষর **ret);

পূর্ণসংখ্যা string_contains_word_strv(ধ্রুবক অক্ষর *string, ধ্রুবক অক্ষর *separators, অক্ষর * ধ্রুবক *শব্দগুলি, ধ্রুবক অক্ষর **ret_word);
স্থির অন্তঃরেখ পূর্ণসংখ্যা string_contains_word(ধ্রুবক অক্ষর *string, ধ্রুবক অক্ষর *separators, ধ্রুবক অক্ষর *word) {
        ফেরত string_contains_word_strv(string, separators, STRV_MAKE(word), ফাঁকা);
}

bool streq_skip_trailing_chars(ধ্রুবক অক্ষর *s1, ধ্রুবক অক্ষর *s2, ধ্রুবক অক্ষর *ok);

অক্ষর* string_replace_char(অক্ষর *str, অক্ষর old_char, অক্ষর new_char);

প্রকারলেখ গণনা MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

পূর্ণসংখ্যা make_cstring(ধ্রুবক শূন্য *s, size_t n, MakeCStringMode mode, অক্ষর **ret);

size_t strspn_from_end(ধ্রুবক অক্ষর *str, ধ্রুবক অক্ষর *accept) _pure_;
size_t strnspn(ধ্রুবক অক্ষর *str, ধ্রুবক অক্ষর *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

অক্ষর* strdupspn(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *accept);
অক্ষর* strdupcspn(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ধ্রুবক অক্ষর *_t = (s);                                   \
                (অক্ষর*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ধ্রুবক অক্ষর *_t = (s);                                   \
                (অক্ষর*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

অক্ষর* find_সারি_startswith_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle);
#define find_সারি_startswith(haystack, needle) \
        const_generic(haystack, find_সারি_startswith_internal(haystack, needle))

অক্ষর* find_সারি_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle);
#define find_সারি(haystack, needle) \
        const_generic(haystack, find_সারি_internal(haystack, needle))

অক্ষর* find_সারি_after_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle);
#define find_সারি_after(haystack, needle) \
        const_generic(haystack, find_সারি_after_internal(haystack, needle))

প্রকারলেখ গণনা VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ধ্রুবক অক্ষর *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ধ্রুবক অক্ষর *x, ধ্রুবক অক্ষর *y);

অক্ষর* strrstr_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

অক্ষর* strrstr_no_case_internal(ধ্রুবক অক্ষর *haystack, ধ্রুবক অক্ষর *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) _pure_;
