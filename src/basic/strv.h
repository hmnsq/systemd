/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

#ضمن "../fundamental/strv.h"   /* IWYU pragma: export */

حرف* strv_پیدا_کړه(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_پیدا_کړه_case(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_پیدا_کړه_مختاړی(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_پیدا_کړه_startswith(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_پیدا_کړه_closest(حرف * ثابت *l, ثابت حرف *name) خالص;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
حرف* strv_پیدا_کړه_لومړی_ډګر(حرف * ثابت *needles, حرف * ثابت *haystack) خالص;

#تعریف_کړه strv_لري(l, s) (!!strv_پیدا_کړه((l), (s)))
#تعریف_کړه strv_لري_case(l, s) (!!strv_پیدا_کړه_case((l), (s)))

حرف** strv_free(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free);
#تعریف_کړه _cleanup_strv_free_ _cleanup_(strv_freep)

حرف** strv_free_erase(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free_erase);
#تعریف_کړه _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

فراغ strv_free_many(حرف ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

حرف** strv_copy_n(حرف * ثابت *l, size_t n);
ساكن مضمن حرف** strv_copy(حرف * ثابت *l) {
        أرجع strv_copy_n(l, SIZE_MAX);
}
صحيح strv_copy_unless_تشه(حرف * ثابت *l, حرف ***ret);

size_t strv_اوږدتیا(حرف * ثابت *l) خالص;

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates);
صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates);

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *مختاړی, ثابت حرف* ثابت *b, ثابت حرف *suffix);
ساكن مضمن صحيح strv_extend_strv_concat(حرف ***a, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        أرجع strv_extend_strv_biconcat(a, عدم, b, suffix);
}

صحيح strv_prepend(حرف ***l, ثابت حرف *ارزښت);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

صحيح strv_extend_with_اندازه(حرف ***l, size_t *n, ثابت حرف *ارزښت);
ساكن مضمن صحيح strv_extend(حرف ***l, ثابت حرف *ارزښت) {
        أرجع strv_extend_with_اندازه(l, عدم, ارزښت);
}

صحيح strv_extend_many_دننی(حرف ***l, ثابت حرف *ارزښت, ...);
#تعریف_کړه strv_extend_many(l, ...) strv_extend_many_دننی(l, __VA_ARGS__, اشاره_ګر_MAX)

صحيح strv_extendf_with_اندازه(حرف ***l, size_t *n, ثابت حرف *format, ...) _printf_(3,4);
#تعریف_کړه strv_extendf(l, ...) strv_extendf_with_اندازه(l, عدم, __VA_ARGS__)

صحيح strv_extend_joined_with_اندازه_sentinel(حرف ***l, size_t *n, ...) _sentinel_;
#تعریف_کړه strv_extend_joined_with_اندازه(l, n, ...) strv_extend_joined_with_اندازه_sentinel(l, n, __VA_ARGS__, عدم)
#تعریف_کړه strv_extend_joined(l, ...) strv_extend_joined_with_اندازه(l, عدم, __VA_ARGS__)

صحيح strv_ټېل_with_اندازه(حرف ***l, size_t *n, حرف *ارزښت);
ساكن مضمن صحيح strv_ټېل(حرف ***l, حرف *ارزښت) {
        أرجع strv_ټېل_with_اندازه(l, عدم, ارزښت);
}
صحيح strv_ټېل_pair(حرف ***l, حرف *a, حرف *b);

صحيح strv_ورننباسه(حرف ***l, size_t position, حرف *ارزښت);

ساكن مضمن صحيح strv_ټېل_prepend(حرف ***l, حرف *ارزښت) {
        أرجع strv_ورننباسه(l, 0, ارزښت);
}

صحيح strv_consume_with_اندازه(حرف ***l, size_t *n, حرف *ارزښت);
ساكن مضمن صحيح strv_consume(حرف ***l, حرف *ارزښت) {
        أرجع strv_consume_with_اندازه(l, عدم, ارزښت);
}

صحيح strv_consume_pair(حرف ***l, حرف *a, حرف *b);
صحيح strv_consume_prepend(حرف ***l, حرف *ارزښت);

حرف** strv_remove(حرف **l, ثابت حرف *s);
حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll);
حرف** strv_uniq(حرف **l);
bool strv_is_uniq(حرف * ثابت *l) خالص;

صحيح strv_compare(حرف * ثابت *a, حرف * ثابت *b) خالص;
ساكن مضمن bool strv_equal(حرف * ثابت *a, حرف * ثابت *b) {
        أرجع strv_compare(a, b) == 0;
}

bool strv_equal_ignore_ترتیب(حرف * ثابت *a, حرف * ثابت *b) خالص;

حرف** strv_new_دننی(ثابت حرف *x, ...) _sentinel_;
حرف** strv_new_ap(ثابت حرف *x, va_list ap);
#تعریف_کړه strv_new(...) strv_new_دننی(__VA_ARGS__, عدم)

#تعریف_کړه STRV_IGNORE ((ثابت حرف *) اشاره_ګر_MAX)

ساكن مضمن ثابت حرف* STRV_IFNOTNULL(ثابت حرف *x) {
        أرجع x ?: STRV_IGNORE;
}

صحيح strv_وېشل_ډک(حرف ***t, ثابت حرف *s, ثابت حرف *separators, ایستل_Flags flags);
حرف** strv_وېشل(ثابت حرف *s, ثابت حرف *separators);

صحيح strv_وېشل_and_extend_ډک(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, ایستل_Flags flags);
صحيح strv_وېشل_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates);

صحيح strv_وېشل_newlines_ډک(حرف ***ret, ثابت حرف *s, ایستل_Flags flags);
حرف** strv_وېشل_newlines(ثابت حرف *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
صحيح strv_وېشل_colon_جوړې(حرف ***t, ثابت حرف *s);

حرف* strv_join_ډک(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *مختاړی);
ساكن مضمن حرف *strv_join(حرف * ثابت *l, ثابت حرف *separator) {
        أرجع strv_join_ډک(l, separator, عدم);
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) خالص;

#تعریف_کړه _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        لكل (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_اوږدتیا(h);           \
                                _len > 0 ? h + _len - 1 : عدم;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#تعریف_کړه STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#تعریف_کړه _STRV_FOREACH_PAIR(x, y, l, i)                          \
        لكل (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#تعریف_کړه STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

حرف** strv_ترتیبول(حرف **l);
حرف** strv_ترتیبول_uniq_ډک(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b));
حرف** strv_ترتیبول_uniq(حرف **l);
فراغ strv_print_ډک(حرف * ثابت *l, ثابت حرف *مختاړی);
ساكن مضمن فراغ strv_print(حرف * ثابت *l) {
        strv_print_ډک(l, عدم);
}

حرف* startswith_strv_دننی(ثابت حرف *s, حرف * ثابت *l);
#تعریف_کړه startswith_strv(s, l) const_generic(s, startswith_strv_دننی(s, l))

#تعریف_کړه STARTSWITH_سېټ(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

حرف* endswith_strv_دننی(ثابت حرف *s, حرف * ثابت *l);
#تعریف_کړه endswith_strv(s, l) const_generic(s, endswith_strv_دننی(s, l))

#تعریف_کړه ENDSWITH_سېټ(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#تعریف_کړه STR_دننه_سېټ(x, ...) strv_لري(STRV_MAKE(__VA_ARGS__), x)
#تعریف_کړه STRPTR_دننه_سېټ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_لري(STRV_MAKE(__VA_ARGS__), _x); \
        })

#تعریف_کړه STRCASE_دننه_سېټ(x, ...) strv_لري_case(STRV_MAKE(__VA_ARGS__), x)
#تعریف_کړه STRCASEPTR_دننه_سېټ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_لري_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#تعریف_کړه _FOREACH_STRING(uniq, x, y, ...)                                \
        لكل (ثابت حرف *x, * ثابت*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#تعریف_کړه FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

حرف** strv_سرچپه(حرف **l);
حرف** strv_shell_escape(حرف **l, ثابت حرف *bad);

bool strv_fnmatch_ډک(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags, size_t *ret_matched_pos);
ساكن مضمن bool strv_fnmatch(حرف* ثابت* patterns, ثابت حرف *s) {
        أرجع strv_fnmatch_ډک(patterns, s, 0, عدم);
}
ساكن مضمن bool strv_fnmatch_or_تشه(حرف* ثابت* patterns, ثابت حرف *s, صحيح flags) {
        أكد(s);
        أرجع strv_isempty(patterns) ||
               strv_fnmatch_ډک(patterns, s, flags, عدم);
}

حرف** strv_skip(حرف **l, size_t n);

صحيح strv_extend_n(حرف ***l, ثابت حرف *ارزښت, size_t n);

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs);

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space);

#تعریف_کړه strv_free_and_replace(a, b)             \
        free_and_replace_ډک(a, b, strv_free)

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *کلی, ثابت حرف *ارزښت);
فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *کلی, ثابت حرف *ارزښت);
صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *کلی, ثابت حرف *ارزښت);
صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *کلی, ثابت حرف *ارزښت);

صحيح strv_rebreak_کرښې(حرف **l, size_t width, حرف ***ret);

حرف** strv_filter_مختاړی(حرف * ثابت *l, ثابت حرف *مختاړی);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
خارجي ثابت حرف* ثابت strv_تشه[1];

#تعریف_کړه STRV_تشه ((حرف**) strv_تشه)
