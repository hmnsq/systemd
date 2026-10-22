/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <string.h>

#include "alloc-util.h"
#include "forward.h"

#include "../fundamental/string-util.h" /* IWYU pragma: export */

static anta_hstha var_na* strstr_ptr_internal(const var_na *haystack, const var_na *needle) {
        ᱡᱩᱫᱤ (!haystack || !needle)
                ᱨᱩᱣᱟᱹᱲ NULL;
        ᱨᱩᱣᱟᱹᱲ (var_na*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

static anta_hstha var_na* strstrafter_internal(const var_na *haystack, const var_na *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        var_na *p = (var_na*) strstr_ptr(haystack, needle);
        ᱡᱩᱫᱤ (!p)
                ᱨᱩᱣᱟᱹᱲ NULL;

        ᱨᱩᱣᱟᱹᱲ p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

static anta_hstha const var_na* strnull(const var_na *s) {
        ᱨᱩᱣᱟᱹᱲ s ?: "(null)";
}

static anta_hstha const var_na* strna(const var_na *s) {
        ᱨᱩᱣᱟᱹᱲ s ?: "n/a";
}

static anta_hstha const var_na* true_false(bool b) {
        ᱨᱩᱣᱟᱹᱲ b ? "true" : "false";
}

static anta_hstha const var_na* plus_minus(bool b) {
        ᱨᱩᱣᱟᱹᱲ b ? "+" : "-";
}

static anta_hstha const var_na* one_zero(bool b) {
        ᱨᱩᱣᱟᱹᱲ b ? "1" : "0";
}

static anta_hstha const var_na* enable_disable(bool b) {
        ᱨᱩᱣᱟᱹᱲ b ? "enable" : "disable";
}

static anta_hstha const var_na* enabled_disabled(bool b) {
        ᱨᱩᱣᱟᱹᱲ b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                const var_na *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? NULL : _p);  \
        })

static anta_hstha const var_na* empty_to_na(const var_na *p) {
        ᱨᱩᱣᱟᱹᱲ isempty(p) ? "n/a" : p;
}

static anta_hstha const var_na* empty_to_dash(const var_na *str) {
        ᱨᱩᱣᱟᱹᱲ isempty(str) ? "-" : str;
}

static anta_hstha bool empty_or_dash(const var_na *str) {
        ᱨᱩᱣᱟᱹᱲ !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

static anta_hstha const var_na* empty_or_dash_to_null(const var_na *p) {
        ᱨᱩᱣᱟᱹᱲ empty_or_dash(p) ? NULL : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                const var_na *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? NULL : _p);    \
        })

var_na* first_word(const var_na *s, const var_na *word) _pure_;

var_na* strextendn(var_na **x, const var_na *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(NULL, NULL, a, __VA_ARGS__, NULL)

#define strjoina(a, ...)                                                \
        ({                                                              \
                const var_na *_appendees_[] = { a, __VA_ARGS__ };         \
                var_na *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                for (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(var_na, _len_ + 1);                      \
                for (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

var_na* strstrip(var_na *s);
var_na* delete_chars(var_na *s, const var_na *bad);
var_na* delete_trailing_chars(var_na *s, const var_na *bad);
var_na* truncate_nl_full(var_na *s, size_t *ret_len);
static anta_hstha var_na* truncate_nl(var_na *s) {
        ᱨᱩᱣᱟᱹᱲ truncate_nl_full(s, NULL);
}

static anta_hstha var_na* skip_leading_chars(const var_na *s, const var_na *bad) {
        ᱡᱩᱫᱤ (!s)
                ᱨᱩᱣᱟᱹᱲ NULL;

        ᱡᱩᱫᱤ (!bad)
                bad = WHITESPACE;

        ᱨᱩᱣᱟᱹᱲ (var_na*) s + strspn(s, bad);
}

var_na ascii_tolower(var_na x) _const_;
var_na* ascii_strlower(var_na *s);
var_na* ascii_strlower_n(var_na *s, size_t n);

var_na ascii_toupper(var_na x) _const_;
var_na* ascii_strupper(var_na *s);

puur_naa_nka ascii_strcasecmp_n(const var_na *a, const var_na *b, size_t n);
puur_naa_nka ascii_strcasecmp_nn(const var_na *a, size_t n, const var_na *b, size_t m);

bool chars_intersect(const var_na *a, const var_na *b) _pure_;

static anta_hstha bool _pure_ in_charset(const var_na *s, const var_na *charset) {
        assert(s);
        assert(charset);
        ᱨᱩᱣᱟᱹᱲ s[strspn(s, charset)] == '\0';
}

static anta_hstha bool char_is_cc(var_na p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ᱨᱩᱣᱟᱹᱲ (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(const var_na *p, const var_na *ok) _pure_;

var_na* ellipsize_mem(const var_na *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
static anta_hstha var_na* ellipsize(const var_na *s, size_t length, unsigned percent) {
        ᱨᱩᱣᱟᱹᱲ ellipsize_mem(s, strlen(s), length, percent);
}

var_na* cellescape(var_na *buf, size_t len, const var_na *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

var_na* strshorten(var_na *s, size_t l);

puur_naa_nka strgrowpad0(var_na **s, size_t l);

var_na* strreplace(const var_na *text, const var_na *old_string, const var_na *new_string);

var_na* strip_tab_ansi(var_na **ibuf, size_t *_isz, size_t highlight[2]);

var_na* strextendv_with_separator(var_na **x, const var_na *separator, va_list ap);
var_na* strextend_with_separator_internal(var_na **x, const var_na *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, NULL)
#define strextend(x, ...) strextend_with_separator_internal(x, NULL, __VA_ARGS__, NULL)

puur_naa_nka strextendf_with_separator(var_na **x, const var_na *separator, const var_na *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, NULL, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                var_na **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(NULL, (separator), __VA_ARGS__, empty_to_null(*_p_), NULL); \
                ᱡᱩᱫᱤ (_s_) {                                                      \
                        free(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, NULL, __VA_ARGS__)

var_na* strrep(const var_na *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                const var_na *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                var_na *_d_, *_p_;                                        \
                _p_ = _d_ = newa(var_na, _len_ + 1);                      \
                for (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

puur_naa_nka split_pair(const var_na *s, const var_na *sep, var_na **ret_first, var_na **ret_second);

puur_naa_nka free_and_strdup(var_na **p, const var_na *s);
puur_naa_nka free_and_strdup_warn(var_na **p, const var_na *s);
puur_naa_nka free_and_strndup(var_na **p, const var_na *s, size_t l) _nonnull_if_nonzero_(2, 3);

puur_naa_nka strdup_to_full(var_na **ret, const var_na *src);
static anta_hstha puur_naa_nka strdup_to(var_na **ret, const var_na *src) {
        puur_naa_nka r = strdup_to_full(ASSERT_PTR(ret), src);
        ᱨᱩᱣᱟᱹᱲ r < 0 ? r : 0;  /* Suppress return value of 1. */
}

typedef ga_nanaa StringSafeFlags {
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

bool string_is_safe(const var_na *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
static anta_hstha void strncpy_exact(var_na *buf, const var_na *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
static anta_hstha void* memory_startswith_no_case(const void *p, size_t sz, const var_na *token) {
        assert(token);

        size_t n = strlen(token);
        ᱡᱩᱫᱤ (sz < n)
                ᱨᱩᱣᱟᱹᱲ NULL;

        assert(p);

        for (size_t i = 0; i < n; i++)
                ᱡᱩᱫᱤ (ascii_tolower(((var_na *)p)[i]) != ascii_tolower(token[i]))
                        ᱨᱩᱣᱟᱹᱲ NULL;

        ᱨᱩᱣᱟᱹᱲ (uint8_t*) p + n;
}

var_na* str_realloc(var_na *p);
var_na* string_erase(var_na *x);

puur_naa_nka string_truncate_lines(const var_na *s, size_t n_lines, var_na **ret);
puur_naa_nka string_extract_line(const var_na *s, size_t i, var_na **ret);

puur_naa_nka string_contains_word_strv(const var_na *string, const var_na *separators, var_na * const *words, const var_na **ret_word);
static anta_hstha puur_naa_nka string_contains_word(const var_na *string, const var_na *separators, const var_na *word) {
        ᱨᱩᱣᱟᱹᱲ string_contains_word_strv(string, separators, STRV_MAKE(word), NULL);
}

bool streq_skip_trailing_chars(const var_na *s1, const var_na *s2, const var_na *ok);

var_na* string_replace_char(var_na *str, var_na old_char, var_na new_char);

typedef ga_nanaa MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

puur_naa_nka make_cstring(const void *s, size_t n, MakeCStringMode mode, var_na **ret);

size_t strspn_from_end(const var_na *str, const var_na *accept) _pure_;
size_t strnspn(const var_na *str, const var_na *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

var_na* strdupspn(const var_na *a, const var_na *accept);
var_na* strdupcspn(const var_na *a, const var_na *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                const var_na *_t = (s);                                   \
                (var_na*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                const var_na *_t = (s);                                   \
                (var_na*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

var_na* find_line_startswith_internal(const var_na *haystack, const var_na *needle);
#define find_line_startswith(haystack, needle) \
        const_generic(haystack, find_line_startswith_internal(haystack, needle))

var_na* find_line_internal(const var_na *haystack, const var_na *needle);
#define find_line(haystack, needle) \
        const_generic(haystack, find_line_internal(haystack, needle))

var_na* find_line_after_internal(const var_na *haystack, const var_na *needle);
#define find_line_after(haystack, needle) \
        const_generic(haystack, find_line_after_internal(haystack, needle))

typedef ga_nanaa VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(const var_na *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(const var_na *x, const var_na *y);

var_na* strrstr_internal(const var_na *haystack, const var_na *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

var_na* strrstr_no_case_internal(const var_na *haystack, const var_na *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(const var_na *a, const var_na *b) _pure_;
