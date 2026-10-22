/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "forward.h"

#ضمن "../fundamental/string-util.h" /* IWYU pragma: export */

ساكن مضمن حرف* strstr_ptr_اندروني(ثابت حرف *haystack, ثابت حرف *needle) {
        إذا (!haystack || !needle)
                أرجع عدم;
        أرجع (حرف*) لڙي_ڳولا(haystack, needle);
}

#وصف_ڪريو strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_اندروني(haystack, needle))

ساكن مضمن حرف* strstrafter_اندروني(ثابت حرف *haystack, ثابت حرف *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        حرف *p = (حرف*) strstr_ptr(haystack, needle);
        إذا (!p)
                أرجع عدم;

        أرجع p + لڙي_ڊيگهه(needle);
}

#وصف_ڪريو strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_اندروني(haystack, needle))

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
#وصف_ڪريو خالي_جايون_منزل_null(p)                                \
        ({                                              \
                ثابت حرف *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? عدم : _p);  \
        })

ساكن مضمن ثابت حرف* خالي_جايون_منزل_na(ثابت حرف *p) {
        أرجع isempty(p) ? "n/a" : p;
}

ساكن مضمن ثابت حرف* خالي_جايون_منزل_dash(ثابت حرف *str) {
        أرجع isempty(str) ? "-" : str;
}

ساكن مضمن bool خالي_جايون_or_dash(ثابت حرف *str) {
        أرجع !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ساكن مضمن ثابت حرف* خالي_جايون_or_dash_منزل_null(ثابت حرف *p) {
        أرجع خالي_جايون_or_dash(p) ? عدم : p;
}
#وصف_ڪريو خالي_جايون_or_dash_منزل_null(p)                                \
        ({                                                      \
                ثابت حرف *_p = (p);                           \
                (typeof(p)) (خالي_جايون_or_dash(_p) ? عدم : _p);    \
        })

حرف* پهريون_لفظ(ثابت حرف *s, ثابت حرف *لفظ) خالص;

حرف* strextendn(حرف **x, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

#وصف_ڪريو strjoin(a, ...) strextend_with_separator_اندروني(عدم, عدم, a, __VA_ARGS__, عدم)

#وصف_ڪريو strjoina(a, ...)                                                \
        ({                                                              \
                ثابت حرف *_appendees_[] = { a, __VA_ARGS__ };         \
                حرف *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                لكل (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += لڙي_ڊيگهه(_appendees_[_i_]);              \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                لكل (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

حرف* strstrip(حرف *s);
حرف* delete_chars(حرف *s, ثابت حرف *bad);
حرف* delete_trailing_chars(حرف *s, ثابت حرف *bad);
حرف* truncate_nl_ڀريل(حرف *s, size_t *ret_len);
ساكن مضمن حرف* truncate_nl(حرف *s) {
        أرجع truncate_nl_ڀريل(s, عدم);
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

ساكن مضمن bool خالص اندريون_charset(ثابت حرف *s, ثابت حرف *charset) {
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

حرف* ellipsize_mem(ثابت حرف *s, size_t old_ڊيگهائي_بائيٽون, size_t new_ڊيگهائي_columns, بنانشان percent);
ساكن مضمن حرف* ellipsize(ثابت حرف *s, size_t length, بنانشان percent) {
        أرجع ellipsize_mem(s, لڙي_ڊيگهه(s), length, percent);
}

حرف* cellescape(حرف *buf, size_t len, ثابت حرف *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#وصف_ڪريو CELLESCAPE_DEFAULT_ڊيگهائي 64

حرف* strshorten(حرف *s, size_t l);

صحيح strgrowpad0(حرف **s, size_t l);

حرف* strreplace(ثابت حرف *متن, ثابت حرف *old_string, ثابت حرف *new_string);

حرف* strip_tab_ansi(حرف **ibuf, size_t *_isz, size_t highlight[2]);

حرف* strextendv_with_separator(حرف **x, ثابت حرف *separator, va_list ap);
حرف* strextend_with_separator_اندروني(حرف **x, ثابت حرف *separator, ...) _sentinel_;
#وصف_ڪريو strextend_with_separator(x, separator, ...) strextend_with_separator_اندروني(x, separator, __VA_ARGS__, عدم)
#وصف_ڪريو strextend(x, ...) strextend_with_separator_اندروني(x, عدم, __VA_ARGS__, عدم)

صحيح strextendf_with_separator(حرف **x, ثابت حرف *separator, ثابت حرف *format, ...) _printf_(3,4);
#وصف_ڪريو strextendf(x, ...) strextendf_with_separator(x, عدم, __VA_ARGS__)

#وصف_ڪريو strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                حرف **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_اندروني(عدم, (separator), __VA_ARGS__, خالي_جايون_منزل_null(*_p_), عدم); \
                إذا (_s_) {                                                      \
                        حرر(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#وصف_ڪريو strprepend(x, ...) strprepend_with_separator(x, عدم, __VA_ARGS__)

حرف* strrep(ثابت حرف *s, size_t n);

#وصف_ڪريو strrepa(s, n)                                                   \
        ({                                                              \
                ثابت حرف *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = لڙي_ڊيگهه(_sss_);              \
                assert_se(MUL_سونپيو_محفوظ(&_len_, _nnn_));              \
                حرف *_d_, *_p_;                                        \
                _p_ = _d_ = newa(حرف, _len_ + 1);                      \
                لكل (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

صحيح ورهايو_pair(ثابت حرف *s, ثابت حرف *sep, حرف **ret_پهريون, حرف **ret_ٻيو);

صحيح free_and_strdup(حرف **p, ثابت حرف *s);
صحيح free_and_strdup_warn(حرف **p, ثابت حرف *s);
صحيح free_and_strndup(حرف **p, ثابت حرف *s, size_t l) _nonnull_if_nonzero_(2, 3);

صحيح strdup_منزل_ڀريل(حرف **ret, ثابت حرف *src);
ساكن مضمن صحيح strdup_منزل(حرف **ret, ثابت حرف *src) {
        صحيح r = strdup_منزل_ڀريل(ASSERT_PTR(ret), src);
        أرجع r < 0 ? r : 0;  /* Suppress return value of 1. */
}

تعريفنوع تعداد String_محفوظ_Flags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_خالي_جايون         = 1 << 1, /* Allow empty strings */
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
ساكن مضمن فراغ strncpy_exact(حرف *buf, ثابت حرف *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ساكن مضمن فراغ* يادگيري_startswith_no_case(ثابت فراغ *p, size_t sz, ثابت حرف *token) {
        أكد(token);

        size_t n = لڙي_ڊيگهه(token);
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

صحيح string_truncate_سٽون(ثابت حرف *s, size_t n_سٽون, حرف **ret);
صحيح string_ڪڍو_سٽ(ثابت حرف *s, size_t i, حرف **ret);

صحيح string_شامل_آهي_لفظ_strv(ثابت حرف *string, ثابت حرف *separators, حرف * ثابت *لفظ_3, ثابت حرف **ret_لفظ);
ساكن مضمن صحيح string_شامل_آهي_لفظ(ثابت حرف *string, ثابت حرف *separators, ثابت حرف *لفظ) {
        أرجع string_شامل_آهي_لفظ_strv(string, separators, STRV_MAKE(لفظ), عدم);
}

bool streq_skip_trailing_chars(ثابت حرف *s1, ثابت حرف *s2, ثابت حرف *ok);

حرف* string_replace_char(حرف *str, حرف old_char, حرف new_char);

تعريفنوع تعداد Make_CString_گهڻائي {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        MAKE_CSTRING_گهڻائي_MAX,
        MAKE_CSTRING_گهڻائي_INVALID = -1,
} Make_CString_گهڻائي;

صحيح make_cstring(ثابت فراغ *s, size_t n, Make_CString_گهڻائي گهڻائي, حرف **ret);

size_t strspn_from_پڇاڙي(ثابت حرف *str, ثابت حرف *accept) خالص;
size_t strnspn(ثابت حرف *str, ثابت حرف *accept, size_t n) خالص _nonnull_if_nonzero_(1, 3);

حرف* strdupspn(ثابت حرف *a, ثابت حرف *accept);
حرف* strdupcspn(ثابت حرف *a, ثابت حرف *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#وصف_ڪريو strdupa_محفوظ(s)                                                 \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, لڙي_ڊيگهه(_t));                \
        })

#وصف_ڪريو strndupa_محفوظ(s, n)                                             \
        ({                                                              \
                ثابت حرف *_t = (s);                                   \
                (حرف*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

حرف* ڳوليو_سٽ_startswith_اندروني(ثابت حرف *haystack, ثابت حرف *needle);
#وصف_ڪريو ڳوليو_سٽ_startswith(haystack, needle) \
        const_generic(haystack, ڳوليو_سٽ_startswith_اندروني(haystack, needle))

حرف* ڳوليو_سٽ_اندروني(ثابت حرف *haystack, ثابت حرف *needle);
#وصف_ڪريو ڳوليو_سٽ(haystack, needle) \
        const_generic(haystack, ڳوليو_سٽ_اندروني(haystack, needle))

حرف* ڳوليو_سٽ_after_اندروني(ثابت حرف *haystack, ثابت حرف *needle);
#وصف_ڪريو ڳوليو_سٽ_after(haystack, needle) \
        const_generic(haystack, ڳوليو_سٽ_after_اندروني(haystack, needle))

تعريفنوع تعداد نسخو_Flags {
        نسخو_ALLOW_خالي_جايون      = 1 << 0,
        نسخو_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        نسخو_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} نسخو_Flags;

bool نسخو_is_valid(ثابت حرف *s, نسخو_Flags flags) خالص;

ssize_t strlevenshtein(ثابت حرف *x, ثابت حرف *y);

حرف* strrstr_اندروني(ثابت حرف *haystack, ثابت حرف *needle) خالص;
#وصف_ڪريو strrstr(haystack, needle) \
        const_generic(haystack, strrstr_اندروني(haystack, needle))

حرف* strrstr_no_case_اندروني(ثابت حرف *haystack, ثابت حرف *needle) خالص;
#وصف_ڪريو strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_اندروني(haystack, needle))

size_t str_common_اڳياڙي(ثابت حرف *a, ثابت حرف *b) خالص;
