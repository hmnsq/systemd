/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/strv.h"   /* IWYU pragma: export */

अक्षर* strv_find(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) _pure_;
अक्षर* strv_find_case(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) _pure_;
अक्षर* strv_find_prefix(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) _pure_;
अक्षर* strv_find_startswith(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) _pure_;
अक्षर* strv_find_closest(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
अक्षर* strv_find_first_field(अक्षर * स्थिरांक *needles, अक्षर * स्थिरांक *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

अक्षर** strv_free(अक्षर **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(अक्षर**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

अक्षर** strv_free_erase(अक्षर **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(अक्षर**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

शून्य strv_free_many(अक्षर ***strvs, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);

अक्षर** strv_copy_n(अक्षर * स्थिरांक *l, माप_प्रकार n);
स्थिर ओळीत अक्षर** strv_copy(अक्षर * स्थिरांक *l) {
        परत strv_copy_n(l, SIZE_MAX);
}
पूर्णांक strv_copy_unless_empty(अक्षर * स्थिरांक *l, अक्षर ***ret);

माप_प्रकार strv_length(अक्षर * स्थिरांक *l) _pure_;

पूर्णांक strv_extend_strv(अक्षर ***a, अक्षर * स्थिरांक *b, bool filter_duplicates);
पूर्णांक strv_extend_strv_consume(अक्षर ***a, अक्षर **b, bool filter_duplicates);

पूर्णांक strv_extend_strv_biconcat(अक्षर ***a, स्थिरांक अक्षर *prefix, स्थिरांक अक्षर* स्थिरांक *b, स्थिरांक अक्षर *suffix);
स्थिर ओळीत पूर्णांक strv_extend_strv_concat(अक्षर ***a, स्थिरांक अक्षर* स्थिरांक *b, स्थिरांक अक्षर *suffix) {
        परत strv_extend_strv_biconcat(a, रिक्त, b, suffix);
}

पूर्णांक strv_prepend(अक्षर ***l, स्थिरांक अक्षर *मूल्य);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

पूर्णांक strv_extend_with_size(अक्षर ***l, माप_प्रकार *n, स्थिरांक अक्षर *मूल्य);
स्थिर ओळीत पूर्णांक strv_extend(अक्षर ***l, स्थिरांक अक्षर *मूल्य) {
        परत strv_extend_with_size(l, रिक्त, मूल्य);
}

पूर्णांक strv_extend_many_internal(अक्षर ***l, स्थिरांक अक्षर *मूल्य, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

पूर्णांक strv_extendf_with_size(अक्षर ***l, माप_प्रकार *n, स्थिरांक अक्षर *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, रिक्त, __VA_ARGS__)

पूर्णांक strv_extend_joined_with_size_sentinel(अक्षर ***l, माप_प्रकार *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, रिक्त)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, रिक्त, __VA_ARGS__)

पूर्णांक strv_ढकला_with_size(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य);
स्थिर ओळीत पूर्णांक strv_ढकला(अक्षर ***l, अक्षर *मूल्य) {
        परत strv_ढकला_with_size(l, रिक्त, मूल्य);
}
पूर्णांक strv_ढकला_pair(अक्षर ***l, अक्षर *a, अक्षर *b);

पूर्णांक strv_घाला(अक्षर ***l, माप_प्रकार position, अक्षर *मूल्य);

स्थिर ओळीत पूर्णांक strv_ढकला_prepend(अक्षर ***l, अक्षर *मूल्य) {
        परत strv_घाला(l, 0, मूल्य);
}

पूर्णांक strv_consume_with_size(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य);
स्थिर ओळीत पूर्णांक strv_consume(अक्षर ***l, अक्षर *मूल्य) {
        परत strv_consume_with_size(l, रिक्त, मूल्य);
}

पूर्णांक strv_consume_pair(अक्षर ***l, अक्षर *a, अक्षर *b);
पूर्णांक strv_consume_prepend(अक्षर ***l, अक्षर *मूल्य);

अक्षर** strv_remove(अक्षर **l, स्थिरांक अक्षर *s);
अक्षर** strv_remove_strv(अक्षर **l, अक्षर *स्थिरांक*ll);
अक्षर** strv_uniq(अक्षर **l);
bool strv_is_uniq(अक्षर * स्थिरांक *l) _pure_;

पूर्णांक strv_compare(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) _pure_;
स्थिर ओळीत bool strv_equal(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) {
        परत strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) _pure_;

अक्षर** strv_new_internal(स्थिरांक अक्षर *x, ...) _sentinel_;
अक्षर** strv_new_ap(स्थिरांक अक्षर *x, बहु_सूची ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, रिक्त)

#define STRV_IGNORE ((स्थिरांक अक्षर *) POINTER_MAX)

स्थिर ओळीत स्थिरांक अक्षर* STRV_IFNOTNULL(स्थिरांक अक्षर *x) {
        परत x ?: STRV_IGNORE;
}

पूर्णांक strv_split_full(अक्षर ***t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators, ExtractFlags flags);
अक्षर** strv_split(स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators);

पूर्णांक strv_split_and_extend_full(अक्षर ***t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators, bool filter_duplicates, ExtractFlags flags);
पूर्णांक strv_split_and_extend(अक्षर ***t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators, bool filter_duplicates);

पूर्णांक strv_split_newlines_full(अक्षर ***ret, स्थिरांक अक्षर *s, ExtractFlags flags);
अक्षर** strv_split_newlines(स्थिरांक अक्षर *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
पूर्णांक strv_split_colon_pairs(अक्षर ***t, स्थिरांक अक्षर *s);

अक्षर* strv_join_full(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *separator, स्थिरांक अक्षर *prefix);
स्थिर ओळीत अक्षर *strv_join(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *separator) {
        परत strv_join_full(l, separator, रिक्त);
}

bool strv_overlap(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        साठी (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                माप_प्रकार _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : रिक्त;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        साठी (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

अक्षर** strv_sort(अक्षर **l);
अक्षर** strv_sort_uniq_full(अक्षर **l, पूर्णांक (*compare)(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b));
अक्षर** strv_sort_uniq(अक्षर **l);
शून्य strv_print_full(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *prefix);
स्थिर ओळीत शून्य strv_print(अक्षर * स्थिरांक *l) {
        strv_print_full(l, रिक्त);
}

अक्षर* startswith_strv_internal(स्थिरांक अक्षर *s, अक्षर * स्थिरांक *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

अक्षर* endswith_strv_internal(स्थिरांक अक्षर *s, अक्षर * स्थिरांक *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                स्थिरांक अक्षर* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                स्थिरांक अक्षर* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        साठी (स्थिरांक अक्षर *x, * स्थिरांक*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

अक्षर** strv_reverse(अक्षर **l);
अक्षर** strv_shell_escape(अक्षर **l, स्थिरांक अक्षर *bad);

bool strv_fnmatch_full(अक्षर* स्थिरांक* patterns, स्थिरांक अक्षर *s, पूर्णांक flags, माप_प्रकार *ret_matched_pos);
स्थिर ओळीत bool strv_fnmatch(अक्षर* स्थिरांक* patterns, स्थिरांक अक्षर *s) {
        परत strv_fnmatch_full(patterns, s, 0, रिक्त);
}
स्थिर ओळीत bool strv_fnmatch_or_empty(अक्षर* स्थिरांक* patterns, स्थिरांक अक्षर *s, पूर्णांक flags) {
        प्रतिज्ञा(s);
        परत strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, रिक्त);
}

अक्षर** strv_skip(अक्षर **l, माप_प्रकार n);

पूर्णांक strv_extend_n(अक्षर ***l, स्थिरांक अक्षर *मूल्य, माप_प्रकार n);

पूर्णांक strv_extend_assignment(अक्षर ***l, स्थिरांक अक्षर *lhs, स्थिरांक अक्षर *rhs);

पूर्णांक fputstrv(खाता *f, अक्षर * स्थिरांक *l, स्थिरांक अक्षर *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

शून्य string_strv_hashmap_remove(Hashmap *h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य);
शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य);
पूर्णांक string_strv_hashmap_put(Hashmap **h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य);
पूर्णांक string_strv_ordered_hashmap_put(OrderedHashmap **h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य);

पूर्णांक strv_rebreak_ओळी(अक्षर **l, माप_प्रकार width, अक्षर ***ret);

अक्षर** strv_filter_prefix(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
बाह्य स्थिरांक अक्षर* स्थिरांक strv_empty[1];

#define STRV_EMPTY ((अक्षर**) strv_empty)
