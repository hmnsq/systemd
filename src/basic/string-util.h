/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "forward.h"

#অন্তর্ভাৱ "../fundamental/string-util.h" /* IWYU pragma: export */

স্থির অন্তঃস্থ ৱর্ণ* strstr_ptr_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle) {
        যদি (!haystack || !needle)
                ঘূৰাই রিক্ত;
        ঘূৰাই (ৱর্ণ*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

স্থির অন্তঃস্থ ৱর্ণ* strstrafter_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        ৱর্ণ *p = (ৱর্ণ*) strstr_ptr(haystack, needle);
        যদি (!p)
                ঘূৰাই রিক্ত;

        ঘূৰাই p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* strnull(ধ্রুৱ ৱর্ণ *s) {
        ঘূৰাই s ?: "(null)";
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* strna(ধ্রুৱ ৱর্ণ *s) {
        ঘূৰাই s ?: "n/a";
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* true_false(bool b) {
        ঘূৰাই b ? "true" : "false";
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* plus_minus(bool b) {
        ঘূৰাই b ? "+" : "-";
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* one_zero(bool b) {
        ঘূৰাই b ? "1" : "0";
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* enable_disable(bool b) {
        ঘূৰাই b ? "enable" : "disable";
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* enabled_disabled(bool b) {
        ঘূৰাই b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                ধ্রুৱ ৱর্ণ *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? রিক্ত : _p);  \
        })

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* empty_to_na(ধ্রুৱ ৱর্ণ *p) {
        ঘূৰাই isempty(p) ? "n/a" : p;
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* empty_to_dash(ধ্রুৱ ৱর্ণ *str) {
        ঘূৰাই isempty(str) ? "-" : str;
}

স্থির অন্তঃস্থ bool empty_or_dash(ধ্রুৱ ৱর্ণ *str) {
        ঘূৰাই !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* empty_or_dash_to_null(ধ্রুৱ ৱর্ণ *p) {
        ঘূৰাই empty_or_dash(p) ? রিক্ত : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                ধ্রুৱ ৱর্ণ *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? রিক্ত : _p);    \
        })

ৱর্ণ* first_word(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *word) _pure_;

ৱর্ণ* strextendn(ৱর্ণ **x, ধ্রুৱ ৱর্ণ *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(রিক্ত, রিক্ত, a, __VA_ARGS__, রিক্ত)

#define strjoina(a, ...)                                                \
        ({                                                              \
                ধ্রুৱ ৱর্ণ *_appendees_[] = { a, __VA_ARGS__ };         \
                ৱর্ণ *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                বাবে (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(ৱর্ণ, _len_ + 1);                      \
                বাবে (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ৱর্ণ* strstrip(ৱর্ণ *s);
ৱর্ণ* delete_chars(ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad);
ৱর্ণ* delete_trailing_chars(ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad);
ৱর্ণ* truncate_nl_full(ৱর্ণ *s, size_t *ret_len);
স্থির অন্তঃস্থ ৱর্ণ* truncate_nl(ৱর্ণ *s) {
        ঘূৰাই truncate_nl_full(s, রিক্ত);
}

স্থির অন্তঃস্থ ৱর্ণ* skip_leading_chars(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *bad) {
        যদি (!s)
                ঘূৰাই রিক্ত;

        যদি (!bad)
                bad = WHITESPACE;

        ঘূৰাই (ৱর্ণ*) s + strspn(s, bad);
}

ৱর্ণ ascii_tolower(ৱর্ণ x) _const_;
ৱর্ণ* ascii_strlower(ৱর্ণ *s);
ৱর্ণ* ascii_strlower_n(ৱর্ণ *s, size_t n);

ৱর্ণ ascii_toupper(ৱর্ণ x) _const_;
ৱর্ণ* ascii_strupper(ৱর্ণ *s);

পূর্ণাঙ্ক ascii_strcasecmp_n(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b, size_t n);
পূর্ণাঙ্ক ascii_strcasecmp_nn(ধ্রুৱ ৱর্ণ *a, size_t n, ধ্রুৱ ৱর্ণ *b, size_t m);

bool chars_intersect(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) _pure_;

স্থির অন্তঃস্থ bool _pure_ in_charset(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *charset) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(charset);
        ঘূৰাই s[strspn(s, charset)] == '\0';
}

স্থির অন্তঃস্থ bool char_is_cc(ৱর্ণ p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ঘূৰাই (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *ok) _pure_;

ৱর্ণ* ellipsize_mem(ধ্রুৱ ৱর্ণ *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
স্থির অন্তঃস্থ ৱর্ণ* ellipsize(ধ্রুৱ ৱর্ণ *s, size_t length, unsigned percent) {
        ঘূৰাই ellipsize_mem(s, strlen(s), length, percent);
}

ৱর্ণ* cellescape(ৱর্ণ *buf, size_t len, ধ্রুৱ ৱর্ণ *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

ৱর্ণ* strshorten(ৱর্ণ *s, size_t l);

পূর্ণাঙ্ক strgrowpad0(ৱর্ণ **s, size_t l);

ৱর্ণ* strreplace(ধ্রুৱ ৱর্ণ *text, ধ্রুৱ ৱর্ণ *old_string, ধ্রুৱ ৱর্ণ *new_string);

ৱর্ণ* strip_tab_ansi(ৱর্ণ **ibuf, size_t *_isz, size_t highlight[2]);

ৱর্ণ* strextendv_with_separator(ৱর্ণ **x, ধ্রুৱ ৱর্ণ *separator, va_list ap);
ৱর্ণ* strextend_with_separator_internal(ৱর্ণ **x, ধ্রুৱ ৱর্ণ *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, রিক্ত)
#define strextend(x, ...) strextend_with_separator_internal(x, রিক্ত, __VA_ARGS__, রিক্ত)

পূর্ণাঙ্ক strextendf_with_separator(ৱর্ণ **x, ধ্রুৱ ৱর্ণ *separator, ধ্রুৱ ৱর্ণ *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, রিক্ত, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                ৱর্ণ **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(রিক্ত, (separator), __VA_ARGS__, empty_to_null(*_p_), রিক্ত); \
                যদি (_s_) {                                                      \
                        মুক্ত(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, রিক্ত, __VA_ARGS__)

ৱর্ণ* strrep(ধ্রুৱ ৱর্ণ *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                ধ্রুৱ ৱর্ণ *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                ৱর্ণ *_d_, *_p_;                                        \
                _p_ = _d_ = newa(ৱর্ণ, _len_ + 1);                      \
                বাবে (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

পূর্ণাঙ্ক split_pair(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *sep, ৱর্ণ **ret_first, ৱর্ণ **ret_second);

পূর্ণাঙ্ক free_and_strdup(ৱর্ণ **p, ধ্রুৱ ৱর্ণ *s);
পূর্ণাঙ্ক free_and_strdup_warn(ৱর্ণ **p, ধ্রুৱ ৱর্ণ *s);
পূর্ণাঙ্ক free_and_strndup(ৱর্ণ **p, ধ্রুৱ ৱর্ণ *s, size_t l) _nonnull_if_nonzero_(2, 3);

পূর্ণাঙ্ক strdup_to_full(ৱর্ণ **ret, ধ্রুৱ ৱর্ণ *src);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strdup_to(ৱর্ণ **ret, ধ্রুৱ ৱর্ণ *src) {
        পূর্ণাঙ্ক r = strdup_to_full(ASSERT_PTR(ret), src);
        ঘূৰাই r < 0 ? r : 0;  /* Suppress return value of 1. */
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

bool string_is_safe(ধ্রুৱ ৱর্ণ *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
স্থির অন্তঃস্থ শূন্য strncpy_exact(ৱর্ণ *buf, ধ্রুৱ ৱর্ণ *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
স্থির অন্তঃস্থ শূন্য* memory_startswith_no_case(ধ্রুৱ শূন্য *p, size_t sz, ধ্রুৱ ৱর্ণ *token) {
        প্রতিজ্ঞা(token);

        size_t n = strlen(token);
        যদি (sz < n)
                ঘূৰাই রিক্ত;

        প্রতিজ্ঞা(p);

        বাবে (size_t i = 0; i < n; i++)
                যদি (ascii_tolower(((ৱর্ণ *)p)[i]) != ascii_tolower(token[i]))
                        ঘূৰাই রিক্ত;

        ঘূৰাই (uint8_t*) p + n;
}

ৱর্ণ* str_realloc(ৱর্ণ *p);
ৱর্ণ* string_erase(ৱর্ণ *x);

পূর্ণাঙ্ক string_truncate_সাৰিগুলি(ধ্রুৱ ৱর্ণ *s, size_t n_সাৰিগুলি, ৱর্ণ **ret);
পূর্ণাঙ্ক string_extract_সাৰি(ধ্রুৱ ৱর্ণ *s, size_t i, ৱর্ণ **ret);

পূর্ণাঙ্ক string_contains_word_strv(ধ্রুৱ ৱর্ণ *string, ধ্রুৱ ৱর্ণ *separators, ৱর্ণ * ধ্রুৱ *শব্দগুলি, ধ্রুৱ ৱর্ণ **ret_word);
স্থির অন্তঃস্থ পূর্ণাঙ্ক string_contains_word(ধ্রুৱ ৱর্ণ *string, ধ্রুৱ ৱর্ণ *separators, ধ্রুৱ ৱর্ণ *word) {
        ঘূৰাই string_contains_word_strv(string, separators, STRV_MAKE(word), রিক্ত);
}

bool streq_skip_trailing_chars(ধ্রুৱ ৱর্ণ *s1, ধ্রুৱ ৱর্ণ *s2, ধ্রুৱ ৱর্ণ *ok);

ৱর্ণ* string_replace_char(ৱর্ণ *str, ৱর্ণ old_char, ৱর্ণ new_char);

প্রকারনির্দেশ গণনা MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

পূর্ণাঙ্ক make_cstring(ধ্রুৱ শূন্য *s, size_t n, MakeCStringMode mode, ৱর্ণ **ret);

size_t strspn_from_end(ধ্রুৱ ৱর্ণ *str, ধ্রুৱ ৱর্ণ *accept) _pure_;
size_t strnspn(ধ্রুৱ ৱর্ণ *str, ধ্রুৱ ৱর্ণ *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

ৱর্ণ* strdupspn(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *accept);
ৱর্ণ* strdupcspn(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                ধ্রুৱ ৱর্ণ *_t = (s);                                   \
                (ৱর্ণ*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                ধ্রুৱ ৱর্ণ *_t = (s);                                   \
                (ৱর্ণ*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

ৱর্ণ* find_সাৰি_startswith_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle);
#define find_সাৰি_startswith(haystack, needle) \
        const_generic(haystack, find_সাৰি_startswith_internal(haystack, needle))

ৱর্ণ* find_সাৰি_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle);
#define find_সাৰি(haystack, needle) \
        const_generic(haystack, find_সাৰি_internal(haystack, needle))

ৱর্ণ* find_সাৰি_after_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle);
#define find_সাৰি_after(haystack, needle) \
        const_generic(haystack, find_সাৰি_after_internal(haystack, needle))

প্রকারনির্দেশ গণনা VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(ধ্রুৱ ৱর্ণ *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(ধ্রুৱ ৱর্ণ *x, ধ্রুৱ ৱর্ণ *y);

ৱর্ণ* strrstr_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

ৱর্ণ* strrstr_no_case_internal(ধ্রুৱ ৱর্ণ *haystack, ধ্রুৱ ৱর্ণ *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) _pure_;
