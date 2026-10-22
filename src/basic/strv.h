/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#ضمن "../fundamental/strv.h"   /* IWYU pragma: export */

حرف* strv_find(حرف * ثابت *l, ثابت حرف *name) _pure_;
حرف* strv_find_case(حرف * ثابت *l, ثابت حرف *name) _pure_;
حرف* strv_find_prefix(حرف * ثابت *l, ثابت حرف *name) _pure_;
حرف* strv_find_startswith(حرف * ثابت *l, ثابت حرف *name) _pure_;
حرف* strv_find_closest(حرف * ثابت *l, ثابت حرف *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
حرف* strv_find_first_field(حرف * ثابت *needles, حرف * ثابت *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

حرف** strv_free(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

حرف** strv_free_erase(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

فراغ strv_free_many(حرف ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

حرف** strv_copy_n(حرف * ثابت *l, size_t n);
ساكن مضمن حرف** strv_copy(حرف * ثابت *l) {
        أرجع strv_copy_n(l, SIZE_MAX);
}
صحيح strv_copy_unless_empty(حرف * ثابت *l, حرف ***ret);

size_t strv_length(حرف * ثابت *l) _pure_;

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates);
صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates);

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *prefix, ثابت حرف* ثابت *b, ثابت حرف *suffix);
ساكن مضمن صحيح strv_extend_strv_concat(حرف ***a, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        أرجع strv_extend_strv_biconcat(a, عدم, b, suffix);
}

صحيح strv_prepend(حرف ***l, ثابت حرف *قدر);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

صحيح strv_extend_with_size(حرف ***l, size_t *n, ثابت حرف *قدر);
ساكن مضمن صحيح strv_extend(حرف ***l, ثابت حرف *قدر) {
        أرجع strv_extend_with_size(l, عدم, قدر);
}

صحيح strv_extend_many_internal(حرف ***l, ثابت حرف *قدر, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

صحيح strv_extendf_with_size(حرف ***l, size_t *n, ثابت حرف *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, عدم, __VA_ARGS__)

صحيح strv_extend_joined_with_size_sentinel(حرف ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, عدم)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, عدم, __VA_ARGS__)

صحيح strv_ڈالو_with_size(حرف ***l, size_t *n, حرف *قدر);
ساكن مضمن صحيح strv_ڈالو(حرف ***l, حرف *قدر) {
        أرجع strv_ڈالو_with_size(l, عدم, قدر);
}
صحيح strv_ڈالو_pair(حرف ***l, حرف *a, حرف *b);

صحيح strv_داخل_کرو(حرف ***l, size_t position, حرف *قدر);

ساكن مضمن صحيح strv_ڈالو_prepend(حرف ***l, حرف *قدر) {
        أرجع strv_داخل_کرو(l, 0, قدر);
}

صحيح strv_consume_with_size(حرف ***l, size_t *n, حرف *قدر);
ساكن مضمن صحيح strv_consume(حرف ***l, حرف *قدر) {
        أرجع strv_consume_with_size(l, عدم, قدر);
}

صحيح strv_consume_pair(حرف ***l, حرف *a, حرف *b);
صحيح strv_consume_prepend(حرف ***l, حرف *قدر);

حرف** strv_remove(حرف **l, ثابت حرف *s);
حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll);
حرف** strv_uniq(حرف **l);
bool strv_is_uniq(حرف * ثابت *l) _pure_;

صحيح strv_compare(حرف * ثابت *a, حرف * ثابت *b) _pure_;
ساكن مضمن bool strv_equal(حرف * ثابت *a, حرف * ثابت *b) {
        أرجع strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(حرف * ثابت *a, حرف * ثابت *b) _pure_;

حرف** strv_new_internal(ثابت حرف *x, ...) _sentinel_;
حرف** strv_new_ap(ثابت حرف *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, عدم)

#define STRV_IGNORE ((ثابت حرف *) POINTER_MAX)

ساكن مضمن ثابت حرف* STRV_IFNOTNULL(ثابت حرف *x) {
        أرجع x ?: STRV_IGNORE;
}

صحيح strv_split_full(حرف ***t, ثابت حرف *s, ثابت حرف *separators, ExtractFlags flags);
حرف** strv_split(ثابت حرف *s, ثابت حرف *separators);

صحيح strv_split_and_extend_full(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, ExtractFlags flags);
صحيح strv_split_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates);

صحيح strv_split_newlines_full(حرف ***ret, ثابت حرف *s, ExtractFlags flags);
حرف** strv_split_newlines(ثابت حرف *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
صحيح strv_split_colon_pairs(حرف ***t, ثابت حرف *s);

حرف* strv_join_full(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *prefix);
ساكن مضمن حرف *strv_join(حرف * ثابت *l, ثابت حرف *separator) {
        أرجع strv_join_full(l, separator, عدم);
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        لكل (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : عدم;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        لكل (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

حرف** strv_sort(حرف **l);
حرف** strv_sort_uniq_full(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b));
حرف** strv_sort_uniq(حرف **l);
فراغ strv_print_full(حرف * ثابت *l, ثابت حرف *prefix);
ساكن مضمن فراغ strv_print(حرف * ثابت *l) {
        strv_print_full(l, عدم);
}

حرف* startswith_strv_internal(ثابت حرف *s, حرف * ثابت *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

حرف* endswith_strv_internal(ثابت حرف *s, حرف * ثابت *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        لكل (ثابت حرف *x, * ثابت*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

حرف** strv_reverse(حرف **l);
حرف** strv_shell_escape(حرف **l, ثابت حرف *bad);

bool strv_fnmatch_full(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags, size_t *ret_matched_pos);
ساكن مضمن bool strv_fnmatch(حرف* ثابت* patterns, ثابت حرف *s) {
        أرجع strv_fnmatch_full(patterns, s, 0, عدم);
}
ساكن مضمن bool strv_fnmatch_or_empty(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags) {
        أكد(s);
        أرجع strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, عدم);
}

حرف** strv_skip(حرف **l, size_t n);

صحيح strv_extend_n(حرف ***l, ثابت حرف *قدر, size_t n);

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs);

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *کلید, ثابت حرف *قدر);
فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *کلید, ثابت حرف *قدر);
صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *کلید, ثابت حرف *قدر);
صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *کلید, ثابت حرف *قدر);

صحيح strv_rebreak_سطریں(حرف **l, size_t width, حرف ***ret);

حرف** strv_filter_prefix(حرف * ثابت *l, ثابت حرف *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
خارجي ثابت حرف* ثابت strv_empty[1];

#define STRV_EMPTY ((حرف**) strv_empty)
