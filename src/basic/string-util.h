/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/string-util.h" /* IWYU pragma: export */

স্থির অন্তঃস্থ বর্ণ* strstr_ptr_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        যদি (!haystack || !needle)
                প্রত্যাগম রিক্ত;
        প্রত্যাগম (বর্ণ*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

স্থির অন্তঃস্থ বর্ণ* strstrafter_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        বর্ণ *p = (বর্ণ*) strstr_ptr(haystack, needle);
        যদি (!p)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

স্থির অন্তঃস্থ ধ্রুব বর্ণ* strnull(ধ্রুব বর্ণ *s) {
        প্রত্যাগম s ?: "(null)";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* strna(ধ্রুব বর্ণ *s) {
        প্রত্যাগম s ?: "n/a";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* true_false(bool b) {
        প্রত্যাগম b ? "true" : "false";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* plus_minus(bool b) {
        প্রত্যাগম b ? "+" : "-";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* one_zero(bool b) {
        প্রত্যাগম b ? "1" : "0";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* enable_disable(bool b) {
        প্রত্যাগম b ? "enable" : "disable";
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* enabled_disabled(bool b) {
        প্রত্যাগম b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ধ্রুব বর্ণ *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? রিক্ত : _p);  \
        })

স্থির অন্তঃস্থ ধ্রুব বর্ণ* empty_to_na(ধ্রুব বর্ণ *p) {
        প্রত্যাগম isempty(p) ? "n/a" : p;
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* empty_to_dash(ধ্রুব বর্ণ *str) {
        প্রত্যাগম isempty(str) ? "-" : str;
}

স্থির অন্তঃস্থ bool empty_or_dash(ধ্রুব বর্ণ *str) {
        প্রত্যাগম !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* empty_or_dash_to_null(ধ্রুব বর্ণ *p) {
        প্রত্যাগম empty_or_dash(p) ? রিক্ত : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ধ্রুব বর্ণ *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? রিক্ত : _p);    \
        })

বর্ণ* first_word(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *word) _pure_;

বর্ণ* strextendn(বর্ণ **x, ধ্রুব বর্ণ *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(রিক্ত, রিক্ত, a, __VA_ARGS__, রিক্ত)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ধ্রুব বর্ণ *_appendees_[] = { a, __VA_ARGS__ };         \
                বর্ণ *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                প্রতি (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(বর্ণ, _len_ + 1);                      \
                প্রতি (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

বর্ণ* strstrip(বর্ণ *s);
বর্ণ* delete_chars(বর্ণ *s, ধ্রুব বর্ণ *bad);
বর্ণ* delete_trailing_chars(বর্ণ *s, ধ্রুব বর্ণ *bad);
বর্ণ* truncate_nl_full(বর্ণ *s, size_t *ret_len);
স্থির অন্তঃস্থ বর্ণ* truncate_nl(বর্ণ *s) {
        প্রত্যাগম truncate_nl_full(s, রিক্ত);
}

স্থির অন্তঃস্থ বর্ণ* skip_leading_chars(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *bad) {
        যদি (!s)
                প্রত্যাগম রিক্ত;

        যদি (!bad)
                bad = WHITESPACE;

        প্রত্যাগম (বর্ণ*) s + strspn(s, bad);
}

বর্ণ ascii_tolower(বর্ণ x) _const_;
বর্ণ* ascii_strlower(বর্ণ *s);
বর্ণ* ascii_strlower_n(বর্ণ *s, size_t n);

বর্ণ ascii_toupper(বর্ণ x) _const_;
বর্ণ* ascii_strupper(বর্ণ *s);

পূর্ণাঙ্ক ascii_strcasecmp_n(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b, size_t n);
পূর্ণাঙ্ক ascii_strcasecmp_nn(ধ্রুব বর্ণ *a, size_t n, ধ্রুব বর্ণ *b, size_t m);

bool chars_intersect(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) _pure_;

স্থির অন্তঃস্থ bool _pure_ in_charset(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *charset) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(charset);
        প্রত্যাগম s[strspn(s, charset)] == '\0';
}

স্থির অন্তঃস্থ bool char_is_cc(বর্ণ p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        প্রত্যাগম (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *ok) _pure_;

বর্ণ* ellipsize_mem(ধ্রুব বর্ণ *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
স্থির অন্তঃস্থ বর্ণ* ellipsize(ধ্রুব বর্ণ *s, size_t length, unsigned percent) {
        প্রত্যাগম ellipsize_mem(s, strlen(s), length, percent);
}

বর্ণ* cellescape(বর্ণ *buf, size_t len, ধ্রুব বর্ণ *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

বর্ণ* strshorten(বর্ণ *s, size_t l);

পূর্ণাঙ্ক strgrowpad0(বর্ণ **s, size_t l);

বর্ণ* strreplace(ধ্রুব বর্ণ *text, ধ্রুব বর্ণ *old_string, ধ্রুব বর্ণ *new_string);

বর্ণ* strip_tab_ansi(বর্ণ **ibuf, size_t *_isz, size_t highlight[2]);

বর্ণ* strextendv_with_separator(বর্ণ **x, ধ্রুব বর্ণ *separator, va_list ap);
বর্ণ* strextend_with_separator_internal(বর্ণ **x, ধ্রুব বর্ণ *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, রিক্ত)
#define strextend(x, ...) strextend_with_separator_internal(x, রিক্ত, __VA_ARGS__, রিক্ত)

পূর্ণাঙ্ক strextendf_with_separator(বর্ণ **x, ধ্রুব বর্ণ *separator, ধ্রুব বর্ণ *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, রিক্ত, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                বর্ণ **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(রিক্ত, (separator), __VA_ARGS__, empty_to_null(*_p_), রিক্ত); \
                যদি (_s_) {                                                      \
                        মুক্ত(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, রিক্ত, __VA_ARGS__)

বর্ণ* strrep(ধ্রুব বর্ণ *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ধ্রুব বর্ণ *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                বর্ণ *_d_, *_p_;                                        \
                _p_ = _d_ = newa(বর্ণ, _len_ + 1);                      \
                প্রতি (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

পূর্ণাঙ্ক split_pair(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *sep, বর্ণ **ret_first, বর্ণ **ret_second);

পূর্ণাঙ্ক free_and_strdup(বর্ণ **p, ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক free_and_strdup_warn(বর্ণ **p, ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক free_and_strndup(বর্ণ **p, ধ্রুব বর্ণ *s, size_t l) _nonnull_if_nonzero_(2, 3);

পূর্ণাঙ্ক strdup_to_full(বর্ণ **ret, ধ্রুব বর্ণ *src);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strdup_to(বর্ণ **ret, ধ্রুব বর্ণ *src) {
        পূর্ণাঙ্ক r = strdup_to_full(ASSERT_PTR(ret), src);
        প্রত্যাগম r < 0 ? r : 0;  /* Suppress return value of 1. */
}

প্রকারনির্দেশ গণনা StringSafeFlags {
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

bool string_is_safe(ধ্রুব বর্ণ *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
স্থির অন্তঃস্থ শূন্য strncpy_exact(বর্ণ *buf, ধ্রুব বর্ণ *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
স্থির অন্তঃস্থ শূন্য* memory_startswith_no_case(ধ্রুব শূন্য *p, size_t sz, ধ্রুব বর্ণ *token) {
        প্রতিজ্ঞা(token);

        size_t n = strlen(token);
        যদি (sz < n)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(p);

        প্রতি (size_t i = 0; i < n; i++)
                যদি (ascii_tolower(((বর্ণ *)p)[i]) != ascii_tolower(token[i]))
                        প্রত্যাগম রিক্ত;

        প্রত্যাগম (uint8_t*) p + n;
}

বর্ণ* str_realloc(বর্ণ *p);
বর্ণ* string_erase(বর্ণ *x);

পূর্ণাঙ্ক string_truncate_সারিগুলি(ধ্রুব বর্ণ *s, size_t n_সারিগুলি, বর্ণ **ret);
পূর্ণাঙ্ক string_extract_সারি(ধ্রুব বর্ণ *s, size_t i, বর্ণ **ret);

পূর্ণাঙ্ক string_contains_word_strv(ধ্রুব বর্ণ *string, ধ্রুব বর্ণ *separators, বর্ণ * ধ্রুব *শব্দগুলি, ধ্রুব বর্ণ **ret_word);
স্থির অন্তঃস্থ পূর্ণাঙ্ক string_contains_word(ধ্রুব বর্ণ *string, ধ্রুব বর্ণ *separators, ধ্রুব বর্ণ *word) {
        প্রত্যাগম string_contains_word_strv(string, separators, STRV_MAKE(word), রিক্ত);
}

bool streq_skip_trailing_chars(ধ্রুব বর্ণ *s1, ধ্রুব বর্ণ *s2, ধ্রুব বর্ণ *ok);

বর্ণ* string_replace_char(বর্ণ *str, বর্ণ old_char, বর্ণ new_char);

প্রকারনির্দেশ গণনা MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

পূর্ণাঙ্ক make_cstring(ধ্রুব শূন্য *s, size_t n, MakeCStringMode mode, বর্ণ **ret);

size_t strspn_from_end(ধ্রুব বর্ণ *str, ধ্রুব বর্ণ *accept) _pure_;
size_t strnspn(ধ্রুব বর্ণ *str, ধ্রুব বর্ণ *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

বর্ণ* strdupspn(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *accept);
বর্ণ* strdupcspn(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ধ্রুব বর্ণ *_t = (s);                                   \
                (বর্ণ*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ধ্রুব বর্ণ *_t = (s);                                   \
                (বর্ণ*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

বর্ণ* find_সারি_startswith_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle);
#define find_সারি_startswith(haystack, needle) \
        const_generic(haystack, find_সারি_startswith_internal(haystack, needle))

বর্ণ* find_সারি_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle);
#define find_সারি(haystack, needle) \
        const_generic(haystack, find_সারি_internal(haystack, needle))

বর্ণ* find_সারি_after_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle);
#define find_সারি_after(haystack, needle) \
        const_generic(haystack, find_সারি_after_internal(haystack, needle))

প্রকারনির্দেশ গণনা VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ধ্রুব বর্ণ *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ধ্রুব বর্ণ *x, ধ্রুব বর্ণ *y);

বর্ণ* strrstr_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

বর্ণ* strrstr_no_case_internal(ধ্রুব বর্ণ *haystack, ধ্রুব বর্ণ *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) _pure_;
