/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/strv.h"   /* IWYU pragma: export */

ਅੱਖਰ* strv_ਲੱਭੋ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;
ਅੱਖਰ* strv_ਲੱਭੋ_case(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;
ਅੱਖਰ* strv_ਲੱਭੋ_ਅਗੇਤਰ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;
ਅੱਖਰ* strv_ਲੱਭੋ_startswith(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;
ਅੱਖਰ* strv_ਲੱਭੋ_closest(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
ਅੱਖਰ* strv_ਲੱਭੋ_ਪਹਿਲਾ_ਖੇਤਰ(ਅੱਖਰ * ਅਚਲ *needles, ਅੱਖਰ * ਅਚਲ *haystack) ਸ਼ੁੱਧ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_ਸ਼ਾਮਲ_ਹੈ(l, s) (!!strv_ਲੱਭੋ((l), (s)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_ਸ਼ਾਮਲ_ਹੈ_case(l, s) (!!strv_ਲੱਭੋ_case((l), (s)))

ਅੱਖਰ** strv_free(ਅੱਖਰ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਅੱਖਰ**, strv_free);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_strv_free_ _cleanup_(strv_freep)

ਅੱਖਰ** strv_free_erase(ਅੱਖਰ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਅੱਖਰ**, strv_free_erase);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

ਖਾਲੀ strv_free_many(ਅੱਖਰ ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

ਅੱਖਰ** strv_copy_n(ਅੱਖਰ * ਅਚਲ *l, size_t n);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ** strv_copy(ਅੱਖਰ * ਅਚਲ *l) {
        ਵਾਪਸ strv_copy_n(l, SIZE_MAX);
}
ਪੂਰਨ strv_copy_unless_ਖਾਲੀ_ਥਾਵਾਂ(ਅੱਖਰ * ਅਚਲ *l, ਅੱਖਰ ***ret);

size_t strv_ਲੰਮਾਈ(ਅੱਖਰ * ਅਚਲ *l) ਸ਼ੁੱਧ;

ਪੂਰਨ strv_extend_strv(ਅੱਖਰ ***a, ਅੱਖਰ * ਅਚਲ *b, bool filter_duplicates);
ਪੂਰਨ strv_extend_strv_consume(ਅੱਖਰ ***a, ਅੱਖਰ **b, bool filter_duplicates);

ਪੂਰਨ strv_extend_strv_biconcat(ਅੱਖਰ ***a, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ* ਅਚਲ *b, ਅਚਲ ਅੱਖਰ *suffix);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strv_extend_strv_concat(ਅੱਖਰ ***a, ਅਚਲ ਅੱਖਰ* ਅਚਲ *b, ਅਚਲ ਅੱਖਰ *suffix) {
        ਵਾਪਸ strv_extend_strv_biconcat(a, ਨਿਰਾ, b, suffix);
}

ਪੂਰਨ strv_prepend(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

ਪੂਰਨ strv_extend_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strv_extend(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ strv_extend_with_ਮਾਪ(l, ਨਿਰਾ, ਮੁੱਲ);
}

ਪੂਰਨ strv_extend_many_ਅੰਦਰੂਨੀ(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_extend_many(l, ...) strv_extend_many_ਅੰਦਰੂਨੀ(l, __VA_ARGS__, ਸੂਚਕ_MAX)

ਪੂਰਨ strv_extendf_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅਚਲ ਅੱਖਰ *format, ...) _printf_(3,4);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_extendf(l, ...) strv_extendf_with_ਮਾਪ(l, ਨਿਰਾ, __VA_ARGS__)

ਪੂਰਨ strv_extend_joined_with_ਮਾਪ_sentinel(ਅੱਖਰ ***l, size_t *n, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_extend_joined_with_ਮਾਪ(l, n, ...) strv_extend_joined_with_ਮਾਪ_sentinel(l, n, __VA_ARGS__, ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_extend_joined(l, ...) strv_extend_joined_with_ਮਾਪ(l, ਨਿਰਾ, __VA_ARGS__)

ਪੂਰਨ strv_ਧੱਕੋ_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅੱਖਰ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strv_ਧੱਕੋ(ਅੱਖਰ ***l, ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ strv_ਧੱਕੋ_with_ਮਾਪ(l, ਨਿਰਾ, ਮੁੱਲ);
}
ਪੂਰਨ strv_ਧੱਕੋ_pair(ਅੱਖਰ ***l, ਅੱਖਰ *a, ਅੱਖਰ *b);

ਪੂਰਨ strv_ਪਾਓ(ਅੱਖਰ ***l, size_t position, ਅੱਖਰ *ਮੁੱਲ);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strv_ਧੱਕੋ_prepend(ਅੱਖਰ ***l, ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ strv_ਪਾਓ(l, 0, ਮੁੱਲ);
}

ਪੂਰਨ strv_consume_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅੱਖਰ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ strv_consume(ਅੱਖਰ ***l, ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ strv_consume_with_ਮਾਪ(l, ਨਿਰਾ, ਮੁੱਲ);
}

ਪੂਰਨ strv_consume_pair(ਅੱਖਰ ***l, ਅੱਖਰ *a, ਅੱਖਰ *b);
ਪੂਰਨ strv_consume_prepend(ਅੱਖਰ ***l, ਅੱਖਰ *ਮੁੱਲ);

ਅੱਖਰ** strv_remove(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *s);
ਅੱਖਰ** strv_remove_strv(ਅੱਖਰ **l, ਅੱਖਰ *ਅਚਲ*ll);
ਅੱਖਰ** strv_uniq(ਅੱਖਰ **l);
bool strv_is_uniq(ਅੱਖਰ * ਅਚਲ *l) ਸ਼ੁੱਧ;

ਪੂਰਨ strv_compare(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool strv_equal(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) {
        ਵਾਪਸ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_ਕ੍ਰਮ(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) ਸ਼ੁੱਧ;

ਅੱਖਰ** strv_new_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *x, ...) _sentinel_;
ਅੱਖਰ** strv_new_ap(ਅਚਲ ਅੱਖਰ *x, va_list ap);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_new(...) strv_new_ਅੰਦਰੂਨੀ(__VA_ARGS__, ਨਿਰਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRV_IGNORE ((ਅਚਲ ਅੱਖਰ *) ਸੂਚਕ_MAX)

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* STRV_IFNOTNULL(ਅਚਲ ਅੱਖਰ *x) {
        ਵਾਪਸ x ?: STRV_IGNORE;
}

ਪੂਰਨ strv_ਵੰਡੋ_ਭਰਿਆ(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags);
ਅੱਖਰ** strv_ਵੰਡੋ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators);

ਪੂਰਨ strv_ਵੰਡੋ_and_extend_ਭਰਿਆ(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators, bool filter_duplicates, ਕੱਢੋ_Flags flags);
ਪੂਰਨ strv_ਵੰਡੋ_and_extend(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators, bool filter_duplicates);

ਪੂਰਨ strv_ਵੰਡੋ_newlines_ਭਰਿਆ(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *s, ਕੱਢੋ_Flags flags);
ਅੱਖਰ** strv_ਵੰਡੋ_newlines(ਅਚਲ ਅੱਖਰ *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
ਪੂਰਨ strv_ਵੰਡੋ_colon_ਜੋੜੇ(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s);

ਅੱਖਰ* strv_join_ਭਰਿਆ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *separator, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ *strv_join(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *separator) {
        ਵਾਪਸ strv_join_ਭਰਿਆ(l, separator, ਨਿਰਾ);
}

bool strv_overlap(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) ਸ਼ੁੱਧ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        ਲਈ (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_ਲੰਮਾਈ(h);           \
                                _len > 0 ? h + _len - 1 : ਨਿਰਾ;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _STRV_FOREACH_PAIR(x, y, l, i)                          \
        ਲਈ (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

ਅੱਖਰ** strv_ਛਾਂਟੋ(ਅੱਖਰ **l);
ਅੱਖਰ** strv_ਛਾਂਟੋ_uniq_ਭਰਿਆ(ਅੱਖਰ **l, ਪੂਰਨ (*compare)(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b));
ਅੱਖਰ** strv_ਛਾਂਟੋ_uniq(ਅੱਖਰ **l);
ਖਾਲੀ strv_print_ਭਰਿਆ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ strv_print(ਅੱਖਰ * ਅਚਲ *l) {
        strv_print_ਭਰਿਆ(l, ਨਿਰਾ);
}

ਅੱਖਰ* startswith_strv_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ * ਅਚਲ *l);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ startswith_strv(s, l) const_generic(s, startswith_strv_ਅੰਦਰੂਨੀ(s, l))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARTSWITH_ਸੈੱਟ(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ਅੱਖਰ* endswith_strv_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ * ਅਚਲ *l);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ endswith_strv(s, l) const_generic(s, endswith_strv_ਅੰਦਰੂਨੀ(s, l))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENDSWITH_ਸੈੱਟ(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STR_ਵਿੱਚ_ਸੈੱਟ(x, ...) strv_ਸ਼ਾਮਲ_ਹੈ(STRV_MAKE(__VA_ARGS__), x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRPTR_ਵਿੱਚ_ਸੈੱਟ(x, ...)                                    \
        ({                                                       \
                ਅਚਲ ਅੱਖਰ* _x = (x);                            \
                _x && strv_ਸ਼ਾਮਲ_ਹੈ(STRV_MAKE(__VA_ARGS__), _x); \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCASE_ਵਿੱਚ_ਸੈੱਟ(x, ...) strv_ਸ਼ਾਮਲ_ਹੈ_case(STRV_MAKE(__VA_ARGS__), x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRCASEPTR_ਵਿੱਚ_ਸੈੱਟ(x, ...)                                    \
        ({                                                       \
                ਅਚਲ ਅੱਖਰ* _x = (x);                            \
                _x && strv_ਸ਼ਾਮਲ_ਹੈ_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _FOREACH_STRING(uniq, x, y, ...)                                \
        ਲਈ (ਅਚਲ ਅੱਖਰ *x, * ਅਚਲ*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

ਅੱਖਰ** strv_ਉਲਟ(ਅੱਖਰ **l);
ਅੱਖਰ** strv_shell_escape(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *bad);

bool strv_fnmatch_ਭਰਿਆ(ਅੱਖਰ* ਅਚਲ* patterns, ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ flags, size_t *ret_matched_pos);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool strv_fnmatch(ਅੱਖਰ* ਅਚਲ* patterns, ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ strv_fnmatch_ਭਰਿਆ(patterns, s, 0, ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool strv_fnmatch_or_ਖਾਲੀ_ਥਾਵਾਂ(ਅੱਖਰ* ਅਚਲ* patterns, ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਵਾਪਸ strv_isempty(patterns) ||
               strv_fnmatch_ਭਰਿਆ(patterns, s, flags, ਨਿਰਾ);
}

ਅੱਖਰ** strv_skip(ਅੱਖਰ **l, size_t n);

ਪੂਰਨ strv_extend_n(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t n);

ਪੂਰਨ strv_extend_assignment(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *lhs, ਅਚਲ ਅੱਖਰ *rhs);

ਪੂਰਨ fputstrv(FILE *f, ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *separator, bool *space);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_free_and_replace(a, b)             \
        free_and_replace_ਭਰਿਆ(a, b, strv_free)

ਖਾਲੀ string_strv_hashmap_remove(Hashmap *h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਖਾਲੀ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਪੂਰਨ string_strv_hashmap_put(Hashmap **h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਪੂਰਨ string_strv_ordered_hashmap_put(OrderedHashmap **h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

ਪੂਰਨ strv_rebreak_ਸਤਰਾਂ(ਅੱਖਰ **l, size_t width, ਅੱਖਰ ***ret);

ਅੱਖਰ** strv_filter_ਅਗੇਤਰ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ* ਅਚਲ strv_ਖਾਲੀ_ਥਾਵਾਂ[1];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRV_ਖਾਲੀ_ਥਾਵਾਂ ((ਅੱਖਰ**) strv_ਖਾਲੀ_ਥਾਵਾਂ)
