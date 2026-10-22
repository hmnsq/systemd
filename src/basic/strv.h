/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/strv.h"   /* IWYU pragma: export */

অক্ষর* strv_find(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) _pure_;
অক্ষর* strv_find_case(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) _pure_;
অক্ষর* strv_find_prefix(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) _pure_;
অক্ষর* strv_find_startswith(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) _pure_;
অক্ষর* strv_find_closest(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
অক্ষর* strv_find_first_field(অক্ষর * ধ্রুবক *needles, অক্ষর * ধ্রুবক *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

অক্ষর** strv_free(অক্ষর **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(অক্ষর**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

অক্ষর** strv_free_erase(অক্ষর **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(অক্ষর**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

শূন্য strv_free_many(অক্ষর ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

অক্ষর** strv_copy_n(অক্ষর * ধ্রুবক *l, size_t n);
স্থির অন্তঃরেখ অক্ষর** strv_copy(অক্ষর * ধ্রুবক *l) {
        ফেরত strv_copy_n(l, SIZE_MAX);
}
পূর্ণসংখ্যা strv_copy_unless_empty(অক্ষর * ধ্রুবক *l, অক্ষর ***ret);

size_t strv_দৈর্ঘ্য(অক্ষর * ধ্রুবক *l) _pure_;

পূর্ণসংখ্যা strv_extend_strv(অক্ষর ***a, অক্ষর * ধ্রুবক *b, bool filter_duplicates);
পূর্ণসংখ্যা strv_extend_strv_consume(অক্ষর ***a, অক্ষর **b, bool filter_duplicates);

পূর্ণসংখ্যা strv_extend_strv_biconcat(অক্ষর ***a, ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর* ধ্রুবক *b, ধ্রুবক অক্ষর *suffix);
স্থির অন্তঃরেখ পূর্ণসংখ্যা strv_extend_strv_concat(অক্ষর ***a, ধ্রুবক অক্ষর* ধ্রুবক *b, ধ্রুবক অক্ষর *suffix) {
        ফেরত strv_extend_strv_biconcat(a, ফাঁকা, b, suffix);
}

পূর্ণসংখ্যা strv_prepend(অক্ষর ***l, ধ্রুবক অক্ষর *মান);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

পূর্ণসংখ্যা strv_extend_with_আকার(অক্ষর ***l, size_t *n, ধ্রুবক অক্ষর *মান);
স্থির অন্তঃরেখ পূর্ণসংখ্যা strv_extend(অক্ষর ***l, ধ্রুবক অক্ষর *মান) {
        ফেরত strv_extend_with_আকার(l, ফাঁকা, মান);
}

পূর্ণসংখ্যা strv_extend_many_internal(অক্ষর ***l, ধ্রুবক অক্ষর *মান, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

পূর্ণসংখ্যা strv_extendf_with_আকার(অক্ষর ***l, size_t *n, ধ্রুবক অক্ষর *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_আকার(l, ফাঁকা, __VA_ARGS__)

পূর্ণসংখ্যা strv_extend_joined_with_আকার_sentinel(অক্ষর ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_আকার(l, n, ...) strv_extend_joined_with_আকার_sentinel(l, n, __VA_ARGS__, ফাঁকা)
#define strv_extend_joined(l, ...) strv_extend_joined_with_আকার(l, ফাঁকা, __VA_ARGS__)

পূর্ণসংখ্যা strv_ঠেলো_with_আকার(অক্ষর ***l, size_t *n, অক্ষর *মান);
স্থির অন্তঃরেখ পূর্ণসংখ্যা strv_ঠেলো(অক্ষর ***l, অক্ষর *মান) {
        ফেরত strv_ঠেলো_with_আকার(l, ফাঁকা, মান);
}
পূর্ণসংখ্যা strv_ঠেলো_pair(অক্ষর ***l, অক্ষর *a, অক্ষর *b);

পূর্ণসংখ্যা strv_ঢোকাও(অক্ষর ***l, size_t position, অক্ষর *মান);

স্থির অন্তঃরেখ পূর্ণসংখ্যা strv_ঠেলো_prepend(অক্ষর ***l, অক্ষর *মান) {
        ফেরত strv_ঢোকাও(l, 0, মান);
}

পূর্ণসংখ্যা strv_consume_with_আকার(অক্ষর ***l, size_t *n, অক্ষর *মান);
স্থির অন্তঃরেখ পূর্ণসংখ্যা strv_consume(অক্ষর ***l, অক্ষর *মান) {
        ফেরত strv_consume_with_আকার(l, ফাঁকা, মান);
}

পূর্ণসংখ্যা strv_consume_pair(অক্ষর ***l, অক্ষর *a, অক্ষর *b);
পূর্ণসংখ্যা strv_consume_prepend(অক্ষর ***l, অক্ষর *মান);

অক্ষর** strv_remove(অক্ষর **l, ধ্রুবক অক্ষর *s);
অক্ষর** strv_remove_strv(অক্ষর **l, অক্ষর *ধ্রুবক*ll);
অক্ষর** strv_uniq(অক্ষর **l);
bool strv_is_uniq(অক্ষর * ধ্রুবক *l) _pure_;

পূর্ণসংখ্যা strv_compare(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) _pure_;
স্থির অন্তঃরেখ bool strv_equal(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) {
        ফেরত strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) _pure_;

অক্ষর** strv_new_internal(ধ্রুবক অক্ষর *x, ...) _sentinel_;
অক্ষর** strv_new_ap(ধ্রুবক অক্ষর *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, ফাঁকা)

#define STRV_IGNORE ((ধ্রুবক অক্ষর *) POINTER_MAX)

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* STRV_IFNOTNULL(ধ্রুবক অক্ষর *x) {
        ফেরত x ?: STRV_IGNORE;
}

পূর্ণসংখ্যা strv_split_full(অক্ষর ***t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators, ExtractFlags flags);
অক্ষর** strv_split(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators);

পূর্ণসংখ্যা strv_split_and_extend_full(অক্ষর ***t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators, bool filter_duplicates, ExtractFlags flags);
পূর্ণসংখ্যা strv_split_and_extend(অক্ষর ***t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators, bool filter_duplicates);

পূর্ণসংখ্যা strv_split_newlines_full(অক্ষর ***ret, ধ্রুবক অক্ষর *s, ExtractFlags flags);
অক্ষর** strv_split_newlines(ধ্রুবক অক্ষর *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
পূর্ণসংখ্যা strv_split_colon_pairs(অক্ষর ***t, ধ্রুবক অক্ষর *s);

অক্ষর* strv_join_full(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *separator, ধ্রুবক অক্ষর *prefix);
স্থির অন্তঃরেখ অক্ষর *strv_join(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *separator) {
        ফেরত strv_join_full(l, separator, ফাঁকা);
}

bool strv_overlap(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        জন্য (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_দৈর্ঘ্য(h);           \
                                _len > 0 ? h + _len - 1 : ফাঁকা;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        জন্য (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

অক্ষর** strv_sort(অক্ষর **l);
অক্ষর** strv_sort_uniq_full(অক্ষর **l, পূর্ণসংখ্যা (*compare)(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b));
অক্ষর** strv_sort_uniq(অক্ষর **l);
শূন্য strv_print_full(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *prefix);
স্থির অন্তঃরেখ শূন্য strv_print(অক্ষর * ধ্রুবক *l) {
        strv_print_full(l, ফাঁকা);
}

অক্ষর* startswith_strv_internal(ধ্রুবক অক্ষর *s, অক্ষর * ধ্রুবক *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

অক্ষর* endswith_strv_internal(ধ্রুবক অক্ষর *s, অক্ষর * ধ্রুবক *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ধ্রুবক অক্ষর* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ধ্রুবক অক্ষর* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        জন্য (ধ্রুবক অক্ষর *x, * ধ্রুবক*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

অক্ষর** strv_reverse(অক্ষর **l);
অক্ষর** strv_shell_escape(অক্ষর **l, ধ্রুবক অক্ষর *bad);

bool strv_fnmatch_full(অক্ষর* ধ্রুবক* patterns, ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা flags, size_t *ret_matched_pos);
স্থির অন্তঃরেখ bool strv_fnmatch(অক্ষর* ধ্রুবক* patterns, ধ্রুবক অক্ষর *s) {
        ফেরত strv_fnmatch_full(patterns, s, 0, ফাঁকা);
}
স্থির অন্তঃরেখ bool strv_fnmatch_or_empty(অক্ষর* ধ্রুবক* patterns, ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা flags) {
        প্রতিজ্ঞা(s);
        ফেরত strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, ফাঁকা);
}

অক্ষর** strv_skip(অক্ষর **l, size_t n);

পূর্ণসংখ্যা strv_extend_n(অক্ষর ***l, ধ্রুবক অক্ষর *মান, size_t n);

পূর্ণসংখ্যা strv_extend_assignment(অক্ষর ***l, ধ্রুবক অক্ষর *lhs, ধ্রুবক অক্ষর *rhs);

পূর্ণসংখ্যা fputstrv(FILE *f, অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

শূন্য string_strv_hashmap_remove(Hashmap *h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);
শূন্য string_strv_ordered_hashmap_remove(OrderedHashmap *h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);
পূর্ণসংখ্যা string_strv_hashmap_put(Hashmap **h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);
পূর্ণসংখ্যা string_strv_ordered_hashmap_put(OrderedHashmap **h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);

পূর্ণসংখ্যা strv_rebreak_সারিগুলি(অক্ষর **l, size_t width, অক্ষর ***ret);

অক্ষর** strv_filter_prefix(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
বাহ্যিক ধ্রুবক অক্ষর* ধ্রুবক strv_empty[1];

#define STRV_EMPTY ((অক্ষর**) strv_empty)
