/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#ضمن "../fundamental/strv.h"   /* IWYU pragma: export */

حرف* strv_ڳوليو(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڳوليو_case(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڳوليو_اڳياڙي(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڳوليو_startswith(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڳوليو_closest(حرف * ثابت *l, ثابت حرف *name) خالص;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
حرف* strv_ڳوليو_پهريون_ميدان(حرف * ثابت *needles, حرف * ثابت *haystack) خالص;

#وصف_ڪريو strv_شامل_آهي(l, s) (!!strv_ڳوليو((l), (s)))
#وصف_ڪريو strv_شامل_آهي_case(l, s) (!!strv_ڳوليو_case((l), (s)))

حرف** strv_free(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free);
#وصف_ڪريو _cleanup_strv_free_ _cleanup_(strv_freep)

حرف** strv_free_erase(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free_erase);
#وصف_ڪريو _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

فراغ strv_free_many(حرف ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

حرف** strv_copy_n(حرف * ثابت *l, size_t n);
ساكن مضمن حرف** strv_copy(حرف * ثابت *l) {
        أرجع strv_copy_n(l, SIZE_MAX);
}
صحيح strv_copy_unless_خالي_جايون(حرف * ثابت *l, حرف ***ret);

size_t strv_ڊيگهائي(حرف * ثابت *l) خالص;

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates);
صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates);

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *اڳياڙي, ثابت حرف* ثابت *b, ثابت حرف *suffix);
ساكن مضمن صحيح strv_extend_strv_concat(حرف ***a, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        أرجع strv_extend_strv_biconcat(a, عدم, b, suffix);
}

صحيح strv_prepend(حرف ***l, ثابت حرف *قدر);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

صحيح strv_extend_with_سائيز(حرف ***l, size_t *n, ثابت حرف *قدر);
ساكن مضمن صحيح strv_extend(حرف ***l, ثابت حرف *قدر) {
        أرجع strv_extend_with_سائيز(l, عدم, قدر);
}

صحيح strv_extend_many_اندروني(حرف ***l, ثابت حرف *قدر, ...);
#وصف_ڪريو strv_extend_many(l, ...) strv_extend_many_اندروني(l, __VA_ARGS__, اشارو_MAX)

صحيح strv_extendf_with_سائيز(حرف ***l, size_t *n, ثابت حرف *format, ...) _printf_(3,4);
#وصف_ڪريو strv_extendf(l, ...) strv_extendf_with_سائيز(l, عدم, __VA_ARGS__)

صحيح strv_extend_joined_with_سائيز_sentinel(حرف ***l, size_t *n, ...) _sentinel_;
#وصف_ڪريو strv_extend_joined_with_سائيز(l, n, ...) strv_extend_joined_with_سائيز_sentinel(l, n, __VA_ARGS__, عدم)
#وصف_ڪريو strv_extend_joined(l, ...) strv_extend_joined_with_سائيز(l, عدم, __VA_ARGS__)

صحيح strv_ڌڪو_with_سائيز(حرف ***l, size_t *n, حرف *قدر);
ساكن مضمن صحيح strv_ڌڪو(حرف ***l, حرف *قدر) {
        أرجع strv_ڌڪو_with_سائيز(l, عدم, قدر);
}
صحيح strv_ڌڪو_pair(حرف ***l, حرف *a, حرف *b);

صحيح strv_وجهو(حرف ***l, size_t position, حرف *قدر);

ساكن مضمن صحيح strv_ڌڪو_prepend(حرف ***l, حرف *قدر) {
        أرجع strv_وجهو(l, 0, قدر);
}

صحيح strv_consume_with_سائيز(حرف ***l, size_t *n, حرف *قدر);
ساكن مضمن صحيح strv_consume(حرف ***l, حرف *قدر) {
        أرجع strv_consume_with_سائيز(l, عدم, قدر);
}

صحيح strv_consume_pair(حرف ***l, حرف *a, حرف *b);
صحيح strv_consume_prepend(حرف ***l, حرف *قدر);

حرف** strv_remove(حرف **l, ثابت حرف *s);
حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll);
حرف** strv_uniq(حرف **l);
bool strv_is_uniq(حرف * ثابت *l) خالص;

صحيح strv_compare(حرف * ثابت *a, حرف * ثابت *b) خالص;
ساكن مضمن bool strv_equal(حرف * ثابت *a, حرف * ثابت *b) {
        أرجع strv_compare(a, b) == 0;
}

bool strv_equal_ignore_ترتيب(حرف * ثابت *a, حرف * ثابت *b) خالص;

حرف** strv_new_اندروني(ثابت حرف *x, ...) _sentinel_;
حرف** strv_new_ap(ثابت حرف *x, va_list ap);
#وصف_ڪريو strv_new(...) strv_new_اندروني(__VA_ARGS__, عدم)

#وصف_ڪريو STRV_IGNORE ((ثابت حرف *) اشارو_MAX)

ساكن مضمن ثابت حرف* STRV_IFNOTNULL(ثابت حرف *x) {
        أرجع x ?: STRV_IGNORE;
}

صحيح strv_ورهايو_ڀريل(حرف ***t, ثابت حرف *s, ثابت حرف *separators, ڪڍو_Flags flags);
حرف** strv_ورهايو(ثابت حرف *s, ثابت حرف *separators);

صحيح strv_ورهايو_and_extend_ڀريل(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, ڪڍو_Flags flags);
صحيح strv_ورهايو_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates);

صحيح strv_ورهايو_newlines_ڀريل(حرف ***ret, ثابت حرف *s, ڪڍو_Flags flags);
حرف** strv_ورهايو_newlines(ثابت حرف *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
صحيح strv_ورهايو_colon_جوڙا(حرف ***t, ثابت حرف *s);

حرف* strv_join_ڀريل(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *اڳياڙي);
ساكن مضمن حرف *strv_join(حرف * ثابت *l, ثابت حرف *separator) {
        أرجع strv_join_ڀريل(l, separator, عدم);
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) خالص;

#وصف_ڪريو _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        لكل (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_ڊيگهائي(h);           \
                                _len > 0 ? h + _len - 1 : عدم;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#وصف_ڪريو STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#وصف_ڪريو _STRV_FOREACH_PAIR(x, y, l, i)                          \
        لكل (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#وصف_ڪريو STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

حرف** strv_ترتيب_ڏيو(حرف **l);
حرف** strv_ترتيب_ڏيو_uniq_ڀريل(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b));
حرف** strv_ترتيب_ڏيو_uniq(حرف **l);
فراغ strv_print_ڀريل(حرف * ثابت *l, ثابت حرف *اڳياڙي);
ساكن مضمن فراغ strv_print(حرف * ثابت *l) {
        strv_print_ڀريل(l, عدم);
}

حرف* startswith_strv_اندروني(ثابت حرف *s, حرف * ثابت *l);
#وصف_ڪريو startswith_strv(s, l) const_generic(s, startswith_strv_اندروني(s, l))

#وصف_ڪريو STARTSWITH_سيٽ(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

حرف* endswith_strv_اندروني(ثابت حرف *s, حرف * ثابت *l);
#وصف_ڪريو endswith_strv(s, l) const_generic(s, endswith_strv_اندروني(s, l))

#وصف_ڪريو ENDSWITH_سيٽ(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#وصف_ڪريو STR_اندريون_سيٽ(x, ...) strv_شامل_آهي(STRV_MAKE(__VA_ARGS__), x)
#وصف_ڪريو STRPTR_اندريون_سيٽ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_شامل_آهي(STRV_MAKE(__VA_ARGS__), _x); \
        })

#وصف_ڪريو STRCASE_اندريون_سيٽ(x, ...) strv_شامل_آهي_case(STRV_MAKE(__VA_ARGS__), x)
#وصف_ڪريو STRCASEPTR_اندريون_سيٽ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_شامل_آهي_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#وصف_ڪريو _FOREACH_STRING(uniq, x, y, ...)                                \
        لكل (ثابت حرف *x, * ثابت*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#وصف_ڪريو FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

حرف** strv_ابتو(حرف **l);
حرف** strv_shell_escape(حرف **l, ثابت حرف *bad);

bool strv_fnmatch_ڀريل(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags, size_t *ret_matched_pos);
ساكن مضمن bool strv_fnmatch(حرف* ثابت* patterns, ثابت حرف *s) {
        أرجع strv_fnmatch_ڀريل(patterns, s, 0, عدم);
}
ساكن مضمن bool strv_fnmatch_or_خالي_جايون(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags) {
        أكد(s);
        أرجع strv_isempty(patterns) ||
               strv_fnmatch_ڀريل(patterns, s, flags, عدم);
}

حرف** strv_skip(حرف **l, size_t n);

صحيح strv_extend_n(حرف ***l, ثابت حرف *قدر, size_t n);

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs);

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space);

#وصف_ڪريو strv_free_and_replace(a, b)             \
        free_and_replace_ڀريل(a, b, strv_free)

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *ڪنجي, ثابت حرف *قدر);
فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *ڪنجي, ثابت حرف *قدر);
صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *ڪنجي, ثابت حرف *قدر);
صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *ڪنجي, ثابت حرف *قدر);

صحيح strv_rebreak_سٽون(حرف **l, size_t width, حرف ***ret);

حرف** strv_filter_اڳياڙي(حرف * ثابت *l, ثابت حرف *اڳياڙي);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
خارجي ثابت حرف* ثابت strv_خالي_جايون[1];

#وصف_ڪريو STRV_خالي_جايون ((حرف**) strv_خالي_جايون)
