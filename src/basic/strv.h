/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#شامل "../fundamental/strv.h"   /* IWYU pragma: export */

حرف* strv_ڈھونڈو(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڈھونڈو_case(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڈھونڈو_سابقہ(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڈھونڈو_startswith(حرف * ثابت *l, ثابت حرف *name) خالص;
حرف* strv_ڈھونڈو_closest(حرف * ثابت *l, ثابت حرف *name) خالص;
/* Given two vectors, the first a list of keys and the second a list of key-value pairs, returns the value
 * of the first key from the first vector that is found in the second vector. */
حرف* strv_ڈھونڈو_پہلا_میدان(حرف * ثابت *needles, حرف * ثابت *haystack) خالص;

#تعریف_کرو strv_شامل_ہے(l, s) (!!strv_ڈھونڈو((l), (s)))
#تعریف_کرو strv_شامل_ہے_case(l, s) (!!strv_ڈھونڈو_case((l), (s)))

حرف** strv_free(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free);
#تعریف_کرو _cleanup_strv_free_ _cleanup_(strv_freep)

حرف** strv_free_erase(حرف **l);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف**, strv_free_erase);
#تعریف_کرو _cleanup_strv_free_erase_ _cleanup_(strv_free_erasep)

خالی strv_free_many(حرف ***strvs, size_t n) _nonnull_if_nonzero_(1, 2);

حرف** strv_copy_n(حرف * ثابت *l, size_t n);
ساکن اندرونی حرف** strv_copy(حرف * ثابت *l) {
        واپس strv_copy_n(l, SIZE_MAX);
}
صحیح strv_copy_unless_خالی_خانے(حرف * ثابت *l, حرف ***ret);

size_t strv_لمبائی(حرف * ثابت *l) خالص;

صحیح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates);
صحیح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates);

صحیح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *سابقہ, ثابت حرف* ثابت *b, ثابت حرف *suffix);
ساکن اندرونی صحیح strv_extend_strv_concat(حرف ***a, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        واپس strv_extend_strv_biconcat(a, تہی, b, suffix);
}

صحیح strv_prepend(حرف ***l, ثابت حرف *قدر);

/* _with_size() are lower-level functions where the size can be provided externally,
 * which allows us to skip iterating over the strv to find the end, which saves
 * a bit of time and reduces the complexity of appending from O(n²) to O(n). */

صحیح strv_extend_with_سائز(حرف ***l, size_t *n, ثابت حرف *قدر);
ساکن اندرونی صحیح strv_extend(حرف ***l, ثابت حرف *قدر) {
        واپس strv_extend_with_سائز(l, تہی, قدر);
}

صحیح strv_extend_many_اندرونی(حرف ***l, ثابت حرف *قدر, ...);
#تعریف_کرو strv_extend_many(l, ...) strv_extend_many_اندرونی(l, __VA_ARGS__, اشارہ_گر_MAX)

صحیح strv_extendf_with_سائز(حرف ***l, size_t *n, ثابت حرف *format, ...) _printf_(3,4);
#تعریف_کرو strv_extendf(l, ...) strv_extendf_with_سائز(l, تہی, __VA_ARGS__)

صحیح strv_extend_joined_with_سائز_sentinel(حرف ***l, size_t *n, ...) _sentinel_;
#تعریف_کرو strv_extend_joined_with_سائز(l, n, ...) strv_extend_joined_with_سائز_sentinel(l, n, __VA_ARGS__, تہی)
#تعریف_کرو strv_extend_joined(l, ...) strv_extend_joined_with_سائز(l, تہی, __VA_ARGS__)

صحیح strv_ڈالو_with_سائز(حرف ***l, size_t *n, حرف *قدر);
ساکن اندرونی صحیح strv_ڈالو(حرف ***l, حرف *قدر) {
        واپس strv_ڈالو_with_سائز(l, تہی, قدر);
}
صحیح strv_ڈالو_pair(حرف ***l, حرف *a, حرف *b);

صحیح strv_داخل_کرو(حرف ***l, size_t position, حرف *قدر);

ساکن اندرونی صحیح strv_ڈالو_prepend(حرف ***l, حرف *قدر) {
        واپس strv_داخل_کرو(l, 0, قدر);
}

صحیح strv_consume_with_سائز(حرف ***l, size_t *n, حرف *قدر);
ساکن اندرونی صحیح strv_consume(حرف ***l, حرف *قدر) {
        واپس strv_consume_with_سائز(l, تہی, قدر);
}

صحیح strv_consume_pair(حرف ***l, حرف *a, حرف *b);
صحیح strv_consume_prepend(حرف ***l, حرف *قدر);

حرف** strv_remove(حرف **l, ثابت حرف *s);
حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll);
حرف** strv_uniq(حرف **l);
bool strv_is_uniq(حرف * ثابت *l) خالص;

صحیح strv_compare(حرف * ثابت *a, حرف * ثابت *b) خالص;
ساکن اندرونی bool strv_equal(حرف * ثابت *a, حرف * ثابت *b) {
        واپس strv_compare(a, b) == 0;
}

bool strv_equal_ignore_ترتیب(حرف * ثابت *a, حرف * ثابت *b) خالص;

حرف** strv_new_اندرونی(ثابت حرف *x, ...) _sentinel_;
حرف** strv_new_ap(ثابت حرف *x, va_list ap);
#تعریف_کرو strv_new(...) strv_new_اندرونی(__VA_ARGS__, تہی)

#تعریف_کرو STRV_IGNORE ((ثابت حرف *) اشارہ_گر_MAX)

ساکن اندرونی ثابت حرف* STRV_IFNOTNULL(ثابت حرف *x) {
        واپس x ?: STRV_IGNORE;
}

صحیح strv_بانٹو_بھرا(حرف ***t, ثابت حرف *s, ثابت حرف *separators, نکالو_Flags flags);
حرف** strv_بانٹو(ثابت حرف *s, ثابت حرف *separators);

صحیح strv_بانٹو_and_extend_بھرا(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, نکالو_Flags flags);
صحیح strv_بانٹو_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates);

صحیح strv_بانٹو_newlines_بھرا(حرف ***ret, ثابت حرف *s, نکالو_Flags flags);
حرف** strv_بانٹو_newlines(ثابت حرف *s);

/* Given a string containing white-space separated tuples of words themselves separated by ':',
 * returns a vector of strings. If the second element in a tuple is missing, the corresponding
 * string in the vector is an empty string. */
صحیح strv_بانٹو_colon_جوڑے(حرف ***t, ثابت حرف *s);

حرف* strv_join_بھرا(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *سابقہ);
ساکن اندرونی حرف *strv_join(حرف * ثابت *l, ثابت حرف *separator) {
        واپس strv_join_بھرا(l, separator, تہی);
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) خالص;

#تعریف_کرو _STRV_FOREACH_BACKWARDS(s, l, h, i)                             \
        برائے (typeof(*(l)) *s, *h = (l), *i = ({                         \
                                size_t _len = strv_لمبائی(h);           \
                                _len > 0 ? h + _len - 1 : تہی;         \
                        });                                             \
             (s = i);                                                   \
             i = PTR_SUB1(i, h))

#تعریف_کرو STRV_FOREACH_BACKWARDS(s, l)                                    \
        _STRV_FOREACH_BACKWARDS(s, l, UNIQ_T(h, UNIQ), UNIQ_T(i, UNIQ))

#تعریف_کرو _STRV_FOREACH_PAIR(x, y, l, i)                          \
        برائے (typeof(*(l)) *x, *y, *i = (l);                     \
             i && *(x = i) && *(y = i + 1);                     \
             i += 2)

#تعریف_کرو STRV_FOREACH_PAIR(x, y, l)                      \
        _STRV_FOREACH_PAIR(x, y, l, UNIQ_T(i, UNIQ))

حرف** strv_ترتیب_دو(حرف **l);
حرف** strv_ترتیب_دو_uniq_بھرا(حرف **l, صحیح (*compare)(حرف * ثابت *a, حرف * ثابت *b));
حرف** strv_ترتیب_دو_uniq(حرف **l);
خالی strv_print_بھرا(حرف * ثابت *l, ثابت حرف *سابقہ);
ساکن اندرونی خالی strv_print(حرف * ثابت *l) {
        strv_print_بھرا(l, تہی);
}

حرف* startswith_strv_اندرونی(ثابت حرف *s, حرف * ثابت *l);
#تعریف_کرو startswith_strv(s, l) const_generic(s, startswith_strv_اندرونی(s, l))

#تعریف_کرو STARTSWITH_سیٹ(p, ...)                                  \
        startswith_strv(p, STRV_MAKE(__VA_ARGS__))

حرف* endswith_strv_اندرونی(ثابت حرف *s, حرف * ثابت *l);
#تعریف_کرو endswith_strv(s, l) const_generic(s, endswith_strv_اندرونی(s, l))

#تعریف_کرو ENDSWITH_سیٹ(p, ...)                                    \
        endswith_strv(p, STRV_MAKE(__VA_ARGS__))

#تعریف_کرو STR_اندر_سیٹ(x, ...) strv_شامل_ہے(STRV_MAKE(__VA_ARGS__), x)
#تعریف_کرو STRPTR_اندر_سیٹ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_شامل_ہے(STRV_MAKE(__VA_ARGS__), _x); \
        })

#تعریف_کرو STRCASE_اندر_سیٹ(x, ...) strv_شامل_ہے_case(STRV_MAKE(__VA_ARGS__), x)
#تعریف_کرو STRCASEPTR_اندر_سیٹ(x, ...)                                    \
        ({                                                       \
                ثابت حرف* _x = (x);                            \
                _x && strv_شامل_ہے_case(STRV_MAKE(__VA_ARGS__), _x); \
        })

#تعریف_کرو _FOREACH_STRING(uniq, x, y, ...)                                \
        برائے (ثابت حرف *x, * ثابت*UNIQ_T(l, uniq) = STRV_MAKE_CONST(({ x = y; }), ##__VA_ARGS__); \
             x;                                                         \
             x = *(++UNIQ_T(l, uniq)))

#تعریف_کرو FOREACH_STRING(x, y, ...)                       \
        _FOREACH_STRING(UNIQ, x, y, ##__VA_ARGS__)

حرف** strv_الٹ(حرف **l);
حرف** strv_shell_escape(حرف **l, ثابت حرف *bad);

bool strv_fnmatch_بھرا(حرف* ثابت* patterns, ثابت حرف *s, صحیح flags, size_t *ret_matched_pos);
ساکن اندرونی bool strv_fnmatch(حرف* ثابت* patterns, ثابت حرف *s) {
        واپس strv_fnmatch_بھرا(patterns, s, 0, تہی);
}
ساکن اندرونی bool strv_fnmatch_or_خالی_خانے(حرف* ثابت* patterns, ثابت حرف *s, صحیح flags) {
        أكد(s);
        واپس strv_isempty(patterns) ||
               strv_fnmatch_بھرا(patterns, s, flags, تہی);
}

حرف** strv_skip(حرف **l, size_t n);

صحیح strv_extend_n(حرف ***l, ثابت حرف *قدر, size_t n);

صحیح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs);

صحیح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space);

#تعریف_کرو strv_free_and_replace(a, b)             \
        free_and_replace_بھرا(a, b, strv_free)

خالی string_strv_hashmap_remove(Hashmap *h, ثابت حرف *کلید, ثابت حرف *قدر);
خالی string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *کلید, ثابت حرف *قدر);
صحیح string_strv_hashmap_put(Hashmap **h, ثابت حرف *کلید, ثابت حرف *قدر);
صحیح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *کلید, ثابت حرف *قدر);

صحیح strv_rebreak_سطریں(حرف **l, size_t width, حرف ***ret);

حرف** strv_filter_سابقہ(حرف * ثابت *l, ثابت حرف *سابقہ);

/* whenever we need to initialize something with a constant non-NULL, but empty strv, we can use this shared
 * one */
خارجی ثابت حرف* ثابت strv_خالی_خانے[1];

#تعریف_کرو STRV_خالی_خانے ((حرف**) strv_خالی_خانے)
