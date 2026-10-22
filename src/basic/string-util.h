/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <string.h>

#समावेश "alloc-util.h"
#समावेश "forward.h"

#समावेश "../fundamental/string-util.h" /* IWYU pragma: export */

अटल अंतरभूत अक्षर* strstr_ptr_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        अगर (!haystack || !needle)
                वापस शून्य;
        वापस (अक्षर*) म_माला(haystack, needle);
}

#घोषणा strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_internal(haystack, needle))

अटल अंतरभूत अक्षर* strstrafter_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        अक्षर *p = (अक्षर*) strstr_ptr(haystack, needle);
        अगर (!p)
                वापस शून्य;

        वापस p + म_माप(needle);
}

#घोषणा strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_internal(haystack, needle))

अटल अंतरभूत स्थिर अक्षर* strnull(स्थिर अक्षर *s) {
        वापस s ?: "(null)";
}

अटल अंतरभूत स्थिर अक्षर* strna(स्थिर अक्षर *s) {
        वापस s ?: "n/a";
}

अटल अंतरभूत स्थिर अक्षर* true_false(bool b) {
        वापस b ? "true" : "false";
}

अटल अंतरभूत स्थिर अक्षर* plus_minus(bool b) {
        वापस b ? "+" : "-";
}

अटल अंतरभूत स्थिर अक्षर* one_zero(bool b) {
        वापस b ? "1" : "0";
}

अटल अंतरभूत स्थिर अक्षर* enable_disable(bool b) {
        वापस b ? "enable" : "disable";
}

अटल अंतरभूत स्थिर अक्षर* enabled_disabled(bool b) {
        वापस b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#घोषणा empty_to_null(p)                                \
        ({                                              \
                स्थिर अक्षर *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? शून्य : _p);  \
        })

अटल अंतरभूत स्थिर अक्षर* empty_to_na(स्थिर अक्षर *p) {
        वापस isempty(p) ? "n/a" : p;
}

अटल अंतरभूत स्थिर अक्षर* empty_to_dash(स्थिर अक्षर *str) {
        वापस isempty(str) ? "-" : str;
}

अटल अंतरभूत bool empty_or_dash(स्थिर अक्षर *str) {
        वापस !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

अटल अंतरभूत स्थिर अक्षर* empty_or_dash_to_null(स्थिर अक्षर *p) {
        वापस empty_or_dash(p) ? शून्य : p;
}
#घोषणा empty_or_dash_to_null(p)                                \
        ({                                                      \
                स्थिर अक्षर *_p = (p);                           \
                (typeof(p)) (empty_or_dash(_p) ? शून्य : _p);    \
        })

अक्षर* first_word(स्थिर अक्षर *s, स्थिर अक्षर *word) _pure_;

अक्षर* strextendn(अक्षर **x, स्थिर अक्षर *s, माप_प्रकार l) _nonnull_if_nonzero_(2, 3);

#घोषणा strjoin(a, ...) strextend_with_separator_internal(शून्य, शून्य, a, __VA_ARGS__, शून्य)

#घोषणा strjoina(a, ...)                                                \
        ({                                                              \
                स्थिर अक्षर *_appendees_[] = { a, __VA_ARGS__ };         \
                अक्षर *_d_, *_p_;                                        \
                माप_प्रकार _len_ = 0;                                       \
                माप_प्रकार _i_;                                             \
                क्रम (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += म_माप(_appendees_[_i_]);              \
                _p_ = _d_ = newa(अक्षर, _len_ + 1);                      \
                क्रम (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

अक्षर* strstrip(अक्षर *s);
अक्षर* delete_chars(अक्षर *s, स्थिर अक्षर *bad);
अक्षर* delete_trailing_chars(अक्षर *s, स्थिर अक्षर *bad);
अक्षर* truncate_nl_full(अक्षर *s, माप_प्रकार *ret_len);
अटल अंतरभूत अक्षर* truncate_nl(अक्षर *s) {
        वापस truncate_nl_full(s, शून्य);
}

अटल अंतरभूत अक्षर* skip_leading_chars(स्थिर अक्षर *s, स्थिर अक्षर *bad) {
        अगर (!s)
                वापस शून्य;

        अगर (!bad)
                bad = WHITESPACE;

        वापस (अक्षर*) s + म_अखोज(s, bad);
}

अक्षर ascii_tolower(अक्षर x) _const_;
अक्षर* ascii_strlower(अक्षर *s);
अक्षर* ascii_strlower_n(अक्षर *s, माप_प्रकार n);

अक्षर ascii_toupper(अक्षर x) _const_;
अक्षर* ascii_strupper(अक्षर *s);

पूर्णांक ascii_strcasecmp_n(स्थिर अक्षर *a, स्थिर अक्षर *b, माप_प्रकार n);
पूर्णांक ascii_strcasecmp_nn(स्थिर अक्षर *a, माप_प्रकार n, स्थिर अक्षर *b, माप_प्रकार m);

bool chars_intersect(स्थिर अक्षर *a, स्थिर अक्षर *b) _pure_;

अटल अंतरभूत bool _pure_ in_charset(स्थिर अक्षर *s, स्थिर अक्षर *charset) {
        निश्चित(s);
        निश्चित(charset);
        वापस s[म_अखोज(s, charset)] == '\0';
}

अटल अंतरभूत bool char_is_cc(अक्षर p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        वापस (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(स्थिर अक्षर *p, स्थिर अक्षर *ok) _pure_;

अक्षर* ellipsize_mem(स्थिर अक्षर *s, माप_प्रकार old_लंबाई_bytes, माप_प्रकार new_लंबाई_columns, अचिन्हित percent);
अटल अंतरभूत अक्षर* ellipsize(स्थिर अक्षर *s, माप_प्रकार length, अचिन्हित percent) {
        वापस ellipsize_mem(s, म_माप(s), length, percent);
}

अक्षर* cellescape(अक्षर *buf, माप_प्रकार len, स्थिर अक्षर *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#घोषणा CELLESCAPE_DEFAULT_लंबाई 64

अक्षर* strshorten(अक्षर *s, माप_प्रकार l);

पूर्णांक strgrowpad0(अक्षर **s, माप_प्रकार l);

अक्षर* strreplace(स्थिर अक्षर *text, स्थिर अक्षर *old_string, स्थिर अक्षर *new_string);

अक्षर* strip_tab_ansi(अक्षर **ibuf, माप_प्रकार *_isz, माप_प्रकार highlight[2]);

अक्षर* strextendv_with_separator(अक्षर **x, स्थिर अक्षर *separator, बहु_सूची ap);
अक्षर* strextend_with_separator_internal(अक्षर **x, स्थिर अक्षर *separator, ...) _sentinel_;
#घोषणा strextend_with_separator(x, separator, ...) strextend_with_separator_internal(x, separator, __VA_ARGS__, शून्य)
#घोषणा strextend(x, ...) strextend_with_separator_internal(x, शून्य, __VA_ARGS__, शून्य)

पूर्णांक strextendf_with_separator(अक्षर **x, स्थिर अक्षर *separator, स्थिर अक्षर *format, ...) _printf_(3,4);
#घोषणा strextendf(x, ...) strextendf_with_separator(x, शून्य, __VA_ARGS__)

#घोषणा strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                अक्षर **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_internal(शून्य, (separator), __VA_ARGS__, empty_to_null(*_p_), शून्य); \
                अगर (_s_) {                                                      \
                        मुक्त(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#घोषणा strprepend(x, ...) strprepend_with_separator(x, शून्य, __VA_ARGS__)

अक्षर* strrep(स्थिर अक्षर *s, माप_प्रकार n);

#घोषणा strrepa(s, n)                                                   \
        ({                                                              \
                स्थिर अक्षर *_sss_ = (s);                                \
                माप_प्रकार _nnn_ = (n), _len_ = म_माप(_sss_);              \
                assert_se(MUL_ASSIGN_SAFE(&_len_, _nnn_));              \
                अक्षर *_d_, *_p_;                                        \
                _p_ = _d_ = newa(अक्षर, _len_ + 1);                      \
                क्रम (माप_प्रकार _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

पूर्णांक split_pair(स्थिर अक्षर *s, स्थिर अक्षर *sep, अक्षर **ret_first, अक्षर **ret_second);

पूर्णांक free_and_strdup(अक्षर **p, स्थिर अक्षर *s);
पूर्णांक free_and_strdup_warn(अक्षर **p, स्थिर अक्षर *s);
पूर्णांक free_and_strndup(अक्षर **p, स्थिर अक्षर *s, माप_प्रकार l) _nonnull_if_nonzero_(2, 3);

पूर्णांक strdup_to_full(अक्षर **ret, स्थिर अक्षर *src);
अटल अंतरभूत पूर्णांक strdup_to(अक्षर **ret, स्थिर अक्षर *src) {
        पूर्णांक r = strdup_to_full(ASSERT_PTR(ret), src);
        वापस r < 0 ? r : 0;  /* Suppress return value of 1. */
}

प्रकार क्रमागत StringSafeFlags {
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

bool string_is_safe(स्थिर अक्षर *p, StringSafeFlags flags) _pure_;

DISABLE_WARNING_STRINGOP_TRUNCATION;
अटल अंतरभूत व्योम strncpy_exact(अक्षर *buf, स्थिर अक्षर *src, माप_प्रकार buf_len) {
        म_नकलन(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
अटल अंतरभूत व्योम* memory_startswith_no_case(स्थिर व्योम *p, माप_प्रकार sz, स्थिर अक्षर *token) {
        निश्चित(token);

        माप_प्रकार n = म_माप(token);
        अगर (sz < n)
                वापस शून्य;

        निश्चित(p);

        क्रम (माप_प्रकार i = 0; i < n; i++)
                अगर (ascii_tolower(((अक्षर *)p)[i]) != ascii_tolower(token[i]))
                        वापस शून्य;

        वापस (uint8_t*) p + n;
}

अक्षर* str_realloc(अक्षर *p);
अक्षर* string_erase(अक्षर *x);

पूर्णांक string_truncate_पंक्तियाँ(स्थिर अक्षर *s, माप_प्रकार n_पंक्तियाँ, अक्षर **ret);
पूर्णांक string_extract_पंक्ति(स्थिर अक्षर *s, माप_प्रकार i, अक्षर **ret);

पूर्णांक string_contains_word_strv(स्थिर अक्षर *string, स्थिर अक्षर *separators, अक्षर * स्थिर *शब्दगण, स्थिर अक्षर **ret_word);
अटल अंतरभूत पूर्णांक string_contains_word(स्थिर अक्षर *string, स्थिर अक्षर *separators, स्थिर अक्षर *word) {
        वापस string_contains_word_strv(string, separators, STRV_MAKE(word), शून्य);
}

bool streq_skip_trailing_chars(स्थिर अक्षर *s1, स्थिर अक्षर *s2, स्थिर अक्षर *ok);

अक्षर* string_replace_char(अक्षर *str, अक्षर old_char, अक्षर new_char);

प्रकार क्रमागत MakeCStringMode {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        _MAKE_CSTRING_MODE_MAX,
        _MAKE_CSTRING_MODE_INVALID = -1,
} MakeCStringMode;

पूर्णांक make_cstring(स्थिर व्योम *s, माप_प्रकार n, MakeCStringMode mode, अक्षर **ret);

माप_प्रकार strspn_from_end(स्थिर अक्षर *str, स्थिर अक्षर *accept) _pure_;
माप_प्रकार strnspn(स्थिर अक्षर *str, स्थिर अक्षर *accept, माप_प्रकार n) _pure_ _nonnull_if_nonzero_(1, 3);

अक्षर* strdupspn(स्थिर अक्षर *a, स्थिर अक्षर *accept);
अक्षर* strdupcspn(स्थिर अक्षर *a, स्थिर अक्षर *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#घोषणा strdupa_safe(s)                                                 \
        ({                                                              \
                स्थिर अक्षर *_t = (s);                                   \
                (अक्षर*) memdupa_suffix0(_t, म_माप(_t));                \
        })

#घोषणा strndupa_safe(s, n)                                             \
        ({                                                              \
                स्थिर अक्षर *_t = (s);                                   \
                (अक्षर*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

अक्षर* find_पंक्ति_startswith_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle);
#घोषणा find_पंक्ति_startswith(haystack, needle) \
        const_generic(haystack, find_पंक्ति_startswith_internal(haystack, needle))

अक्षर* find_पंक्ति_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle);
#घोषणा find_पंक्ति(haystack, needle) \
        const_generic(haystack, find_पंक्ति_internal(haystack, needle))

अक्षर* find_पंक्ति_after_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle);
#घोषणा find_पंक्ति_after(haystack, needle) \
        const_generic(haystack, find_पंक्ति_after_internal(haystack, needle))

प्रकार क्रमागत VersionFlags {
        VERSION_ALLOW_EMPTY      = 1 << 0,
        VERSION_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        VERSION_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} VersionFlags;

bool version_is_valid(स्थिर अक्षर *s, VersionFlags flags) _pure_;

ssize_t strlevenshtein(स्थिर अक्षर *x, स्थिर अक्षर *y);

अक्षर* strrstr_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) _pure_;
#घोषणा strrstr(haystack, needle) \
        const_generic(haystack, strrstr_internal(haystack, needle))

अक्षर* strrstr_no_case_internal(स्थिर अक्षर *haystack, स्थिर अक्षर *needle) _pure_;
#घोषणा strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_internal(haystack, needle))

माप_प्रकार str_common_prefix(स्थिर अक्षर *a, स्थिर अक्षर *b) _pure_;
