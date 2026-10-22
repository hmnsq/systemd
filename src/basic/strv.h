/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/strv.h"   /* IWYU pragma: export */

वर्ण* strv_find(वर्ण * अचर *l, अचर वर्ण *name) _pure_;
वर्ण* strv_find_case(वर्ण * अचर *l, अचर वर्ण *name) _pure_;
वर्ण* strv_find_prefix(वर्ण * अचर *l, अचर वर्ण *name) _pure_;
वर्ण* strv_find_startswith(वर्ण * अचर *l, अचर वर्ण *name) _pure_;
वर्ण* strv_find_closest(वर्ण * अचर *l, अचर वर्ण *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
वर्ण* strv_find_first_field(वर्ण * अचर *needles, वर्ण * अचर *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

वर्ण** strv_free(वर्ण **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

वर्ण** strv_free_erase(वर्ण **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

शून्य strv_free_many(वर्ण ***strvs, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);

वर्ण** strv_copy_n(वर्ण * अचर *l, माप_प्रकार n);
स्थिर अन्तःस्थ वर्ण** strv_copy(वर्ण * अचर *l) {
        फर्काऊ strv_copy_n(l, SIZE_MAX);
}
पूर्णाङ्क strv_copy_unless_empty(वर्ण * अचर *l, वर्ण ***ret);

माप_प्रकार strv_length(वर्ण * अचर *l) _pure_;

पूर्णाङ्क strv_extend_strv(वर्ण ***a, वर्ण * अचर *b, bool filter_duplicates);
पूर्णाङ्क strv_extend_strv_consume(वर्ण ***a, वर्ण **b, bool filter_duplicates);

पूर्णाङ्क strv_extend_strv_biconcat(वर्ण ***a, अचर वर्ण *prefix, अचर वर्ण* अचर *b, अचर वर्ण *suffix);
स्थिर अन्तःस्थ पूर्णाङ्क strv_extend_strv_concat(वर्ण ***a, अचर वर्ण* अचर *b, अचर वर्ण *suffix) {
        फर्काऊ strv_extend_strv_biconcat(a, रिक्त, b, suffix);
}

पूर्णाङ्क strv_prepend(वर्ण ***l, अचर वर्ण *मान);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

पूर्णाङ्क strv_extend_with_size(वर्ण ***l, माप_प्रकार *n, अचर वर्ण *मान);
स्थिर अन्तःस्थ पूर्णाङ्क strv_extend(वर्ण ***l, अचर वर्ण *मान) {
        फर्काऊ strv_extend_with_size(l, रिक्त, मान);
}

पूर्णाङ्क strv_extend_many_internal(वर्ण ***l, अचर वर्ण *मान, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

पूर्णाङ्क strv_extendf_with_size(वर्ण ***l, माप_प्रकार *n, अचर वर्ण *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, रिक्त, __VA_ARGS__)

पूर्णाङ्क strv_extend_joined_with_size_sentinel(वर्ण ***l, माप_प्रकार *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, रिक्त)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, रिक्त, __VA_ARGS__)

पूर्णाङ्क strv_थप_with_size(वर्ण ***l, माप_प्रकार *n, वर्ण *मान);
स्थिर अन्तःस्थ पूर्णाङ्क strv_थप(वर्ण ***l, वर्ण *मान) {
        फर्काऊ strv_थप_with_size(l, रिक्त, मान);
}
पूर्णाङ्क strv_थप_pair(वर्ण ***l, वर्ण *a, वर्ण *b);

पूर्णाङ्क strv_घुसाउ(वर्ण ***l, माप_प्रकार position, वर्ण *मान);

स्थिर अन्तःस्थ पूर्णाङ्क strv_थप_prepend(वर्ण ***l, वर्ण *मान) {
        फर्काऊ strv_घुसाउ(l, 0, मान);
}

पूर्णाङ्क strv_consume_with_size(वर्ण ***l, माप_प्रकार *n, वर्ण *मान);
स्थिर अन्तःस्थ पूर्णाङ्क strv_consume(वर्ण ***l, वर्ण *मान) {
        फर्काऊ strv_consume_with_size(l, रिक्त, मान);
}

पूर्णाङ्क strv_consume_pair(वर्ण ***l, वर्ण *a, वर्ण *b);
पूर्णाङ्क strv_consume_prepend(वर्ण ***l, वर्ण *मान);

वर्ण** strv_remove(वर्ण **l, अचर वर्ण *s);
वर्ण** strv_remove_strv(वर्ण **l, वर्ण *अचर*ll);
वर्ण** strv_uniq(वर्ण **l);
bool strv_is_uniq(वर्ण * अचर *l) _pure_;

पूर्णाङ्क strv_compare(वर्ण * अचर *a, वर्ण * अचर *b) _pure_;
स्थिर अन्तःस्थ bool strv_equal(वर्ण * अचर *a, वर्ण * अचर *b) {
        फर्काऊ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(वर्ण * अचर *a, वर्ण * अचर *b) _pure_;

वर्ण** strv_new_internal(अचर वर्ण *x, ...) _sentinel_;
वर्ण** strv_new_ap(अचर वर्ण *x, बहु_सूची ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, रिक्त)

#define STRV_IGNORE ((अचर वर्ण *) POINTER_MAX)

स्थिर अन्तःस्थ अचर वर्ण* STRV_IFNOTNULL(अचर वर्ण *x) {
        फर्काऊ x ?: STRV_IGNORE;
}

पूर्णाङ्क strv_split_full(वर्ण ***t, अचर वर्ण *s, अचर वर्ण *separators, ExtractFlags flags);
वर्ण** strv_split(अचर वर्ण *s, अचर वर्ण *separators);

पूर्णाङ्क strv_split_and_extend_full(वर्ण ***t, अचर वर्ण *s, अचर वर्ण *separators, bool filter_duplicates, ExtractFlags flags);
पूर्णाङ्क strv_split_and_extend(वर्ण ***t, अचर वर्ण *s, अचर वर्ण *separators, bool filter_duplicates);

पूर्णाङ्क strv_split_newlines_full(वर्ण ***ret, अचर वर्ण *s, ExtractFlags flags);
वर्ण** strv_split_newlines(अचर वर्ण *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
पूर्णाङ्क strv_split_colon_pairs(वर्ण ***t, अचर वर्ण *s);

वर्ण* strv_join_full(वर्ण * अचर *l, अचर वर्ण *separator, अचर वर्ण *prefix);
स्थिर अन्तःस्थ वर्ण *strv_join(वर्ण * अचर *l, अचर वर्ण *separator) {
        फर्काऊ strv_join_full(l, separator, रिक्त);
}

bool strv_overlap(वर्ण * अचर *a, वर्ण * अचर *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        लागि (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                माप_प्रकार _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : रिक्त;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        लागि (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

वर्ण** strv_sort(वर्ण **l);
वर्ण** strv_sort_uniq_full(वर्ण **l, पूर्णाङ्क (*compare)(वर्ण * अचर *a, वर्ण * अचर *b));
वर्ण** strv_sort_uniq(वर्ण **l);
शून्य strv_print_full(वर्ण * अचर *l, अचर वर्ण *prefix);
स्थिर अन्तःस्थ शून्य strv_print(वर्ण * अचर *l) {
        strv_print_full(l, रिक्त);
}

वर्ण* startswith_strv_internal(अचर वर्ण *s, वर्ण * अचर *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

वर्ण* endswith_strv_internal(अचर वर्ण *s, वर्ण * अचर *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                अचर वर्ण* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                अचर वर्ण* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        लागि (अचर वर्ण *x, * अचर*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

वर्ण** strv_reverse(वर्ण **l);
वर्ण** strv_shell_escape(वर्ण **l, अचर वर्ण *bad);

bool strv_fnmatch_full(वर्ण* अचर* patterns, अचर वर्ण *s, पूर्णाङ्क flags, माप_प्रकार *ret_matched_pos);
स्थिर अन्तःस्थ bool strv_fnmatch(वर्ण* अचर* patterns, अचर वर्ण *s) {
        फर्काऊ strv_fnmatch_full(patterns, s, 0, रिक्त);
}
स्थिर अन्तःस्थ bool strv_fnmatch_or_empty(वर्ण* अचर* patterns, अचर वर्ण *s, पूर्णाङ्क flags) {
        प्रतिज्ञा(s);
        फर्काऊ strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, रिक्त);
}

वर्ण** strv_skip(वर्ण **l, माप_प्रकार n);

पूर्णाङ्क strv_extend_n(वर्ण ***l, अचर वर्ण *मान, माप_प्रकार n);

पूर्णाङ्क strv_extend_assignment(वर्ण ***l, अचर वर्ण *lhs, अचर वर्ण *rhs);

पूर्णाङ्क fputstrv(खाता *f, वर्ण * अचर *l, अचर वर्ण *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

शून्य string_strv_hashmap_remove(Hashmap *h, अचर वर्ण *साँचो, अचर वर्ण *मान);
शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, अचर वर्ण *साँचो, अचर वर्ण *मान);
पूर्णाङ्क string_strv_hashmap_put(Hashmap **h, अचर वर्ण *साँचो, अचर वर्ण *मान);
पूर्णाङ्क string_strv_ordered_hashmap_put(OrderedHashmap **h, अचर वर्ण *साँचो, अचर वर्ण *मान);

पूर्णाङ्क strv_rebreak_पङ्क्तिहरू(वर्ण **l, माप_प्रकार width, वर्ण ***ret);

वर्ण** strv_filter_prefix(वर्ण * अचर *l, अचर वर्ण *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
बाह्य अचर वर्ण* अचर strv_empty[1];

#define STRV_EMPTY ((वर्ण**) strv_empty)
