/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <string.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "forward.h"

#అన్తర్భావ "../fundamental/string-util.h" /* IWYU pragma: export */

స్థిర అంతఃవరుస అక్షరం* strstr_ptr_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        ఒకవేళ (!haystack || !needle)
                తిరిగి ఖాళీ;
        తిరిగి (అక్షరం*) అక్షరశ్రేణి_శోధన(haystack, needle);
}

#నిర్వచించు strstr_ptr(haystack, needle) \
        const_generic(haystack, strstr_ptr_అంతర్గత(haystack, needle))

స్థిర అంతఃవరుస అక్షరం* strstrafter_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) {
        /* Returns NULL if not found, or pointer to first character after needle if found */

        అక్షరం *p = (అక్షరం*) strstr_ptr(haystack, needle);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        తిరిగి p + అక్షరశ్రేణి_పొడవు(needle);
}

#నిర్వచించు strstrafter(haystack, needle) \
        const_generic(haystack, strstrafter_అంతర్గత(haystack, needle))

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* strnull(స్థిరరాశి అక్షరం *s) {
        తిరిగి s ?: "(null)";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* strna(స్థిరరాశి అక్షరం *s) {
        తిరిగి s ?: "n/a";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* true_false(bool b) {
        తిరిగి b ? "true" : "false";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* plus_minus(bool b) {
        తిరిగి b ? "+" : "-";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* one_zero(bool b) {
        తిరిగి b ? "1" : "0";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* enable_disable(bool b) {
        తిరిగి b ? "enable" : "disable";
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* enabled_disabled(bool b) {
        తిరిగి b ? "enabled" : "disabled";
}

/* This macro's return pointer will have the "const" qualifier set or unset the same way as the input
 * pointer. */
#నిర్వచించు ఖాళీలు_గమ్యం_null(p)                                \
        ({                                              \
                స్థిరరాశి అక్షరం *_p = (p);                   \
                (typeof(p)) (isempty(_p) ? ఖాళీ : _p);  \
        })

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ఖాళీలు_గమ్యం_na(స్థిరరాశి అక్షరం *p) {
        తిరిగి isempty(p) ? "n/a" : p;
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ఖాళీలు_గమ్యం_dash(స్థిరరాశి అక్షరం *str) {
        తిరిగి isempty(str) ? "-" : str;
}

స్థిర అంతఃవరుస bool ఖాళీలు_or_dash(స్థిరరాశి అక్షరం *str) {
        తిరిగి !str ||
                str[0] == 0 ||
                (str[0] == '-' && str[1] == 0);
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* ఖాళీలు_or_dash_గమ్యం_null(స్థిరరాశి అక్షరం *p) {
        తిరిగి ఖాళీలు_or_dash(p) ? ఖాళీ : p;
}
#నిర్వచించు ఖాళీలు_or_dash_గమ్యం_null(p)                                \
        ({                                                      \
                స్థిరరాశి అక్షరం *_p = (p);                           \
                (typeof(p)) (ఖాళీలు_or_dash(_p) ? ఖాళీ : _p);    \
        })

అక్షరం* మొదటి_మాట(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *మాట) శుద్ధ;

అక్షరం* strextendn(అక్షరం **x, స్థిరరాశి అక్షరం *s, size_t l) _nonnull_if_nonzero_(2, 3);

#నిర్వచించు strjoin(a, ...) strextend_with_separator_అంతర్గత(ఖాళీ, ఖాళీ, a, __VA_ARGS__, ఖాళీ)

#నిర్వచించు strjoina(a, ...)                                                \
        ({                                                              \
                స్థిరరాశి అక్షరం *_appendees_[] = { a, __VA_ARGS__ };         \
                అక్షరం *_d_, *_p_;                                        \
                size_t _len_ = 0;                                       \
                size_t _i_;                                             \
                కొరకు (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _len_ += అక్షరశ్రేణి_పొడవు(_appendees_[_i_]);              \
                _p_ = _d_ = newa(అక్షరం, _len_ + 1);                      \
                కొరకు (_i_ = 0; _i_ < ELEMENTSOF(_appendees_) && _appendees_[_i_]; _i_++) \
                        _p_ = stpcpy(_p_, _appendees_[_i_]);            \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

అక్షరం* strstrip(అక్షరం *s);
అక్షరం* delete_chars(అక్షరం *s, స్థిరరాశి అక్షరం *bad);
అక్షరం* delete_trailing_chars(అక్షరం *s, స్థిరరాశి అక్షరం *bad);
అక్షరం* truncate_nl_నిండిన(అక్షరం *s, size_t *ret_len);
స్థిర అంతఃవరుస అక్షరం* truncate_nl(అక్షరం *s) {
        తిరిగి truncate_nl_నిండిన(s, ఖాళీ);
}

స్థిర అంతఃవరుస అక్షరం* skip_leading_chars(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *bad) {
        ఒకవేళ (!s)
                తిరిగి ఖాళీ;

        ఒకవేళ (!bad)
                bad = WHITESPACE;

        తిరిగి (అక్షరం*) s + strspn(s, bad);
}

అక్షరం ascii_tolower(అక్షరం x) _const_;
అక్షరం* ascii_strlower(అక్షరం *s);
అక్షరం* ascii_strlower_n(అక్షరం *s, size_t n);

అక్షరం ascii_toupper(అక్షరం x) _const_;
అక్షరం* ascii_strupper(అక్షరం *s);

పూర్ణాంకం ascii_strcasecmp_n(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b, size_t n);
పూర్ణాంకం ascii_strcasecmp_nn(స్థిరరాశి అక్షరం *a, size_t n, స్థిరరాశి అక్షరం *b, size_t m);

bool chars_intersect(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) శుద్ధ;

స్థిర అంతఃవరుస bool శుద్ధ లోపల_charset(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *charset) {
        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(charset);
        తిరిగి s[strspn(s, charset)] == '\0';
}

స్థిర అంతఃవరుస bool char_is_cc(అక్షరం p) {
        /* char is unsigned on some architectures, e.g. aarch64. So, compiler may warn the condition
         * p >= 0 is always true. See #19543. Hence, let's cast to unsigned before the comparison. Note
         * that the cast in the right hand side is redundant, as according to the C standard, compilers
         * automatically cast a signed value to unsigned when comparing with an unsigned variable. Just
         * for safety and readability. */
        తిరిగి (uint8_t) p < (uint8_t) ' ' || p == 127;
}
bool string_has_cc(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *ok) శుద్ధ;

అక్షరం* ellipsize_mem(స్థిరరాశి అక్షరం *s, size_t old_నిడివి_బైట్లు, size_t new_నిడివి_columns, గుర్తులేని percent);
స్థిర అంతఃవరుస అక్షరం* ellipsize(స్థిరరాశి అక్షరం *s, size_t length, గుర్తులేని percent) {
        తిరిగి ellipsize_mem(s, అక్షరశ్రేణి_పొడవు(s), length, percent);
}

అక్షరం* cellescape(అక్షరం *buf, size_t len, స్థిరరాశి అక్షరం *s);

/* This limit is arbitrary, enough to give some idea what the string contains */
#నిర్వచించు CELLESCAPE_DEFAULT_నిడివి 64

అక్షరం* strshorten(అక్షరం *s, size_t l);

పూర్ణాంకం strgrowpad0(అక్షరం **s, size_t l);

అక్షరం* strreplace(స్థిరరాశి అక్షరం *పాఠ్యం, స్థిరరాశి అక్షరం *old_string, స్థిరరాశి అక్షరం *new_string);

అక్షరం* strip_tab_ansi(అక్షరం **ibuf, size_t *_isz, size_t highlight[2]);

అక్షరం* strextendv_with_separator(అక్షరం **x, స్థిరరాశి అక్షరం *separator, va_list ap);
అక్షరం* strextend_with_separator_అంతర్గత(అక్షరం **x, స్థిరరాశి అక్షరం *separator, ...) _sentinel_;
#నిర్వచించు strextend_with_separator(x, separator, ...) strextend_with_separator_అంతర్గత(x, separator, __VA_ARGS__, ఖాళీ)
#నిర్వచించు strextend(x, ...) strextend_with_separator_అంతర్గత(x, ఖాళీ, __VA_ARGS__, ఖాళీ)

పూర్ణాంకం strextendf_with_separator(అక్షరం **x, స్థిరరాశి అక్షరం *separator, స్థిరరాశి అక్షరం *format, ...) _printf_(3,4);
#నిర్వచించు strextendf(x, ...) strextendf_with_separator(x, ఖాళీ, __VA_ARGS__)

#నిర్వచించు strprepend_with_separator(x, separator, ...)                            \
        ({                                                                      \
                అక్షరం **_p_ = ASSERT_PTR(x), *_s_;                               \
                _s_ = strextend_with_separator_అంతర్గత(ఖాళీ, (separator), __VA_ARGS__, ఖాళీలు_గమ్యం_null(*_p_), ఖాళీ); \
                ఒకవేళ (_s_) {                                                      \
                        విడుదల(*_p_);                                             \
                        *_p_ = _s_;                                             \
                }                                                               \
                _s_;                                                            \
        })
#నిర్వచించు strprepend(x, ...) strprepend_with_separator(x, ఖాళీ, __VA_ARGS__)

అక్షరం* strrep(స్థిరరాశి అక్షరం *s, size_t n);

#నిర్వచించు strrepa(s, n)                                                   \
        ({                                                              \
                స్థిరరాశి అక్షరం *_sss_ = (s);                                \
                size_t _nnn_ = (n), _len_ = అక్షరశ్రేణి_పొడవు(_sss_);              \
                assert_se(MUL_కేటాయించు_సురక్షిత(&_len_, _nnn_));              \
                అక్షరం *_d_, *_p_;                                        \
                _p_ = _d_ = newa(అక్షరం, _len_ + 1);                      \
                కొరకు (size_t _i_ = 0; _i_ < _nnn_; _i_++)                \
                        _p_ = stpcpy(_p_, _sss_);                       \
                *_p_ = 0;                                               \
                _d_;                                                    \
        })

పూర్ణాంకం విభజించు_pair(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *sep, అక్షరం **ret_మొదటి, అక్షరం **ret_రెండవ);

పూర్ణాంకం free_and_strdup(అక్షరం **p, స్థిరరాశి అక్షరం *s);
పూర్ణాంకం free_and_strdup_warn(అక్షరం **p, స్థిరరాశి అక్షరం *s);
పూర్ణాంకం free_and_strndup(అక్షరం **p, స్థిరరాశి అక్షరం *s, size_t l) _nonnull_if_nonzero_(2, 3);

పూర్ణాంకం strdup_గమ్యం_నిండిన(అక్షరం **ret, స్థిరరాశి అక్షరం *src);
స్థిర అంతఃవరుస పూర్ణాంకం strdup_గమ్యం(అక్షరం **ret, స్థిరరాశి అక్షరం *src) {
        పూర్ణాంకం r = strdup_గమ్యం_నిండిన(ASSERT_PTR(ret), src);
        తిరిగి r < 0 ? r : 0;  /* Suppress return value of 1. */
}

రకనిర్వచనం గణన String_సురక్షిత_Flags {
        STRING_ASCII               = 1 << 0, /* Verify string is 7-Bit ASCII (rather than just UTF-8) */
        STRING_ALLOW_ఖాళీలు         = 1 << 1, /* Allow empty strings */
        STRING_ALLOW_NEWLINES      = 1 << 2, /* Allow newlines (\n) */
        STRING_ALLOW_BACKSLASHES   = 1 << 3, /* Allow backslashes (\) */
        STRING_ALLOW_QUOTES        = 1 << 4, /* Allow quotes (" or ') */
        STRING_ALLOW_GLOBS         = 1 << 5, /* Allow globs (?, * or [) */
        STRING_FILENAME            = 1 << 6, /* Verify the string is valid as regular filename */
        STRING_FILENAME_PART       = 1 << 7, /* Verify the string is valid as part of a regular filename */
        STRING_DISALLOW_WHITESPACE = 1 << 8, /* Refuse whitespace (space, tab, newline, …) */
} String_సురక్షిత_Flags;

bool string_is_సురక్షిత(స్థిరరాశి అక్షరం *p, String_సురక్షిత_Flags flags) శుద్ధ;

DISABLE_WARNING_STRINGOP_TRUNCATION;
స్థిర అంతఃవరుస శూన్య strncpy_exact(అక్షరం *buf, స్థిరరాశి అక్షరం *src, size_t buf_len) {
        strncpy(buf, src, buf_len);
}
REENABLE_WARNING;

/* Like startswith_no_case(), but operates on arbitrary memory blocks.
 * It works only for ASCII strings.
 */
స్థిర అంతఃవరుస శూన్య* జ్ఞాపకం_startswith_no_case(స్థిరరాశి శూన్య *p, size_t sz, స్థిరరాశి అక్షరం *token) {
        ప్రతిజ్ఞా(token);

        size_t n = అక్షరశ్రేణి_పొడవు(token);
        ఒకవేళ (sz < n)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(p);

        కొరకు (size_t i = 0; i < n; i++)
                ఒకవేళ (ascii_tolower(((అక్షరం *)p)[i]) != ascii_tolower(token[i]))
                        తిరిగి ఖాళీ;

        తిరిగి (uint8_t*) p + n;
}

అక్షరం* str_realloc(అక్షరం *p);
అక్షరం* string_erase(అక్షరం *x);

పూర్ణాంకం string_truncate_పంక్తులు(స్థిరరాశి అక్షరం *s, size_t n_పంక్తులు, అక్షరం **ret);
పూర్ణాంకం string_వెలికితీయి_పంక్తి(స్థిరరాశి అక్షరం *s, size_t i, అక్షరం **ret);

పూర్ణాంకం string_కలిగియుంది_మాట_strv(స్థిరరాశి అక్షరం *string, స్థిరరాశి అక్షరం *separators, అక్షరం * స్థిరరాశి *పదాలు, స్థిరరాశి అక్షరం **ret_మాట);
స్థిర అంతఃవరుస పూర్ణాంకం string_కలిగియుంది_మాట(స్థిరరాశి అక్షరం *string, స్థిరరాశి అక్షరం *separators, స్థిరరాశి అక్షరం *మాట) {
        తిరిగి string_కలిగియుంది_మాట_strv(string, separators, STRV_MAKE(మాట), ఖాళీ);
}

bool streq_skip_trailing_chars(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2, స్థిరరాశి అక్షరం *ok);

అక్షరం* string_replace_char(అక్షరం *str, అక్షరం old_char, అక్షరం new_char);

రకనిర్వచనం గణన Make_CString_బహుళకం {
        MAKE_CSTRING_REFUSE_TRAILING_NUL,
        MAKE_CSTRING_ALLOW_TRAILING_NUL,
        MAKE_CSTRING_REQUIRE_TRAILING_NUL,
        MAKE_CSTRING_బహుళకం_MAX,
        MAKE_CSTRING_బహుళకం_INVALID = -1,
} Make_CString_బహుళకం;

పూర్ణాంకం make_cstring(స్థిరరాశి శూన్య *s, size_t n, Make_CString_బహుళకం బహుళకం, అక్షరం **ret);

size_t strspn_from_చివర(స్థిరరాశి అక్షరం *str, స్థిరరాశి అక్షరం *accept) శుద్ధ;
size_t strnspn(స్థిరరాశి అక్షరం *str, స్థిరరాశి అక్షరం *accept, size_t n) శుద్ధ _nonnull_if_nonzero_(1, 3);

అక్షరం* strdupspn(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *accept);
అక్షరం* strdupcspn(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *reject);

/* These are like strdupa()/strndupa(), but honour ALLOCA_MAX */
#నిర్వచించు strdupa_సురక్షిత(s)                                                 \
        ({                                                              \
                స్థిరరాశి అక్షరం *_t = (s);                                   \
                (అక్షరం*) memdupa_suffix0(_t, అక్షరశ్రేణి_పొడవు(_t));                \
        })

#నిర్వచించు strndupa_సురక్షిత(s, n)                                             \
        ({                                                              \
                స్థిరరాశి అక్షరం *_t = (s);                                   \
                (అక్షరం*) memdupa_suffix0(_t, strnlen(_t, n));            \
        })

అక్షరం* కనుగొను_పంక్తి_startswith_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle);
#నిర్వచించు కనుగొను_పంక్తి_startswith(haystack, needle) \
        const_generic(haystack, కనుగొను_పంక్తి_startswith_అంతర్గత(haystack, needle))

అక్షరం* కనుగొను_పంక్తి_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle);
#నిర్వచించు కనుగొను_పంక్తి(haystack, needle) \
        const_generic(haystack, కనుగొను_పంక్తి_అంతర్గత(haystack, needle))

అక్షరం* కనుగొను_పంక్తి_after_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle);
#నిర్వచించు కనుగొను_పంక్తి_after(haystack, needle) \
        const_generic(haystack, కనుగొను_పంక్తి_after_అంతర్గత(haystack, needle))

రకనిర్వచనం గణన సంస్కరణ_Flags {
        సంస్కరణ_ALLOW_ఖాళీలు      = 1 << 0,
        సంస్కరణ_ALLOW_UNDERSCORE = 1 << 1, /* Allow "_" as separator (recommended separator) */
        సంస్కరణ_ALLOW_PLUS       = 1 << 2, /* Allow "+" as separator (sometimes used as separator for boot attempt counters) */
} సంస్కరణ_Flags;

bool సంస్కరణ_is_valid(స్థిరరాశి అక్షరం *s, సంస్కరణ_Flags flags) శుద్ధ;

ssize_t strlevenshtein(స్థిరరాశి అక్షరం *x, స్థిరరాశి అక్షరం *y);

అక్షరం* strrstr_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) శుద్ధ;
#నిర్వచించు strrstr(haystack, needle) \
        const_generic(haystack, strrstr_అంతర్గత(haystack, needle))

అక్షరం* strrstr_no_case_అంతర్గత(స్థిరరాశి అక్షరం *haystack, స్థిరరాశి అక్షరం *needle) శుద్ధ;
#నిర్వచించు strrstr_no_case(haystack, needle) \
        const_generic(haystack, strrstr_no_case_అంతర్గత(haystack, needle))

size_t str_common_ఉపసర్గ(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) శుద్ధ;
