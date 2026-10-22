/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#include "../fundamental/strv.h"   /* IWYU pragma: export */

var_na* strv_find(var_na * const *l, const var_na *name) _pure_;
var_na* strv_find_case(var_na * const *l, const var_na *name) _pure_;
var_na* strv_find_prefix(var_na * const *l, const var_na *name) _pure_;
var_na* strv_find_startswith(var_na * const *l, const var_na *name) _pure_;
var_na* strv_find_closest(var_na * const *l, const var_na *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
var_na* strv_find_first_field(var_na * const *needles, var_na * const *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

var_na** strv_free(var_na **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

var_na** strv_free_erase(var_na **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

void strv_free_many(var_na ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

var_na** strv_copy_n(var_na * const *l, size_t n);
static anta_hstha var_na** strv_copy(var_na * const *l) {
        ສົ່ງຄືນ strv_copy_n(l, SIZE_MAX);
}
ຈຳນວນເຕັມ strv_copy_unless_empty(var_na * const *l, var_na ***ret);

size_t strv_length(var_na * const *l) _pure_;

ຈຳນວນເຕັມ strv_extend_strv(var_na ***a, var_na * const *b, bool filter_duplicates);
ຈຳນວນເຕັມ strv_extend_strv_consume(var_na ***a, var_na **b, bool filter_duplicates);

ຈຳນວນເຕັມ strv_extend_strv_biconcat(var_na ***a, const var_na *prefix, const var_na* const *b, const var_na *suffix);
static anta_hstha ຈຳນວນເຕັມ strv_extend_strv_concat(var_na ***a, const var_na* const *b, const var_na *suffix) {
        ສົ່ງຄືນ strv_extend_strv_biconcat(a, NULL, b, suffix);
}

ຈຳນວນເຕັມ strv_prepend(var_na ***l, const var_na *value);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

ຈຳນວນເຕັມ strv_extend_with_size(var_na ***l, size_t *n, const var_na *value);
static anta_hstha ຈຳນວນເຕັມ strv_extend(var_na ***l, const var_na *value) {
        ສົ່ງຄືນ strv_extend_with_size(l, NULL, value);
}

ຈຳນວນເຕັມ strv_extend_many_internal(var_na ***l, const var_na *value, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

ຈຳນວນເຕັມ strv_extendf_with_size(var_na ***l, size_t *n, const var_na *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, NULL, __VA_ARGS__)

ຈຳນວນເຕັມ strv_extend_joined_with_size_sentinel(var_na ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, NULL)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, NULL, __VA_ARGS__)

ຈຳນວນເຕັມ strv_push_with_size(var_na ***l, size_t *n, var_na *value);
static anta_hstha ຈຳນວນເຕັມ strv_push(var_na ***l, var_na *value) {
        ສົ່ງຄືນ strv_push_with_size(l, NULL, value);
}
ຈຳນວນເຕັມ strv_push_pair(var_na ***l, var_na *a, var_na *b);

ຈຳນວນເຕັມ strv_insert(var_na ***l, size_t position, var_na *value);

static anta_hstha ຈຳນວນເຕັມ strv_push_prepend(var_na ***l, var_na *value) {
        ສົ່ງຄືນ strv_insert(l, 0, value);
}

ຈຳນວນເຕັມ strv_consume_with_size(var_na ***l, size_t *n, var_na *value);
static anta_hstha ຈຳນວນເຕັມ strv_consume(var_na ***l, var_na *value) {
        ສົ່ງຄືນ strv_consume_with_size(l, NULL, value);
}

ຈຳນວນເຕັມ strv_consume_pair(var_na ***l, var_na *a, var_na *b);
ຈຳນວນເຕັມ strv_consume_prepend(var_na ***l, var_na *value);

var_na** strv_remove(var_na **l, const var_na *s);
var_na** strv_remove_strv(var_na **l, var_na *const*ll);
var_na** strv_uniq(var_na **l);
bool strv_is_uniq(var_na * const *l) _pure_;

ຈຳນວນເຕັມ strv_compare(var_na * const *a, var_na * const *b) _pure_;
static anta_hstha bool strv_equal(var_na * const *a, var_na * const *b) {
        ສົ່ງຄືນ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(var_na * const *a, var_na * const *b) _pure_;

var_na** strv_new_internal(const var_na *x, ...) _sentinel_;
var_na** strv_new_ap(const var_na *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, NULL)

#define STRV_IGNORE ((const var_na *) POINTER_MAX)

static anta_hstha const var_na* STRV_IFNOTNULL(const var_na *x) {
        ສົ່ງຄືນ x ?: STRV_IGNORE;
}

ຈຳນວນເຕັມ strv_split_full(var_na ***t, const var_na *s, const var_na *separators, ExtractFlags flags);
var_na** strv_split(const var_na *s, const var_na *separators);

ຈຳນວນເຕັມ strv_split_and_extend_full(var_na ***t, const var_na *s, const var_na *separators, bool filter_duplicates, ExtractFlags flags);
ຈຳນວນເຕັມ strv_split_and_extend(var_na ***t, const var_na *s, const var_na *separators, bool filter_duplicates);

ຈຳນວນເຕັມ strv_split_newlines_full(var_na ***ret, const var_na *s, ExtractFlags flags);
var_na** strv_split_newlines(const var_na *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
ຈຳນວນເຕັມ strv_split_colon_pairs(var_na ***t, const var_na *s);

var_na* strv_join_full(var_na * const *l, const var_na *separator, const var_na *prefix);
static anta_hstha var_na *strv_join(var_na * const *l, const var_na *separator) {
        ສົ່ງຄືນ strv_join_full(l, separator, NULL);
}

bool strv_overlap(var_na * const *a, var_na * const *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        ສຳລັບ (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : NULL;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        ສຳລັບ (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

var_na** strv_sort(var_na **l);
var_na** strv_sort_uniq_full(var_na **l, ຈຳນວນເຕັມ (*compare)(var_na * const *a, var_na * const *b));
var_na** strv_sort_uniq(var_na **l);
void strv_print_full(var_na * const *l, const var_na *prefix);
static anta_hstha void strv_print(var_na * const *l) {
        strv_print_full(l, NULL);
}

var_na* startswith_strv_internal(const var_na *s, var_na * const *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

var_na* endswith_strv_internal(const var_na *s, var_na * const *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                const var_na* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                const var_na* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        ສຳລັບ (const var_na *x, * const*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

var_na** strv_reverse(var_na **l);
var_na** strv_shell_escape(var_na **l, const var_na *bad);

bool strv_fnmatch_full(var_na* const* patterns, const var_na *s, ຈຳນວນເຕັມ flags, size_t *ret_matched_pos);
static anta_hstha bool strv_fnmatch(var_na* const* patterns, const var_na *s) {
        ສົ່ງຄືນ strv_fnmatch_full(patterns, s, 0, NULL);
}
static anta_hstha bool strv_fnmatch_or_empty(var_na* const* patterns, const var_na *s, ຈຳນວນເຕັມ flags) {
        assert(s);
        ສົ່ງຄືນ strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, NULL);
}

var_na** strv_skip(var_na **l, size_t n);

ຈຳນວນເຕັມ strv_extend_n(var_na ***l, const var_na *value, size_t n);

ຈຳນວນເຕັມ strv_extend_assignment(var_na ***l, const var_na *lhs, const var_na *rhs);

ຈຳນວນເຕັມ fputstrv(FILE *f, var_na * const *l, const var_na *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

void string_strv_hashmap_remove(Hashmap *h, const var_na *key, const var_na *value);
void string_strv_ordered_hashmap_remove(OrderedHashmap *h, const var_na *key, const var_na *value);
ຈຳນວນເຕັມ string_strv_hashmap_put(Hashmap **h, const var_na *key, const var_na *value);
ຈຳນວນເຕັມ string_strv_ordered_hashmap_put(OrderedHashmap **h, const var_na *key, const var_na *value);

ຈຳນວນເຕັມ strv_rebreak_lines(var_na **l, size_t width, var_na ***ret);

var_na** strv_filter_prefix(var_na * const *l, const var_na *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
extern const var_na* const strv_empty[1];

#define STRV_EMPTY ((var_na**) strv_empty)
