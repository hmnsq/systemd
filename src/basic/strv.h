/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/strv.h"   /* IWYU pragma: export */

वर्ण* strv_find(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) _pure_;
वर्ण* strv_find_case(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) _pure_;
वर्ण* strv_find_prefix(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) _pure_;
वर्ण* strv_find_startswith(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) _pure_;
वर्ण* strv_find_closest(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
वर्ण* strv_find_first_field(वर्ण * ध्रुव *needles, वर्ण * ध्रुव *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

वर्ण** strv_free(वर्ण **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

वर्ण** strv_free_erase(वर्ण **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

शून्य strv_free_many(वर्ण ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

वर्ण** strv_copy_n(वर्ण * ध्रुव *l, size_t n);
स्थिर अन्तःस्थ वर्ण** strv_copy(वर्ण * ध्रुव *l) {
        प्रत्यागम strv_copy_n(l, SIZE_MAX);
}
पूर्णाङ्क strv_copy_unless_empty(वर्ण * ध्रुव *l, वर्ण ***ret);

size_t strv_length(वर्ण * ध्रुव *l) _pure_;

पूर्णाङ्क strv_extend_strv(वर्ण ***a, वर्ण * ध्रुव *b, bool filter_duplicates);
पूर्णाङ्क strv_extend_strv_consume(वर्ण ***a, वर्ण **b, bool filter_duplicates);

पूर्णाङ्क strv_extend_strv_biconcat(वर्ण ***a, ध्रुव वर्ण *prefix, ध्रुव वर्ण* ध्रुव *b, ध्रुव वर्ण *suffix);
स्थिर अन्तःस्थ पूर्णाङ्क strv_extend_strv_concat(वर्ण ***a, ध्रुव वर्ण* ध्रुव *b, ध्रुव वर्ण *suffix) {
        प्रत्यागम strv_extend_strv_biconcat(a, रिक्त, b, suffix);
}

पूर्णाङ्क strv_prepend(वर्ण ***l, ध्रुव वर्ण *मूल्य);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

पूर्णाङ्क strv_extend_with_size(वर्ण ***l, size_t *n, ध्रुव वर्ण *मूल्य);
स्थिर अन्तःस्थ पूर्णाङ्क strv_extend(वर्ण ***l, ध्रुव वर्ण *मूल्य) {
        प्रत्यागम strv_extend_with_size(l, रिक्त, मूल्य);
}

पूर्णाङ्क strv_extend_many_internal(वर्ण ***l, ध्रुव वर्ण *मूल्य, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

पूर्णाङ्क strv_extendf_with_size(वर्ण ***l, size_t *n, ध्रुव वर्ण *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, रिक्त, __VA_ARGS__)

पूर्णाङ्क strv_extend_joined_with_size_sentinel(वर्ण ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, रिक्त)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, रिक्त, __VA_ARGS__)

पूर्णाङ्क strv_धकेलो_with_size(वर्ण ***l, size_t *n, वर्ण *मूल्य);
स्थिर अन्तःस्थ पूर्णाङ्क strv_धकेलो(वर्ण ***l, वर्ण *मूल्य) {
        प्रत्यागम strv_धकेलो_with_size(l, रिक्त, मूल्य);
}
पूर्णाङ्क strv_धकेलो_pair(वर्ण ***l, वर्ण *a, वर्ण *b);

पूर्णाङ्क strv_जोड़ो(वर्ण ***l, size_t position, वर्ण *मूल्य);

स्थिर अन्तःस्थ पूर्णाङ्क strv_धकेलो_prepend(वर्ण ***l, वर्ण *मूल्य) {
        प्रत्यागम strv_जोड़ो(l, 0, मूल्य);
}

पूर्णाङ्क strv_consume_with_size(वर्ण ***l, size_t *n, वर्ण *मूल्य);
स्थिर अन्तःस्थ पूर्णाङ्क strv_consume(वर्ण ***l, वर्ण *मूल्य) {
        प्रत्यागम strv_consume_with_size(l, रिक्त, मूल्य);
}

पूर्णाङ्क strv_consume_pair(वर्ण ***l, वर्ण *a, वर्ण *b);
पूर्णाङ्क strv_consume_prepend(वर्ण ***l, वर्ण *मूल्य);

वर्ण** strv_remove(वर्ण **l, ध्रुव वर्ण *s);
वर्ण** strv_remove_strv(वर्ण **l, वर्ण *ध्रुव*ll);
वर्ण** strv_uniq(वर्ण **l);
bool strv_is_uniq(वर्ण * ध्रुव *l) _pure_;

पूर्णाङ्क strv_compare(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) _pure_;
स्थिर अन्तःस्थ bool strv_equal(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) {
        प्रत्यागम strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) _pure_;

वर्ण** strv_new_internal(ध्रुव वर्ण *x, ...) _sentinel_;
वर्ण** strv_new_ap(ध्रुव वर्ण *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, रिक्त)

#define STRV_IGNORE ((ध्रुव वर्ण *) POINTER_MAX)

स्थिर अन्तःस्थ ध्रुव वर्ण* STRV_IFNOTNULL(ध्रुव वर्ण *x) {
        प्रत्यागम x ?: STRV_IGNORE;
}

पूर्णाङ्क strv_split_full(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, ExtractFlags flags);
वर्ण** strv_split(ध्रुव वर्ण *s, ध्रुव वर्ण *separators);

पूर्णाङ्क strv_split_and_extend_full(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, bool filter_duplicates, ExtractFlags flags);
पूर्णाङ्क strv_split_and_extend(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, bool filter_duplicates);

पूर्णाङ्क strv_split_newlines_full(वर्ण ***ret, ध्रुव वर्ण *s, ExtractFlags flags);
वर्ण** strv_split_newlines(ध्रुव वर्ण *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
पूर्णाङ्क strv_split_colon_pairs(वर्ण ***t, ध्रुव वर्ण *s);

वर्ण* strv_join_full(वर्ण * ध्रुव *l, ध्रुव वर्ण *separator, ध्रुव वर्ण *prefix);
स्थिर अन्तःस्थ वर्ण *strv_join(वर्ण * ध्रुव *l, ध्रुव वर्ण *separator) {
        प्रत्यागम strv_join_full(l, separator, रिक्त);
}

bool strv_overlap(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        प्रति (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : रिक्त;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        प्रति (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

वर्ण** strv_sort(वर्ण **l);
वर्ण** strv_sort_uniq_full(वर्ण **l, पूर्णाङ्क (*compare)(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b));
वर्ण** strv_sort_uniq(वर्ण **l);
शून्य strv_print_full(वर्ण * ध्रुव *l, ध्रुव वर्ण *prefix);
स्थिर अन्तःस्थ शून्य strv_print(वर्ण * ध्रुव *l) {
        strv_print_full(l, रिक्त);
}

वर्ण* startswith_strv_internal(ध्रुव वर्ण *s, वर्ण * ध्रुव *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

वर्ण* endswith_strv_internal(ध्रुव वर्ण *s, वर्ण * ध्रुव *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ध्रुव वर्ण* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ध्रुव वर्ण* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        प्रति (ध्रुव वर्ण *x, * ध्रुव*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

वर्ण** strv_reverse(वर्ण **l);
वर्ण** strv_shell_escape(वर्ण **l, ध्रुव वर्ण *bad);

bool strv_fnmatch_full(वर्ण* ध्रुव* patterns, ध्रुव वर्ण *s, पूर्णाङ्क flags, size_t *ret_matched_pos);
स्थिर अन्तःस्थ bool strv_fnmatch(वर्ण* ध्रुव* patterns, ध्रुव वर्ण *s) {
        प्रत्यागम strv_fnmatch_full(patterns, s, 0, रिक्त);
}
स्थिर अन्तःस्थ bool strv_fnmatch_or_empty(वर्ण* ध्रुव* patterns, ध्रुव वर्ण *s, पूर्णाङ्क flags) {
        प्रतिज्ञा(s);
        प्रत्यागम strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, रिक्त);
}

वर्ण** strv_skip(वर्ण **l, size_t n);

पूर्णाङ्क strv_extend_n(वर्ण ***l, ध्रुव वर्ण *मूल्य, size_t n);

पूर्णाङ्क strv_extend_assignment(वर्ण ***l, ध्रुव वर्ण *lhs, ध्रुव वर्ण *rhs);

पूर्णाङ्क fputstrv(FILE *f, वर्ण * ध्रुव *l, ध्रुव वर्ण *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

शून्य string_strv_hashmap_remove(Hashmap *h, ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य);
शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य);
पूर्णाङ्क string_strv_hashmap_put(Hashmap **h, ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य);
पूर्णाङ्क string_strv_ordered_hashmap_put(OrderedHashmap **h, ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य);

पूर्णाङ्क strv_rebreak_पंक्तियाँ(वर्ण **l, size_t width, वर्ण ***ret);

वर्ण** strv_filter_prefix(वर्ण * ध्रुव *l, ध्रुव वर्ण *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
बाह्य ध्रुव वर्ण* ध्रुव strv_empty[1];

#define STRV_EMPTY ((वर्ण**) strv_empty)
