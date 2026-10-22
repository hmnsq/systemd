/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "../fundamental/strv.h"   /* IWYU pragma: export */

അക്ഷരം* strv_find(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) _pure_;
അക്ഷരം* strv_find_case(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) _pure_;
അക്ഷരം* strv_find_prefix(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) _pure_;
അക്ഷരം* strv_find_startswith(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) _pure_;
അക്ഷരം* strv_find_closest(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
അക്ഷരം* strv_find_first_field(അക്ഷരം * സ്ഥിരം *needles, അക്ഷരം * സ്ഥിരം *haystack) _pure_;

#define strv_contains(l, s) (!!strv_find((l), (s)))
#define strv_contains_case(l, s) (!!strv_find_case((l), (s)))

അക്ഷരം** strv_free(അക്ഷരം **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(അക്ഷരം**, strv_free);
#define _cleanup_strv_free_ _cleanup_(strv_freep)

അക്ഷരം** strv_free_erase(അക്ഷരം **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(അക്ഷരം**, strv_free_erase);
#define _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

ശൂന്യം strv_free_many(അക്ഷരം ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

അക്ഷരം** strv_copy_n(അക്ഷരം * സ്ഥിരം *l, size_t n);
സ്ഥിതം വരിയിൽ അക്ഷരം** strv_copy(അക്ഷരം * സ്ഥിരം *l) {
        തിരികെ strv_copy_n(l, SIZE_MAX);
}
പൂർണ്ണസംഖ്യ strv_copy_unless_empty(അക്ഷരം * സ്ഥിരം *l, അക്ഷരം ***ret);

size_t strv_length(അക്ഷരം * സ്ഥിരം *l) _pure_;

പൂർണ്ണസംഖ്യ strv_extend_strv(അക്ഷരം ***a, അക്ഷരം * സ്ഥിരം *b, bool filter_duplicates);
പൂർണ്ണസംഖ്യ strv_extend_strv_consume(അക്ഷരം ***a, അക്ഷരം **b, bool filter_duplicates);

പൂർണ്ണസംഖ്യ strv_extend_strv_biconcat(അക്ഷരം ***a, സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം* സ്ഥിരം *b, സ്ഥിരം അക്ഷരം *suffix);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strv_extend_strv_concat(അക്ഷരം ***a, സ്ഥിരം അക്ഷരം* സ്ഥിരം *b, സ്ഥിരം അക്ഷരം *suffix) {
        തിരികെ strv_extend_strv_biconcat(a, ശൂന്യ, b, suffix);
}

പൂർണ്ണസംഖ്യ strv_prepend(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

പൂർണ്ണസംഖ്യ strv_extend_with_size(അക്ഷരം ***l, size_t *n, സ്ഥിരം അക്ഷരം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strv_extend(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        തിരികെ strv_extend_with_size(l, ശൂന്യ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ strv_extend_many_internal(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം, ...);
#define strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

പൂർണ്ണസംഖ്യ strv_extendf_with_size(അക്ഷരം ***l, size_t *n, സ്ഥിരം അക്ഷരം *format, ...) _printf_(3,4);
#define strv_extendf(l, ...) strv_extendf_with_size(l, ശൂന്യ, __VA_ARGS__)

പൂർണ്ണസംഖ്യ strv_extend_joined_with_size_sentinel(അക്ഷരം ***l, size_t *n, ...) _sentinel_;
#define strv_extend_joined_with_size(l, n, ...) strv_extend_joined_with_size_sentinel(l, n, __VA_ARGS__, ശൂന്യ)
#define strv_extend_joined(l, ...) strv_extend_joined_with_size(l, ശൂന്യ, __VA_ARGS__)

പൂർണ്ണസംഖ്യ strv_തള്ളുക_with_size(അക്ഷരം ***l, size_t *n, അക്ഷരം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strv_തള്ളുക(അക്ഷരം ***l, അക്ഷരം *മൂല്യം) {
        തിരികെ strv_തള്ളുക_with_size(l, ശൂന്യ, മൂല്യം);
}
പൂർണ്ണസംഖ്യ strv_തള്ളുക_pair(അക്ഷരം ***l, അക്ഷരം *a, അക്ഷരം *b);

പൂർണ്ണസംഖ്യ strv_തിരുകുക(അക്ഷരം ***l, size_t position, അക്ഷരം *മൂല്യം);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strv_തള്ളുക_prepend(അക്ഷരം ***l, അക്ഷരം *മൂല്യം) {
        തിരികെ strv_തിരുകുക(l, 0, മൂല്യം);
}

പൂർണ്ണസംഖ്യ strv_consume_with_size(അക്ഷരം ***l, size_t *n, അക്ഷരം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ strv_consume(അക്ഷരം ***l, അക്ഷരം *മൂല്യം) {
        തിരികെ strv_consume_with_size(l, ശൂന്യ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ strv_consume_pair(അക്ഷരം ***l, അക്ഷരം *a, അക്ഷരം *b);
പൂർണ്ണസംഖ്യ strv_consume_prepend(അക്ഷരം ***l, അക്ഷരം *മൂല്യം);

അക്ഷരം** strv_remove(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *s);
അക്ഷരം** strv_remove_strv(അക്ഷരം **l, അക്ഷരം *സ്ഥിരം*ll);
അക്ഷരം** strv_uniq(അക്ഷരം **l);
bool strv_is_uniq(അക്ഷരം * സ്ഥിരം *l) _pure_;

പൂർണ്ണസംഖ്യ strv_compare(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) _pure_;
സ്ഥിതം വരിയിൽ bool strv_equal(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) {
        തിരികെ strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) _pure_;

അക്ഷരം** strv_new_internal(സ്ഥിരം അക്ഷരം *x, ...) _sentinel_;
അക്ഷരം** strv_new_ap(സ്ഥിരം അക്ഷരം *x, va_list ap);
#define strv_new(...) strv_new_internal(__VA_ARGS__, ശൂന്യ)

#define STRV_IGNORE ((സ്ഥിരം അക്ഷരം *) POINTER_MAX)

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* STRV_IFNOTNULL(സ്ഥിരം അക്ഷരം *x) {
        തിരികെ x ?: STRV_IGNORE;
}

പൂർണ്ണസംഖ്യ strv_split_full(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags);
അക്ഷരം** strv_split(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators);

പൂർണ്ണസംഖ്യ strv_split_and_extend_full(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators, bool filter_duplicates, ExtractFlags flags);
പൂർണ്ണസംഖ്യ strv_split_and_extend(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators, bool filter_duplicates);

പൂർണ്ണസംഖ്യ strv_split_newlines_full(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *s, ExtractFlags flags);
അക്ഷരം** strv_split_newlines(സ്ഥിരം അക്ഷരം *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
പൂർണ്ണസംഖ്യ strv_split_colon_pairs(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s);

അക്ഷരം* strv_join_full(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *separator, സ്ഥിരം അക്ഷരം *prefix);
സ്ഥിതം വരിയിൽ അക്ഷരം *strv_join(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *separator) {
        തിരികെ strv_join_full(l, separator, ശൂന്യ);
}

bool strv_overlap(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) _pure_;

#define _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        വേണ്ടി (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_length(h);           \
                                _len > 0 ? h + _len - 1 : ശൂന്യ;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#define STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#define _STRV_FOREACH_PAIR(x, y, l, i)                          \
        വേണ്ടി (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#define STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

അക്ഷരം** strv_sort(അക്ഷരം **l);
അക്ഷരം** strv_sort_uniq_full(അക്ഷരം **l, പൂർണ്ണസംഖ്യ (*compare)(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b));
അക്ഷരം** strv_sort_uniq(അക്ഷരം **l);
ശൂന്യം strv_print_full(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *prefix);
സ്ഥിതം വരിയിൽ ശൂന്യം strv_print(അക്ഷരം * സ്ഥിരം *l) {
        strv_print_full(l, ശൂന്യ);
}

അക്ഷരം* startswith_strv_internal(സ്ഥിരം അക്ഷരം *s, അക്ഷരം * സ്ഥിരം *l);
#define startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#define STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

അക്ഷരം* endswith_strv_internal(സ്ഥിരം അക്ഷരം *s, അക്ഷരം * സ്ഥിരം *l);
#define endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#define ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#define STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#define STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                സ്ഥിരം അക്ഷരം* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#define STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                സ്ഥിരം അക്ഷരം* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#define _FOREACH_STRING(uniq, x, y, ...)                                \
        വേണ്ടി (സ്ഥിരം അക്ഷരം *x, * സ്ഥിരം*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#define FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

അക്ഷരം** strv_reverse(അക്ഷരം **l);
അക്ഷരം** strv_shell_escape(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *bad);

bool strv_fnmatch_full(അക്ഷരം* സ്ഥിരം* patterns, സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ flags, size_t *ret_matched_pos);
സ്ഥിതം വരിയിൽ bool strv_fnmatch(അക്ഷരം* സ്ഥിരം* patterns, സ്ഥിരം അക്ഷരം *s) {
        തിരികെ strv_fnmatch_full(patterns, s, 0, ശൂന്യ);
}
സ്ഥിതം വരിയിൽ bool strv_fnmatch_or_empty(അക്ഷരം* സ്ഥിരം* patterns, സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ flags) {
        പ്രതിജ്ഞാ(s);
        തിരികെ strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, ശൂന്യ);
}

അക്ഷരം** strv_skip(അക്ഷരം **l, size_t n);

പൂർണ്ണസംഖ്യ strv_extend_n(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം, size_t n);

പൂർണ്ണസംഖ്യ strv_extend_assignment(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *lhs, സ്ഥിരം അക്ഷരം *rhs);

പൂർണ്ണസംഖ്യ fputstrv(FILE *f, അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *separator, bool *space);

#define strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

ശൂന്യം string_strv_hashmap_remove(Hashmap *h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);
ശൂന്യം string_strv_ordered_hashmap_remove(OrderedHashmap *h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);
പൂർണ്ണസംഖ്യ string_strv_hashmap_put(Hashmap **h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);
പൂർണ്ണസംഖ്യ string_strv_ordered_hashmap_put(OrderedHashmap **h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);

പൂർണ്ണസംഖ്യ strv_rebreak_വരികൾ(അക്ഷരം **l, size_t width, അക്ഷരം ***ret);

അക്ഷരം** strv_filter_prefix(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
ബാഹ്യ സ്ഥിരം അക്ഷരം* സ്ഥിരം strv_empty[1];

#define STRV_EMPTY ((അക്ഷരം**) strv_empty)
