/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "env-util.h"
#ضمن "escape.h"
#ضمن "extract-word.h"
#ضمن "fileio.h"
#ضمن "gunicode.h"
#ضمن "hashmap.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "sort-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"

حرف* strv_پیدا_کن(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                إذا (streq(*i, name))
                        أرجع *i;

        أرجع عدم;
}

حرف* strv_پیدا_کن_case(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                إذا (strcaseeq(*i, name))
                        أرجع *i;

        أرجع عدم;
}

حرف* strv_پیدا_کن_پیشوند(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                إذا (startswith(*i, name))
                        أرجع *i;

        أرجع عدم;
}

حرف* strv_پیدا_کن_startswith(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                حرف *e;

                e = startswith(*i, name);
                إذا (e)
                        أرجع e;
        }

        أرجع عدم;
}

ساكن حرف* strv_پیدا_کن_closest_پیشوند(حرف * ثابت *l, ثابت حرف *name) {
        size_t بهترین_فاصله = SIZE_MAX;
        حرف *بهترین = عدم;

        أكد(name);

        STRV_FOREACH(s, l) {
                حرف *e = startswith(*s, name);
                إذا (!e)
                        استمر;

                size_t n = طول_رشته(e);
                إذا (n < بهترین_فاصله) {
                        بهترین_فاصله = n;
                        بهترین = *s;
                }
        }

        أرجع بهترین;
}

ساكن حرف* strv_پیدا_کن_closest_by_levenshtein(حرف * ثابت *l, ثابت حرف *name) {
        ssize_t بهترین_فاصله = SSIZE_MAX;
        حرف *بهترین = عدم;

        أكد(name);

        STRV_FOREACH(i, l) {
                ssize_t فاصله;

                فاصله = strlevenshtein(*i, name);
                إذا (فاصله < 0) {
                        log_debug_errno(فاصله, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        أرجع عدم;
                }

                إذا (فاصله > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        استمر;

                إذا (فاصله < بهترین_فاصله) {
                        بهترین_فاصله = فاصله;
                        بهترین = *i;
                }
        }

        أرجع بهترین;
}

حرف* strv_پیدا_کن_closest(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        حرف *یافت = strv_پیدا_کن_closest_پیشوند(l, name);
        إذا (یافت)
                أرجع یافت;

        أرجع strv_پیدا_کن_closest_by_levenshtein(l, name);
}

حرف* strv_پیدا_کن_اولی_فیلد(حرف * ثابت *needles, حرف * ثابت *haystack) {
        STRV_FOREACH(k, needles) {
                حرف *مقدار = strv_env_جفتها_get((حرف **)haystack, *k);
                إذا (مقدار)
                        أرجع مقدار;
        }

        أرجع عدم;
}

حرف** strv_free(حرف **l) {
        STRV_FOREACH(k, l)
                حرر(*k);

        أرجع mfree(l);
}

حرف** strv_free_erase(حرف **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        أرجع mfree(l);
}

فراغ strv_free_many(حرف ***strvs, size_t n) {
        أكد(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        حرر(strvs);
}

حرف** strv_copy_n(حرف * ثابت *l, size_t n) {
        _cleanup_strv_free_ حرف **نتیجه = عدم;
        حرف **k;

        نتیجه = new(حرف*, MIN(strv_درازی(l), n) + 1);
        إذا (!نتیجه)
                أرجع عدم;

        k = نتیجه;
        STRV_FOREACH(i, l) {
                إذا (n == 0)
                        اكسر;

                *k = strdup(*i);
                إذا (!*k)
                        أرجع عدم;
                k++;

                إذا (n != SIZE_MAX)
                        n--;
        }

        *k = عدم;
        أرجع TAKE_PTR(نتیجه);
}

صحيح strv_copy_unless_خالی(حرف * ثابت *l, حرف ***ret) {
        أكد(ret);

        إذا (strv_isempty(l)) {
                *ret = عدم;
                أرجع 0;
        }

        حرف **copy = strv_copy(l);
        إذا (!copy)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(copy);
        أرجع 1;
}

size_t strv_درازی(حرف * ثابت *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        أرجع n;
}

حرف** strv_new_ap(ثابت حرف *x, va_list ap) {
        _cleanup_strv_free_ حرف **a = عدم;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        لكل (ثابت حرف *s = x; s; s = va_arg(aq, ثابت حرف*)) {
                إذا (s == STRV_IGNORE)
                        استمر;

                n++;
        }
        va_end(aq);

        a = new(حرف*, n+1);
        إذا (!a)
                أرجع عدم;

        لكل (ثابت حرف *s = x; s; s = va_arg(ap, ثابت حرف*)) {
                إذا (s == STRV_IGNORE)
                        استمر;

                a[i] = strdup(s);
                إذا (!a[i])
                        أرجع عدم;

                i++;
        }

        a[i] = عدم;

        أرجع TAKE_PTR(a);
}

حرف** strv_new_داخلی(ثابت حرف *x, ...) {
        حرف **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        أرجع r;
}

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        أكد(a);

        q = strv_درازی(b);
        إذا (q == 0)
                أرجع 0;

        p = strv_درازی(*a);
        إذا (p >= SIZE_MAX - q)
                أرجع -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_دور_به_بالا(p + q + 1), حجم(حرف *));
        إذا (!t)
                أرجع -ENOMEM;

        t[p] = عدم;
        *a = t;

        STRV_FOREACH(s, b) {
                إذا (filter_duplicates && strv_شامل_است(t, *s))
                        استمر;

                t[p+i] = strdup(*s);
                إذا (!t[p+i])
                        اذهب rollback;

                i++;
                t[p+i] = عدم;
        }

        أكد(i <= q);

        أرجع (صحيح) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = عدم;
        أرجع -ENOMEM;
}

صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates) {
        _cleanup_strv_free_ حرف **b_consume = b;
        size_t p, q, i;

        أكد(a);

        q = strv_درازی(b);
        إذا (q == 0)
                أرجع 0;

        p = strv_درازی(*a);
        إذا (p == 0) {
                strv_free_and_replace(*a, b_consume);

                إذا (filter_duplicates)
                        strv_uniq(*a);

                أرجع strv_درازی(*a);
        }

        إذا (p >= SIZE_MAX - q)
                أرجع -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_دور_به_بالا(p + q + 1), حجم(حرف *));
        إذا (!t)
                أرجع -ENOMEM;

        t[p] = عدم;
        *a = t;

        إذا (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = عدم;
                i = q;
        } وإلا {
                i = 0;

                STRV_FOREACH(s, b) {
                        إذا (strv_شامل_است(t, *s)) {
                                حرر(*s);
                                استمر;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = عدم;
                }
        }

        أكد(i <= q);

        b_consume = mfree(b_consume);

        أرجع (صحيح) i;
}

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *پیشوند, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        صحيح r;

        أكد(a);

        STRV_FOREACH(s, b) {
                حرف *v;

                v = strjoin(strempty(پیشوند), *s, suffix);
                إذا (!v)
                        أرجع -ENOMEM;

                r = strv_consume(a, v);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح strv_تقسیم_newlines_پر(حرف ***ret, ثابت حرف *s, استخراج_Flags flags) {
        _cleanup_strv_free_ حرف **l = عدم;
        size_t n;
        صحيح r;

        أكد(ret);
        أكد(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_تقسیم_پر(&l, s, NEWLINE, flags);
        إذا (r < 0)
                أرجع r;

        n = strv_درازی(l);
        إذا (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        أرجع n;
}

حرف** strv_تقسیم_newlines(ثابت حرف *s) {
        حرف **ret;

        إذا (strv_تقسیم_newlines_پر(&ret, s, 0) < 0)
                أرجع عدم;

        أرجع ret;
}

صحيح strv_تقسیم_پر(حرف ***t, ثابت حرف *s, ثابت حرف *separators, استخراج_Flags flags) {
        _cleanup_strv_free_ حرف **l = عدم;
        size_t n = 0;
        صحيح r;

        أكد(t);
        أكد(s);

        لكل (;;) {
                _cleanup_free_ حرف *کلمه = عدم;

                r = استخراج_اولی_کلمه(&s, &کلمه, separators, flags);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                إذا (!GREEDY_REALLOC(l, n + 2))
                        أرجع -ENOMEM;

                l[n++] = TAKE_PTR(کلمه);
                l[n] = عدم;
        }

        إذا (!l) {
                l = new0(حرف*, 1);
                إذا (!l)
                        أرجع -ENOMEM;
        }

        *t = TAKE_PTR(l);

        أرجع (صحيح) n;
}

حرف** strv_تقسیم(ثابت حرف *s, ثابت حرف *separators) {
        حرف **ret;

        إذا (strv_تقسیم_پر(&ret, s, separators, استخراج_RETAIN_ESCAPE) < 0)
                أرجع عدم;

        أرجع ret;
}

صحيح strv_تقسیم_and_extend_پر(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, استخراج_Flags flags) {
        حرف **l;
        صحيح r;

        أكد(t);
        أكد(s);

        r = strv_تقسیم_پر(&l, s, separators, flags);
        إذا (r < 0)
                أرجع r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        إذا (r < 0)
                أرجع r;

        أرجع (صحيح) strv_درازی(*t);
}

صحيح strv_تقسیم_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates) {
        أرجع strv_تقسیم_and_extend_پر(t, s, separators, filter_duplicates, 0);
}

صحيح strv_تقسیم_colon_جفتها(حرف ***t, ثابت حرف *s) {
        _cleanup_strv_free_ حرف **l = عدم;
        size_t n = 0;
        صحيح r;

        أكد(t);
        أكد(s);

        لكل (;;) {
                _cleanup_free_ حرف *اولی = عدم, *دومی = عدم, *tuple = عدم, *دومی_or_خالی = عدم;

                r = استخراج_اولی_کلمه(&s, &tuple, عدم, استخراج_UNQUOTE|استخراج_RETAIN_ESCAPE);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                ثابت حرف *p = tuple;
                r = استخراج_many_واژگان(&p, ":", استخراج_CUNESCAPE|استخراج_UNESCAPE_SEPARATORS,
                                       &اولی, &دومی);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        استمر;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                إذا (!isempty(p))
                        أرجع -EINVAL;

                دومی_or_خالی = strdup(strempty(دومی));
                إذا (!دومی_or_خالی)
                        أرجع -ENOMEM;

                إذا (!GREEDY_REALLOC(l, n + 3))
                        أرجع -ENOMEM;

                l[n++] = TAKE_PTR(اولی);
                l[n++] = TAKE_PTR(دومی_or_خالی);

                l[n] = عدم;
        }

        إذا (!l) {
                l = new0(حرف*, 1);
                إذا (!l)
                        أرجع -ENOMEM;
        }

        *t = TAKE_PTR(l);

        أرجع (صحيح) n;
}

حرف* strv_join_پر(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *پیشوند) {
        حرف *r, *e;
        size_t n, k, m;

        إذا (!separator)
                separator = " ";

        k = طول_رشته(separator);
        m = strlen_ptr(پیشوند);

        n = 0;
        STRV_FOREACH(s, l) {
                إذا (s != l)
                        n += k;

                n += m + طول_رشته(*s);
        }

        r = new(حرف, n+1);
        إذا (!r)
                أرجع عدم;

        e = r;
        STRV_FOREACH(s, l) {
                إذا (s != l)
                        e = stpcpy(e, separator);

                إذا (پیشوند)
                        e = stpcpy(e, پیشوند);

                e = stpcpy(e, *s);
        }

        *e = 0;

        أرجع r;
}

صحيح strv_افزودن_with_اندازه(حرف ***l, size_t *n, حرف *مقدار) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        أكد(l);
        اشاره_گر_MAY_BE_NULL(n);

        إذا (!مقدار)
                أرجع 0;

        size_t اندازه = n ? *n : SIZE_MAX;
        إذا (اندازه == SIZE_MAX)
                اندازه = strv_درازی(*l);

        /* Check for overflow */
        إذا (اندازه > SIZE_MAX-2)
                أرجع -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_دور_به_بالا(اندازه + 2), حجم(حرف*));
        إذا (!c)
                أرجع -ENOMEM;

        c[اندازه] = مقدار;
        c[اندازه+1] = عدم;

        *l = c;
        إذا (n)
                *n = اندازه + 1;
        أرجع 0;
}

صحيح strv_افزودن_pair(حرف ***l, حرف *a, حرف *b) {
        حرف **c;
        size_t n;

        أكد(l);

        إذا (!a && !b)
                أرجع 0;

        n = strv_درازی(*l);

        /* Check for overflow */
        إذا (n > SIZE_MAX-3)
                أرجع -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_دور_به_بالا(n + !!a + !!b + 1), حجم(حرف*));
        إذا (!c)
                أرجع -ENOMEM;

        إذا (a)
                c[n++] = a;
        إذا (b)
                c[n++] = b;
        c[n] = عدم;

        *l = c;
        أرجع 0;
}

صحيح strv_درج(حرف ***l, size_t position, حرف *مقدار) {
        حرف **c;
        size_t n, m;

        أكد(l);

        إذا (!مقدار)
                أرجع 0;

        n = strv_درازی(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        إذا (n > SIZE_MAX - 2)
                أرجع -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_دور_به_بالا(m), حجم(حرف*));
        إذا (!c)
                أرجع -ENOMEM;

        إذا (n > position)
                memmove(c + position + 1, c + position, (n - position) * حجم(حرف*));

        c[position] = مقدار;
        c[n + 1] = عدم;

        *l = c;
        أرجع 0;
}

صحيح strv_consume_with_اندازه(حرف ***l, size_t *n, حرف *مقدار) {
        صحيح r;

        r = strv_افزودن_with_اندازه(l, n, مقدار);
        إذا (r < 0)
                حرر(مقدار);

        أرجع r;
}

صحيح strv_consume_pair(حرف ***l, حرف *a, حرف *b) {
        صحيح r;

        r = strv_افزودن_pair(l, a, b);
        إذا (r < 0) {
                حرر(a);
                حرر(b);
        }

        أرجع r;
}

صحيح strv_consume_prepend(حرف ***l, حرف *مقدار) {
        صحيح r;

        r = strv_افزودن_prepend(l, مقدار);
        إذا (r < 0)
                حرر(مقدار);

        أرجع r;
}

صحيح strv_prepend(حرف ***l, ثابت حرف *مقدار) {
        حرف *v;

        إذا (!مقدار)
                أرجع 0;

        v = strdup(مقدار);
        إذا (!v)
                أرجع -ENOMEM;

        أرجع strv_consume_prepend(l, v);
}

صحيح strv_extend_with_اندازه(حرف ***l, size_t *n, ثابت حرف *مقدار) {
        حرف *v;

        إذا (!مقدار)
                أرجع 0;

        v = strdup(مقدار);
        إذا (!v)
                أرجع -ENOMEM;

        أرجع strv_consume_with_اندازه(l, n, v);
}

صحيح strv_extend_many_داخلی(حرف ***l, ثابت حرف *مقدار, ...) {
        va_list ap;
        size_t n, m;
        صحيح r;

        أكد(l);

        m = n = strv_درازی(*l);

        r = 0;
        va_start(ap, مقدار);
        لكل (ثابت حرف *s = مقدار; s != اشاره_گر_MAX; s = va_arg(ap, ثابت حرف*)) {
                إذا (!s)
                        استمر;

                إذا (m > SIZE_MAX-1) { /*سرریز*/
                        r = -ENOMEM;
                        اكسر;
                }
                m++;
        }
        va_end(ap);

        إذا (r < 0)
                أرجع r;
        إذا (m > SIZE_MAX-1)
                أرجع -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_دور_به_بالا(m+1), حجم(حرف*));
        إذا (!c)
                أرجع -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, مقدار);
        لكل (ثابت حرف *s = مقدار; s != اشاره_گر_MAX; s = va_arg(ap, ثابت حرف*)) {
                إذا (!s)
                        استمر;

                c[i] = strdup(s);
                إذا (!c[i]) {
                        r = -ENOMEM;
                        اكسر;
                }
                i++;
        }
        va_end(ap);

        إذا (r < 0) {
                /* rollback on error */
                لكل (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                أرجع r;
        }

        c[i] = عدم;
        أرجع 0;
}

حرف** strv_uniq(حرف **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        أرجع l;
}

bool strv_is_uniq(حرف * ثابت *l) {
        STRV_FOREACH(i, l)
                إذا (strv_شامل_است(i+1, *i))
                        أرجع false;

        أرجع true;
}

حرف** strv_remove(حرف **l, ثابت حرف *s) {
        إذا (!l)
                أرجع عدم;

        أكد(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        حرف **f, **t;
        لكل (f = t = l; *f; f++)
                إذا (streq(*f, s))
                        حرر(*f);
                وإلا
                        *(t++) = *f;

        *t = عدم;
        أرجع l;
}

حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll) {

        إذا (strv_isempty(l))
                أرجع l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        أرجع l;
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) {
        اشاره_گر_MAY_BE_NULL(a);
        اشاره_گر_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                إذا (strv_شامل_است(b, *i))
                        أرجع true;

        أرجع false;
}

ساكن صحيح str_compare(حرف * ثابت *a, حرف * ثابت *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشاره_گر_MAY_BE_NULL(a);
        اشاره_گر_MAY_BE_NULL(b);

        أرجع strcmp(*a, *b);
}

حرف** strv_مرتب_سازی(حرف **l) {
        typesafe_qsort(l, strv_درازی(l), str_compare);
        أرجع l;
}

حرف** strv_مرتب_سازی_uniq_پر(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b)) {
        أكد(compare);

        إذا (strv_isempty(l))
                أرجع l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_درازی(l), compare);

        حرف **دم_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                إذا (compare(i, دم_2 - 1) == 0)
                        حرر(*i);
                وإلا
                        *(دم_2++) = *i;

        *دم_2 = عدم;
        أرجع l;
}

حرف** strv_مرتب_سازی_uniq(حرف **l) {
        أرجع strv_مرتب_سازی_uniq_پر(l, str_compare);
}

صحيح strv_compare(حرف * ثابت *a, حرف * ثابت *b) {
        صحيح r;

        اشاره_گر_MAY_BE_NULL(a);
        اشاره_گر_MAY_BE_NULL(b);

        إذا (strv_isempty(a)) {
                إذا (strv_isempty(b))
                        أرجع 0;
                وإلا
                        أرجع -1;
        }

        إذا (strv_isempty(b))
                أرجع 1;

        لكل ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                إذا (r != 0)
                        أرجع r;
        }

        أرجع 0;
}

bool strv_equal_ignore_ترتیب(حرف * ثابت *a, حرف * ثابت *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        إذا (a == b)
                أرجع true;

        STRV_FOREACH(i, a)
                إذا (!strv_شامل_است(b, *i))
                        أرجع false;

        STRV_FOREACH(i, b)
                إذا (!strv_شامل_است(a, *i))
                        أرجع false;

        أرجع true;
}

فراغ strv_print_پر(حرف * ثابت *l, ثابت حرف *پیشوند) {
        STRV_FOREACH(s, l)
                چاپ_کن("%s%s\n", strempty(پیشوند), *s);
}

صحيح strv_extendf_with_اندازه(حرف ***l, size_t *n, ثابت حرف *format, ...) {
        va_list ap;
        حرف *x;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع -ENOMEM;

        أرجع strv_consume_with_اندازه(l, n, x);
}

صحيح strv_extend_joined_with_اندازه_sentinel(حرف ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        حرف *x = strextendv_with_separator(/* x= */ عدم, /* separator= */ عدم, ap);
        va_end(ap);
        إذا (!x)
                أرجع -ENOMEM;

        أرجع strv_consume_with_اندازه(l, n, x);
}

حرف* startswith_strv_داخلی(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *یافت = (حرف*) startswith(s, *i);
                إذا (یافت)
                        أرجع یافت;
        }

        أرجع عدم;
}

حرف* endswith_strv_داخلی(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *یافت = (حرف*) endswith(s, *i);
                إذا (یافت)
                        أرجع یافت;
        }

        أرجع عدم;
}

حرف** strv_معکوس(حرف **l) {
        size_t n;

        n = strv_درازی(l);
        إذا (n <= 1)
                أرجع l;

        لكل (size_t i = 0; i < n / 2; i++)
                جابجا_TWO(l[i], l[n-1-i]);

        أرجع l;
}

حرف** strv_shell_escape(حرف **l, ثابت حرف *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                حرف *v;

                v = shell_escape(*s, bad);
                إذا (!v)
                        أرجع عدم;

                free_and_replace(*s, v);
        }

        أرجع l;
}

bool strv_fnmatch_پر(
                حرف* ثابت* patterns,
                ثابت حرف *s,
                صحيح flags,
                size_t *ret_matched_pos) {

        أكد(s);

        إذا (patterns)
                لكل (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        إذا (fnmatch(patterns[i], s, flags) == 0) {
                                إذا (ret_matched_pos)
                                        *ret_matched_pos = i;
                                أرجع true;
                        }

        إذا (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        أرجع false;
}

حرف** strv_skip(حرف **l, size_t n) {
        طالما (n > 0) {
                إذا (strv_isempty(l))
                        أرجع عدم;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        إذا (strv_isempty(l))
                أرجع عدم;
        أرجع l;
}

صحيح strv_extend_n(حرف ***l, ثابت حرف *مقدار, size_t n) {
        size_t i, k;
        حرف **nl;

        أكد(l);

        إذا (!مقدار)
                أرجع 0;
        إذا (n == 0)
                أرجع 0;

        /* Adds the value n times to l */

        k = strv_درازی(*l);
        إذا (n >= SIZE_MAX - k)
                أرجع -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_دور_به_بالا(k + n + 1), حجم(حرف *));
        إذا (!nl)
                أرجع -ENOMEM;

        *l = nl;

        لكل (i = k; i < k + n; i++) {
                nl[i] = strdup(مقدار);
                إذا (!nl[i])
                        اذهب rollback;
        }
        nl[i] = عدم;

        أرجع 0;

rollback:
        لكل (size_t j = k; j < i; j++)
                حرر(nl[j]);
        nl[k] = عدم;

        أرجع -ENOMEM;
}

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs) {
        حرف *j;

        أكد(l);
        أكد(lhs);

        إذا (!rhs) /* value is optional, in which case we suppress the field */
                أرجع 0;

        j = strjoin(lhs, "=", rhs);
        إذا (!j)
                أرجع -ENOMEM;

        أرجع strv_consume(l, j);
}

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space) {
        bool b = false;
        صحيح r;

        أكد(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        إذا (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *کلید, ثابت حرف *مقدار) {
        أكد(کلید);

        إذا (مقدار) {
                حرف **l = hashmap_get(h, کلید);
                إذا (!l)
                        أرجع;

                strv_remove(l, مقدار);
                إذا (!strv_isempty(l))
                        أرجع;
        }

        استفادهنشده _cleanup_free_ حرف *کلید_free = عدم;
        strv_free(hashmap_remove2(h, کلید, (فراغ**) &کلید_free));
}

فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *کلید, ثابت حرف *مقدار) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), کلید, مقدار);
}

ساكن صحيح string_strv_hashmap_put_داخلی(Hashmap *h, ثابت حرف *کلید, ثابت حرف *مقدار) {
        حرف **l;
        صحيح r;

        أكد(h);
        أكد(کلید);
        أكد(مقدار);

        l = hashmap_get(h, کلید);
        إذا (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                إذا (strv_شامل_است(l, مقدار))
                        أرجع 0;

                r = strv_extend(&l, مقدار);
                إذا (r < 0)
                        أرجع r;

                assert_se(hashmap_update(h, کلید, l) >= 0);
        } وإلا {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ حرف **l2 = عدم;
                _cleanup_free_ حرف *t = عدم;

                t = strdup(کلید);
                إذا (!t)
                        أرجع -ENOMEM;

                r = strv_extend(&l2, مقدار);
                إذا (r < 0)
                        أرجع r;

                r = hashmap_put(h, t, l2);
                إذا (r < 0)
                        أرجع r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        أرجع 1;
}

صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *کلید, ثابت حرف *مقدار) {
        صحيح r;

        أكد(h);
        أكد(کلید);
        أكد(مقدار);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        إذا (r < 0)
                أرجع r;

        أرجع string_strv_hashmap_put_داخلی(*h, کلید, مقدار);
}

صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *کلید, ثابت حرف *مقدار) {
        صحيح r;

        أكد(h);
        أكد(کلید);
        أكد(مقدار);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        إذا (r < 0)
                أرجع r;

        أرجع string_strv_hashmap_put_داخلی(PLAIN_HASHMAP(*h), کلید, مقدار);
}

ساكن صحيح rebreak_flush_سطر(
                حرف ***broken,
                ثابت حرف *شروع,
                ثابت حرف *انتها,
                bool درون_پیشوند,
                ثابت حرف *whitespace_begin,
                ثابت حرف *whitespace_انتها) {

        أكد(broken);
        أكد(شروع);
        أكد(انتها >= شروع);

        إذا (درون_پیشوند) /* Never seen anything non-whitespace? Generate empty line! */
                أرجع strv_extend(broken, "");

        إذا (whitespace_begin && !whitespace_انتها) /* Ends in whitespace? Chop it off! */
                انتها = whitespace_begin;

        _cleanup_free_ حرف *truncated = strndup(شروع, انتها - شروع);
        إذا (!truncated)
                أرجع -ENOMEM;

        أرجع strv_consume(broken, TAKE_PTR(truncated));
}

صحيح strv_rebreak_سطرها(حرف **l, size_t width, حرف ***ret) {
        _cleanup_strv_free_ حرف **broken = عدم;
        صحيح r;

        أكد(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        إذا (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                إذا (!broken)
                        أرجع -ENOMEM;

                *ret = TAKE_PTR(broken);
                أرجع 0;
        }

        STRV_FOREACH(i, l) {
                ثابت حرف *شروع = *i, *whitespace_begin = عدم, *whitespace_انتها = عدم;
                bool درون_پیشوند = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                لكل (ثابت حرف *p = شروع; *p != 0; ) {
                        إذا (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_سطر(&broken, شروع, p, درون_پیشوند, whitespace_begin, whitespace_انتها);
                                إذا (r < 0)
                                        أرجع r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                شروع = p;
                                درون_پیشوند = true;
                                after_newline = true;
                                whitespace_begin = whitespace_انتها = عدم;
                                w = 0;
                                استمر;
                        } وإلا إذا (strchr(WHITESPACE, *p)) {
                                إذا (!درون_پیشوند && (!whitespace_begin || whitespace_انتها)) {
                                        whitespace_begin = p;
                                        whitespace_انتها = عدم;
                                }
                        } وإلا {
                                إذا (whitespace_begin && !whitespace_انتها)
                                        whitespace_انتها = p;

                                درون_پیشوند = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        صحيح cw = utf8_char_console_عرض(p);
                        إذا (cw < 0)
                                أرجع log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        إذا (w > width && whitespace_begin && whitespace_انتها) {
                                _cleanup_free_ حرف *truncated = عدم;

                                truncated = strndup(شروع, whitespace_begin - شروع);
                                إذا (!truncated)
                                        أرجع -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                إذا (r < 0)
                                        أرجع r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = شروع = whitespace_انتها;
                                after_newline = false;
                                whitespace_begin = whitespace_انتها = عدم;
                                w = 0;
                                استمر;
                        }

                        p = utf8_بعدی_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                أكد(شروع);
                إذا (after_newline && درون_پیشوند)
                        استمر;

                r = rebreak_flush_سطر(&broken, شروع, شروع + طول_رشته(شروع), درون_پیشوند, whitespace_begin, whitespace_انتها);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(broken);
        أرجع 0;
}

حرف** strv_filter_پیشوند(حرف * ثابت *l, ثابت حرف *پیشوند) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        إذا (isempty(پیشوند))
                أرجع strv_copy(l);

        _cleanup_strv_free_ حرف **f = عدم;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                إذا (!startswith(*i, پیشوند))
                        استمر;

                إذا (strv_extend_with_اندازه(&f, &sz, *i) < 0)
                        أرجع عدم;
        }

        أرجع TAKE_PTR(f);
}

ثابت حرف* ثابت strv_خالی[] = { عدم };
