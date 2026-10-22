/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/string-util.h" /* IWYU pragma: export */

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* strstr_ptr_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        ਜੇ (!haystack || !needle)
                ਵਾਪਸ ਨਿਰਾ;
        ਵਾਪਸ (ਅੱਖਰ*) ਲੜੀ_ਖੋਜ(haystack, needle);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_ਅੰਦਰੂਨੀ(haystack, needle))

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* strstrafter_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        ਅੱਖਰ *p = (ਅੱਖਰ*) strstr_ptr(haystack, needle);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ p + ਲੜੀ_ਲੰਬਾਈ(needle);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_ਅੰਦਰੂਨੀ(haystack, needle))

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* strnull(ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ s ?: "(null)";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* strna(ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ s ?: "n/a";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* true_false(bool b) {
        ਵਾਪਸ b ? "true" : "false";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* plus_minus(bool b) {
        ਵਾਪਸ b ? "+" : "-";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* one_zero(bool b) {
        ਵਾਪਸ b ? "1" : "0";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* enable_disable(bool b) {
        ਵਾਪਸ b ? "enable" : "disable";
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* enabled_disabled(bool b) {
        ਵਾਪਸ b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(p)                                \
        ({                                              \
                ਅਚਲ ਅੱਖਰ *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ਨਿਰਾ : _p);  \
        })

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_na(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ isempty(p) ? "n/a" : p;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_dash(ਅਚਲ ਅੱਖਰ *str) {
        ਵਾਪਸ isempty(str) ? "-" : str;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਖਾਲੀ_ਥਾਵਾਂ_or_dash(ਅਚਲ ਅੱਖਰ *str) {
        ਵਾਪਸ !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* ਖਾਲੀ_ਥਾਵਾਂ_or_dash_ਮੰਜ਼ਿਲ_null(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_or_dash(p) ? ਨਿਰਾ : p;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖਾਲੀ_ਥਾਵਾਂ_or_dash_ਮੰਜ਼ਿਲ_null(p)                                \
        ({                                                      \
                ਅਚਲ ਅੱਖਰ *_p = (p);                           \
                (typeof(p)) (ਖਾਲੀ_ਥਾਵਾਂ_or_dash(_p) ? ਨਿਰਾ : _p);    \
        })

ਅੱਖਰ* ਪਹਿਲਾ_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ) ਸ਼ੁੱਧ;

ਅੱਖਰ* strextendn(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *s, size_t l) _nonnull_if_nonzero_(2, 3);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strjoin(a, ...) strextend_with_separator_ਅੰਦਰੂਨੀ(ਨਿਰਾ, ਨਿਰਾ, a, __VA_ARGS__, ਨਿਰਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strjoina(a, ...)                                                \
        ({                                                              \
                ਅਚਲ ਅੱਖਰ *_appendees_[] = { a, __VA_ARGS__ };         \
                ਅੱਖਰ *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                ਲਈ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += ਲੜੀ_ਲੰਬਾਈ(_appendees_[_i_]);              \
                _p_ = _d_ = newa(ਅੱਖਰ, _len_ + 1);                      \
                ਲਈ (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ਅੱਖਰ* strstrip(ਅੱਖਰ *s);
ਅੱਖਰ* delete_chars(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad);
ਅੱਖਰ* delete_trailing_chars(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad);
ਅੱਖਰ* truncate_nl_ਭਰਿਆ(ਅੱਖਰ *s, size_t *ret_len);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* truncate_nl(ਅੱਖਰ *s) {
        ਵਾਪਸ truncate_nl_ਭਰਿਆ(s, ਨਿਰਾ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* skip_leading_chars(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *bad) {
        ਜੇ (!s)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (!bad)
                bad = WHITESPACE;

        ਵਾਪਸ (ਅੱਖਰ*) s + strspn(s, bad);
}

ਅੱਖਰ ascii_tolower(ਅੱਖਰ x) _const_;
ਅੱਖਰ* ascii_strlower(ਅੱਖਰ *s);
ਅੱਖਰ* ascii_strlower_n(ਅੱਖਰ *s, size_t n);

ਅੱਖਰ ascii_toupper(ਅੱਖਰ x) _const_;
ਅੱਖਰ* ascii_strupper(ਅੱਖਰ *s);

ਪੂਰਨ ascii_strcasecmp_n(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b, size_t n);
ਪੂਰਨ ascii_strcasecmp_nn(ਅਚਲ ਅੱਖਰ *a, size_t n, ਅਚਲ ਅੱਖਰ *b, size_t m);

bool chars_intersect(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) ਸ਼ੁੱਧ;

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਸ਼ੁੱਧ ਵਿੱਚ_charset(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *charset) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(charset);
        ਵਾਪਸ s[strspn(s, charset)] == '\0';
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool char_is_cc(ਅੱਖਰ p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        ਵਾਪਸ (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ok) ਸ਼ੁੱਧ;

ਅੱਖਰ* ellipsize_mem(ਅਚਲ ਅੱਖਰ *s, size_t old_ਲੰਮਾਈ_ਬਾਈਟਾਂ, size_t new_ਲੰਮਾਈ_columns, ਬਿਨਾਨਿਸ਼ਾਨੀ percent);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* ellipsize(ਅਚਲ ਅੱਖਰ *s, size_t length, ਬਿਨਾਨਿਸ਼ਾਨੀ percent) {
        ਵਾਪਸ ellipsize_mem(s, ਲੜੀ_ਲੰਬਾਈ(s), length, percent);
}

ਅੱਖਰ* cellescape(ਅੱਖਰ *buf, size_t len, ਅਚਲ ਅੱਖਰ *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CELLESCAPE_DEFAULT_ਲੰਮਾਈ 64

ਅੱਖਰ* strshorten(ਅੱਖਰ *s, size_t l);

ਪੂਰਨ strgrowpad0(ਅੱਖਰ **s, size_t l);

ਅੱਖਰ* strreplace(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਅਚਲ ਅੱਖਰ *old_string, ਅਚਲ ਅੱਖਰ *new_string);

ਅੱਖਰ* strip_tab_ansi(ਅੱਖਰ **ibuf, size_t *_isz, size_t highlight[2]);

ਅੱਖਰ* strextendv_with_separator(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *separator, va_list ap);
ਅੱਖਰ* strextend_with_separator_ਅੰਦਰੂਨੀ(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *separator, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strextend_with_separator(x, separator, ...) strextend_with_separator_ਅੰਦਰੂਨੀ(x, separator, __VA_ARGS__, ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strextend(x, ...) strextend_with_separator_ਅੰਦਰੂਨੀ(x, ਨਿਰਾ, __VA_ARGS__, ਨਿਰਾ)

ਪੂਰਨ strextendf_with_separator(ਅੱਖਰ **x, ਅਚਲ ਅੱਖਰ *separator, ਅਚਲ ਅੱਖਰ *format, ...) _printf_(3,4);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strextendf(x, ...) strextendf_with_separator(x, ਨਿਰਾ, __VA_ARGS__)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                ਅੱਖਰ **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_ਅੰਦਰੂਨੀ(ਨਿਰਾ, (separator), __VA_ARGS__, ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(*_p_), ਨਿਰਾ); \
                ਜੇ (_s_) {                                                      \
                        ਮੁਕਤ(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strprepend(x, ...) strprepend_with_separator(x, ਨਿਰਾ, __VA_ARGS__)

ਅੱਖਰ* strrep(ਅਚਲ ਅੱਖਰ *s, size_t n);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strrepa(s, n)                                                   \
        ({                                                              \
                ਅਚਲ ਅੱਖਰ *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = ਲੜੀ_ਲੰਬਾਈ(_sss_);              \
                assert_se(MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&_len_, _nnn_));              \
                ਅੱਖਰ *_d_, *_p_;                                        \
                _p_ = _d_ = newa(ਅੱਖਰ, _len_ + 1);                      \
                ਲਈ (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

ਪੂਰਨ ਵੰਡੋ_pair(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *sep, ਅੱਖਰ **ret_ਪਹਿਲਾ, ਅੱਖਰ **ret_ਦੂਜਾ);

ਪੂਰਨ free_and_strdup(ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ free_and_strdup_warn(ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ free_and_strndup(ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *s, size_t l) _nonnull_if_nonzero_(2, 3);

ਪੂਰਨ strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *src);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strdup_ਮੰਜ਼ਿਲ(ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *src) {
        ਪੂਰਨ r = strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ASSERT_PTR(ret), src);
        ਵਾਪਸ r < 0 ? r : 0;  /* Suppress return value of 1. */
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ String_ਸੁਰੱਖਿਅਤ_Flags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_ਖਾਲੀ_ਥਾਵਾਂ         = 1 << 1, /* Allow empty strings */
        STRING_ALLOW_NEWLINES      = 1 << 2, /* Allow newlines (\n) */
        STRING_ALLOW_BACKSLASHES   = 1 << 3, /* Allow backslashes (\) */
        STRING_ALLOW_QUOTES        = 1 << 4, /* Allow quotes (" or ') */
        STRING_ALLOW_GLOBS         = 1 << 5, /* Allow globs (?, * or [) */
        STRING_FILENAME            = 1 << 6, /* Verify the string is valid as regular filename */
        STRING_FILENAME_PART       = 1 << 7, /* Verify the string is valid as part of a regular filename */
        STRING_DISALLOW_WHITESPACE = 1 << 8, /* Refuse whitespace (space, tab, newline, …) */
} String_ਸੁਰੱਖਿਅਤ_Flags;

bool string_is_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *p, String_ਸੁਰੱਖਿਅਤ_Flags flags) ਸ਼ੁੱਧ;

DISABLE_WARNING_STRINGOP_TRUNCATION;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ strncpy_exact(ਅੱਖਰ *buf, ਅਚਲ ਅੱਖਰ *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ਸਮ੍ਰਿਤੀ_startswith_no_case(ਅਚਲ ਖਾਲੀ *p, size_t sz, ਅਚਲ ਅੱਖਰ *token) {
        ਪ੍ਰਤਿਜ੍ਞਾ(token);

        size_t n = ਲੜੀ_ਲੰਬਾਈ(token);
        ਜੇ (sz < n)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਲਈ (size_t i = 0; i < n; i++)
                ਜੇ (ascii_tolower(((ਅੱਖਰ *)p)[i]) != ascii_tolower(token[i]))
                        ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ (uint8_t*) p + n;
}

ਅੱਖਰ* str_realloc(ਅੱਖਰ *p);
ਅੱਖਰ* string_erase(ਅੱਖਰ *x);

ਪੂਰਨ string_truncate_ਸਤਰਾਂ(ਅਚਲ ਅੱਖਰ *s, size_t n_ਸਤਰਾਂ, ਅੱਖਰ **ret);
ਪੂਰਨ string_ਕੱਢੋ_ਸਤਰ(ਅਚਲ ਅੱਖਰ *s, size_t i, ਅੱਖਰ **ret);

ਪੂਰਨ string_ਸ਼ਾਮਲ_ਹੈ_ਸ਼ਬਦ_strv(ਅਚਲ ਅੱਖਰ *string, ਅਚਲ ਅੱਖਰ *separators, ਅੱਖਰ * ਅਚਲ *ਸ਼ਬਦ_3, ਅਚਲ ਅੱਖਰ **ret_ਸ਼ਬਦ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ string_ਸ਼ਾਮਲ_ਹੈ_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ *string, ਅਚਲ ਅੱਖਰ *separators, ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ) {
        ਵਾਪਸ string_ਸ਼ਾਮਲ_ਹੈ_ਸ਼ਬਦ_strv(string, separators, STRV_MAKE(ਸ਼ਬਦ), ਨਿਰਾ);
}

bool streq_skip_trailing_chars(ਅਚਲ ਅੱਖਰ *s1, ਅਚਲ ਅੱਖਰ *s2, ਅਚਲ ਅੱਖਰ *ok);

ਅੱਖਰ* string_replace_char(ਅੱਖਰ *str, ਅੱਖਰ old_char, ਅੱਖਰ new_char);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Make_CString_ਬਹੁਲਕ {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        MAKE_CSTRING_ਬਹੁਲਕ_MAX,
        MAKE_CSTRING_ਬਹੁਲਕ_INVALID = -1,
} Make_CString_ਬਹੁਲਕ;

ਪੂਰਨ make_cstring(ਅਚਲ ਖਾਲੀ *s, size_t n, Make_CString_ਬਹੁਲਕ ਬਹੁਲਕ, ਅੱਖਰ **ret);

size_t strspn_from_ਸਿਰਾ(ਅਚਲ ਅੱਖਰ *str, ਅਚਲ ਅੱਖਰ *accept) ਸ਼ੁੱਧ;
size_t strnspn(ਅਚਲ ਅੱਖਰ *str, ਅਚਲ ਅੱਖਰ *accept, size_t n) ਸ਼ੁੱਧ _nonnull_if_nonzero_(1, 3);

ਅੱਖਰ* strdupspn(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *accept);
ਅੱਖਰ* strdupcspn(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strdupa_ਸੁਰੱਖਿਅਤ(s)                                                 \
        ({                                                              \
                ਅਚਲ ਅੱਖਰ *_t = (s);                                   \
                (ਅੱਖਰ*) memdupa_suffix0(_t, ਲੜੀ_ਲੰਬਾਈ(_t));                \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strndupa_ਸੁਰੱਖਿਅਤ(s, n)                                             \
        ({                                                              \
                ਅਚਲ ਅੱਖਰ *_t = (s);                                   \
                (ਅੱਖਰ*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

ਅੱਖਰ* ਲੱਭੋ_ਸਤਰ_startswith_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_ਸਤਰ_startswith(haystack, needle) \
        const_generic(haystack, ਲੱਭੋ_ਸਤਰ_startswith_ਅੰਦਰੂਨੀ(haystack, needle))

ਅੱਖਰ* ਲੱਭੋ_ਸਤਰ_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_ਸਤਰ(haystack, needle) \
        const_generic(haystack, ਲੱਭੋ_ਸਤਰ_ਅੰਦਰੂਨੀ(haystack, needle))

ਅੱਖਰ* ਲੱਭੋ_ਸਤਰ_after_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਲੱਭੋ_ਸਤਰ_after(haystack, needle) \
        const_generic(haystack, ਲੱਭੋ_ਸਤਰ_after_ਅੰਦਰੂਨੀ(haystack, needle))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਸੰਸਕਰਣ_Flags {
        ਸੰਸਕਰਣ_ALLOW_ਖਾਲੀ_ਥਾਵਾਂ      = 1 << 0,
        ਸੰਸਕਰਣ_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        ਸੰਸਕਰਣ_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} ਸੰਸਕਰਣ_Flags;

bool ਸੰਸਕਰਣ_is_valid(ਅਚਲ ਅੱਖਰ *s, ਸੰਸਕਰਣ_Flags flags) ਸ਼ੁੱਧ;

ssize_t strlevenshtein(ਅਚਲ ਅੱਖਰ *x, ਅਚਲ ਅੱਖਰ *y);

ਅੱਖਰ* strrstr_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) ਸ਼ੁੱਧ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strrstr(haystack, needle) \
        const_generic(haystack, strrstr_ਅੰਦਰੂਨੀ(haystack, needle))

ਅੱਖਰ* strrstr_no_case_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *haystack, ਅਚਲ ਅੱਖਰ *needle) ਸ਼ੁੱਧ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_ਅੰਦਰੂਨੀ(haystack, needle))

size_t str_common_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) ਸ਼ੁੱਧ;
