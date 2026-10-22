/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "forward.h"

#ଅନ୍ତର୍ଭାଵ "../fundamental/string-util.h" /* IWYU pragma: export */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* strstr_ptr_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        ଯଦି (!haystack || !needle)
                ଫେରନ୍ତୁ ଖାଲି;
        ଫେରନ୍ତୁ (ଅକ୍ଷର*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* strstrafter_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        ଅକ୍ଷର *p = (ଅକ୍ଷର*) strstr_ptr(haystack, needle);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* strnull(ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ s ?: "(null)";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* strna(ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ s ?: "n/a";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* true_false(bool b) {
        ଫେରନ୍ତୁ b ? "true" : "false";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* plus_minus(bool b) {
        ଫେରନ୍ତୁ b ? "+" : "-";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* one_zero(bool b) {
        ଫେରନ୍ତୁ b ? "1" : "0";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* enable_disable(bool b) {
        ଫେରନ୍ତୁ b ? "enable" : "disable";
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* enabled_disabled(bool b) {
        ଫେରନ୍ତୁ b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ଅଚଳ ଅକ୍ଷର *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ଖାଲି : _p);  \
        })

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* empty_to_na(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ isempty(p) ? "n/a" : p;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* empty_to_dash(ଅଚଳ ଅକ୍ଷର *str) {
        ଫେରନ୍ତୁ isempty(str) ? "-" : str;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool empty_or_dash(ଅଚଳ ଅକ୍ଷର *str) {
        ଫେରନ୍ତୁ !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* empty_or_dash_to_null(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ empty_or_dash(p) ? ଖାଲି : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ଅଚଳ ଅକ୍ଷର *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? ଖାଲି : _p);    \
        })

ଅକ୍ଷର* first_word(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *word) _pure_;

ଅକ୍ଷର* strextendn(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(ଖାଲି, ଖାଲି, a, __VA_ARGS__, ଖାଲି)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ଅଚଳ ଅକ୍ଷର *_appendees_[] = { a, __VA_ARGS__ };         \
                ଅକ୍ଷର *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                ପାଇଁ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(ଅକ୍ଷର, _len_ + 1);                      \
                ପାଇଁ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ଅକ୍ଷର* strstrip(ଅକ୍ଷର *s);
ଅକ୍ଷର* delete_chars(ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad);
ଅକ୍ଷର* delete_trailing_chars(ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad);
ଅକ୍ଷର* truncate_nl_full(ଅକ୍ଷର *s, size_t *ret_len);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* truncate_nl(ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ truncate_nl_full(s, ଖାଲି);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* skip_leading_chars(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *bad) {
        ଯଦି (!s)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (!bad)
                bad = WHITESPACE;

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) s + strspn(s, bad);
}

ଅକ୍ଷର ascii_tolower(ଅକ୍ଷର x) _const_;
ଅକ୍ଷର* ascii_strlower(ଅକ୍ଷର *s);
ଅକ୍ଷର* ascii_strlower_n(ଅକ୍ଷର *s, size_t n);

ଅକ୍ଷର ascii_toupper(ଅକ୍ଷର x) _const_;
ଅକ୍ଷର* ascii_strupper(ଅକ୍ଷର *s);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ascii_strcasecmp_n(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b, size_t n);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ascii_strcasecmp_nn(ଅଚଳ ଅକ୍ଷର *a, size_t n, ଅଚଳ ଅକ୍ଷର *b, size_t m);

bool chars_intersect(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) _pure_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool _pure_ in_charset(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *charset) {
        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(charset);
        ଫେରନ୍ତୁ s[strspn(s, charset)] == '\0';
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool char_is_cc(ଅକ୍ଷର p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ଫେରନ୍ତୁ (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *ok) _pure_;

ଅକ୍ଷର* ellipsize_mem(ଅଚଳ ଅକ୍ଷର *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* ellipsize(ଅଚଳ ଅକ୍ଷର *s, size_t length, unsigned percent) {
        ଫେରନ୍ତୁ ellipsize_mem(s, strlen(s), length, percent);
}

ଅକ୍ଷର* cellescape(ଅକ୍ଷର *buf, size_t len, ଅଚଳ ଅକ୍ଷର *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

ଅକ୍ଷର* strshorten(ଅକ୍ଷର *s, size_t l);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strgrowpad0(ଅକ୍ଷର **s, size_t l);

ଅକ୍ଷର* strreplace(ଅଚଳ ଅକ୍ଷର *text, ଅଚଳ ଅକ୍ଷର *old_string, ଅଚଳ ଅକ୍ଷର *new_string);

ଅକ୍ଷର* strip_tab_ansi(ଅକ୍ଷର **ibuf, size_t *_isz, size_t highlight[2]);

ଅକ୍ଷର* strextendv_with_separator(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *separator, va_list ap);
ଅକ୍ଷର* strextend_with_separator_internal(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, ଖାଲି)
#define strextend(x, ...) strextend_with_separator_internal(x, ଖାଲି, __VA_ARGS__, ଖାଲି)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strextendf_with_separator(ଅକ୍ଷର **x, ଅଚଳ ଅକ୍ଷର *separator, ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, ଖାଲି, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                ଅକ୍ଷର **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(ଖାଲି, (separator), __VA_ARGS__, empty_to_null(*_p_), ଖାଲି); \
                ଯଦି (_s_) {                                                      \
                        ମୁକ୍ତ(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, ଖାଲି, __VA_ARGS__)

ଅକ୍ଷର* strrep(ଅଚଳ ଅକ୍ଷର *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ଅଚଳ ଅକ୍ଷର *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                ଅକ୍ଷର *_d_, *_p_;                                        \
                _p_ = _d_ = newa(ଅକ୍ଷର, _len_ + 1);                      \
                ପାଇଁ (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ପୂର୍ଣ୍ଣସଂଖ୍ୟା split_pair(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *sep, ଅକ୍ଷର **ret_first, ଅକ୍ଷର **ret_second);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_and_strdup(ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_and_strdup_warn(ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_and_strndup(ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *s, size_t l) _nonnull_if_nonzero_(2, 3);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strdup_to_full(ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *src);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strdup_to(ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *src) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = strdup_to_full(ASSERT_PTR(ret), src);
        ଫେରନ୍ତୁ r < 0 ? r : 0;  /* Suppress return value of 1. */
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା StringSafeFlags {
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

bool string_is_safe(ଅଚଳ ଅକ୍ଷର *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ strncpy_exact(ଅକ୍ଷର *buf, ଅଚଳ ଅକ୍ଷର *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* memory_startswith_no_case(ଅଚଳ ଶୂନ୍ୟ *p, size_t sz, ଅଚଳ ଅକ୍ଷର *token) {
        ପ୍ରତିଜ୍ଞା(token);

        size_t n = strlen(token);
        ଯଦି (sz < n)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(p);

        ପାଇଁ (size_t i = 0; i < n; i++)
                ଯଦି (ascii_tolower(((ଅକ୍ଷର *)p)[i]) != ascii_tolower(token[i]))
                        ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ (uint8_t*) p + n;
}

ଅକ୍ଷର* str_realloc(ଅକ୍ଷର *p);
ଅକ୍ଷର* string_erase(ଅକ୍ଷର *x);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_truncate_ଧାଡ଼ିଗୁଡ଼ିକ(ଅଚଳ ଅକ୍ଷର *s, size_t n_ଧାଡ଼ିଗୁଡ଼ିକ, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_extract_ଧାଡ଼ି(ଅଚଳ ଅକ୍ଷର *s, size_t i, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_contains_word_strv(ଅଚଳ ଅକ୍ଷର *string, ଅଚଳ ଅକ୍ଷର *separators, ଅକ୍ଷର * ଅଚଳ *ଶବ୍ଦଗୁଡ଼ିକ, ଅଚଳ ଅକ୍ଷର **ret_word);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_contains_word(ଅଚଳ ଅକ୍ଷର *string, ଅଚଳ ଅକ୍ଷର *separators, ଅଚଳ ଅକ୍ଷର *word) {
        ଫେରନ୍ତୁ string_contains_word_strv(string, separators, STRV_MAKE(word), ଖାଲି);
}

bool streq_skip_trailing_chars(ଅଚଳ ଅକ୍ଷର *s1, ଅଚଳ ଅକ୍ଷର *s2, ଅଚଳ ଅକ୍ଷର *ok);

ଅକ୍ଷର* string_replace_char(ଅକ୍ଷର *str, ଅକ୍ଷର old_char, ଅକ୍ଷର new_char);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_cstring(ଅଚଳ ଶୂନ୍ୟ *s, size_t n, MakeCStringMode mode, ଅକ୍ଷର **ret);

size_t strspn_from_end(ଅଚଳ ଅକ୍ଷର *str, ଅଚଳ ଅକ୍ଷର *accept) _pure_;
size_t strnspn(ଅଚଳ ଅକ୍ଷର *str, ଅଚଳ ଅକ୍ଷର *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

ଅକ୍ଷର* strdupspn(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *accept);
ଅକ୍ଷର* strdupcspn(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ଅଚଳ ଅକ୍ଷର *_t = (s);                                   \
                (ଅକ୍ଷର*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ଅଚଳ ଅକ୍ଷର *_t = (s);                                   \
                (ଅକ୍ଷର*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

ଅକ୍ଷର* find_ଧାଡ଼ି_startswith_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle);
#define find_ଧାଡ଼ି_startswith(haystack, needle) \
        const_generic(haystack, find_ଧାଡ଼ି_startswith_internal(haystack, needle))

ଅକ୍ଷର* find_ଧାଡ଼ି_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle);
#define find_ଧାଡ଼ି(haystack, needle) \
        const_generic(haystack, find_ଧାଡ଼ି_internal(haystack, needle))

ଅକ୍ଷର* find_ଧାଡ଼ି_after_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle);
#define find_ଧାଡ଼ି_after(haystack, needle) \
        const_generic(haystack, find_ଧାଡ଼ି_after_internal(haystack, needle))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ଅଚଳ ଅକ୍ଷର *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ଅଚଳ ଅକ୍ଷର *x, ଅଚଳ ଅକ୍ଷର *y);

ଅକ୍ଷର* strrstr_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

ଅକ୍ଷର* strrstr_no_case_internal(ଅଚଳ ଅକ୍ଷର *haystack, ଅଚଳ ଅକ୍ଷର *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) _pure_;
