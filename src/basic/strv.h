/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#समावेश "../fundamental/strv.h"   /* IWYU pragma: export */

अक्षर* strv_find(अक्षर * स्थिर *l, स्थिर अक्षर *name) _pure_;
अक्षर* strv_find_case(अक्षर * स्थिर *l, स्थिर अक्षर *name) _pure_;
अक्षर* strv_find_prefix(अक्षर * स्थिर *l, स्थिर अक्षर *name) _pure_;
अक्षर* strv_find_startswith(अक्षर * स्थिर *l, स्थिर अक्षर *name) _pure_;
अक्षर* strv_find_closest(अक्षर * स्थिर *l, स्थिर अक्षर *name) _pure_;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
अक्षर* strv_find_first_field(अक्षर * स्थिर *needles, अक्षर * स्थिर *haystack) _pure_;

#घोषणा strv_contains(l, s) (!!strv_find((l), (s)))
#घोषणा strv_contains_case(l, s) (!!strv_find_case((l), (s)))

अक्षर** strv_free(अक्षर **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(अक्षर**, strv_free);
#घोषणा _cleanup_strv_free_ _cleanup_(strv_freep)

अक्षर** strv_free_erase(अक्षर **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(अक्षर**, strv_free_erase);
#घोषणा _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

व्योम strv_free_many(अक्षर ***strvs, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);

अक्षर** strv_copy_n(अक्षर * स्थिर *l, माप_प्रकार n);
अटल अंतरभूत अक्षर** strv_copy(अक्षर * स्थिर *l) {
        वापस strv_copy_n(l, SIZE_MAX);
}
पूर्णांक strv_copy_unless_empty(अक्षर * स्थिर *l, अक्षर ***ret);

माप_प्रकार strv_लंबाई(अक्षर * स्थिर *l) _pure_;

पूर्णांक strv_extend_strv(अक्षर ***a, अक्षर * स्थिर *b, bool filter_duplicates);
पूर्णांक strv_extend_strv_consume(अक्षर ***a, अक्षर **b, bool filter_duplicates);

पूर्णांक strv_extend_strv_biconcat(अक्षर ***a, स्थिर अक्षर *prefix, स्थिर अक्षर* स्थिर *b, स्थिर अक्षर *suffix);
अटल अंतरभूत पूर्णांक strv_extend_strv_concat(अक्षर ***a, स्थिर अक्षर* स्थिर *b, स्थिर अक्षर *suffix) {
        वापस strv_extend_strv_biconcat(a, शून्य, b, suffix);
}

पूर्णांक strv_prepend(अक्षर ***l, स्थिर अक्षर *मूल्य);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

पूर्णांक strv_extend_with_आकार(अक्षर ***l, माप_प्रकार *n, स्थिर अक्षर *मूल्य);
अटल अंतरभूत पूर्णांक strv_extend(अक्षर ***l, स्थिर अक्षर *मूल्य) {
        वापस strv_extend_with_आकार(l, शून्य, मूल्य);
}

पूर्णांक strv_extend_many_internal(अक्षर ***l, स्थिर अक्षर *मूल्य, ...);
#घोषणा strv_extend_many(l, ...) strv_extend_many_internal(l, __VA_ARGS__, POINTER_MAX)

पूर्णांक strv_extendf_with_आकार(अक्षर ***l, माप_प्रकार *n, स्थिर अक्षर *format, ...) _printf_(3,4);
#घोषणा strv_extendf(l, ...) strv_extendf_with_आकार(l, शून्य, __VA_ARGS__)

पूर्णांक strv_extend_joined_with_आकार_sentinel(अक्षर ***l, माप_प्रकार *n, ...) _sentinel_;
#घोषणा strv_extend_joined_with_आकार(l, n, ...) strv_extend_joined_with_आकार_sentinel(l, n, __VA_ARGS__, शून्य)
#घोषणा strv_extend_joined(l, ...) strv_extend_joined_with_आकार(l, शून्य, __VA_ARGS__)

पूर्णांक strv_धकेलो_with_आकार(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य);
अटल अंतरभूत पूर्णांक strv_धकेलो(अक्षर ***l, अक्षर *मूल्य) {
        वापस strv_धकेलो_with_आकार(l, शून्य, मूल्य);
}
पूर्णांक strv_धकेलो_pair(अक्षर ***l, अक्षर *a, अक्षर *b);

पूर्णांक strv_जोड़ो(अक्षर ***l, माप_प्रकार position, अक्षर *मूल्य);

अटल अंतरभूत पूर्णांक strv_धकेलो_prepend(अक्षर ***l, अक्षर *मूल्य) {
        वापस strv_जोड़ो(l, 0, मूल्य);
}

पूर्णांक strv_consume_with_आकार(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य);
अटल अंतरभूत पूर्णांक strv_consume(अक्षर ***l, अक्षर *मूल्य) {
        वापस strv_consume_with_आकार(l, शून्य, मूल्य);
}

पूर्णांक strv_consume_pair(अक्षर ***l, अक्षर *a, अक्षर *b);
पूर्णांक strv_consume_prepend(अक्षर ***l, अक्षर *मूल्य);

अक्षर** strv_remove(अक्षर **l, स्थिर अक्षर *s);
अक्षर** strv_remove_strv(अक्षर **l, अक्षर *स्थिर*ll);
अक्षर** strv_uniq(अक्षर **l);
bool strv_is_uniq(अक्षर * स्थिर *l) _pure_;

पूर्णांक strv_compare(अक्षर * स्थिर *a, अक्षर * स्थिर *b) _pure_;
अटल अंतरभूत bool strv_equal(अक्षर * स्थिर *a, अक्षर * स्थिर *b) {
        वापस strv_compare(a, b) == 0;
}

bool strv_equal_ignore_order(अक्षर * स्थिर *a, अक्षर * स्थिर *b) _pure_;

अक्षर** strv_new_internal(स्थिर अक्षर *x, ...) _sentinel_;
अक्षर** strv_new_ap(स्थिर अक्षर *x, बहु_सूची ap);
#घोषणा strv_new(...) strv_new_internal(__VA_ARGS__, शून्य)

#घोषणा STRV_IGNORE ((स्थिर अक्षर *) POINTER_MAX)

अटल अंतरभूत स्थिर अक्षर* STRV_IFNOTNULL(स्थिर अक्षर *x) {
        वापस x ?: STRV_IGNORE;
}

पूर्णांक strv_split_full(अक्षर ***t, स्थिर अक्षर *s, स्थिर अक्षर *separators, ExtractFlags flags);
अक्षर** strv_split(स्थिर अक्षर *s, स्थिर अक्षर *separators);

पूर्णांक strv_split_and_extend_full(अक्षर ***t, स्थिर अक्षर *s, स्थिर अक्षर *separators, bool filter_duplicates, ExtractFlags flags);
पूर्णांक strv_split_and_extend(अक्षर ***t, स्थिर अक्षर *s, स्थिर अक्षर *separators, bool filter_duplicates);

पूर्णांक strv_split_newlines_full(अक्षर ***ret, स्थिर अक्षर *s, ExtractFlags flags);
अक्षर** strv_split_newlines(स्थिर अक्षर *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
पूर्णांक strv_split_colon_pairs(अक्षर ***t, स्थिर अक्षर *s);

अक्षर* strv_join_full(अक्षर * स्थिर *l, स्थिर अक्षर *separator, स्थिर अक्षर *prefix);
अटल अंतरभूत अक्षर *strv_join(अक्षर * स्थिर *l, स्थिर अक्षर *separator) {
        वापस strv_join_full(l, separator, शून्य);
}

bool strv_overlap(अक्षर * स्थिर *a, अक्षर * स्थिर *b) _pure_;

#घोषणा _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        क्रम (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                माप_प्रकार _len = strv_लंबाई(h);           \
                                _len > 0 ? h + _len - 1 : शून्य;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#घोषणा STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#घोषणा _STRV_FOREACH_PAIR(x, y, l, i)                          \
        क्रम (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#घोषणा STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

अक्षर** strv_sort(अक्षर **l);
अक्षर** strv_sort_uniq_full(अक्षर **l, पूर्णांक (*compare)(अक्षर * स्थिर *a, अक्षर * स्थिर *b));
अक्षर** strv_sort_uniq(अक्षर **l);
व्योम strv_print_full(अक्षर * स्थिर *l, स्थिर अक्षर *prefix);
अटल अंतरभूत व्योम strv_print(अक्षर * स्थिर *l) {
        strv_print_full(l, शून्य);
}

अक्षर* startswith_strv_internal(स्थिर अक्षर *s, अक्षर * स्थिर *l);
#घोषणा startswith_strv(s, l) const_generic(s, startswith_strv_internal(s, l))

#घोषणा STARTSWITH_SET(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

अक्षर* endswith_strv_internal(स्थिर अक्षर *s, अक्षर * स्थिर *l);
#घोषणा endswith_strv(s, l) const_generic(s, endswith_strv_internal(s, l))

#घोषणा ENDSWITH_SET(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#घोषणा STR_IN_SET(x, ...) strv_contains(STRV_MAKE(__VA_ARGS__), x)
#घोषणा STRPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                स्थिर अक्षर* _x = (x);                            \
                _x && strv_contains(STRV_MAKE(__VA_ARGS__), _x); \
        })

#घोषणा STRCASE_IN_SET(x, ...) strv_contains_case(STRV_MAKE(__VA_ARGS__), x)
#घोषणा STRCASEPTR_IN_SET(x, ...)                                    \
        ({                                                       \
                स्थिर अक्षर* _x = (x);                            \
                _x && strv_contains_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#घोषणा _FOREACH_STRING(uniq, x, y, ...)                                \
        क्रम (स्थिर अक्षर *x, * स्थिर*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#घोषणा FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

अक्षर** strv_reverse(अक्षर **l);
अक्षर** strv_shell_escape(अक्षर **l, स्थिर अक्षर *bad);

bool strv_fnmatch_full(अक्षर* स्थिर* patterns, स्थिर अक्षर *s, पूर्णांक flags, माप_प्रकार *ret_matched_pos);
अटल अंतरभूत bool strv_fnmatch(अक्षर* स्थिर* patterns, स्थिर अक्षर *s) {
        वापस strv_fnmatch_full(patterns, s, 0, शून्य);
}
अटल अंतरभूत bool strv_fnmatch_or_empty(अक्षर* स्थिर* patterns, स्थिर अक्षर *s, पूर्णांक flags) {
        निश्चित(s);
        वापस strv_isempty(patterns) ||
               strv_fnmatch_full(patterns, s, flags, शून्य);
}

अक्षर** strv_skip(अक्षर **l, माप_प्रकार n);

पूर्णांक strv_extend_n(अक्षर ***l, स्थिर अक्षर *मूल्य, माप_प्रकार n);

पूर्णांक strv_extend_assignment(अक्षर ***l, स्थिर अक्षर *lhs, स्थिर अक्षर *rhs);

पूर्णांक fputstrv(खाता *f, अक्षर * स्थिर *l, स्थिर अक्षर *separator, bool *space);

#घोषणा strv_free_and_replace(a, b)             \
        free_and_replace_full(a, b, strv_free)

व्योम string_strv_hashmap_remove(Hashmap *h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);
व्योम string_strv_ordered_hashmap_remove(OrderedHashmap *h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);
पूर्णांक string_strv_hashmap_put(Hashmap **h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);
पूर्णांक string_strv_ordered_hashmap_put(OrderedHashmap **h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);

पूर्णांक strv_rebreak_पंक्तियाँ(अक्षर **l, माप_प्रकार width, अक्षर ***ret);

अक्षर** strv_filter_prefix(अक्षर * स्थिर *l, स्थिर अक्षर *prefix);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
बाह्य स्थिर अक्षर* स्थिर strv_empty[1];

#घोषणा STRV_EMPTY ((अक्षर**) strv_empty)
