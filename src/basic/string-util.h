/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "forward.h"

#ضمن "../fundamental/string-util.h" /* IWYU pragma: export */

ساكن مضمن حرف* strstr_ptr_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        إذا (!haystack || !needle)
                أرجع عدم;
        أرجع (حرف*) جستجوی_رشته(haystack, needle);
}

#تعریف_کن strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_داخلی(haystack, needle))

ساكن مضمن حرف* strstrafter_داخلی(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        حرف *p = (حرف*) strstr_ptr(haystack, needle);
        إذا (!p)
                أرجع عدم;

        أرجع p + طول_رشته(needle);
}

#تعریف_کن strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_داخلی(haystack, needle))

ساكن مضمن ثابت حرف* strnull(ثابت حرف *s) {
        أرجع s ?: "(null)";
}

ساكن مضمن ثابت حرف* strna(ثابت حرف *s) {
        أرجع s ?: "n/a";
}

ساكن مضمن ثابت حرف* true_false(bool b) {
        أرجع b ? "true" : "false";
}

ساكن مضمن ثابت حرف* plus_minus(bool b) {
        أرجع b ? "+" : "-";
}

ساكن مضمن ثابت حرف* one_zero(bool b) {
        أرجع b ? "1" : "0";
}

ساكن مضمن ثابت حرف* enable_disable(bool b) {
        أرجع b ? "enable" : "disable";
}

ساكن مضمن ثابت حرف* enabled_disabled(bool b) {
        أرجع b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#تعریف_کن خالی_مقصد_null(p)                                \
        ({                                              \
                ثابت حرف *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? عدم : _p);  \
        })

ساكن مضمن ثابت حرف* خالی_مقصد_na(ثابت حرف *p) {
        أرجع isempty(p) ? "n/a" : p;
}

ساكن مضمن ثابت حرف* خالی_مقصد_dash(ثابت حرف *str) {
        أرجع isempty(str) ? "-" : str;
}

ساكن مضمن bool خالی_or_dash(ثابت حرف *str) {
        أرجع !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ساكن مضمن ثابت حرف* خالی_or_dash_مقصد_null(ثابت حرف *p) {
        أرجع خالی_or_dash(p) ? عدم : p;
}
#تعریف_کن خالی_or_dash_مقصد_null(p)                                \
        ({                                                      \
                ثابت حرف *_p = (p);                           \
                (typeof(p)) (خالی_or_dash(_p) ? عدم : _p);    \
        })

حرف* اولی_کلمه(ثابت حرف *s, ثابت حرف *کلمه) خالص;

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

#تعریف_کن strjoin(a, ...) strextend_with_separator_داخلی(عدم, عدم, a, __VA_ARGS__, عدم)

#تعریف_کن strjoina(a, ...)                                                \
        ({                                                              \
                ثابت حرف *_appendees_[] = { a, __VA_ARGS__ };         \
                حرف *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                لكل (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += طول_رشته(_appendees_[_i_]);              \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                لكل (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

حرف* strstrip(حرف *s);
حرف* delete_chars(حرف *s, ثابت حرف *bad);
حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad);
حرف* truncate_nl_پر(حرف *s, size_t *ret_len);
ساكن مضمن حرف* truncate_nl(حرف *s) {
        أرجع truncate_nl_پر(s, عدم);
}

ساكن مضمن حرف* skip_leading_chars(ثابت حرف *s, ثابت حرف *bad) {
        إذا (!s)
                أرجع عدم;

        إذا (!bad)
                bad = WHITESPACE;

        أرجع (حرف*) s + strspn(s, bad);
}

حرف ascii_tolower(حرف x) _const_;
حرف* ascii_strlower(حرف *s);
حرف* ascii_strlower_n(حرف *s, size_t n);

حرف ascii_toupper(حرف x) _const_;
حرف* ascii_strupper(حرف *s);

صحيح ascii_strcasecmp_n(ثابت حرف *a, ثابت حرف *b, size_t n);
صحيح ascii_strcasecmp_nn(ثابت حرف *a, size_t n, ثابت حرف *b, size_t m);

bool chars_intersect(ثابت حرف *a, ثابت حرف *b) خالص;

ساكن مضمن bool خالص درون_charset(ثابت حرف *s, ثابت حرف *charset) {
        أكد(s);
        أكد(charset);
        أرجع s[strspn(s, charset)] == '\0';
}

ساكن مضمن bool char_is_cc(حرف p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        أرجع (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ثابت حرف *p, ثابت حرف *ok) خالص;

حرف* ellipsize_mem(ثابت حرف *s, size_t old_درازی_بایتها, size_t new_درازی_columns, بیعلامت percent);
ساكن مضمن حرف* ellipsize(ثابت حرف *s, size_t length, بیعلامت percent) {
        أرجع ellipsize_mem(s, طول_رشته(s), length, percent);
}

حرف* cellescape(حرف *buf, size_t len, ثابت حرف *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#تعریف_کن CELLESCAPE_DEFAULT_درازی 64

حرف* strshorten(حرف *s, size_t l);

صحيح strgrowpad0(حرف **s, size_t l);

حرف* strreplace(ثابت حرف *متن, ثابت حرف *old_string, ثابت حرف *new_string);

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]);

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap);
حرف* strextend_with_separator_داخلی(حرف **x, ثابت حرف *separator, ...) _sentinel_;
#تعریف_کن strextend_with_separator(x, separator, ...) strextend_with_separator_داخلی(x, separator, __VA_ARGS__, عدم)
#تعریف_کن strextend(x, ...) strextend_with_separator_داخلی(x, عدم, __VA_ARGS__, عدم)

صحيح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) _printf_(3,4);
#تعریف_کن strextendf(x, ...) strextendf_with_separator(x, عدم, __VA_ARGS__)

#تعریف_کن strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                حرف **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_داخلی(عدم, (separator), __VA_ARGS__, خالی_مقصد_null(*_p_), عدم); \
                إذا (_s_) {                                                      \
                        حرر(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#تعریف_کن strprepend(x, ...) strprepend_with_separator(x, عدم, __VA_ARGS__)

حرف* strrep(ثابت حرف *s, size_t n);

#تعریف_کن strrepa(s, n)                                                   \
        ({                                                              \
                ثابت حرف *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = طول_رشته(_sss_);              \
                assert_se(MUL_تخصیص_بده_امن(&_len_, _nnn_));              \
                حرف *_d_, *_p_;                                        \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                لكل (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

صحيح تقسیم_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_اولی, حرف **ret_دومی);

صحيح free_and_strdup(حرف **p, ثابت حرف *s);
صحيح free_and_strdup_warn(حرف **p, ثابت حرف *s);
صحيح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

صحيح strdup_مقصد_پر(حرف **ret, ثابت حرف *src);
ساكن مضمن صحيح strdup_مقصد(حرف **ret, ثابت حرف *src) {
        صحيح r = strdup_مقصد_پر(ASSERT_PTR(ret), src);
        أرجع r < 0 ? r : 0;  /* Suppress return value of 1. */
}

تعريفنوع تعداد String_امن_Flags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_خالی         = 1 << 1, /* Allow empty strings */
        STRING_ALLOW_NEWLINES      = 1 << 2, /* Allow newlines (\n) */
        STRING_ALLOW_BACKSLASHES   = 1 << 3, /* Allow backslashes (\) */
        STRING_ALLOW_QUOTES        = 1 << 4, /* Allow quotes (" or ') */
        STRING_ALLOW_GLOBS         = 1 << 5, /* Allow globs (?, * or [) */
        STRING_FILENAME            = 1 << 6, /* Verify the string is valid as regular filename */
        STRING_FILENAME_PART       = 1 << 7, /* Verify the string is valid as part of a regular filename */
        STRING_DISALLOW_WHITESPACE = 1 << 8, /* Refuse whitespace (space, tab, newline, …) */
} String_امن_Flags;

bool string_is_امن(ثابت حرف *p, String_امن_Flags flags) خالص;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ساكن مضمن فراغ strncpy_exact(حرف *buf, ثابت حرف *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ساكن مضمن فراغ* حافظه_startswith_no_case(ثابت فراغ *p, size_t sz, ثابت حرف *token) {
        أكد(token);

        size_t n = طول_رشته(token);
        إذا (sz < n)
                أرجع عدم;

        أكد(p);

        لكل (size_t i = 0; i < n; i++)
                إذا (ascii_tolower(((حرف *)p)[i]) != ascii_tolower(token[i]))
                        أرجع عدم;

        أرجع (uint8_t*) p + n;
}

حرف* str_realloc(حرف *p);
حرف* string_erase(حرف *x);

صحيح string_truncate_سطرها(ثابت حرف *s, size_t n_سطرها, حرف **ret);
صحيح string_استخراج_سطر(ثابت حرف *s, size_t i, حرف **ret);

صحيح string_شامل_است_کلمه_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *واژگان, ثابت حرف **ret_کلمه);
ساكن مضمن صحيح string_شامل_است_کلمه(ثابت حرف *string, ثابت حرف *separators, ثابت حرف *کلمه) {
        أرجع string_شامل_است_کلمه_strv(string, separators, STRV_MAKE(کلمه), عدم);
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok);

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char);

تعريفنوع تعداد Make_CString_نما {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        MAKE_CSTRING_نما_MAX,
        MAKE_CSTRING_نما_INVALID = -1,
} Make_CString_نما;

صحيح make_cstring(ثابت فراغ *s, size_t n, Make_CString_نما نما, حرف **ret);

size_t strspn_from_انتها(ثابت حرف *str, ثابت حرف *accept) خالص;
size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) خالص _nonnull_if_nonzero_(1, 3);

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept);
حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#تعریف_کن strdupa_امن(s)                                                 \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, طول_رشته(_t));                \
        })

#تعریف_کن strndupa_امن(s, n)                                             \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

حرف* پیدا_کن_سطر_startswith_داخلی(ثابت حرف *haystack, ثابت حرف *needle);
#تعریف_کن پیدا_کن_سطر_startswith(haystack, needle) \
        const_generic(haystack, پیدا_کن_سطر_startswith_داخلی(haystack, needle))

حرف* پیدا_کن_سطر_داخلی(ثابت حرف *haystack, ثابت حرف *needle);
#تعریف_کن پیدا_کن_سطر(haystack, needle) \
        const_generic(haystack, پیدا_کن_سطر_داخلی(haystack, needle))

حرف* پیدا_کن_سطر_after_داخلی(ثابت حرف *haystack, ثابت حرف *needle);
#تعریف_کن پیدا_کن_سطر_after(haystack, needle) \
        const_generic(haystack, پیدا_کن_سطر_after_داخلی(haystack, needle))

تعريفنوع تعداد نسخه_Flags {
        نسخه_ALLOW_خالی      = 1 << 0,
        نسخه_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        نسخه_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} نسخه_Flags;

bool نسخه_is_valid(ثابت حرف *s, نسخه_Flags flags) خالص;

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y);

حرف* strrstr_داخلی(ثابت حرف *haystack, ثابت حرف *needle) خالص;
#تعریف_کن strrstr(haystack, needle) \
        const_generic(haystack, strrstr_داخلی(haystack, needle))

حرف* strrstr_no_case_داخلی(ثابت حرف *haystack, ثابت حرف *needle) خالص;
#تعریف_کن strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_داخلی(haystack, needle))

size_t str_common_پیشوند(ثابت حرف *a, ثابت حرف *b) خالص;
