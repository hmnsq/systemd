/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <string.h>

#شامل "alloc-util.h"
#شامل "forward.h"

#شامل "../fundamental/string-util.h" /* IWYU pragma: export */

ساکن اندرونی حرف* strstr_ptr_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        اگر (!haystack || !needle)
                واپس تہی;
        واپس (حرف*) لڑی_تلاش(haystack, needle);
}

#تعریف_کرو strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_اندرونی(haystack, needle))

ساکن اندرونی حرف* strstrafter_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        حرف *p = (حرف*) strstr_ptr(haystack, needle);
        اگر (!p)
                واپس تہی;

        واپس p + لڑی_لمبائی(needle);
}

#تعریف_کرو strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_اندرونی(haystack, needle))

ساکن اندرونی ثابت حرف* strnull(ثابت حرف *s) {
        واپس s ?: "(null)";
}

ساکن اندرونی ثابت حرف* strna(ثابت حرف *s) {
        واپس s ?: "n/a";
}

ساکن اندرونی ثابت حرف* true_false(bool b) {
        واپس b ? "true" : "false";
}

ساکن اندرونی ثابت حرف* plus_minus(bool b) {
        واپس b ? "+" : "-";
}

ساکن اندرونی ثابت حرف* one_zero(bool b) {
        واپس b ? "1" : "0";
}

ساکن اندرونی ثابت حرف* enable_disable(bool b) {
        واپس b ? "enable" : "disable";
}

ساکن اندرونی ثابت حرف* enabled_disabled(bool b) {
        واپس b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#تعریف_کرو خالی_خانے_مقصد_null(p)                                \
        ({                                              \
                ثابت حرف *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? تہی : _p);  \
        })

ساکن اندرونی ثابت حرف* خالی_خانے_مقصد_na(ثابت حرف *p) {
        واپس isempty(p) ? "n/a" : p;
}

ساکن اندرونی ثابت حرف* خالی_خانے_مقصد_dash(ثابت حرف *str) {
        واپس isempty(str) ? "-" : str;
}

ساکن اندرونی bool خالی_خانے_or_dash(ثابت حرف *str) {
        واپس !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ساکن اندرونی ثابت حرف* خالی_خانے_or_dash_مقصد_null(ثابت حرف *p) {
        واپس خالی_خانے_or_dash(p) ? تہی : p;
}
#تعریف_کرو خالی_خانے_or_dash_مقصد_null(p)                                \
        ({                                                      \
                ثابت حرف *_p = (p);                           \
                (typeof(p)) (خالی_خانے_or_dash(_p) ? تہی : _p);    \
        })

حرف* پہلا_لفظ(ثابت حرف *s, ثابت حرف *لفظ) خالص;

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

#تعریف_کرو strjoin(a, ...) strextend_with_separator_اندرونی(تہی, تہی, a, __VA_ARGS__, تہی)

#تعریف_کرو strjoina(a, ...)                                                \
        ({                                                              \
                ثابت حرف *_appendees_[] = { a, __VA_ARGS__ };         \
                حرف *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                برائے (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += لڑی_لمبائی(_appendees_[_i_]);              \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                برائے (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

حرف* strstrip(حرف *s);
حرف* delete_chars(حرف *s, ثابت حرف *bad);
حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad);
حرف* truncate_nl_بھرا(حرف *s, size_t *ret_len);
ساکن اندرونی حرف* truncate_nl(حرف *s) {
        واپس truncate_nl_بھرا(s, تہی);
}

ساکن اندرونی حرف* skip_leading_chars(ثابت حرف *s, ثابت حرف *bad) {
        اگر (!s)
                واپس تہی;

        اگر (!bad)
                bad = WHITESPACE;

        واپس (حرف*) s + strspn(s, bad);
}

حرف ascii_tolower(حرف x) _const_;
حرف* ascii_strlower(حرف *s);
حرف* ascii_strlower_n(حرف *s, size_t n);

حرف ascii_toupper(حرف x) _const_;
حرف* ascii_strupper(حرف *s);

صحیح ascii_strcasecmp_n(ثابت حرف *a, ثابت حرف *b, size_t n);
صحیح ascii_strcasecmp_nn(ثابت حرف *a, size_t n, ثابت حرف *b, size_t m);

bool chars_intersect(ثابت حرف *a, ثابت حرف *b) خالص;

ساکن اندرونی bool خالص اندر_charset(ثابت حرف *s, ثابت حرف *charset) {
        أكد(s);
        أكد(charset);
        واپس s[strspn(s, charset)] == '\0';
}

ساکن اندرونی bool char_is_cc(حرف p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        واپس (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ثابت حرف *p, ثابت حرف *ok) خالص;

حرف* ellipsize_mem(ثابت حرف *s, size_t old_لمبائی_بائٹیں, size_t new_لمبائی_columns, غیرعلامتی percent);
ساکن اندرونی حرف* ellipsize(ثابت حرف *s, size_t length, غیرعلامتی percent) {
        واپس ellipsize_mem(s, لڑی_لمبائی(s), length, percent);
}

حرف* cellescape(حرف *buf, size_t len, ثابت حرف *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#تعریف_کرو CELLESCAPE_DEFAULT_لمبائی 64

حرف* strshorten(حرف *s, size_t l);

صحیح strgrowpad0(حرف **s, size_t l);

حرف* strreplace(ثابت حرف *متن, ثابت حرف *old_string, ثابت حرف *new_string);

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]);

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap);
حرف* strextend_with_separator_اندرونی(حرف **x, ثابت حرف *separator, ...) _sentinel_;
#تعریف_کرو strextend_with_separator(x, separator, ...) strextend_with_separator_اندرونی(x, separator, __VA_ARGS__, تہی)
#تعریف_کرو strextend(x, ...) strextend_with_separator_اندرونی(x, تہی, __VA_ARGS__, تہی)

صحیح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) _printf_(3,4);
#تعریف_کرو strextendf(x, ...) strextendf_with_separator(x, تہی, __VA_ARGS__)

#تعریف_کرو strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                حرف **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_اندرونی(تہی, (separator), __VA_ARGS__, خالی_خانے_مقصد_null(*_p_), تہی); \
                اگر (_s_) {                                                      \
                        آزاد(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#تعریف_کرو strprepend(x, ...) strprepend_with_separator(x, تہی, __VA_ARGS__)

حرف* strrep(ثابت حرف *s, size_t n);

#تعریف_کرو strrepa(s, n)                                                   \
        ({                                                              \
                ثابت حرف *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = لڑی_لمبائی(_sss_);              \
                assert_se(MUL_تفویض_محفوظ(&_len_, _nnn_));              \
                حرف *_d_, *_p_;                                        \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                برائے (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

صحیح بانٹو_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_پہلا, حرف **ret_دوسرا);

صحیح free_and_strdup(حرف **p, ثابت حرف *s);
صحیح free_and_strdup_warn(حرف **p, ثابت حرف *s);
صحیح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

صحیح strdup_مقصد_بھرا(حرف **ret, ثابت حرف *src);
ساکن اندرونی صحیح strdup_مقصد(حرف **ret, ثابت حرف *src) {
        صحیح r = strdup_مقصد_بھرا(ASSERT_PTR(ret), src);
        واپس r < 0 ? r : 0;  /* Suppress return value of 1. */
}

قسمتعریف شمار String_محفوظ_Flags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_خالی_خانے         = 1 << 1, /* Allow empty strings */
        STRING_ALLOW_NEWLINES      = 1 << 2, /* Allow newlines (\n) */
        STRING_ALLOW_BACKSLASHES   = 1 << 3, /* Allow backslashes (\) */
        STRING_ALLOW_QUOTES        = 1 << 4, /* Allow quotes (" or ') */
        STRING_ALLOW_GLOBS         = 1 << 5, /* Allow globs (?, * or [) */
        STRING_FILENAME            = 1 << 6, /* Verify the string is valid as regular filename */
        STRING_FILENAME_PART       = 1 << 7, /* Verify the string is valid as part of a regular filename */
        STRING_DISALLOW_WHITESPACE = 1 << 8, /* Refuse whitespace (space, tab, newline, …) */
} String_محفوظ_Flags;

bool string_is_محفوظ(ثابت حرف *p, String_محفوظ_Flags flags) خالص;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ساکن اندرونی خالی strncpy_exact(حرف *buf, ثابت حرف *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ساکن اندرونی خالی* حافظہ_startswith_no_case(ثابت خالی *p, size_t sz, ثابت حرف *token) {
        أكد(token);

        size_t n = لڑی_لمبائی(token);
        اگر (sz < n)
                واپس تہی;

        أكد(p);

        برائے (size_t i = 0; i < n; i++)
                اگر (ascii_tolower(((حرف *)p)[i]) != ascii_tolower(token[i]))
                        واپس تہی;

        واپس (uint8_t*) p + n;
}

حرف* str_realloc(حرف *p);
حرف* string_erase(حرف *x);

صحیح string_truncate_سطریں(ثابت حرف *s, size_t n_سطریں, حرف **ret);
صحیح string_نکالو_سطر(ثابت حرف *s, size_t i, حرف **ret);

صحیح string_شامل_ہے_لفظ_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *الفاظ, ثابت حرف **ret_لفظ);
ساکن اندرونی صحیح string_شامل_ہے_لفظ(ثابت حرف *string, ثابت حرف *separators, ثابت حرف *لفظ) {
        واپس string_شامل_ہے_لفظ_strv(string, separators, STRV_MAKE(لفظ), تہی);
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok);

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char);

قسمتعریف شمار Make_CString_منوال {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        MAKE_CSTRING_منوال_MAX,
        MAKE_CSTRING_منوال_INVALID = -1,
} Make_CString_منوال;

صحیح make_cstring(ثابت خالی *s, size_t n, Make_CString_منوال منوال, حرف **ret);

size_t strspn_from_انتہا(ثابت حرف *str, ثابت حرف *accept) خالص;
size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) خالص _nonnull_if_nonzero_(1, 3);

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept);
حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#تعریف_کرو strdupa_محفوظ(s)                                                 \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, لڑی_لمبائی(_t));                \
        })

#تعریف_کرو strndupa_محفوظ(s, n)                                             \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

حرف* ڈھونڈو_سطر_startswith_اندرونی(ثابت حرف *haystack, ثابت حرف *needle);
#تعریف_کرو ڈھونڈو_سطر_startswith(haystack, needle) \
        const_generic(haystack, ڈھونڈو_سطر_startswith_اندرونی(haystack, needle))

حرف* ڈھونڈو_سطر_اندرونی(ثابت حرف *haystack, ثابت حرف *needle);
#تعریف_کرو ڈھونڈو_سطر(haystack, needle) \
        const_generic(haystack, ڈھونڈو_سطر_اندرونی(haystack, needle))

حرف* ڈھونڈو_سطر_after_اندرونی(ثابت حرف *haystack, ثابت حرف *needle);
#تعریف_کرو ڈھونڈو_سطر_after(haystack, needle) \
        const_generic(haystack, ڈھونڈو_سطر_after_اندرونی(haystack, needle))

قسمتعریف شمار نسخہ_Flags {
        نسخہ_ALLOW_خالی_خانے      = 1 << 0,
        نسخہ_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        نسخہ_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} نسخہ_Flags;

bool نسخہ_is_valid(ثابت حرف *s, نسخہ_Flags flags) خالص;

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y);

حرف* strrstr_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) خالص;
#تعریف_کرو strrstr(haystack, needle) \
        const_generic(haystack, strrstr_اندرونی(haystack, needle))

حرف* strrstr_no_case_اندرونی(ثابت حرف *haystack, ثابت حرف *needle) خالص;
#تعریف_کرو strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_اندرونی(haystack, needle))

size_t str_common_سابقہ(ثابت حرف *a, ثابت حرف *b) خالص;
