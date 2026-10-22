/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "../fundamental/string-util.h" /* IWYU pragma: export */

સ્થિર અંતઃરેખ અક્ષર* strstr_ptr_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        જો (!haystack || !needle)
                પાછા ખાલી;
        પાછા (અક્ષર*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

સ્થિર અંતઃરેખ અક્ષર* strstrafter_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        અક્ષર *p = (અક્ષર*) strstr_ptr(haystack, needle);
        જો (!p)
                પાછા ખાલી;

        પાછા p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

સ્થિર અંતઃરેખ અચલ અક્ષર* strnull(અચલ અક્ષર *s) {
        પાછા s ?: "(null)";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* strna(અચલ અક્ષર *s) {
        પાછા s ?: "n/a";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* true_false(bool b) {
        પાછા b ? "true" : "false";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* plus_minus(bool b) {
        પાછા b ? "+" : "-";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* one_zero(bool b) {
        પાછા b ? "1" : "0";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* enable_disable(bool b) {
        પાછા b ? "enable" : "disable";
}

સ્થિર અંતઃરેખ અચલ અક્ષર* enabled_disabled(bool b) {
        પાછા b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                અચલ અક્ષર *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ખાલી : _p);  \
        })

સ્થિર અંતઃરેખ અચલ અક્ષર* empty_to_na(અચલ અક્ષર *p) {
        પાછા isempty(p) ? "n/a" : p;
}

સ્થિર અંતઃરેખ અચલ અક્ષર* empty_to_dash(અચલ અક્ષર *str) {
        પાછા isempty(str) ? "-" : str;
}

સ્થિર અંતઃરેખ bool empty_or_dash(અચલ અક્ષર *str) {
        પાછા !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

સ્થિર અંતઃરેખ અચલ અક્ષર* empty_or_dash_to_null(અચલ અક્ષર *p) {
        પાછા empty_or_dash(p) ? ખાલી : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                અચલ અક્ષર *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? ખાલી : _p);    \
        })

અક્ષર* first_word(અચલ અક્ષર *s, અચલ અક્ષર *word) _pure_;

અક્ષર* strextendn(અક્ષર **x, અચલ અક્ષર *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(ખાલી, ખાલી, a, __VA_ARGS__, ખાલી)

#define strjoina(a, ...)                                                \
        ({                                                              \
                અચલ અક્ષર *_appendees_[] = { a, __VA_ARGS__ };         \
                અક્ષર *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                માટે (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(અક્ષર, _len_ + 1);                      \
                માટે (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

અક્ષર* strstrip(અક્ષર *s);
અક્ષર* delete_chars(અક્ષર *s, અચલ અક્ષર *bad);
અક્ષર* delete_trailing_chars(અક્ષર *s, અચલ અક્ષર *bad);
અક્ષર* truncate_nl_full(અક્ષર *s, size_t *ret_len);
સ્થિર અંતઃરેખ અક્ષર* truncate_nl(અક્ષર *s) {
        પાછા truncate_nl_full(s, ખાલી);
}

સ્થિર અંતઃરેખ અક્ષર* skip_leading_chars(અચલ અક્ષર *s, અચલ અક્ષર *bad) {
        જો (!s)
                પાછા ખાલી;

        જો (!bad)
                bad = WHITESPACE;

        પાછા (અક્ષર*) s + strspn(s, bad);
}

અક્ષર ascii_tolower(અક્ષર x) _const_;
અક્ષર* ascii_strlower(અક્ષર *s);
અક્ષર* ascii_strlower_n(અક્ષર *s, size_t n);

અક્ષર ascii_toupper(અક્ષર x) _const_;
અક્ષર* ascii_strupper(અક્ષર *s);

પૂર્ણાંક ascii_strcasecmp_n(અચલ અક્ષર *a, અચલ અક્ષર *b, size_t n);
પૂર્ણાંક ascii_strcasecmp_nn(અચલ અક્ષર *a, size_t n, અચલ અક્ષર *b, size_t m);

bool chars_intersect(અચલ અક્ષર *a, અચલ અક્ષર *b) _pure_;

સ્થિર અંતઃરેખ bool _pure_ in_charset(અચલ અક્ષર *s, અચલ અક્ષર *charset) {
        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(charset);
        પાછા s[strspn(s, charset)] == '\0';
}

સ્થિર અંતઃરેખ bool char_is_cc(અક્ષર p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        પાછા (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(અચલ અક્ષર *p, અચલ અક્ષર *ok) _pure_;

અક્ષર* ellipsize_mem(અચલ અક્ષર *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
સ્થિર અંતઃરેખ અક્ષર* ellipsize(અચલ અક્ષર *s, size_t length, unsigned percent) {
        પાછા ellipsize_mem(s, strlen(s), length, percent);
}

અક્ષર* cellescape(અક્ષર *buf, size_t len, અચલ અક્ષર *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

અક્ષર* strshorten(અક્ષર *s, size_t l);

પૂર્ણાંક strgrowpad0(અક્ષર **s, size_t l);

અક્ષર* strreplace(અચલ અક્ષર *text, અચલ અક્ષર *old_string, અચલ અક્ષર *new_string);

અક્ષર* strip_tab_ansi(અક્ષર **ibuf, size_t *_isz, size_t highlight[2]);

અક્ષર* strextendv_with_separator(અક્ષર **x, અચલ અક્ષર *separator, va_list ap);
અક્ષર* strextend_with_separator_internal(અક્ષર **x, અચલ અક્ષર *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, ખાલી)
#define strextend(x, ...) strextend_with_separator_internal(x, ખાલી, __VA_ARGS__, ખાલી)

પૂર્ણાંક strextendf_with_separator(અક્ષર **x, અચલ અક્ષર *separator, અચલ અક્ષર *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, ખાલી, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                અક્ષર **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(ખાલી, (separator), __VA_ARGS__, empty_to_null(*_p_), ખાલી); \
                જો (_s_) {                                                      \
                        મુક્ત(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, ખાલી, __VA_ARGS__)

અક્ષર* strrep(અચલ અક્ષર *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                અચલ અક્ષર *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                અક્ષર *_d_, *_p_;                                        \
                _p_ = _d_ = newa(અક્ષર, _len_ + 1);                      \
                માટે (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

પૂર્ણાંક split_pair(અચલ અક્ષર *s, અચલ અક્ષર *sep, અક્ષર **ret_first, અક્ષર **ret_second);

પૂર્ણાંક free_and_strdup(અક્ષર **p, અચલ અક્ષર *s);
પૂર્ણાંક free_and_strdup_warn(અક્ષર **p, અચલ અક્ષર *s);
પૂર્ણાંક free_and_strndup(અક્ષર **p, અચલ અક્ષર *s, size_t l) _nonnull_if_nonzero_(2, 3);

પૂર્ણાંક strdup_to_full(અક્ષર **ret, અચલ અક્ષર *src);
સ્થિર અંતઃરેખ પૂર્ણાંક strdup_to(અક્ષર **ret, અચલ અક્ષર *src) {
        પૂર્ણાંક r = strdup_to_full(ASSERT_PTR(ret), src);
        પાછા r < 0 ? r : 0;  /* Suppress return value of 1. */
}

પ્રકારવ્યાખ્યા ગણના StringSafeFlags {
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

bool string_is_safe(અચલ અક્ષર *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
સ્થિર અંતઃરેખ શૂન્ય strncpy_exact(અક્ષર *buf, અચલ અક્ષર *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
સ્થિર અંતઃરેખ શૂન્ય* memory_startswith_no_case(અચલ શૂન્ય *p, size_t sz, અચલ અક્ષર *token) {
        પ્રતિજ્ઞા(token);

        size_t n = strlen(token);
        જો (sz < n)
                પાછા ખાલી;

        પ્રતિજ્ઞા(p);

        માટે (size_t i = 0; i < n; i++)
                જો (ascii_tolower(((અક્ષર *)p)[i]) != ascii_tolower(token[i]))
                        પાછા ખાલી;

        પાછા (uint8_t*) p + n;
}

અક્ષર* str_realloc(અક્ષર *p);
અક્ષર* string_erase(અક્ષર *x);

પૂર્ણાંક string_truncate_લીટીઓ(અચલ અક્ષર *s, size_t n_લીટીઓ, અક્ષર **ret);
પૂર્ણાંક string_extract_લીટી(અચલ અક્ષર *s, size_t i, અક્ષર **ret);

પૂર્ણાંક string_contains_word_strv(અચલ અક્ષર *string, અચલ અક્ષર *separators, અક્ષર * અચલ *શબ્દો, અચલ અક્ષર **ret_word);
સ્થિર અંતઃરેખ પૂર્ણાંક string_contains_word(અચલ અક્ષર *string, અચલ અક્ષર *separators, અચલ અક્ષર *word) {
        પાછા string_contains_word_strv(string, separators, STRV_MAKE(word), ખાલી);
}

bool streq_skip_trailing_chars(અચલ અક્ષર *s1, અચલ અક્ષર *s2, અચલ અક્ષર *ok);

અક્ષર* string_replace_char(અક્ષર *str, અક્ષર old_char, અક્ષર new_char);

પ્રકારવ્યાખ્યા ગણના MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

પૂર્ણાંક make_cstring(અચલ શૂન્ય *s, size_t n, MakeCStringMode mode, અક્ષર **ret);

size_t strspn_from_end(અચલ અક્ષર *str, અચલ અક્ષર *accept) _pure_;
size_t strnspn(અચલ અક્ષર *str, અચલ અક્ષર *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

અક્ષર* strdupspn(અચલ અક્ષર *a, અચલ અક્ષર *accept);
અક્ષર* strdupcspn(અચલ અક્ષર *a, અચલ અક્ષર *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                અચલ અક્ષર *_t = (s);                                   \
                (અક્ષર*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                અચલ અક્ષર *_t = (s);                                   \
                (અક્ષર*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

અક્ષર* find_લીટી_startswith_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle);
#define find_લીટી_startswith(haystack, needle) \
        const_generic(haystack, find_લીટી_startswith_internal(haystack, needle))

અક્ષર* find_લીટી_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle);
#define find_લીટી(haystack, needle) \
        const_generic(haystack, find_લીટી_internal(haystack, needle))

અક્ષર* find_લીટી_after_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle);
#define find_લીટી_after(haystack, needle) \
        const_generic(haystack, find_લીટી_after_internal(haystack, needle))

પ્રકારવ્યાખ્યા ગણના VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(અચલ અક્ષર *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(અચલ અક્ષર *x, અચલ અક્ષર *y);

અક્ષર* strrstr_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

અક્ષર* strrstr_no_case_internal(અચલ અક્ષર *haystack, અચલ અક્ષર *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(અચલ અક્ષર *a, અચલ અક્ષર *b) _pure_;
