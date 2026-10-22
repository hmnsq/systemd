/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/strv.h"   /* IWYU pragma: export */

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;
ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_case(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;
ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;
ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_startswith(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;
ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_closest(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕ್ಷೇತ್ರ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *needles, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *haystack) ಶುದ್ಧ;

#ವ್ಯಾಖ್ಯಾನಿಸು strv_ಒಳಗೊಂಡಿದೆ(l, s) (!!strv_ಕಂಡುಹಿಡಿ((l), (s)))
#ವ್ಯಾಖ್ಯಾನಿಸು strv_ಒಳಗೊಂಡಿದೆ_case(l, s) (!!strv_ಕಂಡುಹಿಡಿ_case((l), (s)))

ಅಕ್ಷರ** strv_free(ಅಕ್ಷರ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಅಕ್ಷರ**, strv_free);
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_strv_free_ _cleanup_(strv_freep)

ಅಕ್ಷರ** strv_free_erase(ಅಕ್ಷರ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಅಕ್ಷರ**, strv_free_erase);
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

ಶೂನ್ಯ strv_free_many(ಅಕ್ಷರ ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

ಅಕ್ಷರ** strv_copy_n(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, size_t n);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ** strv_copy(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        ಹಿಂತಿರುಗಿ strv_copy_n(l, SIZE_MAX);
}
ಪೂರ್ಣಾಂಕ strv_copy_unless_ಖಾಲಿಗಳು(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಅಕ್ಷರ ***ret);

size_t strv_ಉದ್ದಳತೆ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ strv_extend_strv(ಅಕ್ಷರ ***a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b, bool filter_duplicates);
ಪೂರ್ಣಾಂಕ strv_extend_strv_consume(ಅಕ್ಷರ ***a, ಅಕ್ಷರ **b, bool filter_duplicates);

ಪೂರ್ಣಾಂಕ strv_extend_strv_biconcat(ಅಕ್ಷರ ***a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ *b, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strv_extend_strv_concat(ಅಕ್ಷರ ***a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ *b, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix) {
        ಹಿಂತಿರುಗಿ strv_extend_strv_biconcat(a, ಖಾಲಿ, b, suffix);
}

ಪೂರ್ಣಾಂಕ strv_prepend(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

ಪೂರ್ಣಾಂಕ strv_extend_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strv_extend(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ strv_extend_with_ಗಾತ್ರ(l, ಖಾಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ strv_extend_many_ಆಂತರಿಕ(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು strv_extend_many(l, ...) strv_extend_many_ಆಂತರಿಕ(l, __VA_ARGS__, ಸೂಚಕ_MAX)

ಪೂರ್ಣಾಂಕ strv_extendf_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(3,4);
#ವ್ಯಾಖ್ಯಾನಿಸು strv_extendf(l, ...) strv_extendf_with_ಗಾತ್ರ(l, ಖಾಲಿ, __VA_ARGS__)

ಪೂರ್ಣಾಂಕ strv_extend_joined_with_ಗಾತ್ರ_sentinel(ಅಕ್ಷರ ***l, size_t *n, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು strv_extend_joined_with_ಗಾತ್ರ(l, n, ...) strv_extend_joined_with_ಗಾತ್ರ_sentinel(l, n, __VA_ARGS__, ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು strv_extend_joined(l, ...) strv_extend_joined_with_ಗಾತ್ರ(l, ಖಾಲಿ, __VA_ARGS__)

ಪೂರ್ಣಾಂಕ strv_ತಳ್ಳು_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strv_ತಳ್ಳು(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ strv_ತಳ್ಳು_with_ಗಾತ್ರ(l, ಖಾಲಿ, ಮೌಲ್ಯ);
}
ಪೂರ್ಣಾಂಕ strv_ತಳ್ಳು_pair(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *a, ಅಕ್ಷರ *b);

ಪೂರ್ಣಾಂಕ strv_ಸೇರಿಸು(ಅಕ್ಷರ ***l, size_t position, ಅಕ್ಷರ *ಮೌಲ್ಯ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strv_ತಳ್ಳು_prepend(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ strv_ಸೇರಿಸು(l, 0, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ strv_consume_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ strv_consume(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ strv_consume_with_ಗಾತ್ರ(l, ಖಾಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ strv_consume_pair(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *a, ಅಕ್ಷರ *b);
ಪೂರ್ಣಾಂಕ strv_consume_prepend(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *ಮೌಲ್ಯ);

ಅಕ್ಷರ** strv_remove(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಅಕ್ಷರ** strv_remove_strv(ಅಕ್ಷರ **l, ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ*ll);
ಅಕ್ಷರ** strv_uniq(ಅಕ್ಷರ **l);
bool strv_is_uniq(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ strv_compare(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool strv_equal(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) {
        ಹಿಂತಿರುಗಿ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_ಕ್ರಮ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) ಶುದ್ಧ;

ಅಕ್ಷರ** strv_new_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, ...) _sentinel_;
ಅಕ್ಷರ** strv_new_ap(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, va_list ap);
#ವ್ಯಾಖ್ಯಾನಿಸು strv_new(...) strv_new_ಆಂತರಿಕ(__VA_ARGS__, ಖಾಲಿ)

#ವ್ಯಾಖ್ಯಾನಿಸು STRV_IGNORE ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) ಸೂಚಕ_MAX)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* STRV_IFNOTNULL(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x) {
        ಹಿಂತಿರುಗಿ x ?: STRV_IGNORE;
}

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_ತುಂಬಿದ(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags);
ಅಕ್ಷರ** strv_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators);

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_and_extend_ತುಂಬಿದ(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, bool filter_duplicates, ಹೊರತೆಗೆ_Flags flags);
ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_and_extend(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, bool filter_duplicates);

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_newlines_ತುಂಬಿದ(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಹೊರತೆಗೆ_Flags flags);
ಅಕ್ಷರ** strv_ವಿಭಜಿಸು_newlines(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_colon_ಜೋಡಿಗಳು(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

ಅಕ್ಷರ* strv_join_ತುಂಬಿದ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ *strv_join(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator) {
        ಹಿಂತಿರುಗಿ strv_join_ತುಂಬಿದ(l, separator, ಖಾಲಿ);
}

bool strv_overlap(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) ಶುದ್ಧ;

#ವ್ಯಾಖ್ಯಾನಿಸು _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        ಗಾಗಿ (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_ಉದ್ದಳತೆ(h);           \
                                _len > 0 ? h + _len - 1 : ಖಾಲಿ;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#ವ್ಯಾಖ್ಯಾನಿಸು STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#ವ್ಯಾಖ್ಯಾನಿಸು _STRV_FOREACH_PAIR(x, y, l, i)                          \
        ಗಾಗಿ (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#ವ್ಯಾಖ್ಯಾನಿಸು STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

ಅಕ್ಷರ** strv_ವಿಂಗಡಿಸು(ಅಕ್ಷರ **l);
ಅಕ್ಷರ** strv_ವಿಂಗಡಿಸು_uniq_ತುಂಬಿದ(ಅಕ್ಷರ **l, ಪೂರ್ಣಾಂಕ (*compare)(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b));
ಅಕ್ಷರ** strv_ವಿಂಗಡಿಸು_uniq(ಅಕ್ಷರ **l);
ಶೂನ್ಯ strv_print_ತುಂಬಿದ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ strv_print(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        strv_print_ತುಂಬಿದ(l, ಖಾಲಿ);
}

ಅಕ್ಷರ* startswith_strv_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l);
#ವ್ಯಾಖ್ಯಾನಿಸು startswith_strv(s, l) const_generic(s, startswith_strv_ಆಂತರಿಕ(s, l))

#ವ್ಯಾಖ್ಯಾನಿಸು STARTSWITH_ಗಣ(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ಅಕ್ಷರ* endswith_strv_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l);
#ವ್ಯಾಖ್ಯಾನಿಸು endswith_strv(s, l) const_generic(s, endswith_strv_ಆಂತರಿಕ(s, l))

#ವ್ಯಾಖ್ಯಾನಿಸು ENDSWITH_ಗಣ(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#ವ್ಯಾಖ್ಯಾನಿಸು STR_ಒಳ_ಗಣ(x, ...) strv_ಒಳಗೊಂಡಿದೆ(STRV_MAKE(__VA_ARGS__), x)
#ವ್ಯಾಖ್ಯಾನಿಸು STRPTR_ಒಳ_ಗಣ(x, ...)                                    \
        ({                                                       \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* _x = (x);                            \
                _x && strv_ಒಳಗೊಂಡಿದೆ(STRV_MAKE(__VA_ARGS__), _x); \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು STRCASE_ಒಳ_ಗಣ(x, ...) strv_ಒಳಗೊಂಡಿದೆ_case(STRV_MAKE(__VA_ARGS__), x)
#ವ್ಯಾಖ್ಯಾನಿಸು STRCASEPTR_ಒಳ_ಗಣ(x, ...)                                    \
        ({                                                       \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* _x = (x);                            \
                _x && strv_ಒಳಗೊಂಡಿದೆ_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು _FOREACH_STRING(uniq, x, y, ...)                                \
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, * ಸ್ಥಿರಾಂಕ*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

ಅಕ್ಷರ** strv_ಹಿಮ್ಮುಖ(ಅಕ್ಷರ **l);
ಅಕ್ಷರ** strv_shell_escape(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad);

bool strv_fnmatch_ತುಂಬಿದ(ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* patterns, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ flags, size_t *ret_matched_pos);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool strv_fnmatch(ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* patterns, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ strv_fnmatch_ತುಂಬಿದ(patterns, s, 0, ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool strv_fnmatch_or_ಖಾಲಿಗಳು(ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* patterns, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ flags) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಹಿಂತಿರುಗಿ strv_isempty(patterns) ||
               strv_fnmatch_ತುಂಬಿದ(patterns, s, flags, ಖಾಲಿ);
}

ಅಕ್ಷರ** strv_skip(ಅಕ್ಷರ **l, size_t n);

ಪೂರ್ಣಾಂಕ strv_extend_n(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t n);

ಪೂರ್ಣಾಂಕ strv_extend_assignment(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lhs, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rhs);

ಪೂರ್ಣಾಂಕ fputstrv(FILE *f, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, bool *space);

#ವ್ಯಾಖ್ಯಾನಿಸು strv_free_and_replace(a, b)             \
        free_and_replace_ತುಂಬಿದ(a, b, strv_free)

ಶೂನ್ಯ string_strv_hashmap_remove(Hashmap *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಶೂನ್ಯ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ string_strv_hashmap_put(Hashmap **h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ string_strv_ordered_hashmap_put(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಂಕ strv_rebreak_ಸಾಲುಗಳು(ಅಕ್ಷರ **l, size_t width, ಅಕ್ಷರ ***ret);

ಅಕ್ಷರ** strv_filter_ಪೂರ್ವಪ್ರತ್ಯಯ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ strv_ಖಾಲಿಗಳು[1];

#ವ್ಯಾಖ್ಯಾನಿಸು STRV_ಖಾಲಿಗಳು ((ಅಕ್ಷರ**) strv_ಖಾಲಿಗಳು)
