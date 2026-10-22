/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "../fundamental/strv.h"   /* IWYU pragma: export */

અક્ષર* strv_find(અક્ષર * અચલ *l, અચલ અક્ષર *name) _pure_;
અક્ષર* strv_find_case(અક્ષર * અચલ *l, અચલ અક્ષર *name) _pure_;
અક્ષર* strv_find_prefix(અક્ષર * અચલ *l, અચલ અક્ષર *name) _pure_;
અક્ષર* strv_find_startswith(અક્ષર * અચલ *l, અચલ અક્ષર *name) _pure_;
અક્ષર* strv_find_closest(અક્ષર * અચલ *l, અચલ અક્ષર *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
અક્ષર* strv_find_first_field(અક્ષર * અચલ *needles, અક્ષર * અચલ *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

અક્ષર** strv_free(અક્ષર **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(અક્ષર**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

અક્ષર** strv_free_erase(અક્ષર **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(અક્ષર**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

શૂન્ય strv_free_many(અક્ષર ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

અક્ષર** strv_copy_n(અક્ષર * અચલ *l, size_t n);
સ્થિર અંતઃરેખ અક્ષર** strv_copy(અક્ષર * અચલ *l) {
        પાછા strv_copy_n(l, SIZE_MAX);
}
પૂર્ણાંક strv_copy_unless_empty(અક્ષર * અચલ *l, અક્ષર ***ret);

size_t strv_length(અક્ષર * અચલ *l) _pure_;

પૂર્ણાંક strv_extend_strv(અક્ષર ***a, અક્ષર * અચલ *b, bool filter_duplicates);
પૂર્ણાંક strv_extend_strv_consume(અક્ષર ***a, અક્ષર **b, bool filter_duplicates);

પૂર્ણાંક strv_extend_strv_biconcat(અક્ષર ***a, અચલ અક્ષર *prefix, અચલ અક્ષર* અચલ *b, અચલ અક્ષર *suffix);
સ્થિર અંતઃરેખ પૂર્ણાંક strv_extend_strv_concat(અક્ષર ***a, અચલ અક્ષર* અચલ *b, અચલ અક્ષર *suffix) {
        પાછા strv_extend_strv_biconcat(a, ખાલી, b, suffix);
}

પૂર્ણાંક strv_prepend(અક્ષર ***l, અચલ અક્ષર *કિંમત);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

પૂર્ણાંક strv_extend_with_size(અક્ષર ***l, size_t *n, અચલ અક્ષર *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક strv_extend(અક્ષર ***l, અચલ અક્ષર *કિંમત) {
        પાછા strv_extend_with_size(l, ખાલી, કિંમત);
}

પૂર્ણાંક strv_extend_many_internal(અક્ષર ***l, અચલ અક્ષર *કિંમત, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

પૂર્ણાંક strv_extendf_with_size(અક્ષર ***l, size_t *n, અચલ અક્ષર *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, ખાલી, __VA_ARGS__)

પૂર્ણાંક strv_extend_joined_with_size_sentinel(અક્ષર ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, ખાલી)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, ખાલી, __VA_ARGS__)

પૂર્ણાંક strv_ધકેલો_with_size(અક્ષર ***l, size_t *n, અક્ષર *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક strv_ધકેલો(અક્ષર ***l, અક્ષર *કિંમત) {
        પાછા strv_ધકેલો_with_size(l, ખાલી, કિંમત);
}
પૂર્ણાંક strv_ધકેલો_pair(અક્ષર ***l, અક્ષર *a, અક્ષર *b);

પૂર્ણાંક strv_ઉમેરો(અક્ષર ***l, size_t position, અક્ષર *કિંમત);

સ્થિર અંતઃરેખ પૂર્ણાંક strv_ધકેલો_prepend(અક્ષર ***l, અક્ષર *કિંમત) {
        પાછા strv_ઉમેરો(l, 0, કિંમત);
}

પૂર્ણાંક strv_consume_with_size(અક્ષર ***l, size_t *n, અક્ષર *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક strv_consume(અક્ષર ***l, અક્ષર *કિંમત) {
        પાછા strv_consume_with_size(l, ખાલી, કિંમત);
}

પૂર્ણાંક strv_consume_pair(અક્ષર ***l, અક્ષર *a, અક્ષર *b);
પૂર્ણાંક strv_consume_prepend(અક્ષર ***l, અક્ષર *કિંમત);

અક્ષર** strv_remove(અક્ષર **l, અચલ અક્ષર *s);
અક્ષર** strv_remove_strv(અક્ષર **l, અક્ષર *અચલ*ll);
અક્ષર** strv_uniq(અક્ષર **l);
bool strv_is_uniq(અક્ષર * અચલ *l) _pure_;

પૂર્ણાંક strv_compare(અક્ષર * અચલ *a, અક્ષર * અચલ *b) _pure_;
સ્થિર અંતઃરેખ bool strv_equal(અક્ષર * અચલ *a, અક્ષર * અચલ *b) {
        પાછા strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(અક્ષર * અચલ *a, અક્ષર * અચલ *b) _pure_;

અક્ષર** strv_new_internal(અચલ અક્ષર *x, ...) _sentinel_;
અક્ષર** strv_new_ap(અચલ અક્ષર *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, ખાલી)

#define STRV_IGNORE ((અચલ અક્ષર *) POINTER_MAX)

સ્થિર અંતઃરેખ અચલ અક્ષર* STRV_IFNOTNULL(અચલ અક્ષર *x) {
        પાછા x ?: STRV_IGNORE;
}

પૂર્ણાંક strv_split_full(અક્ષર ***t, અચલ અક્ષર *s, અચલ અક્ષર *separators, ExtractFlags flags);
અક્ષર** strv_split(અચલ અક્ષર *s, અચલ અક્ષર *separators);

પૂર્ણાંક strv_split_and_extend_full(અક્ષર ***t, અચલ અક્ષર *s, અચલ અક્ષર *separators, bool filter_duplicates, ExtractFlags flags);
પૂર્ણાંક strv_split_and_extend(અક્ષર ***t, અચલ અક્ષર *s, અચલ અક્ષર *separators, bool filter_duplicates);

પૂર્ણાંક strv_split_newlines_full(અક્ષર ***ret, અચલ અક્ષર *s, ExtractFlags flags);
અક્ષર** strv_split_newlines(અચલ અક્ષર *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
પૂર્ણાંક strv_split_colon_pairs(અક્ષર ***t, અચલ અક્ષર *s);

અક્ષર* strv_join_full(અક્ષર * અચલ *l, અચલ અક્ષર *separator, અચલ અક્ષર *prefix);
સ્થિર અંતઃરેખ અક્ષર *strv_join(અક્ષર * અચલ *l, અચલ અક્ષર *separator) {
        પાછા strv_join_full(l, separator, ખાલી);
}

bool strv_overlap(અક્ષર * અચલ *a, અક્ષર * અચલ *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        માટે (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : ખાલી;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        માટે (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

અક્ષર** strv_sort(અક્ષર **l);
અક્ષર** strv_sort_uniq_full(અક્ષર **l, પૂર્ણાંક (*compare)(અક્ષર * અચલ *a, અક્ષર * અચલ *b));
અક્ષર** strv_sort_uniq(અક્ષર **l);
શૂન્ય strv_print_full(અક્ષર * અચલ *l, અચલ અક્ષર *prefix);
સ્થિર અંતઃરેખ શૂન્ય strv_print(અક્ષર * અચલ *l) {
        strv_print_full(l, ખાલી);
}

અક્ષર* startswith_strv_internal(અચલ અક્ષર *s, અક્ષર * અચલ *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

અક્ષર* endswith_strv_internal(અચલ અક્ષર *s, અક્ષર * અચલ *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                અચલ અક્ષર* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                અચલ અક્ષર* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        માટે (અચલ અક્ષર *x, * અચલ*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

અક્ષર** strv_reverse(અક્ષર **l);
અક્ષર** strv_shell_escape(અક્ષર **l, અચલ અક્ષર *bad);

bool strv_fnmatch_full(અક્ષર* અચલ* patterns, અચલ અક્ષર *s, પૂર્ણાંક flags, size_t *ret_matched_pos);
સ્થિર અંતઃરેખ bool strv_fnmatch(અક્ષર* અચલ* patterns, અચલ અક્ષર *s) {
        પાછા strv_fnmatch_full(patterns, s, 0, ખાલી);
}
સ્થિર અંતઃરેખ bool strv_fnmatch_or_empty(અક્ષર* અચલ* patterns, અચલ અક્ષર *s, પૂર્ણાંક flags) {
        પ્રતિજ્ઞા(s);
        પાછા strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, ખાલી);
}

અક્ષર** strv_skip(અક્ષર **l, size_t n);

પૂર્ણાંક strv_extend_n(અક્ષર ***l, અચલ અક્ષર *કિંમત, size_t n);

પૂર્ણાંક strv_extend_assignment(અક્ષર ***l, અચલ અક્ષર *lhs, અચલ અક્ષર *rhs);

પૂર્ણાંક fputstrv(FILE *f, અક્ષર * અચલ *l, અચલ અક્ષર *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

શૂન્ય string_strv_hashmap_remove(Hashmap *h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);
શૂન્ય string_strv_ordered_hashmap_remove(OrderedHashmap *h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);
પૂર્ણાંક string_strv_hashmap_put(Hashmap **h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);
પૂર્ણાંક string_strv_ordered_hashmap_put(OrderedHashmap **h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);

પૂર્ણાંક strv_rebreak_લીટીઓ(અક્ષર **l, size_t width, અક્ષર ***ret);

અક્ષર** strv_filter_prefix(અક્ષર * અચલ *l, અચલ અક્ષર *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
બાહ્ય અચલ અક્ષર* અચલ strv_empty[1];

#define STRV_EMPTY ((અક્ષર**) strv_empty)
