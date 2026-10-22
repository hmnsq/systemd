/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/strv.h"   /* IWYU pragma: export */

ವರ್ಣ* strv_find(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) _pure_;
ವರ್ಣ* strv_find_case(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) _pure_;
ವರ್ಣ* strv_find_prefix(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) _pure_;
ವರ್ಣ* strv_find_startswith(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) _pure_;
ವರ್ಣ* strv_find_closest(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
ವರ್ಣ* strv_find_first_field(ವರ್ಣ * ಧ್ರುವ *needles, ವರ್ಣ * ಧ್ರುವ *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

ವರ್ಣ** strv_free(ವರ್ಣ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ವರ್ಣ**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

ವರ್ಣ** strv_free_erase(ವರ್ಣ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ವರ್ಣ**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

ಶೂನ್ಯ strv_free_many(ವರ್ಣ ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

ವರ್ಣ** strv_copy_n(ವರ್ಣ * ಧ್ರುವ *l, size_t n);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ** strv_copy(ವರ್ಣ * ಧ್ರುವ *l) {
        ಪ್ರತ್ಯಾಗಮ strv_copy_n(l, SIZE_MAX);
}
ಪೂರ್ಣಾಙ್ಕ strv_copy_unless_empty(ವರ್ಣ * ಧ್ರುವ *l, ವರ್ಣ ***ret);

size_t strv_length(ವರ್ಣ * ಧ್ರುವ *l) _pure_;

ಪೂರ್ಣಾಙ್ಕ strv_extend_strv(ವರ್ಣ ***a, ವರ್ಣ * ಧ್ರುವ *b, bool filter_duplicates);
ಪೂರ್ಣಾಙ್ಕ strv_extend_strv_consume(ವರ್ಣ ***a, ವರ್ಣ **b, bool filter_duplicates);

ಪೂರ್ಣಾಙ್ಕ strv_extend_strv_biconcat(ವರ್ಣ ***a, ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ *b, ಧ್ರುವ ವರ್ಣ *suffix);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strv_extend_strv_concat(ವರ್ಣ ***a, ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ *b, ಧ್ರುವ ವರ್ಣ *suffix) {
        ಪ್ರತ್ಯಾಗಮ strv_extend_strv_biconcat(a, ರಿಕ್ತ, b, suffix);
}

ಪೂರ್ಣಾಙ್ಕ strv_prepend(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

ಪೂರ್ಣಾಙ್ಕ strv_extend_with_size(ವರ್ಣ ***l, size_t *n, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strv_extend(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ strv_extend_with_size(l, ರಿಕ್ತ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_many_internal(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

ಪೂರ್ಣಾಙ್ಕ strv_extendf_with_size(ವರ್ಣ ***l, size_t *n, ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, ರಿಕ್ತ, __VA_ARGS__)

ಪೂರ್ಣಾಙ್ಕ strv_extend_joined_with_size_sentinel(ವರ್ಣ ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, ರಿಕ್ತ)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, ರಿಕ್ತ, __VA_ARGS__)

ಪೂರ್ಣಾಙ್ಕ strv_ತಳ್ಳು_with_size(ವರ್ಣ ***l, size_t *n, ವರ್ಣ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strv_ತಳ್ಳು(ವರ್ಣ ***l, ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ strv_ತಳ್ಳು_with_size(l, ರಿಕ್ತ, ಮೌಲ್ಯ);
}
ಪೂರ್ಣಾಙ್ಕ strv_ತಳ್ಳು_pair(ವರ್ಣ ***l, ವರ್ಣ *a, ವರ್ಣ *b);

ಪೂರ್ಣಾಙ್ಕ strv_ಸೇರಿಸು(ವರ್ಣ ***l, size_t position, ವರ್ಣ *ಮೌಲ್ಯ);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strv_ತಳ್ಳು_prepend(ವರ್ಣ ***l, ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ strv_ಸೇರಿಸು(l, 0, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ strv_consume_with_size(ವರ್ಣ ***l, size_t *n, ವರ್ಣ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ strv_consume(ವರ್ಣ ***l, ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ strv_consume_with_size(l, ರಿಕ್ತ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ strv_consume_pair(ವರ್ಣ ***l, ವರ್ಣ *a, ವರ್ಣ *b);
ಪೂರ್ಣಾಙ್ಕ strv_consume_prepend(ವರ್ಣ ***l, ವರ್ಣ *ಮೌಲ್ಯ);

ವರ್ಣ** strv_remove(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *s);
ವರ್ಣ** strv_remove_strv(ವರ್ಣ **l, ವರ್ಣ *ಧ್ರುವ*ll);
ವರ್ಣ** strv_uniq(ವರ್ಣ **l);
bool strv_is_uniq(ವರ್ಣ * ಧ್ರುವ *l) _pure_;

ಪೂರ್ಣಾಙ್ಕ strv_compare(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool strv_equal(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) {
        ಪ್ರತ್ಯಾಗಮ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) _pure_;

ವರ್ಣ** strv_new_internal(ಧ್ರುವ ವರ್ಣ *x, ...) _sentinel_;
ವರ್ಣ** strv_new_ap(ಧ್ರುವ ವರ್ಣ *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, ರಿಕ್ತ)

#define STRV_IGNORE ((ಧ್ರುವ ವರ್ಣ *) POINTER_MAX)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* STRV_IFNOTNULL(ಧ್ರುವ ವರ್ಣ *x) {
        ಪ್ರತ್ಯಾಗಮ x ?: STRV_IGNORE;
}

ಪೂರ್ಣಾಙ್ಕ strv_split_full(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags);
ವರ್ಣ** strv_split(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators);

ಪೂರ್ಣಾಙ್ಕ strv_split_and_extend_full(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators, bool filter_duplicates, ExtractFlags flags);
ಪೂರ್ಣಾಙ್ಕ strv_split_and_extend(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators, bool filter_duplicates);

ಪೂರ್ಣಾಙ್ಕ strv_split_newlines_full(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *s, ExtractFlags flags);
ವರ್ಣ** strv_split_newlines(ಧ್ರುವ ವರ್ಣ *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
ಪೂರ್ಣಾಙ್ಕ strv_split_colon_pairs(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s);

ವರ್ಣ* strv_join_full(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *separator, ಧ್ರುವ ವರ್ಣ *prefix);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ *strv_join(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *separator) {
        ಪ್ರತ್ಯಾಗಮ strv_join_full(l, separator, ರಿಕ್ತ);
}

bool strv_overlap(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        ಪ್ರತಿ (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : ರಿಕ್ತ;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        ಪ್ರತಿ (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

ವರ್ಣ** strv_sort(ವರ್ಣ **l);
ವರ್ಣ** strv_sort_uniq_full(ವರ್ಣ **l, ಪೂರ್ಣಾಙ್ಕ (*compare)(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b));
ವರ್ಣ** strv_sort_uniq(ವರ್ಣ **l);
ಶೂನ್ಯ strv_print_full(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *prefix);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ strv_print(ವರ್ಣ * ಧ್ರುವ *l) {
        strv_print_full(l, ರಿಕ್ತ);
}

ವರ್ಣ* startswith_strv_internal(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ * ಧ್ರುವ *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ವರ್ಣ* endswith_strv_internal(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ * ಧ್ರುವ *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ಧ್ರುವ ವರ್ಣ* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ಧ್ರುವ ವರ್ಣ* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *x, * ಧ್ರುವ*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

ವರ್ಣ** strv_reverse(ವರ್ಣ **l);
ವರ್ಣ** strv_shell_escape(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *bad);

bool strv_fnmatch_full(ವರ್ಣ* ಧ್ರುವ* patterns, ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ flags, size_t *ret_matched_pos);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool strv_fnmatch(ವರ್ಣ* ಧ್ರುವ* patterns, ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ strv_fnmatch_full(patterns, s, 0, ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool strv_fnmatch_or_empty(ವರ್ಣ* ಧ್ರುವ* patterns, ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತ್ಯಾಗಮ strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, ರಿಕ್ತ);
}

ವರ್ಣ** strv_skip(ವರ್ಣ **l, size_t n);

ಪೂರ್ಣಾಙ್ಕ strv_extend_n(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t n);

ಪೂರ್ಣಾಙ್ಕ strv_extend_assignment(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *lhs, ಧ್ರುವ ವರ್ಣ *rhs);

ಪೂರ್ಣಾಙ್ಕ fputstrv(FILE *f, ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

ಶೂನ್ಯ string_strv_hashmap_remove(Hashmap *h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಶೂನ್ಯ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ string_strv_hashmap_put(Hashmap **h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ string_strv_ordered_hashmap_put(OrderedHashmap **h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಙ್ಕ strv_rebreak_ಸಾಲುಗಳು(ವರ್ಣ **l, size_t width, ವರ್ಣ ***ret);

ವರ್ಣ** strv_filter_prefix(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ strv_empty[1];

#define STRV_EMPTY ((ವರ್ಣ**) strv_empty)
