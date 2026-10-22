/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#అన్తర్భావ "../fundamental/strv.h"   /* IWYU pragma: export */

అక్షరం* strv_కనుగొను(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) శుద్ధ;
అక్షరం* strv_కనుగొను_case(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) శుద్ధ;
అక్షరం* strv_కనుగొను_ఉపసర్గ(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) శుద్ధ;
అక్షరం* strv_కనుగొను_startswith(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) శుద్ధ;
అక్షరం* strv_కనుగొను_closest(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) శుద్ధ;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
అక్షరం* strv_కనుగొను_మొదటి_క్షేత్రం(అక్షరం * స్థిరరాశి *needles, అక్షరం * స్థిరరాశి *haystack) శుద్ధ;

#నిర్వచించు strv_కలిగియుంది(l, s) (!!strv_కనుగొను((l), (s)))
#నిర్వచించు strv_కలిగియుంది_case(l, s) (!!strv_కనుగొను_case((l), (s)))

అక్షరం** strv_free(అక్షరం **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(అక్షరం**, strv_free);
#నిర్వచించు _cleanup_strv_free_ _cleanup_(strv_freep)

అక్షరం** strv_free_erase(అక్షరం **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(అక్షరం**, strv_free_erase);
#నిర్వచించు _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

శూన్య strv_free_many(అక్షరం ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

అక్షరం** strv_copy_n(అక్షరం * స్థిరరాశి *l, size_t n);
స్థిర అంతఃవరుస అక్షరం** strv_copy(అక్షరం * స్థిరరాశి *l) {
        తిరిగి strv_copy_n(l, SIZE_MAX);
}
పూర్ణాంకం strv_copy_unless_ఖాళీలు(అక్షరం * స్థిరరాశి *l, అక్షరం ***ret);

size_t strv_నిడివి(అక్షరం * స్థిరరాశి *l) శుద్ధ;

పూర్ణాంకం strv_extend_strv(అక్షరం ***a, అక్షరం * స్థిరరాశి *b, bool filter_duplicates);
పూర్ణాంకం strv_extend_strv_consume(అక్షరం ***a, అక్షరం **b, bool filter_duplicates);

పూర్ణాంకం strv_extend_strv_biconcat(అక్షరం ***a, స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం* స్థిరరాశి *b, స్థిరరాశి అక్షరం *suffix);
స్థిర అంతఃవరుస పూర్ణాంకం strv_extend_strv_concat(అక్షరం ***a, స్థిరరాశి అక్షరం* స్థిరరాశి *b, స్థిరరాశి అక్షరం *suffix) {
        తిరిగి strv_extend_strv_biconcat(a, ఖాళీ, b, suffix);
}

పూర్ణాంకం strv_prepend(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

పూర్ణాంకం strv_extend_with_కొలత(అక్షరం ***l, size_t *n, స్థిరరాశి అక్షరం *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం strv_extend(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ) {
        తిరిగి strv_extend_with_కొలత(l, ఖాళీ, విలువ);
}

పూర్ణాంకం strv_extend_many_అంతర్గత(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ, ...);
#నిర్వచించు strv_extend_many(l, ...) strv_extend_many_అంతర్గత(l, __VA_ARGS__, సూచిక_MAX)

పూర్ణాంకం strv_extendf_with_కొలత(అక్షరం ***l, size_t *n, స్థిరరాశి అక్షరం *format, ...) _printf_(3,4);
#నిర్వచించు strv_extendf(l, ...) strv_extendf_with_కొలత(l, ఖాళీ, __VA_ARGS__)

పూర్ణాంకం strv_extend_joined_with_కొలత_sentinel(అక్షరం ***l, size_t *n, ...) _sentinel_;
#నిర్వచించు strv_extend_joined_with_కొలత(l, n, ...) strv_extend_joined_with_కొలత_sentinel(l, n, __VA_ARGS__, ఖాళీ)
#నిర్వచించు strv_extend_joined(l, ...) strv_extend_joined_with_కొలత(l, ఖాళీ, __VA_ARGS__)

పూర్ణాంకం strv_నెట్టు_with_కొలత(అక్షరం ***l, size_t *n, అక్షరం *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం strv_నెట్టు(అక్షరం ***l, అక్షరం *విలువ) {
        తిరిగి strv_నెట్టు_with_కొలత(l, ఖాళీ, విలువ);
}
పూర్ణాంకం strv_నెట్టు_pair(అక్షరం ***l, అక్షరం *a, అక్షరం *b);

పూర్ణాంకం strv_చొప్పించు(అక్షరం ***l, size_t position, అక్షరం *విలువ);

స్థిర అంతఃవరుస పూర్ణాంకం strv_నెట్టు_prepend(అక్షరం ***l, అక్షరం *విలువ) {
        తిరిగి strv_చొప్పించు(l, 0, విలువ);
}

పూర్ణాంకం strv_consume_with_కొలత(అక్షరం ***l, size_t *n, అక్షరం *విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం strv_consume(అక్షరం ***l, అక్షరం *విలువ) {
        తిరిగి strv_consume_with_కొలత(l, ఖాళీ, విలువ);
}

పూర్ణాంకం strv_consume_pair(అక్షరం ***l, అక్షరం *a, అక్షరం *b);
పూర్ణాంకం strv_consume_prepend(అక్షరం ***l, అక్షరం *విలువ);

అక్షరం** strv_remove(అక్షరం **l, స్థిరరాశి అక్షరం *s);
అక్షరం** strv_remove_strv(అక్షరం **l, అక్షరం *స్థిరరాశి*ll);
అక్షరం** strv_uniq(అక్షరం **l);
bool strv_is_uniq(అక్షరం * స్థిరరాశి *l) శుద్ధ;

పూర్ణాంకం strv_compare(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) శుద్ధ;
స్థిర అంతఃవరుస bool strv_equal(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) {
        తిరిగి strv_compare(a, b) == 0;
}

bool strv_equal_ignore_క్రమం(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) శుద్ధ;

అక్షరం** strv_new_అంతర్గత(స్థిరరాశి అక్షరం *x, ...) _sentinel_;
అక్షరం** strv_new_ap(స్థిరరాశి అక్షరం *x, va_list ap);
#నిర్వచించు strv_new(...) strv_new_అంతర్గత(__VA_ARGS__, ఖాళీ)

#నిర్వచించు STRV_IGNORE ((స్థిరరాశి అక్షరం *) సూచిక_MAX)

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* STRV_IFNOTNULL(స్థిరరాశి అక్షరం *x) {
        తిరిగి x ?: STRV_IGNORE;
}

పూర్ణాంకం strv_విభజించు_నిండిన(అక్షరం ***t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags);
అక్షరం** strv_విభజించు(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators);

పూర్ణాంకం strv_విభజించు_and_extend_నిండిన(అక్షరం ***t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators, bool filter_duplicates, వెలికితీయి_Flags flags);
పూర్ణాంకం strv_విభజించు_and_extend(అక్షరం ***t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators, bool filter_duplicates);

పూర్ణాంకం strv_విభజించు_newlines_నిండిన(అక్షరం ***ret, స్థిరరాశి అక్షరం *s, వెలికితీయి_Flags flags);
అక్షరం** strv_విభజించు_newlines(స్థిరరాశి అక్షరం *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
పూర్ణాంకం strv_విభజించు_colon_జతలు(అక్షరం ***t, స్థిరరాశి అక్షరం *s);

అక్షరం* strv_join_నిండిన(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *separator, స్థిరరాశి అక్షరం *ఉపసర్గ);
స్థిర అంతఃవరుస అక్షరం *strv_join(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *separator) {
        తిరిగి strv_join_నిండిన(l, separator, ఖాళీ);
}

bool strv_overlap(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) శుద్ధ;

#నిర్వచించు _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        కొరకు (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_నిడివి(h);           \
                                _len > 0 ? h + _len - 1 : ఖాళీ;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#నిర్వచించు STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#నిర్వచించు _STRV_FOREACH_PAIR(x, y, l, i)                          \
        కొరకు (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#నిర్వచించు STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

అక్షరం** strv_క్రమీకరించు(అక్షరం **l);
అక్షరం** strv_క్రమీకరించు_uniq_నిండిన(అక్షరం **l, పూర్ణాంకం (*compare)(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b));
అక్షరం** strv_క్రమీకరించు_uniq(అక్షరం **l);
శూన్య strv_print_నిండిన(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *ఉపసర్గ);
స్థిర అంతఃవరుస శూన్య strv_print(అక్షరం * స్థిరరాశి *l) {
        strv_print_నిండిన(l, ఖాళీ);
}

అక్షరం* startswith_strv_అంతర్గత(స్థిరరాశి అక్షరం *s, అక్షరం * స్థిరరాశి *l);
#నిర్వచించు startswith_strv(s, l) const_generic(s, startswith_strv_అంతర్గత(s, l))

#నిర్వచించు STARTSWITH_సమితి(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

అక్షరం* endswith_strv_అంతర్గత(స్థిరరాశి అక్షరం *s, అక్షరం * స్థిరరాశి *l);
#నిర్వచించు endswith_strv(s, l) const_generic(s, endswith_strv_అంతర్గత(s, l))

#నిర్వచించు ENDSWITH_సమితి(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#నిర్వచించు STR_లోపల_సమితి(x, ...) strv_కలిగియుంది(STRV_MAKE(__VA_ARGS__), x)
#నిర్వచించు STRPTR_లోపల_సమితి(x, ...)                                    \
        ({                                                       \
                స్థిరరాశి అక్షరం* _x = (x);                            \
                _x && strv_కలిగియుంది(STRV_MAKE(__VA_ARGS__), _x); \
        })

#నిర్వచించు STRCASE_లోపల_సమితి(x, ...) strv_కలిగియుంది_case(STRV_MAKE(__VA_ARGS__), x)
#నిర్వచించు STRCASEPTR_లోపల_సమితి(x, ...)                                    \
        ({                                                       \
                స్థిరరాశి అక్షరం* _x = (x);                            \
                _x && strv_కలిగియుంది_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#నిర్వచించు _FOREACH_STRING(uniq, x, y, ...)                                \
        కొరకు (స్థిరరాశి అక్షరం *x, * స్థిరరాశి*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#నిర్వచించు FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

అక్షరం** strv_తిరగేయి(అక్షరం **l);
అక్షరం** strv_shell_escape(అక్షరం **l, స్థిరరాశి అక్షరం *bad);

bool strv_fnmatch_నిండిన(అక్షరం* స్థిరరాశి* patterns, స్థిరరాశి అక్షరం *s, పూర్ణాంకం flags, size_t *ret_matched_pos);
స్థిర అంతఃవరుస bool strv_fnmatch(అక్షరం* స్థిరరాశి* patterns, స్థిరరాశి అక్షరం *s) {
        తిరిగి strv_fnmatch_నిండిన(patterns, s, 0, ఖాళీ);
}
స్థిర అంతఃవరుస bool strv_fnmatch_or_ఖాళీలు(అక్షరం* స్థిరరాశి* patterns, స్థిరరాశి అక్షరం *s, పూర్ణాంకం flags) {
        ప్రతిజ్ఞా(s);
        తిరిగి strv_isempty(patterns) ||
               strv_fnmatch_నిండిన(patterns, s, flags, ఖాళీ);
}

అక్షరం** strv_skip(అక్షరం **l, size_t n);

పూర్ణాంకం strv_extend_n(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ, size_t n);

పూర్ణాంకం strv_extend_assignment(అక్షరం ***l, స్థిరరాశి అక్షరం *lhs, స్థిరరాశి అక్షరం *rhs);

పూర్ణాంకం fputstrv(FILE *f, అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *separator, bool *space);

#నిర్వచించు strv_free_and_replace(a, b)             \
        free_and_replace_నిండిన(a, b, strv_free)

శూన్య string_strv_hashmap_remove(Hashmap *h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);
శూన్య string_strv_ordered_hashmap_remove(OrderedHashmap *h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);
పూర్ణాంకం string_strv_hashmap_put(Hashmap **h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);
పూర్ణాంకం string_strv_ordered_hashmap_put(OrderedHashmap **h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);

పూర్ణాంకం strv_rebreak_పంక్తులు(అక్షరం **l, size_t width, అక్షరం ***ret);

అక్షరం** strv_filter_ఉపసర్గ(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *ఉపసర్గ);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
బాహ్య స్థిరరాశి అక్షరం* స్థిరరాశి strv_ఖాళీలు[1];

#నిర్వచించు STRV_ఖాళీలు ((అక్షరం**) strv_ఖాళీలు)
