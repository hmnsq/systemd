/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

#شامل "../fundamental/strv.h"   /* IWYU pragma: export */

نویسه* strv_find(نویسه * ثابت *l, ثابت نویسه *name) _pure_;
نویسه* strv_find_case(نویسه * ثابت *l, ثابت نویسه *name) _pure_;
نویسه* strv_find_prefix(نویسه * ثابت *l, ثابت نویسه *name) _pure_;
نویسه* strv_find_startswith(نویسه * ثابت *l, ثابت نویسه *name) _pure_;
نویسه* strv_find_closest(نویسه * ثابت *l, ثابت نویسه *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
نویسه* strv_find_first_field(نویسه * ثابت *needles, نویسه * ثابت *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

نویسه** strv_free(نویسه **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(نویسه**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

نویسه** strv_free_erase(نویسه **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(نویسه**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

تهی strv_free_many(نویسه ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

نویسه** strv_copy_n(نویسه * ثابت *l, size_t n);
ایستا درونخطی نویسه** strv_copy(نویسه * ثابت *l) {
        بازگشت strv_copy_n(l, SIZE_MAX);
}
صحیح strv_copy_unless_empty(نویسه * ثابت *l, نویسه ***ret);

size_t strv_length(نویسه * ثابت *l) _pure_;

صحیح strv_extend_strv(نویسه ***a, نویسه * ثابت *b, bool filter_duplicates);
صحیح strv_extend_strv_consume(نویسه ***a, نویسه **b, bool filter_duplicates);

صحیح strv_extend_strv_biconcat(نویسه ***a, ثابت نویسه *prefix, ثابت نویسه* ثابت *b, ثابت نویسه *suffix);
ایستا درونخطی صحیح strv_extend_strv_concat(نویسه ***a, ثابت نویسه* ثابت *b, ثابت نویسه *suffix) {
        بازگشت strv_extend_strv_biconcat(a, پوچ, b, suffix);
}

صحیح strv_prepend(نویسه ***l, ثابت نویسه *مقدار);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

صحیح strv_extend_with_size(نویسه ***l, size_t *n, ثابت نویسه *مقدار);
ایستا درونخطی صحیح strv_extend(نویسه ***l, ثابت نویسه *مقدار) {
        بازگشت strv_extend_with_size(l, پوچ, مقدار);
}

صحیح strv_extend_many_internal(نویسه ***l, ثابت نویسه *مقدار, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

صحیح strv_extendf_with_size(نویسه ***l, size_t *n, ثابت نویسه *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, پوچ, __VA_ARGS__)

صحیح strv_extend_joined_with_size_sentinel(نویسه ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, پوچ)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, پوچ, __VA_ARGS__)

صحیح strv_افزودن_with_size(نویسه ***l, size_t *n, نویسه *مقدار);
ایستا درونخطی صحیح strv_افزودن(نویسه ***l, نویسه *مقدار) {
        بازگشت strv_افزودن_with_size(l, پوچ, مقدار);
}
صحیح strv_افزودن_pair(نویسه ***l, نویسه *a, نویسه *b);

صحیح strv_درج(نویسه ***l, size_t position, نویسه *مقدار);

ایستا درونخطی صحیح strv_افزودن_prepend(نویسه ***l, نویسه *مقدار) {
        بازگشت strv_درج(l, 0, مقدار);
}

صحیح strv_consume_with_size(نویسه ***l, size_t *n, نویسه *مقدار);
ایستا درونخطی صحیح strv_consume(نویسه ***l, نویسه *مقدار) {
        بازگشت strv_consume_with_size(l, پوچ, مقدار);
}

صحیح strv_consume_pair(نویسه ***l, نویسه *a, نویسه *b);
صحیح strv_consume_prepend(نویسه ***l, نویسه *مقدار);

نویسه** strv_remove(نویسه **l, ثابت نویسه *s);
نویسه** strv_remove_strv(نویسه **l, نویسه *ثابت*ll);
نویسه** strv_uniq(نویسه **l);
bool strv_is_uniq(نویسه * ثابت *l) _pure_;

صحیح strv_compare(نویسه * ثابت *a, نویسه * ثابت *b) _pure_;
ایستا درونخطی bool strv_equal(نویسه * ثابت *a, نویسه * ثابت *b) {
        بازگشت strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(نویسه * ثابت *a, نویسه * ثابت *b) _pure_;

نویسه** strv_new_internal(ثابت نویسه *x, ...) _sentinel_;
نویسه** strv_new_ap(ثابت نویسه *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, پوچ)

#define STRV_IGNORE ((ثابت نویسه *) POINTER_MAX)

ایستا درونخطی ثابت نویسه* STRV_IFNOTNULL(ثابت نویسه *x) {
        بازگشت x ?: STRV_IGNORE;
}

صحیح strv_split_full(نویسه ***t, ثابت نویسه *s, ثابت نویسه *separators, ExtractFlags flags);
نویسه** strv_split(ثابت نویسه *s, ثابت نویسه *separators);

صحیح strv_split_and_extend_full(نویسه ***t, ثابت نویسه *s, ثابت نویسه *separators, bool filter_duplicates, ExtractFlags flags);
صحیح strv_split_and_extend(نویسه ***t, ثابت نویسه *s, ثابت نویسه *separators, bool filter_duplicates);

صحیح strv_split_newlines_full(نویسه ***ret, ثابت نویسه *s, ExtractFlags flags);
نویسه** strv_split_newlines(ثابت نویسه *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
صحیح strv_split_colon_pairs(نویسه ***t, ثابت نویسه *s);

نویسه* strv_join_full(نویسه * ثابت *l, ثابت نویسه *separator, ثابت نویسه *prefix);
ایستا درونخطی نویسه *strv_join(نویسه * ثابت *l, ثابت نویسه *separator) {
        بازگشت strv_join_full(l, separator, پوچ);
}

bool strv_overlap(نویسه * ثابت *a, نویسه * ثابت *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        برای (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : پوچ;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        برای (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

نویسه** strv_sort(نویسه **l);
نویسه** strv_sort_uniq_full(نویسه **l, صحیح (*compare)(نویسه * ثابت *a, نویسه * ثابت *b));
نویسه** strv_sort_uniq(نویسه **l);
تهی strv_print_full(نویسه * ثابت *l, ثابت نویسه *prefix);
ایستا درونخطی تهی strv_print(نویسه * ثابت *l) {
        strv_print_full(l, پوچ);
}

نویسه* startswith_strv_internal(ثابت نویسه *s, نویسه * ثابت *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

نویسه* endswith_strv_internal(ثابت نویسه *s, نویسه * ثابت *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ثابت نویسه* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ثابت نویسه* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        برای (ثابت نویسه *x, * ثابت*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

نویسه** strv_reverse(نویسه **l);
نویسه** strv_shell_escape(نویسه **l, ثابت نویسه *bad);

bool strv_fnmatch_full(نویسه* ثابت* patterns, ثابت نویسه *s, صحیح flags, size_t *ret_matched_pos);
ایستا درونخطی bool strv_fnmatch(نویسه* ثابت* patterns, ثابت نویسه *s) {
        بازگشت strv_fnmatch_full(patterns, s, 0, پوچ);
}
ایستا درونخطی bool strv_fnmatch_or_empty(نویسه* ثابت* patterns, ثابت نویسه *s, صحیح flags) {
        أكد(s);
        بازگشت strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, پوچ);
}

نویسه** strv_skip(نویسه **l, size_t n);

صحیح strv_extend_n(نویسه ***l, ثابت نویسه *مقدار, size_t n);

صحیح strv_extend_assignment(نویسه ***l, ثابت نویسه *lhs, ثابت نویسه *rhs);

صحیح fputstrv(FILE *f, نویسه * ثابت *l, ثابت نویسه *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

تهی string_strv_hashmap_remove(Hashmap *h, ثابت نویسه *کلید, ثابت نویسه *مقدار);
تهی string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت نویسه *کلید, ثابت نویسه *مقدار);
صحیح string_strv_hashmap_put(Hashmap **h, ثابت نویسه *کلید, ثابت نویسه *مقدار);
صحیح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت نویسه *کلید, ثابت نویسه *مقدار);

صحیح strv_rebreak_سطرها(نویسه **l, size_t width, نویسه ***ret);

نویسه** strv_filter_prefix(نویسه * ثابت *l, ثابت نویسه *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
بیرونی ثابت نویسه* ثابت strv_empty[1];

#define STRV_EMPTY ((نویسه**) strv_empty)
