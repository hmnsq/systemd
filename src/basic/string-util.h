/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "../fundamental/string-util.h" /* IWYU pragma: export */

സ്ഥിതം വരിയിൽ അക്ഷരം* strstr_ptr_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        എങ്കിൽ (!haystack || !needle)
                തിരികെ ശൂന്യ;
        തിരികെ (അക്ഷരം*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

സ്ഥിതം വരിയിൽ അക്ഷരം* strstrafter_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        അക്ഷരം *p = (അക്ഷരം*) strstr_ptr(haystack, needle);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        തിരികെ p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* strnull(സ്ഥിരം അക്ഷരം *s) {
        തിരികെ s ?: "(null)";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* strna(സ്ഥിരം അക്ഷരം *s) {
        തിരികെ s ?: "n/a";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* true_false(bool b) {
        തിരികെ b ? "true" : "false";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* plus_minus(bool b) {
        തിരികെ b ? "+" : "-";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* one_zero(bool b) {
        തിരികെ b ? "1" : "0";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* enable_disable(bool b) {
        തിരികെ b ? "enable" : "disable";
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* enabled_disabled(bool b) {
        തിരികെ b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                സ്ഥിരം അക്ഷരം *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ശൂന്യ : _p);  \
        })

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* empty_to_na(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ isempty(p) ? "n/a" : p;
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* empty_to_dash(സ്ഥിരം അക്ഷരം *str) {
        തിരികെ isempty(str) ? "-" : str;
}

സ്ഥിതം വരിയിൽ bool empty_or_dash(സ്ഥിരം അക്ഷരം *str) {
        തിരികെ !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* empty_or_dash_to_null(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ empty_or_dash(p) ? ശൂന്യ : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                സ്ഥിരം അക്ഷരം *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? ശൂന്യ : _p);    \
        })

അക്ഷരം* first_word(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *word) _pure_;

അക്ഷരം* strextendn(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(ശൂന്യ, ശൂന്യ, a, __VA_ARGS__, ശൂന്യ)

#define strjoina(a, ...)                                                \
        ({                                                              \
                സ്ഥിരം അക്ഷരം *_appendees_[] = { a, __VA_ARGS__ };         \
                അക്ഷരം *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                വേണ്ടി (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(അക്ഷരം, _len_ + 1);                      \
                വേണ്ടി (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

അക്ഷരം* strstrip(അക്ഷരം *s);
അക്ഷരം* delete_chars(അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad);
അക്ഷരം* delete_trailing_chars(അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad);
അക്ഷരം* truncate_nl_full(അക്ഷരം *s, size_t *ret_len);
സ്ഥിതം വരിയിൽ അക്ഷരം* truncate_nl(അക്ഷരം *s) {
        തിരികെ truncate_nl_full(s, ശൂന്യ);
}

സ്ഥിതം വരിയിൽ അക്ഷരം* skip_leading_chars(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *bad) {
        എങ്കിൽ (!s)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (!bad)
                bad = WHITESPACE;

        തിരികെ (അക്ഷരം*) s + strspn(s, bad);
}

അക്ഷരം ascii_tolower(അക്ഷരം x) _const_;
അക്ഷരം* ascii_strlower(അക്ഷരം *s);
അക്ഷരം* ascii_strlower_n(അക്ഷരം *s, size_t n);

അക്ഷരം ascii_toupper(അക്ഷരം x) _const_;
അക്ഷരം* ascii_strupper(അക്ഷരം *s);

പൂർണ്ണസംഖ്യ ascii_strcasecmp_n(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b, size_t n);
പൂർണ്ണസംഖ്യ ascii_strcasecmp_nn(സ്ഥിരം അക്ഷരം *a, size_t n, സ്ഥിരം അക്ഷരം *b, size_t m);

bool chars_intersect(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) _pure_;

സ്ഥിതം വരിയിൽ bool _pure_ in_charset(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *charset) {
        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(charset);
        തിരികെ s[strspn(s, charset)] == '\0';
}

സ്ഥിതം വരിയിൽ bool char_is_cc(അക്ഷരം p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        തിരികെ (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *ok) _pure_;

അക്ഷരം* ellipsize_mem(സ്ഥിരം അക്ഷരം *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
സ്ഥിതം വരിയിൽ അക്ഷരം* ellipsize(സ്ഥിരം അക്ഷരം *s, size_t length, unsigned percent) {
        തിരികെ ellipsize_mem(s, strlen(s), length, percent);
}

അക്ഷരം* cellescape(അക്ഷരം *buf, size_t len, സ്ഥിരം അക്ഷരം *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

അക്ഷരം* strshorten(അക്ഷരം *s, size_t l);

പൂർണ്ണസംഖ്യ strgrowpad0(അക്ഷരം **s, size_t l);

അക്ഷരം* strreplace(സ്ഥിരം അക്ഷരം *text, സ്ഥിരം അക്ഷരം *old_string, സ്ഥിരം അക്ഷരം *new_string);

അക്ഷരം* strip_tab_ansi(അക്ഷരം **ibuf, size_t *_isz, size_t highlight[2]);

അക്ഷരം* strextendv_with_separator(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *separator, va_list ap);
അക്ഷരം* strextend_with_separator_internal(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, ശൂന്യ)
#define strextend(x, ...) strextend_with_separator_internal(x, ശൂന്യ, __VA_ARGS__, ശൂന്യ)

പൂർണ്ണസംഖ്യ strextendf_with_separator(അക്ഷരം **x, സ്ഥിരം അക്ഷരം *separator, സ്ഥിരം അക്ഷരം *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, ശൂന്യ, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                അക്ഷരം **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(ശൂന്യ, (separator), __VA_ARGS__, empty_to_null(*_p_), ശൂന്യ); \
                എങ്കിൽ (_s_) {                                                      \
                        സ്വതന്ത്രം(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, ശൂന്യ, __VA_ARGS__)

അക്ഷരം* strrep(സ്ഥിരം അക്ഷരം *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                സ്ഥിരം അക്ഷരം *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                അക്ഷരം *_d_, *_p_;                                        \
                _p_ = _d_ = newa(അക്ഷരം, _len_ + 1);                      \
                വേണ്ടി (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

പൂർണ്ണസംഖ്യ split_pair(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *sep, അക്ഷരം **ret_first, അക്ഷരം **ret_second);

പൂർണ്ണസംഖ്യ free_and_strdup(അക്ഷരം **p, സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ free_and_strdup_warn(അക്ഷരം **p, സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ free_and_strndup(അക്ഷരം **p, സ്ഥിരം അക്ഷരം *s, size_t l) _nonnull_if_nonzero_(2, 3);

പൂർണ്ണസംഖ്യ strdup_to_full(അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *src);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strdup_to(അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *src) {
        പൂർണ്ണസംഖ്യ r = strdup_to_full(ASSERT_PTR(ret), src);
        തിരികെ r < 0 ? r : 0;  /* Suppress return value of 1. */
}

തരനിർവചനം എണ്ണൽ StringSafeFlags {
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

bool string_is_safe(സ്ഥിരം അക്ഷരം *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
സ്ഥിതം വരിയിൽ ശൂന്യം strncpy_exact(അക്ഷരം *buf, സ്ഥിരം അക്ഷരം *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
സ്ഥിതം വരിയിൽ ശൂന്യം* memory_startswith_no_case(സ്ഥിരം ശൂന്യം *p, size_t sz, സ്ഥിരം അക്ഷരം *token) {
        പ്രതിജ്ഞാ(token);

        size_t n = strlen(token);
        എങ്കിൽ (sz < n)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(p);

        വേണ്ടി (size_t i = 0; i < n; i++)
                എങ്കിൽ (ascii_tolower(((അക്ഷരം *)p)[i]) != ascii_tolower(token[i]))
                        തിരികെ ശൂന്യ;

        തിരികെ (uint8_t*) p + n;
}

അക്ഷരം* str_realloc(അക്ഷരം *p);
അക്ഷരം* string_erase(അക്ഷരം *x);

പൂർണ്ണസംഖ്യ string_truncate_വരികൾ(സ്ഥിരം അക്ഷരം *s, size_t n_വരികൾ, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ string_extract_വരി(സ്ഥിരം അക്ഷരം *s, size_t i, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ string_contains_word_strv(സ്ഥിരം അക്ഷരം *string, സ്ഥിരം അക്ഷരം *separators, അക്ഷരം * സ്ഥിരം *വാക്കുകൾ, സ്ഥിരം അക്ഷരം **ret_word);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ string_contains_word(സ്ഥിരം അക്ഷരം *string, സ്ഥിരം അക്ഷരം *separators, സ്ഥിരം അക്ഷരം *word) {
        തിരികെ string_contains_word_strv(string, separators, STRV_MAKE(word), ശൂന്യ);
}

bool streq_skip_trailing_chars(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2, സ്ഥിരം അക്ഷരം *ok);

അക്ഷരം* string_replace_char(അക്ഷരം *str, അക്ഷരം old_char, അക്ഷരം new_char);

തരനിർവചനം എണ്ണൽ MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

പൂർണ്ണസംഖ്യ make_cstring(സ്ഥിരം ശൂന്യം *s, size_t n, MakeCStringMode mode, അക്ഷരം **ret);

size_t strspn_from_end(സ്ഥിരം അക്ഷരം *str, സ്ഥിരം അക്ഷരം *accept) _pure_;
size_t strnspn(സ്ഥിരം അക്ഷരം *str, സ്ഥിരം അക്ഷരം *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

അക്ഷരം* strdupspn(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *accept);
അക്ഷരം* strdupcspn(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                സ്ഥിരം അക്ഷരം *_t = (s);                                   \
                (അക്ഷരം*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                സ്ഥിരം അക്ഷരം *_t = (s);                                   \
                (അക്ഷരം*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

അക്ഷരം* find_വരി_startswith_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle);
#define find_വരി_startswith(haystack, needle) \
        const_generic(haystack, find_വരി_startswith_internal(haystack, needle))

അക്ഷരം* find_വരി_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle);
#define find_വരി(haystack, needle) \
        const_generic(haystack, find_വരി_internal(haystack, needle))

അക്ഷരം* find_വരി_after_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle);
#define find_വരി_after(haystack, needle) \
        const_generic(haystack, find_വരി_after_internal(haystack, needle))

തരനിർവചനം എണ്ണൽ VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(സ്ഥിരം അക്ഷരം *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(സ്ഥിരം അക്ഷരം *x, സ്ഥിരം അക്ഷരം *y);

അക്ഷരം* strrstr_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

അക്ഷരം* strrstr_no_case_internal(സ്ഥിരം അക്ഷരം *haystack, സ്ഥിരം അക്ഷരം *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) _pure_;
