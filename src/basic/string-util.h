/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "forward.h"

#அந்தர்பாவ "../fundamental/string-util.h" /* IWYU pragma: export */

நிலையான உள்வரி எழுத்து* strstr_ptr_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        எனில் (!haystack || !needle)
                திரும்பு வெற்று;
        திரும்பு (எழுத்து*) strstr(haystack, needle);
}

#define strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

நிலையான உள்வரி எழுத்து* strstrafter_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        எழுத்து *p = (எழுத்து*) strstr_ptr(haystack, needle);
        எனில் (!p)
                திரும்பு வெற்று;

        திரும்பு p + strlen(needle);
}

#define strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

நிலையான உள்வரி மாறிலி எழுத்து* strnull(மாறிலி எழுத்து *s) {
        திரும்பு s ?: "(null)";
}

நிலையான உள்வரி மாறிலி எழுத்து* strna(மாறிலி எழுத்து *s) {
        திரும்பு s ?: "n/a";
}

நிலையான உள்வரி மாறிலி எழுத்து* true_false(bool b) {
        திரும்பு b ? "true" : "false";
}

நிலையான உள்வரி மாறிலி எழுத்து* plus_minus(bool b) {
        திரும்பு b ? "+" : "-";
}

நிலையான உள்வரி மாறிலி எழுத்து* one_zero(bool b) {
        திரும்பு b ? "1" : "0";
}

நிலையான உள்வரி மாறிலி எழுத்து* enable_disable(bool b) {
        திரும்பு b ? "enable" : "disable";
}

நிலையான உள்வரி மாறிலி எழுத்து* enabled_disabled(bool b) {
        திரும்பு b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#define empty_to_null(p)                                \
        ({                                              \
                மாறிலி எழுத்து *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? வெற்று : _p);  \
        })

நிலையான உள்வரி மாறிலி எழுத்து* empty_to_na(மாறிலி எழுத்து *p) {
        திரும்பு isempty(p) ? "n/a" : p;
}

நிலையான உள்வரி மாறிலி எழுத்து* empty_to_dash(மாறிலி எழுத்து *str) {
        திரும்பு isempty(str) ? "-" : str;
}

நிலையான உள்வரி bool empty_or_dash(மாறிலி எழுத்து *str) {
        திரும்பு !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

நிலையான உள்வரி மாறிலி எழுத்து* empty_or_dash_to_null(மாறிலி எழுத்து *p) {
        திரும்பு empty_or_dash(p) ? வெற்று : p;
}
#define empty_or_dash_to_null(p)                                \
        ({                                                      \
                மாறிலி எழுத்து *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? வெற்று : _p);    \
        })

எழுத்து* first_word(மாறிலி எழுத்து *s, மாறிலி எழுத்து *word) _pure_;

எழுத்து* strextendn(எழுத்து **x, மாறிலி எழுத்து *s, size_t l) _nonnull_if_nonzero_(2, 3);

#define strjoin(a, ...) strextend_with_separator_internal(வெற்று, வெற்று, a, __VA_ARGS__, வெற்று)

#define strjoina(a, ...)                                                \
        ({                                                              \
                மாறிலி எழுத்து *_appendees_[] = { a, __VA_ARGS__ };         \
                எழுத்து *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                க்கு (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += strlen(_appendees_[_i_]);              \
                _p_ = _d_ = newa(எழுத்து, _len_ + 1);                      \
                க்கு (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

எழுத்து* strstrip(எழுத்து *s);
எழுத்து* delete_chars(எழுத்து *s, மாறிலி எழுத்து *bad);
எழுத்து* delete_trailing_chars(எழுத்து *s, மாறிலி எழுத்து *bad);
எழுத்து* truncate_nl_full(எழுத்து *s, size_t *ret_len);
நிலையான உள்வரி எழுத்து* truncate_nl(எழுத்து *s) {
        திரும்பு truncate_nl_full(s, வெற்று);
}

நிலையான உள்வரி எழுத்து* skip_leading_chars(மாறிலி எழுத்து *s, மாறிலி எழுத்து *bad) {
        எனில் (!s)
                திரும்பு வெற்று;

        எனில் (!bad)
                bad = WHITESPACE;

        திரும்பு (எழுத்து*) s + strspn(s, bad);
}

எழுத்து ascii_tolower(எழுத்து x) _const_;
எழுத்து* ascii_strlower(எழுத்து *s);
எழுத்து* ascii_strlower_n(எழுத்து *s, size_t n);

எழுத்து ascii_toupper(எழுத்து x) _const_;
எழுத்து* ascii_strupper(எழுத்து *s);

முழு ascii_strcasecmp_n(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b, size_t n);
முழு ascii_strcasecmp_nn(மாறிலி எழுத்து *a, size_t n, மாறிலி எழுத்து *b, size_t m);

bool chars_intersect(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) _pure_;

நிலையான உள்வரி bool _pure_ in_charset(மாறிலி எழுத்து *s, மாறிலி எழுத்து *charset) {
        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(charset);
        திரும்பு s[strspn(s, charset)] == '\0';
}

நிலையான உள்வரி bool char_is_cc(எழுத்து p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        திரும்பு (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(மாறிலி எழுத்து *p, மாறிலி எழுத்து *ok) _pure_;

எழுத்து* ellipsize_mem(மாறிலி எழுத்து *s, size_t old_length_bytes, size_t new_length_columns, unsigned percent);
நிலையான உள்வரி எழுத்து* ellipsize(மாறிலி எழுத்து *s, size_t length, unsigned percent) {
        திரும்பு ellipsize_mem(s, strlen(s), length, percent);
}

எழுத்து* cellescape(எழுத்து *buf, size_t len, மாறிலி எழுத்து *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#define CELLESCAPE_DEFAULT_LENGTH 64

எழுத்து* strshorten(எழுத்து *s, size_t l);

முழு strgrowpad0(எழுத்து **s, size_t l);

எழுத்து* strreplace(மாறிலி எழுத்து *text, மாறிலி எழுத்து *old_string, மாறிலி எழுத்து *new_string);

எழுத்து* strip_tab_ansi(எழுத்து **ibuf, size_t *_isz, size_t highlight[2]);

எழுத்து* strextendv_with_separator(எழுத்து **x, மாறிலி எழுத்து *separator, va_list ap);
எழுத்து* strextend_with_separator_internal(எழுத்து **x, மாறிலி எழுத்து *separator, ...) _sentinel_;
#define strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, வெற்று)
#define strextend(x, ...) strextend_with_separator_internal(x, வெற்று, __VA_ARGS__, வெற்று)

முழு strextendf_with_separator(எழுத்து **x, மாறிலி எழுத்து *separator, மாறிலி எழுத்து *format, ...) _printf_(3,4);
#define strextendf(x, ...) strextendf_with_separator(x, வெற்று, __VA_ARGS__)

#define strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                எழுத்து **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(வெற்று, (separator), __VA_ARGS__, empty_to_null(*_p_), வெற்று); \
                எனில் (_s_) {                                                      \
                        விடுவி(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#define strprepend(x, ...) strprepend_with_separator(x, வெற்று, __VA_ARGS__)

எழுத்து* strrep(மாறிலி எழுத்து *s, size_t n);

#define strrepa(s, n)                                                   \
        ({                                                              \
                மாறிலி எழுத்து *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = strlen(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                எழுத்து *_d_, *_p_;                                        \
                _p_ = _d_ = newa(எழுத்து, _len_ + 1);                      \
                க்கு (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

முழு split_pair(மாறிலி எழுத்து *s, மாறிலி எழுத்து *sep, எழுத்து **ret_first, எழுத்து **ret_second);

முழு free_and_strdup(எழுத்து **p, மாறிலி எழுத்து *s);
முழு free_and_strdup_warn(எழுத்து **p, மாறிலி எழுத்து *s);
முழு free_and_strndup(எழுத்து **p, மாறிலி எழுத்து *s, size_t l) _nonnull_if_nonzero_(2, 3);

முழு strdup_to_full(எழுத்து **ret, மாறிலி எழுத்து *src);
நிலையான உள்வரி முழு strdup_to(எழுத்து **ret, மாறிலி எழுத்து *src) {
        முழு r = strdup_to_full(ASSERT_PTR(ret), src);
        திரும்பு r < 0 ? r : 0;  /* Suppress return value of 1. */
}

வகைவரை எண்ணிக்கை StringSafeFlags {
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

bool string_is_safe(மாறிலி எழுத்து *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
நிலையான உள்வரி வெறுமை strncpy_exact(எழுத்து *buf, மாறிலி எழுத்து *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
நிலையான உள்வரி வெறுமை* memory_startswith_no_case(மாறிலி வெறுமை *p, size_t sz, மாறிலி எழுத்து *token) {
        ப்ரதிஜ்ஞா(token);

        size_t n = strlen(token);
        எனில் (sz < n)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(p);

        க்கு (size_t i = 0; i < n; i++)
                எனில் (ascii_tolower(((எழுத்து *)p)[i]) != ascii_tolower(token[i]))
                        திரும்பு வெற்று;

        திரும்பு (uint8_t*) p + n;
}

எழுத்து* str_realloc(எழுத்து *p);
எழுத்து* string_erase(எழுத்து *x);

முழு string_truncate_வரிகள்(மாறிலி எழுத்து *s, size_t n_வரிகள், எழுத்து **ret);
முழு string_extract_வரி(மாறிலி எழுத்து *s, size_t i, எழுத்து **ret);

முழு string_contains_word_strv(மாறிலி எழுத்து *string, மாறிலி எழுத்து *separators, எழுத்து * மாறிலி *சொற்கள், மாறிலி எழுத்து **ret_word);
நிலையான உள்வரி முழு string_contains_word(மாறிலி எழுத்து *string, மாறிலி எழுத்து *separators, மாறிலி எழுத்து *word) {
        திரும்பு string_contains_word_strv(string, separators, STRV_MAKE(word), வெற்று);
}

bool streq_skip_trailing_chars(மாறிலி எழுத்து *s1, மாறிலி எழுத்து *s2, மாறிலி எழுத்து *ok);

எழுத்து* string_replace_char(எழுத்து *str, எழுத்து old_char, எழுத்து new_char);

வகைவரை எண்ணிக்கை MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

முழு make_cstring(மாறிலி வெறுமை *s, size_t n, MakeCStringMode mode, எழுத்து **ret);

size_t strspn_from_end(மாறிலி எழுத்து *str, மாறிலி எழுத்து *accept) _pure_;
size_t strnspn(மாறிலி எழுத்து *str, மாறிலி எழுத்து *accept, size_t n) _pure_ _nonnull_if_nonzero_(1, 3);

எழுத்து* strdupspn(மாறிலி எழுத்து *a, மாறிலி எழுத்து *accept);
எழுத்து* strdupcspn(மாறிலி எழுத்து *a, மாறிலி எழுத்து *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#define strdupa_safe(s)                                                 \
        ({                                                              \
                மாறிலி எழுத்து *_t = (s);                                   \
                (எழுத்து*) memdupa_suffix0(_t, strlen(_t));                \
        })

#define strndupa_safe(s, n)                                             \
        ({                                                              \
                மாறிலி எழுத்து *_t = (s);                                   \
                (எழுத்து*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

எழுத்து* find_வரி_startswith_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle);
#define find_வரி_startswith(haystack, needle) \
        const_generic(haystack, find_வரி_startswith_internal(haystack, needle))

எழுத்து* find_வரி_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle);
#define find_வரி(haystack, needle) \
        const_generic(haystack, find_வரி_internal(haystack, needle))

எழுத்து* find_வரி_after_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle);
#define find_வரி_after(haystack, needle) \
        const_generic(haystack, find_வரி_after_internal(haystack, needle))

வகைவரை எண்ணிக்கை VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(மாறிலி எழுத்து *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(மாறிலி எழுத்து *x, மாறிலி எழுத்து *y);

எழுத்து* strrstr_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) _pure_;
#define strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

எழுத்து* strrstr_no_case_internal(மாறிலி எழுத்து *haystack, மாறிலி எழுத்து *needle) _pure_;
#define strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

size_t str_common_prefix(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) _pure_;
