/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#அந்தர்பாவ "../fundamental/strv.h"   /* IWYU pragma: export */

எழுத்து* strv_find(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) _pure_;
எழுத்து* strv_find_case(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) _pure_;
எழுத்து* strv_find_prefix(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) _pure_;
எழுத்து* strv_find_startswith(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) _pure_;
எழுத்து* strv_find_closest(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
எழுத்து* strv_find_first_field(எழுத்து * மாறிலி *needles, எழுத்து * மாறிலி *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

எழுத்து** strv_free(எழுத்து **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(எழுத்து**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

எழுத்து** strv_free_erase(எழுத்து **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(எழுத்து**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

வெறுமை strv_free_many(எழுத்து ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

எழுத்து** strv_copy_n(எழுத்து * மாறிலி *l, size_t n);
நிலையான உள்வரி எழுத்து** strv_copy(எழுத்து * மாறிலி *l) {
        திரும்பு strv_copy_n(l, SIZE_MAX);
}
முழு strv_copy_unless_empty(எழுத்து * மாறிலி *l, எழுத்து ***ret);

size_t strv_length(எழுத்து * மாறிலி *l) _pure_;

முழு strv_extend_strv(எழுத்து ***a, எழுத்து * மாறிலி *b, bool filter_duplicates);
முழு strv_extend_strv_consume(எழுத்து ***a, எழுத்து **b, bool filter_duplicates);

முழு strv_extend_strv_biconcat(எழுத்து ***a, மாறிலி எழுத்து *prefix, மாறிலி எழுத்து* மாறிலி *b, மாறிலி எழுத்து *suffix);
நிலையான உள்வரி முழு strv_extend_strv_concat(எழுத்து ***a, மாறிலி எழுத்து* மாறிலி *b, மாறிலி எழுத்து *suffix) {
        திரும்பு strv_extend_strv_biconcat(a, வெற்று, b, suffix);
}

முழு strv_prepend(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

முழு strv_extend_with_size(எழுத்து ***l, size_t *n, மாறிலி எழுத்து *மதிப்பு);
நிலையான உள்வரி முழு strv_extend(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு) {
        திரும்பு strv_extend_with_size(l, வெற்று, மதிப்பு);
}

முழு strv_extend_many_internal(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

முழு strv_extendf_with_size(எழுத்து ***l, size_t *n, மாறிலி எழுத்து *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, வெற்று, __VA_ARGS__)

முழு strv_extend_joined_with_size_sentinel(எழுத்து ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, வெற்று)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, வெற்று, __VA_ARGS__)

முழு strv_தள்ளு_with_size(எழுத்து ***l, size_t *n, எழுத்து *மதிப்பு);
நிலையான உள்வரி முழு strv_தள்ளு(எழுத்து ***l, எழுத்து *மதிப்பு) {
        திரும்பு strv_தள்ளு_with_size(l, வெற்று, மதிப்பு);
}
முழு strv_தள்ளு_pair(எழுத்து ***l, எழுத்து *a, எழுத்து *b);

முழு strv_செருகு(எழுத்து ***l, size_t position, எழுத்து *மதிப்பு);

நிலையான உள்வரி முழு strv_தள்ளு_prepend(எழுத்து ***l, எழுத்து *மதிப்பு) {
        திரும்பு strv_செருகு(l, 0, மதிப்பு);
}

முழு strv_consume_with_size(எழுத்து ***l, size_t *n, எழுத்து *மதிப்பு);
நிலையான உள்வரி முழு strv_consume(எழுத்து ***l, எழுத்து *மதிப்பு) {
        திரும்பு strv_consume_with_size(l, வெற்று, மதிப்பு);
}

முழு strv_consume_pair(எழுத்து ***l, எழுத்து *a, எழுத்து *b);
முழு strv_consume_prepend(எழுத்து ***l, எழுத்து *மதிப்பு);

எழுத்து** strv_remove(எழுத்து **l, மாறிலி எழுத்து *s);
எழுத்து** strv_remove_strv(எழுத்து **l, எழுத்து *மாறிலி*ll);
எழுத்து** strv_uniq(எழுத்து **l);
bool strv_is_uniq(எழுத்து * மாறிலி *l) _pure_;

முழு strv_compare(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) _pure_;
நிலையான உள்வரி bool strv_equal(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) {
        திரும்பு strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) _pure_;

எழுத்து** strv_new_internal(மாறிலி எழுத்து *x, ...) _sentinel_;
எழுத்து** strv_new_ap(மாறிலி எழுத்து *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, வெற்று)

#define STRV_IGNORE ((மாறிலி எழுத்து *) POINTER_MAX)

நிலையான உள்வரி மாறிலி எழுத்து* STRV_IFNOTNULL(மாறிலி எழுத்து *x) {
        திரும்பு x ?: STRV_IGNORE;
}

முழு strv_split_full(எழுத்து ***t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators, ExtractFlags flags);
எழுத்து** strv_split(மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators);

முழு strv_split_and_extend_full(எழுத்து ***t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators, bool filter_duplicates, ExtractFlags flags);
முழு strv_split_and_extend(எழுத்து ***t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators, bool filter_duplicates);

முழு strv_split_newlines_full(எழுத்து ***ret, மாறிலி எழுத்து *s, ExtractFlags flags);
எழுத்து** strv_split_newlines(மாறிலி எழுத்து *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
முழு strv_split_colon_pairs(எழுத்து ***t, மாறிலி எழுத்து *s);

எழுத்து* strv_join_full(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *separator, மாறிலி எழுத்து *prefix);
நிலையான உள்வரி எழுத்து *strv_join(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *separator) {
        திரும்பு strv_join_full(l, separator, வெற்று);
}

bool strv_overlap(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        க்கு (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : வெற்று;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        க்கு (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

எழுத்து** strv_sort(எழுத்து **l);
எழுத்து** strv_sort_uniq_full(எழுத்து **l, முழு (*compare)(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b));
எழுத்து** strv_sort_uniq(எழுத்து **l);
வெறுமை strv_print_full(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *prefix);
நிலையான உள்வரி வெறுமை strv_print(எழுத்து * மாறிலி *l) {
        strv_print_full(l, வெற்று);
}

எழுத்து* startswith_strv_internal(மாறிலி எழுத்து *s, எழுத்து * மாறிலி *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

எழுத்து* endswith_strv_internal(மாறிலி எழுத்து *s, எழுத்து * மாறிலி *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                மாறிலி எழுத்து* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                மாறிலி எழுத்து* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        க்கு (மாறிலி எழுத்து *x, * மாறிலி*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

எழுத்து** strv_reverse(எழுத்து **l);
எழுத்து** strv_shell_escape(எழுத்து **l, மாறிலி எழுத்து *bad);

bool strv_fnmatch_full(எழுத்து* மாறிலி* patterns, மாறிலி எழுத்து *s, முழு flags, size_t *ret_matched_pos);
நிலையான உள்வரி bool strv_fnmatch(எழுத்து* மாறிலி* patterns, மாறிலி எழுத்து *s) {
        திரும்பு strv_fnmatch_full(patterns, s, 0, வெற்று);
}
நிலையான உள்வரி bool strv_fnmatch_or_empty(எழுத்து* மாறிலி* patterns, மாறிலி எழுத்து *s, முழு flags) {
        ப்ரதிஜ்ஞா(s);
        திரும்பு strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, வெற்று);
}

எழுத்து** strv_skip(எழுத்து **l, size_t n);

முழு strv_extend_n(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு, size_t n);

முழு strv_extend_assignment(எழுத்து ***l, மாறிலி எழுத்து *lhs, மாறிலி எழுத்து *rhs);

முழு fputstrv(FILE *f, எழுத்து * மாறிலி *l, மாறிலி எழுத்து *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

வெறுமை string_strv_hashmap_remove(Hashmap *h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);
வெறுமை string_strv_ordered_hashmap_remove(OrderedHashmap *h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);
முழு string_strv_hashmap_put(Hashmap **h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);
முழு string_strv_ordered_hashmap_put(OrderedHashmap **h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);

முழு strv_rebreak_வரிகள்(எழுத்து **l, size_t width, எழுத்து ***ret);

எழுத்து** strv_filter_prefix(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
வெளி மாறிலி எழுத்து* மாறிலி strv_empty[1];

#define STRV_EMPTY ((எழுத்து**) strv_empty)
