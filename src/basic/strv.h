/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/strv.h"   /* IWYU pragma: export */

বর্ণ* strv_find(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) _pure_;
বর্ণ* strv_find_case(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) _pure_;
বর্ণ* strv_find_prefix(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) _pure_;
বর্ণ* strv_find_startswith(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) _pure_;
বর্ণ* strv_find_closest(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
বর্ণ* strv_find_first_field(বর্ণ * ধ্রুব *needles, বর্ণ * ধ্রুব *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

বর্ণ** strv_free(বর্ণ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(বর্ণ**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

বর্ণ** strv_free_erase(বর্ণ **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(বর্ণ**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

শূন্য strv_free_many(বর্ণ ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

বর্ণ** strv_copy_n(বর্ণ * ধ্রুব *l, size_t n);
স্থির অন্তঃস্থ বর্ণ** strv_copy(বর্ণ * ধ্রুব *l) {
        প্রত্যাগম strv_copy_n(l, SIZE_MAX);
}
পূর্ণাঙ্ক strv_copy_unless_empty(বর্ণ * ধ্রুব *l, বর্ণ ***ret);

size_t strv_length(বর্ণ * ধ্রুব *l) _pure_;

পূর্ণাঙ্ক strv_extend_strv(বর্ণ ***a, বর্ণ * ধ্রুব *b, bool filter_duplicates);
পূর্ণাঙ্ক strv_extend_strv_consume(বর্ণ ***a, বর্ণ **b, bool filter_duplicates);

পূর্ণাঙ্ক strv_extend_strv_biconcat(বর্ণ ***a, ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ* ধ্রুব *b, ধ্রুব বর্ণ *suffix);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_extend_strv_concat(বর্ণ ***a, ধ্রুব বর্ণ* ধ্রুব *b, ধ্রুব বর্ণ *suffix) {
        প্রত্যাগম strv_extend_strv_biconcat(a, রিক্ত, b, suffix);
}

পূর্ণাঙ্ক strv_prepend(বর্ণ ***l, ধ্রুব বর্ণ *মান);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

পূর্ণাঙ্ক strv_extend_with_size(বর্ণ ***l, size_t *n, ধ্রুব বর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_extend(বর্ণ ***l, ধ্রুব বর্ণ *মান) {
        প্রত্যাগম strv_extend_with_size(l, রিক্ত, মান);
}

পূর্ণাঙ্ক strv_extend_many_internal(বর্ণ ***l, ধ্রুব বর্ণ *মান, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

পূর্ণাঙ্ক strv_extendf_with_size(বর্ণ ***l, size_t *n, ধ্রুব বর্ণ *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, রিক্ত, __VA_ARGS__)

পূর্ণাঙ্ক strv_extend_joined_with_size_sentinel(বর্ণ ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, রিক্ত)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, রিক্ত, __VA_ARGS__)

পূর্ণাঙ্ক strv_ঠেলো_with_size(বর্ণ ***l, size_t *n, বর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_ঠেলো(বর্ণ ***l, বর্ণ *মান) {
        প্রত্যাগম strv_ঠেলো_with_size(l, রিক্ত, মান);
}
পূর্ণাঙ্ক strv_ঠেলো_pair(বর্ণ ***l, বর্ণ *a, বর্ণ *b);

পূর্ণাঙ্ক strv_ঢোকাও(বর্ণ ***l, size_t position, বর্ণ *মান);

স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_ঠেলো_prepend(বর্ণ ***l, বর্ণ *মান) {
        প্রত্যাগম strv_ঢোকাও(l, 0, মান);
}

পূর্ণাঙ্ক strv_consume_with_size(বর্ণ ***l, size_t *n, বর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক strv_consume(বর্ণ ***l, বর্ণ *মান) {
        প্রত্যাগম strv_consume_with_size(l, রিক্ত, মান);
}

পূর্ণাঙ্ক strv_consume_pair(বর্ণ ***l, বর্ণ *a, বর্ণ *b);
পূর্ণাঙ্ক strv_consume_prepend(বর্ণ ***l, বর্ণ *মান);

বর্ণ** strv_remove(বর্ণ **l, ধ্রুব বর্ণ *s);
বর্ণ** strv_remove_strv(বর্ণ **l, বর্ণ *ধ্রুব*ll);
বর্ণ** strv_uniq(বর্ণ **l);
bool strv_is_uniq(বর্ণ * ধ্রুব *l) _pure_;

পূর্ণাঙ্ক strv_compare(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) _pure_;
স্থির অন্তঃস্থ bool strv_equal(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) {
        প্রত্যাগম strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) _pure_;

বর্ণ** strv_new_internal(ধ্রুব বর্ণ *x, ...) _sentinel_;
বর্ণ** strv_new_ap(ধ্রুব বর্ণ *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, রিক্ত)

#define STRV_IGNORE ((ধ্রুব বর্ণ *) POINTER_MAX)

স্থির অন্তঃস্থ ধ্রুব বর্ণ* STRV_IFNOTNULL(ধ্রুব বর্ণ *x) {
        প্রত্যাগম x ?: STRV_IGNORE;
}

পূর্ণাঙ্ক strv_split_full(বর্ণ ***t, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators, ExtractFlags flags);
বর্ণ** strv_split(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators);

পূর্ণাঙ্ক strv_split_and_extend_full(বর্ণ ***t, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators, bool filter_duplicates, ExtractFlags flags);
পূর্ণাঙ্ক strv_split_and_extend(বর্ণ ***t, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators, bool filter_duplicates);

পূর্ণাঙ্ক strv_split_newlines_full(বর্ণ ***ret, ধ্রুব বর্ণ *s, ExtractFlags flags);
বর্ণ** strv_split_newlines(ধ্রুব বর্ণ *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
পূর্ণাঙ্ক strv_split_colon_pairs(বর্ণ ***t, ধ্রুব বর্ণ *s);

বর্ণ* strv_join_full(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *separator, ধ্রুব বর্ণ *prefix);
স্থির অন্তঃস্থ বর্ণ *strv_join(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *separator) {
        প্রত্যাগম strv_join_full(l, separator, রিক্ত);
}

bool strv_overlap(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        প্রতি (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : রিক্ত;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        প্রতি (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

বর্ণ** strv_sort(বর্ণ **l);
বর্ণ** strv_sort_uniq_full(বর্ণ **l, পূর্ণাঙ্ক (*compare)(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b));
বর্ণ** strv_sort_uniq(বর্ণ **l);
শূন্য strv_print_full(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *prefix);
স্থির অন্তঃস্থ শূন্য strv_print(বর্ণ * ধ্রুব *l) {
        strv_print_full(l, রিক্ত);
}

বর্ণ* startswith_strv_internal(ধ্রুব বর্ণ *s, বর্ণ * ধ্রুব *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

বর্ণ* endswith_strv_internal(ধ্রুব বর্ণ *s, বর্ণ * ধ্রুব *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ধ্রুব বর্ণ* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                ধ্রুব বর্ণ* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        প্রতি (ধ্রুব বর্ণ *x, * ধ্রুব*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

বর্ণ** strv_reverse(বর্ণ **l);
বর্ণ** strv_shell_escape(বর্ণ **l, ধ্রুব বর্ণ *bad);

bool strv_fnmatch_full(বর্ণ* ধ্রুব* patterns, ধ্রুব বর্ণ *s, পূর্ণাঙ্ক flags, size_t *ret_matched_pos);
স্থির অন্তঃস্থ bool strv_fnmatch(বর্ণ* ধ্রুব* patterns, ধ্রুব বর্ণ *s) {
        প্রত্যাগম strv_fnmatch_full(patterns, s, 0, রিক্ত);
}
স্থির অন্তঃস্থ bool strv_fnmatch_or_empty(বর্ণ* ধ্রুব* patterns, ধ্রুব বর্ণ *s, পূর্ণাঙ্ক flags) {
        প্রতিজ্ঞা(s);
        প্রত্যাগম strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, রিক্ত);
}

বর্ণ** strv_skip(বর্ণ **l, size_t n);

পূর্ণাঙ্ক strv_extend_n(বর্ণ ***l, ধ্রুব বর্ণ *মান, size_t n);

পূর্ণাঙ্ক strv_extend_assignment(বর্ণ ***l, ধ্রুব বর্ণ *lhs, ধ্রুব বর্ণ *rhs);

পূর্ণাঙ্ক fputstrv(FILE *f, বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

শূন্য string_strv_hashmap_remove(Hashmap *h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান);
শূন্য string_strv_ordered_hashmap_remove(OrderedHashmap *h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান);
পূর্ণাঙ্ক string_strv_hashmap_put(Hashmap **h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান);
পূর্ণাঙ্ক string_strv_ordered_hashmap_put(OrderedHashmap **h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান);

পূর্ণাঙ্ক strv_rebreak_সারিগুলি(বর্ণ **l, size_t width, বর্ণ ***ret);

বর্ণ** strv_filter_prefix(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
বাহ্য ধ্রুব বর্ণ* ধ্রুব strv_empty[1];

#define STRV_EMPTY ((বর্ণ**) strv_empty)
