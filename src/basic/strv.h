/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

#ଅନ୍ତର୍ଭାଵ "../fundamental/strv.h"   /* IWYU pragma: export */

ଅକ୍ଷର* strv_find(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) _pure_;
ଅକ୍ଷର* strv_find_case(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) _pure_;
ଅକ୍ଷର* strv_find_prefix(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) _pure_;
ଅକ୍ଷର* strv_find_startswith(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) _pure_;
ଅକ୍ଷର* strv_find_closest(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
ଅକ୍ଷର* strv_find_first_field(ଅକ୍ଷର * ଅଚଳ *needles, ଅକ୍ଷର * ଅଚଳ *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

ଅକ୍ଷର** strv_free(ଅକ୍ଷର **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ଅକ୍ଷର**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

ଅକ୍ଷର** strv_free_erase(ଅକ୍ଷର **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ଅକ୍ଷର**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

ଶୂନ୍ୟ strv_free_many(ଅକ୍ଷର ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

ଅକ୍ଷର** strv_copy_n(ଅକ୍ଷର * ଅଚଳ *l, size_t n);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର** strv_copy(ଅକ୍ଷର * ଅଚଳ *l) {
        ଫେରନ୍ତୁ strv_copy_n(l, SIZE_MAX);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_copy_unless_empty(ଅକ୍ଷର * ଅଚଳ *l, ଅକ୍ଷର ***ret);

size_t strv_length(ଅକ୍ଷର * ଅଚଳ *l) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv(ଅକ୍ଷର ***a, ଅକ୍ଷର * ଅଚଳ *b, bool filter_duplicates);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv_consume(ଅକ୍ଷର ***a, ଅକ୍ଷର **b, bool filter_duplicates);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv_biconcat(ଅକ୍ଷର ***a, ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର* ଅଚଳ *b, ଅଚଳ ଅକ୍ଷର *suffix);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv_concat(ଅକ୍ଷର ***a, ଅଚଳ ଅକ୍ଷର* ଅଚଳ *b, ଅଚଳ ଅକ୍ଷର *suffix) {
        ଫେରନ୍ତୁ strv_extend_strv_biconcat(a, ଖାଲି, b, suffix);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_prepend(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_with_size(ଅକ୍ଷର ***l, size_t *n, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ strv_extend_with_size(l, ଖାଲି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_many_internal(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extendf_with_size(ଅକ୍ଷର ***l, size_t *n, ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, ଖାଲି, __VA_ARGS__)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_joined_with_size_sentinel(ଅକ୍ଷର ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, ଖାଲି)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, ଖାଲି, __VA_ARGS__)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଠେଲ_with_size(ଅକ୍ଷର ***l, size_t *n, ଅକ୍ଷର *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଠେଲ(ଅକ୍ଷର ***l, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ strv_ଠେଲ_with_size(l, ଖାଲି, ମୂଲ୍ୟ);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଠେଲ_pair(ଅକ୍ଷର ***l, ଅକ୍ଷର *a, ଅକ୍ଷର *b);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଭର୍ତ୍ତି(ଅକ୍ଷର ***l, size_t position, ଅକ୍ଷର *ମୂଲ୍ୟ);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଠେଲ_prepend(ଅକ୍ଷର ***l, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ strv_ଭର୍ତ୍ତି(l, 0, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume_with_size(ଅକ୍ଷର ***l, size_t *n, ଅକ୍ଷର *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume(ଅକ୍ଷର ***l, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ strv_consume_with_size(l, ଖାଲି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume_pair(ଅକ୍ଷର ***l, ଅକ୍ଷର *a, ଅକ୍ଷର *b);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume_prepend(ଅକ୍ଷର ***l, ଅକ୍ଷର *ମୂଲ୍ୟ);

ଅକ୍ଷର** strv_remove(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *s);
ଅକ୍ଷର** strv_remove_strv(ଅକ୍ଷର **l, ଅକ୍ଷର *ଅଚଳ*ll);
ଅକ୍ଷର** strv_uniq(ଅକ୍ଷର **l);
bool strv_is_uniq(ଅକ୍ଷର * ଅଚଳ *l) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_compare(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool strv_equal(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) {
        ଫେରନ୍ତୁ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) _pure_;

ଅକ୍ଷର** strv_new_internal(ଅଚଳ ଅକ୍ଷର *x, ...) _sentinel_;
ଅକ୍ଷର** strv_new_ap(ଅଚଳ ଅକ୍ଷର *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, ଖାଲି)

#define STRV_IGNORE ((ଅଚଳ ଅକ୍ଷର *) POINTER_MAX)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* STRV_IFNOTNULL(ଅଚଳ ଅକ୍ଷର *x) {
        ଫେରନ୍ତୁ x ?: STRV_IGNORE;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_full(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags);
ଅକ୍ଷର** strv_split(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_and_extend_full(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators, bool filter_duplicates, ExtractFlags flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_and_extend(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators, bool filter_duplicates);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_newlines_full(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *s, ExtractFlags flags);
ଅକ୍ଷର** strv_split_newlines(ଅଚଳ ଅକ୍ଷର *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_colon_pairs(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s);

ଅକ୍ଷର* strv_join_full(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *separator, ଅଚଳ ଅକ୍ଷର *prefix);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର *strv_join(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *separator) {
        ଫେରନ୍ତୁ strv_join_full(l, separator, ଖାଲି);
}

bool strv_overlap(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        ପାଇଁ (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : ଖାଲି;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        ପାଇଁ (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

ଅକ୍ଷର** strv_sort(ଅକ୍ଷର **l);
ଅକ୍ଷର** strv_sort_uniq_full(ଅକ୍ଷର **l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*compare)(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b));
ଅକ୍ଷର** strv_sort_uniq(ଅକ୍ଷର **l);
ଶୂନ୍ୟ strv_print_full(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *prefix);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ strv_print(ଅକ୍ଷର * ଅଚଳ *l) {
        strv_print_full(l, ଖାଲି);
}

ଅକ୍ଷର* startswith_strv_internal(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର * ଅଚଳ *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ଅକ୍ଷର* endswith_strv_internal(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର * ଅଚଳ *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ଅଚଳ ଅକ୍ଷର* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ଅଚଳ ଅକ୍ଷର* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *x, * ଅଚଳ*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

ଅକ୍ଷର** strv_reverse(ଅକ୍ଷର **l);
ଅକ୍ଷର** strv_shell_escape(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *bad);

bool strv_fnmatch_full(ଅକ୍ଷର* ଅଚଳ* patterns, ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, size_t *ret_matched_pos);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool strv_fnmatch(ଅକ୍ଷର* ଅଚଳ* patterns, ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ strv_fnmatch_full(patterns, s, 0, ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool strv_fnmatch_or_empty(ଅକ୍ଷର* ଅଚଳ* patterns, ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ପ୍ରତିଜ୍ଞା(s);
        ଫେରନ୍ତୁ strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, ଖାଲି);
}

ଅକ୍ଷର** strv_skip(ଅକ୍ଷର **l, size_t n);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_n(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t n);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_assignment(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *lhs, ଅଚଳ ଅକ୍ଷର *rhs);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fputstrv(FILE *f, ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

ଶୂନ୍ୟ string_strv_hashmap_remove(Hashmap *h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ଶୂନ୍ୟ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_strv_hashmap_put(Hashmap **h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_strv_ordered_hashmap_put(OrderedHashmap **h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_rebreak_ଧାଡ଼ିଗୁଡ଼ିକ(ଅକ୍ଷର **l, size_t width, ଅକ୍ଷର ***ret);

ଅକ୍ଷର** strv_filter_prefix(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* ଅଚଳ strv_empty[1];

#define STRV_EMPTY ((ଅକ୍ଷର**) strv_empty)
