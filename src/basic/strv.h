/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/strv.h"   /* IWYU pragma: export */

वर्ण* strv_find(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) विशुद्धम्;
वर्ण* strv_find_case(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) विशुद्धम्;
वर्ण* strv_find_उपसर्गः(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) विशुद्धम्;
वर्ण* strv_find_startswith(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) विशुद्धम्;
वर्ण* strv_find_closest(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) विशुद्धम्;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
वर्ण* strv_find_first_क्षेत्रम्(वर्ण * ध्रुव *needles, वर्ण * ध्रुव *haystack) विशुद्धम्;

#परिभाषय strv_अन्तर्भवति(l, s) (!!strv_find((l), (s)))
#परिभाषय strv_अन्तर्भवति_case(l, s) (!!strv_find_case((l), (s)))

वर्ण** strv_free(वर्ण **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण**, strv_free);
#परिभाषय _cleanup_strv_free_ _cleanup_(strv_freep)

वर्ण** strv_free_erase(वर्ण **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण**, strv_free_erase);
#परिभाषय _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

शून्य strv_free_many(वर्ण ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

वर्ण** strv_copy_n(वर्ण * ध्रुव *l, size_t n);
स्थिर अन्तःस्थ वर्ण** strv_copy(वर्ण * ध्रुव *l) {
        प्रत्यागम strv_copy_n(l, SIZE_MAX);
}
पूर्णाङ्क strv_copy_unless_रिक्तानि(वर्ण * ध्रुव *l, वर्ण ***ret);

size_t strv_दीर्घता(वर्ण * ध्रुव *l) विशुद्धम्;

पूर्णाङ्क strv_extend_strv(वर्ण ***a, वर्ण * ध्रुव *b, bool filter_duplicates);
पूर्णाङ्क strv_extend_strv_consume(वर्ण ***a, वर्ण **b, bool filter_duplicates);

पूर्णाङ्क strv_extend_strv_biconcat(वर्ण ***a, ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण* ध्रुव *b, ध्रुव वर्ण *suffix);
स्थिर अन्तःस्थ पूर्णाङ्क strv_extend_strv_concat(वर्ण ***a, ध्रुव वर्ण* ध्रुव *b, ध्रुव वर्ण *suffix) {
        प्रत्यागम strv_extend_strv_biconcat(a, रिक्त, b, suffix);
}

पूर्णाङ्क strv_prepend(वर्ण ***l, ध्रुव वर्ण *मानम्);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

पूर्णाङ्क strv_extend_with_आकार(वर्ण ***l, size_t *n, ध्रुव वर्ण *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क strv_extend(वर्ण ***l, ध्रुव वर्ण *मानम्) {
        प्रत्यागम strv_extend_with_आकार(l, रिक्त, मानम्);
}

पूर्णाङ्क strv_extend_many_आन्तरिकम्(वर्ण ***l, ध्रुव वर्ण *मानम्, ...);
#परिभाषय strv_extend_many(l, ...) strv_extend_many_आन्तरिकम्(l, __VA_ARGS__, POINTER_MAX)

पूर्णाङ्क strv_extendf_with_आकार(वर्ण ***l, size_t *n, ध्रुव वर्ण *format, ...) _printf_(3,4);
#परिभाषय strv_extendf(l, ...) strv_extendf_with_आकार(l, रिक्त, __VA_ARGS__)

पूर्णाङ्क strv_extend_joined_with_आकार_sentinel(वर्ण ***l, size_t *n, ...) _sentinel_;
#परिभाषय strv_extend_joined_with_आकार(l, n, ...) strv_extend_joined_with_आकार_sentinel(l, n, __VA_ARGS__, रिक्त)
#परिभाषय strv_extend_joined(l, ...) strv_extend_joined_with_आकार(l, रिक्त, __VA_ARGS__)

पूर्णाङ्क strv_प्रक्षिप_with_आकार(वर्ण ***l, size_t *n, वर्ण *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क strv_प्रक्षिप(वर्ण ***l, वर्ण *मानम्) {
        प्रत्यागम strv_प्रक्षिप_with_आकार(l, रिक्त, मानम्);
}
पूर्णाङ्क strv_प्रक्षिप_pair(वर्ण ***l, वर्ण *a, वर्ण *b);

पूर्णाङ्क strv_निवेशय(वर्ण ***l, size_t position, वर्ण *मानम्);

स्थिर अन्तःस्थ पूर्णाङ्क strv_प्रक्षिप_prepend(वर्ण ***l, वर्ण *मानम्) {
        प्रत्यागम strv_निवेशय(l, 0, मानम्);
}

पूर्णाङ्क strv_consume_with_आकार(वर्ण ***l, size_t *n, वर्ण *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क strv_consume(वर्ण ***l, वर्ण *मानम्) {
        प्रत्यागम strv_consume_with_आकार(l, रिक्त, मानम्);
}

पूर्णाङ्क strv_consume_pair(वर्ण ***l, वर्ण *a, वर्ण *b);
पूर्णाङ्क strv_consume_prepend(वर्ण ***l, वर्ण *मानम्);

वर्ण** strv_remove(वर्ण **l, ध्रुव वर्ण *s);
वर्ण** strv_remove_strv(वर्ण **l, वर्ण *ध्रुव*ll);
वर्ण** strv_uniq(वर्ण **l);
bool strv_is_uniq(वर्ण * ध्रुव *l) विशुद्धम्;

पूर्णाङ्क strv_compare(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) विशुद्धम्;
स्थिर अन्तःस्थ bool strv_equal(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) {
        प्रत्यागम strv_compare(a, b) == 0;
}

bool strv_equal_ignore_क्रमः(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) विशुद्धम्;

वर्ण** strv_new_आन्तरिकम्(ध्रुव वर्ण *x, ...) _sentinel_;
वर्ण** strv_new_ap(ध्रुव वर्ण *x, va_list ap);
#परिभाषय strv_new(...) strv_new_आन्तरिकम्(__VA_ARGS__, रिक्त)

#परिभाषय STRV_IGNORE ((ध्रुव वर्ण *) POINTER_MAX)

स्थिर अन्तःस्थ ध्रुव वर्ण* STRV_IFNOTNULL(ध्रुव वर्ण *x) {
        प्रत्यागम x ?: STRV_IGNORE;
}

पूर्णाङ्क strv_विभज_पूर्णम्(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, ExtractFlags flags);
वर्ण** strv_विभज(ध्रुव वर्ण *s, ध्रुव वर्ण *separators);

पूर्णाङ्क strv_विभज_and_extend_पूर्णम्(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, bool filter_duplicates, ExtractFlags flags);
पूर्णाङ्क strv_विभज_and_extend(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, bool filter_duplicates);

पूर्णाङ्क strv_विभज_newlines_पूर्णम्(वर्ण ***ret, ध्रुव वर्ण *s, ExtractFlags flags);
वर्ण** strv_विभज_newlines(ध्रुव वर्ण *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
पूर्णाङ्क strv_विभज_colon_युग्मानि(वर्ण ***t, ध्रुव वर्ण *s);

वर्ण* strv_join_पूर्णम्(वर्ण * ध्रुव *l, ध्रुव वर्ण *separator, ध्रुव वर्ण *उपसर्गः);
स्थिर अन्तःस्थ वर्ण *strv_join(वर्ण * ध्रुव *l, ध्रुव वर्ण *separator) {
        प्रत्यागम strv_join_पूर्णम्(l, separator, रिक्त);
}

bool strv_overlap(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) विशुद्धम्;

#परिभाषय _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        प्रति (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_दीर्घता(h);           \
                                _len > 0 ? h + _len - 1 : रिक्त;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#परिभाषय STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#परिभाषय _STRV_FOREACH_PAIR(x, y, l, i)                          \
        प्रति (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#परिभाषय STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

वर्ण** strv_क्रमय(वर्ण **l);
वर्ण** strv_क्रमय_uniq_पूर्णम्(वर्ण **l, पूर्णाङ्क (*compare)(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b));
वर्ण** strv_क्रमय_uniq(वर्ण **l);
शून्य strv_print_पूर्णम्(वर्ण * ध्रुव *l, ध्रुव वर्ण *उपसर्गः);
स्थिर अन्तःस्थ शून्य strv_print(वर्ण * ध्रुव *l) {
        strv_print_पूर्णम्(l, रिक्त);
}

वर्ण* startswith_strv_आन्तरिकम्(ध्रुव वर्ण *s, वर्ण * ध्रुव *l);
#परिभाषय startswith_strv(s, l) const_generic(s, startswith_strv_आन्तरिकम्(s, l))

#परिभाषय STARTSWITH_समुच्चयः(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

वर्ण* endswith_strv_आन्तरिकम्(ध्रुव वर्ण *s, वर्ण * ध्रुव *l);
#परिभाषय endswith_strv(s, l) const_generic(s, endswith_strv_आन्तरिकम्(s, l))

#परिभाषय ENDSWITH_समुच्चयः(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#परिभाषय STR_अन्तर्_समुच्चयः(x, ...) strv_अन्तर्भवति(STRV_MAKE(__VA_ARGS__), x)
#परिभाषय STRPTR_अन्तर्_समुच्चयः(x, ...)                                    \
        ({                                                       \
                ध्रुव वर्ण* _x = (x);                            \
                _x && strv_अन्तर्भवति(STRV_MAKE(__VA_ARGS__), _x); \
        })

#परिभाषय STRCASE_अन्तर्_समुच्चयः(x, ...) strv_अन्तर्भवति_case(STRV_MAKE(__VA_ARGS__), x)
#परिभाषय STRCASEPTR_अन्तर्_समुच्चयः(x, ...)                                    \
        ({                                                       \
                ध्रुव वर्ण* _x = (x);                            \
                _x && strv_अन्तर्भवति_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#परिभाषय _FOREACH_STRING(uniq, x, y, ...)                                \
        प्रति (ध्रुव वर्ण *x, * ध्रुव*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#परिभाषय FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

वर्ण** strv_reverse(वर्ण **l);
वर्ण** strv_shell_escape(वर्ण **l, ध्रुव वर्ण *bad);

bool strv_fnmatch_पूर्णम्(वर्ण* ध्रुव* patterns, ध्रुव वर्ण *s, पूर्णाङ्क flags, size_t *ret_matched_pos);
स्थिर अन्तःस्थ bool strv_fnmatch(वर्ण* ध्रुव* patterns, ध्रुव वर्ण *s) {
        प्रत्यागम strv_fnmatch_पूर्णम्(patterns, s, 0, रिक्त);
}
स्थिर अन्तःस्थ bool strv_fnmatch_or_रिक्तानि(वर्ण* ध्रुव* patterns, ध्रुव वर्ण *s, पूर्णाङ्क flags) {
        प्रतिज्ञा(s);
        प्रत्यागम strv_isempty(patterns) ||
               strv_fnmatch_पूर्णम्(patterns, s, flags, रिक्त);
}

वर्ण** strv_skip(वर्ण **l, size_t n);

पूर्णाङ्क strv_extend_n(वर्ण ***l, ध्रुव वर्ण *मानम्, size_t n);

पूर्णाङ्क strv_extend_assignment(वर्ण ***l, ध्रुव वर्ण *lhs, ध्रुव वर्ण *rhs);

पूर्णाङ्क fputstrv(FILE *f, वर्ण * ध्रुव *l, ध्रुव वर्ण *separator, bool *space);

#परिभाषय strv_free_and_replace(a, b)             \
        free_and_replace_पूर्णम्(a, b, strv_free)

शून्य string_strv_hashmap_remove(Hashmap *h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);
शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);
पूर्णाङ्क string_strv_hashmap_put(Hashmap **h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);
पूर्णाङ्क string_strv_ordered_hashmap_put(OrderedHashmap **h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);

पूर्णाङ्क strv_rebreak_पङ्क्तयः(वर्ण **l, size_t width, वर्ण ***ret);

वर्ण** strv_filter_उपसर्गः(वर्ण * ध्रुव *l, ध्रुव वर्ण *उपसर्गः);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
बाह्य ध्रुव वर्ण* ध्रुव strv_रिक्तानि[1];

#परिभाषय STRV_रिक्तानि ((वर्ण**) strv_रिक्तानि)
