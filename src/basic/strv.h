/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

#অন্তর্ভাৱ "../fundamental/strv.h"   /* IWYU pragma: export */

ৱর্ণ* strv_find(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) _pure_;
ৱর্ণ* strv_find_case(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) _pure_;
ৱর্ণ* strv_find_prefix(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) _pure_;
ৱর্ণ* strv_find_startswith(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) _pure_;
ৱর্ণ* strv_find_closest(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
ৱর্ণ* strv_find_first_field(ৱর্ণ * ধ্রুৱ *needles, ৱর্ণ * ধ্রুৱ *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

ৱর্ণ** strv_free(ৱর্ণ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ৱর্ণ**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

ৱর্ণ** strv_free_erase(ৱর্ণ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(ৱর্ণ**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

শূন্য strv_free_many(ৱর্ণ ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

ৱর্ণ** strv_copy_n(ৱর্ণ * ধ্রুৱ *l, size_t n);
স্থির অন্তঃস্থ ৱর্ণ** strv_copy(ৱর্ণ * ধ্রুৱ *l) {
        ঘূৰাই strv_copy_n(l, SIZE_MAX);
}
পূর্ণাঙ্ক strv_copy_unless_empty(ৱর্ণ * ধ্রুৱ *l, ৱর্ণ ***ret);

size_t strv_length(ৱর্ণ * ধ্রুৱ *l) _pure_;

পূর্ণাঙ্ক strv_extend_strv(ৱর্ণ ***a, ৱর্ণ * ধ্রুৱ *b, bool filter_duplicates);
পূর্ণাঙ্ক strv_extend_strv_consume(ৱর্ণ ***a, ৱর্ণ **b, bool filter_duplicates);

পূর্ণাঙ্ক strv_extend_strv_biconcat(ৱর্ণ ***a, ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ* ধ্রুৱ *b, ধ্রুৱ ৱর্ণ *suffix);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_extend_strv_concat(ৱর্ণ ***a, ধ্রুৱ ৱর্ণ* ধ্রুৱ *b, ধ্রুৱ ৱর্ণ *suffix) {
        ঘূৰাই strv_extend_strv_biconcat(a, রিক্ত, b, suffix);
}

পূর্ণাঙ্ক strv_prepend(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

পূর্ণাঙ্ক strv_extend_with_size(ৱর্ণ ***l, size_t *n, ধ্রুৱ ৱর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_extend(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান) {
        ঘূৰাই strv_extend_with_size(l, রিক্ত, মান);
}

পূর্ণাঙ্ক strv_extend_many_internal(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

পূর্ণাঙ্ক strv_extendf_with_size(ৱর্ণ ***l, size_t *n, ধ্রুৱ ৱর্ণ *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, রিক্ত, __VA_ARGS__)

পূর্ণাঙ্ক strv_extend_joined_with_size_sentinel(ৱর্ণ ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, রিক্ত)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, রিক্ত, __VA_ARGS__)

পূর্ণাঙ্ক strv_ঠেলো_with_size(ৱর্ণ ***l, size_t *n, ৱর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_ঠেলো(ৱর্ণ ***l, ৱর্ণ *মান) {
        ঘূৰাই strv_ঠেলো_with_size(l, রিক্ত, মান);
}
পূর্ণাঙ্ক strv_ঠেলো_pair(ৱর্ণ ***l, ৱর্ণ *a, ৱর্ণ *b);

পূর্ণাঙ্ক strv_ঢোকাও(ৱর্ণ ***l, size_t position, ৱর্ণ *মান);

স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_ঠেলো_prepend(ৱর্ণ ***l, ৱর্ণ *মান) {
        ঘূৰাই strv_ঢোকাও(l, 0, মান);
}

পূর্ণাঙ্ক strv_consume_with_size(ৱর্ণ ***l, size_t *n, ৱর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_consume(ৱর্ণ ***l, ৱর্ণ *মান) {
        ঘূৰাই strv_consume_with_size(l, রিক্ত, মান);
}

পূর্ণাঙ্ক strv_consume_pair(ৱর্ণ ***l, ৱর্ণ *a, ৱর্ণ *b);
পূর্ণাঙ্ক strv_consume_prepend(ৱর্ণ ***l, ৱর্ণ *মান);

ৱর্ণ** strv_remove(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *s);
ৱর্ণ** strv_remove_strv(ৱর্ণ **l, ৱর্ণ *ধ্রুৱ*ll);
ৱর্ণ** strv_uniq(ৱর্ণ **l);
bool strv_is_uniq(ৱর্ণ * ধ্রুৱ *l) _pure_;

পূর্ণাঙ্ক strv_compare(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) _pure_;
স্থির অন্তঃস্থ bool strv_equal(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) {
        ঘূৰাই strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) _pure_;

ৱর্ণ** strv_new_internal(ধ্রুৱ ৱর্ণ *x, ...) _sentinel_;
ৱর্ণ** strv_new_ap(ধ্রুৱ ৱর্ণ *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, রিক্ত)

#define STRV_IGNORE ((ধ্রুৱ ৱর্ণ *) POINTER_MAX)

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* STRV_IFNOTNULL(ধ্রুৱ ৱর্ণ *x) {
        ঘূৰাই x ?: STRV_IGNORE;
}

পূর্ণাঙ্ক strv_split_full(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators, ExtractFlags flags);
ৱর্ণ** strv_split(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators);

পূর্ণাঙ্ক strv_split_and_extend_full(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators, bool filter_duplicates, ExtractFlags flags);
পূর্ণাঙ্ক strv_split_and_extend(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators, bool filter_duplicates);

পূর্ণাঙ্ক strv_split_newlines_full(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *s, ExtractFlags flags);
ৱর্ণ** strv_split_newlines(ধ্রুৱ ৱর্ণ *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
পূর্ণাঙ্ক strv_split_colon_pairs(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s);

ৱর্ণ* strv_join_full(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *separator, ধ্রুৱ ৱর্ণ *prefix);
স্থির অন্তঃস্থ ৱর্ণ *strv_join(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *separator) {
        ঘূৰাই strv_join_full(l, separator, রিক্ত);
}

bool strv_overlap(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        বাবে (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : রিক্ত;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        বাবে (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

ৱর্ণ** strv_sort(ৱর্ণ **l);
ৱর্ণ** strv_sort_uniq_full(ৱর্ণ **l, পূর্ণাঙ্ক (*compare)(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b));
ৱর্ণ** strv_sort_uniq(ৱর্ণ **l);
শূন্য strv_print_full(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *prefix);
স্থির অন্তঃস্থ শূন্য strv_print(ৱর্ণ * ধ্রুৱ *l) {
        strv_print_full(l, রিক্ত);
}

ৱর্ণ* startswith_strv_internal(ধ্রুৱ ৱর্ণ *s, ৱর্ণ * ধ্রুৱ *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ৱর্ণ* endswith_strv_internal(ধ্রুৱ ৱর্ণ *s, ৱর্ণ * ধ্রুৱ *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ধ্রুৱ ৱর্ণ* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ধ্রুৱ ৱর্ণ* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        বাবে (ধ্রুৱ ৱর্ণ *x, * ধ্রুৱ*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

ৱর্ণ** strv_reverse(ৱর্ণ **l);
ৱর্ণ** strv_shell_escape(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *bad);

bool strv_fnmatch_full(ৱর্ণ* ধ্রুৱ* patterns, ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক flags, size_t *ret_matched_pos);
স্থির অন্তঃস্থ bool strv_fnmatch(ৱর্ণ* ধ্রুৱ* patterns, ধ্রুৱ ৱর্ণ *s) {
        ঘূৰাই strv_fnmatch_full(patterns, s, 0, রিক্ত);
}
স্থির অন্তঃস্থ bool strv_fnmatch_or_empty(ৱর্ণ* ধ্রুৱ* patterns, ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক flags) {
        প্রতিজ্ঞা(s);
        ঘূৰাই strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, রিক্ত);
}

ৱর্ণ** strv_skip(ৱর্ণ **l, size_t n);

পূর্ণাঙ্ক strv_extend_n(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান, size_t n);

পূর্ণাঙ্ক strv_extend_assignment(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *lhs, ধ্রুৱ ৱর্ণ *rhs);

পূর্ণাঙ্ক fputstrv(FILE *f, ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

শূন্য string_strv_hashmap_remove(Hashmap *h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);
শূন্য string_strv_ordered_hashmap_remove(OrderedHashmap *h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);
পূর্ণাঙ্ক string_strv_hashmap_put(Hashmap **h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);
পূর্ণাঙ্ক string_strv_ordered_hashmap_put(OrderedHashmap **h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);

পূর্ণাঙ্ক strv_rebreak_সাৰিগুলি(ৱর্ণ **l, size_t width, ৱর্ণ ***ret);

ৱর্ণ** strv_filter_prefix(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
বাহ্য ধ্রুৱ ৱর্ণ* ধ্রুৱ strv_empty[1];

#define STRV_EMPTY ((ৱর্ণ**) strv_empty)
