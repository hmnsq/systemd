/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

#ضمن "../fundamental/strv.h"   /* IWYU pragma: export */

حرف* strv_لبھو(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_لبھو_case(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_لبھو_اگیتر(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_لبھو_startswith(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_لبھو_closest(حرف * ثابت *l, ثابت حرف *name) خالص;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
حرف* strv_لبھو_پہلا_میدان(حرف * ثابت *needles, حرف * ثابت *haystack) خالص;

#تعریف_کرو strv_شامل_اے(l, s) (!!strv_لبھو((l), (s)))
#تعریف_کرو strv_شامل_اے_case(l, s) (!!strv_لبھو_case((l), (s)))

حرف** strv_free(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free);
#تعریف_کرو _cleanup_strv_free_ _cleanup_(strv_freep)

حرف** strv_free_erase(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free_erase);
#تعریف_کرو _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

فراغ strv_free_many(حرف ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

حرف** strv_copy_n(حرف * ثابت *l, size_t n);
ساكن مضمن حرف** strv_copy(حرف * ثابت *l) {
        أرجع strv_copy_n(l, SIZE_MAX);
}
صحيح strv_copy_unless_خالی_تھاواں(حرف * ثابت *l, حرف ***ret);

size_t strv_لمائی(حرف * ثابت *l) خالص;

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates);
صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates);

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *اگیتر, ثابت حرف* ثابت *b, ثابت حرف *suffix);
ساكن مضمن صحيح strv_extend_strv_concat(حرف ***a, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        أرجع strv_extend_strv_biconcat(a, عدم, b, suffix);
}

صحيح strv_prepend(حرف ***l, ثابت حرف *قدر);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

صحيح strv_extend_with_ناپ(حرف ***l, size_t *n, ثابت حرف *قدر);
ساكن مضمن صحيح strv_extend(حرف ***l, ثابت حرف *قدر) {
        أرجع strv_extend_with_ناپ(l, عدم, قدر);
}

صحيح strv_extend_many_اندرونی(حرف ***l, ثابت حرف *قدر, ...);
#تعریف_کرو strv_extend_many(l, ...) strv_extend_many_اندرونی(l, __VA_ARGS__, اشارہ_گر_MAX)

صحيح strv_extendf_with_ناپ(حرف ***l, size_t *n, ثابت حرف *format, ...) _printf_(3,4);
#تعریف_کرو strv_extendf(l, ...) strv_extendf_with_ناپ(l, عدم, __VA_ARGS__)

صحيح strv_extend_joined_with_ناپ_sentinel(حرف ***l, size_t *n, ...) _sentinel_;
#تعریف_کرو strv_extend_joined_with_ناپ(l, n, ...) strv_extend_joined_with_ناپ_sentinel(l, n, __VA_ARGS__, عدم)
#تعریف_کرو strv_extend_joined(l, ...) strv_extend_joined_with_ناپ(l, عدم, __VA_ARGS__)

صحيح strv_ڈالو_with_ناپ(حرف ***l, size_t *n, حرف *قدر);
ساكن مضمن صحيح strv_ڈالو(حرف ***l, حرف *قدر) {
        أرجع strv_ڈالو_with_ناپ(l, عدم, قدر);
}
صحيح strv_ڈالو_pair(حرف ***l, حرف *a, حرف *b);

صحيح strv_داخل_کرو(حرف ***l, size_t position, حرف *قدر);

ساكن مضمن صحيح strv_ڈالو_prepend(حرف ***l, حرف *قدر) {
        أرجع strv_داخل_کرو(l, 0, قدر);
}

صحيح strv_consume_with_ناپ(حرف ***l, size_t *n, حرف *قدر);
ساكن مضمن صحيح strv_consume(حرف ***l, حرف *قدر) {
        أرجع strv_consume_with_ناپ(l, عدم, قدر);
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

bool strv_equal_ignore_ترتیب(حرف * ثابت *a, حرف * ثابت *b) خالص;

حرف** strv_new_اندرونی(ثابت حرف *x, ...) _sentinel_;
حرف** strv_new_ap(ثابت حرف *x, va_list ap);
#تعریف_کرو strv_new(...) strv_new_اندرونی(__VA_ARGS__, عدم)

#تعریف_کرو STRV_IGNORE ((ثابت حرف *) اشارہ_گر_MAX)

ساكن مضمن ثابت حرف* STRV_IFNOTNULL(ثابت حرف *x) {
        أرجع x ?: STRV_IGNORE;
}

صحيح strv_ونڈو_بھریا(حرف ***t, ثابت حرف *s, ثابت حرف *separators, کڈھو_Flags flags);
حرف** strv_ونڈو(ثابت حرف *s, ثابت حرف *separators);

صحيح strv_ونڈو_and_extend_بھریا(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, کڈھو_Flags flags);
صحيح strv_ونڈو_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates);

صحيح strv_ونڈو_newlines_بھریا(حرف ***ret, ثابت حرف *s, کڈھو_Flags flags);
حرف** strv_ونڈو_newlines(ثابت حرف *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
صحيح strv_ونڈو_colon_جوڑے(حرف ***t, ثابت حرف *s);

حرف* strv_join_بھریا(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *اگیتر);
ساكن مضمن حرف *strv_join(حرف * ثابت *l, ثابت حرف *separator) {
        أرجع strv_join_بھریا(l, separator, عدم);
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) خالص;

#تعریف_کرو _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        لكل (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_لمائی(h);           \
                                _len > 0 ? h + _len - 1 : عدم;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#تعریف_کرو STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#تعریف_کرو _STRV_FOREACH_PAIR(x, y, l, i)                          \
        لكل (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#تعریف_کرو STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

حرف** strv_ترتیب_دیو(حرف **l);
حرف** strv_ترتیب_دیو_uniq_بھریا(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b));
حرف** strv_ترتیب_دیو_uniq(حرف **l);
فراغ strv_print_بھریا(حرف * ثابت *l, ثابت حرف *اگیتر);
ساكن مضمن فراغ strv_print(حرف * ثابت *l) {
        strv_print_بھریا(l, عدم);
}

حرف* startswith_strv_اندرونی(ثابت حرف *s, حرف * ثابت *l);
#تعریف_کرو startswith_strv(s, l) const_generic(s, startswith_strv_اندرونی(s, l))

#تعریف_کرو STARTSWITH_سیٹ(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

حرف* endswith_strv_اندرونی(ثابت حرف *s, حرف * ثابت *l);
#تعریف_کرو endswith_strv(s, l) const_generic(s, endswith_strv_اندرونی(s, l))

#تعریف_کرو ENDSWITH_سیٹ(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#تعریف_کرو STR_وچ_سیٹ(x, ...) strv_شامل_اے(STRV_MAKE(__VA_ARGS__), x)
#تعریف_کرو STRPTR_وچ_سیٹ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_شامل_اے(STRV_MAKE(__VA_ARGS__), _x); \
        })

#تعریف_کرو STRCASE_وچ_سیٹ(x, ...) strv_شامل_اے_case(STRV_MAKE(__VA_ARGS__), x)
#تعریف_کرو STRCASEPTR_وچ_سیٹ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_شامل_اے_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#تعریف_کرو _FOREACH_STRING(uniq, x, y, ...)                                \
        لكل (ثابت حرف *x, * ثابت*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#تعریف_کرو FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

حرف** strv_الٹ(حرف **l);
حرف** strv_shell_escape(حرف **l, ثابت حرف *bad);

bool strv_fnmatch_بھریا(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags, size_t *ret_matched_pos);
ساكن مضمن bool strv_fnmatch(حرف* ثابت* patterns, ثابت حرف *s) {
        أرجع strv_fnmatch_بھریا(patterns, s, 0, عدم);
}
ساكن مضمن bool strv_fnmatch_or_خالی_تھاواں(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags) {
        أكد(s);
        أرجع strv_isempty(patterns) ||
               strv_fnmatch_بھریا(patterns, s, flags, عدم);
}

حرف** strv_skip(حرف **l, size_t n);

صحيح strv_extend_n(حرف ***l, ثابت حرف *قدر, size_t n);

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs);

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space);

#تعریف_کرو strv_free_and_replace(a, b)             \
        free_and_replace_بھریا(a, b, strv_free)

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *کلید, ثابت حرف *قدر);
فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *کلید, ثابت حرف *قدر);
صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *کلید, ثابت حرف *قدر);
صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *کلید, ثابت حرف *قدر);

صحيح strv_rebreak_سطریں(حرف **l, size_t width, حرف ***ret);

حرف** strv_filter_اگیتر(حرف * ثابت *l, ثابت حرف *اگیتر);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
خارجي ثابت حرف* ثابت strv_خالی_تھاواں[1];

#تعریف_کرو STRV_خالی_تھاواں ((حرف**) strv_خالی_تھاواں)
