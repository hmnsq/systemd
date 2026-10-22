/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fnmatch.h>
#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "env-util.h"
#شامل "escape.h"
#شامل "extract-word.h"
#شامل "fileio.h"
#شامل "gunicode.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "sort-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"

حرف* strv_ڈھونڈو(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                اگر (streq(*i, name))
                        واپس *i;

        واپس تہی;
}

حرف* strv_ڈھونڈو_case(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                اگر (strcaseeq(*i, name))
                        واپس *i;

        واپس تہی;
}

حرف* strv_ڈھونڈو_سابقہ(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                اگر (startswith(*i, name))
                        واپس *i;

        واپس تہی;
}

حرف* strv_ڈھونڈو_startswith(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                حرف *e;

                e = startswith(*i, name);
                اگر (e)
                        واپس e;
        }

        واپس تہی;
}

ساکن حرف* strv_ڈھونڈو_closest_سابقہ(حرف * ثابت *l, ثابت حرف *name) {
        size_t بہترین_فاصلہ = SIZE_MAX;
        حرف *بہترین = تہی;

        أكد(name);

        STRV_FOREACH(s, l) {
                حرف *e = startswith(*s, name);
                اگر (!e)
                        جاری;

                size_t n = لڑی_لمبائی(e);
                اگر (n < بہترین_فاصلہ) {
                        بہترین_فاصلہ = n;
                        بہترین = *s;
                }
        }

        واپس بہترین;
}

ساکن حرف* strv_ڈھونڈو_closest_by_levenshtein(حرف * ثابت *l, ثابت حرف *name) {
        ssize_t بہترین_فاصلہ = SSIZE_MAX;
        حرف *بہترین = تہی;

        أكد(name);

        STRV_FOREACH(i, l) {
                ssize_t فاصلہ;

                فاصلہ = strlevenshtein(*i, name);
                اگر (فاصلہ < 0) {
                        log_debug_errno(فاصلہ, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        واپس تہی;
                }

                اگر (فاصلہ > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        جاری;

                اگر (فاصلہ < بہترین_فاصلہ) {
                        بہترین_فاصلہ = فاصلہ;
                        بہترین = *i;
                }
        }

        واپس بہترین;
}

حرف* strv_ڈھونڈو_closest(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        حرف *ملا = strv_ڈھونڈو_closest_سابقہ(l, name);
        اگر (ملا)
                واپس ملا;

        واپس strv_ڈھونڈو_closest_by_levenshtein(l, name);
}

حرف* strv_ڈھونڈو_پہلا_میدان(حرف * ثابت *needles, حرف * ثابت *haystack) {
        STRV_FOREACH(k, needles) {
                حرف *قدر = strv_env_جوڑے_get((حرف **)haystack, *k);
                اگر (قدر)
                        واپس قدر;
        }

        واپس تہی;
}

حرف** strv_free(حرف **l) {
        STRV_FOREACH(k, l)
                آزاد(*k);

        واپس mfree(l);
}

حرف** strv_free_erase(حرف **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        واپس mfree(l);
}

خالی strv_free_many(حرف ***strvs, size_t n) {
        أكد(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        آزاد(strvs);
}

حرف** strv_copy_n(حرف * ثابت *l, size_t n) {
        _cleanup_strv_free_ حرف **نتیجہ = تہی;
        حرف **k;

        نتیجہ = new(حرف*, MIN(strv_لمبائی(l), n) + 1);
        اگر (!نتیجہ)
                واپس تہی;

        k = نتیجہ;
        STRV_FOREACH(i, l) {
                اگر (n == 0)
                        توڑو;

                *k = strdup(*i);
                اگر (!*k)
                        واپس تہی;
                k++;

                اگر (n != SIZE_MAX)
                        n--;
        }

        *k = تہی;
        واپس TAKE_PTR(نتیجہ);
}

صحیح strv_copy_unless_خالی_خانے(حرف * ثابت *l, حرف ***ret) {
        أكد(ret);

        اگر (strv_isempty(l)) {
                *ret = تہی;
                واپس 0;
        }

        حرف **copy = strv_copy(l);
        اگر (!copy)
                واپس -ENOMEM;

        *ret = TAKE_PTR(copy);
        واپس 1;
}

size_t strv_لمبائی(حرف * ثابت *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        واپس n;
}

حرف** strv_new_ap(ثابت حرف *x, va_list ap) {
        _cleanup_strv_free_ حرف **a = تہی;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        برائے (ثابت حرف *s = x; s; s = va_arg(aq, ثابت حرف*)) {
                اگر (s == STRV_IGNORE)
                        جاری;

                n++;
        }
        va_end(aq);

        a = new(حرف*, n+1);
        اگر (!a)
                واپس تہی;

        برائے (ثابت حرف *s = x; s; s = va_arg(ap, ثابت حرف*)) {
                اگر (s == STRV_IGNORE)
                        جاری;

                a[i] = strdup(s);
                اگر (!a[i])
                        واپس تہی;

                i++;
        }

        a[i] = تہی;

        واپس TAKE_PTR(a);
}

حرف** strv_new_اندرونی(ثابت حرف *x, ...) {
        حرف **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        واپس r;
}

صحیح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        أكد(a);

        q = strv_لمبائی(b);
        اگر (q == 0)
                واپس 0;

        p = strv_لمبائی(*a);
        اگر (p >= SIZE_MAX - q)
                واپس -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_باری_اوپر(p + q + 1), حجم(حرف *));
        اگر (!t)
                واپس -ENOMEM;

        t[p] = تہی;
        *a = t;

        STRV_FOREACH(s, b) {
                اگر (filter_duplicates && strv_شامل_ہے(t, *s))
                        جاری;

                t[p+i] = strdup(*s);
                اگر (!t[p+i])
                        جاؤ rollback;

                i++;
                t[p+i] = تہی;
        }

        أكد(i <= q);

        واپس (صحیح) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = تہی;
        واپس -ENOMEM;
}

صحیح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates) {
        _cleanup_strv_free_ حرف **b_consume = b;
        size_t p, q, i;

        أكد(a);

        q = strv_لمبائی(b);
        اگر (q == 0)
                واپس 0;

        p = strv_لمبائی(*a);
        اگر (p == 0) {
                strv_free_and_replace(*a, b_consume);

                اگر (filter_duplicates)
                        strv_uniq(*a);

                واپس strv_لمبائی(*a);
        }

        اگر (p >= SIZE_MAX - q)
                واپس -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_باری_اوپر(p + q + 1), حجم(حرف *));
        اگر (!t)
                واپس -ENOMEM;

        t[p] = تہی;
        *a = t;

        اگر (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = تہی;
                i = q;
        } ورنہ {
                i = 0;

                STRV_FOREACH(s, b) {
                        اگر (strv_شامل_ہے(t, *s)) {
                                آزاد(*s);
                                جاری;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = تہی;
                }
        }

        أكد(i <= q);

        b_consume = mfree(b_consume);

        واپس (صحیح) i;
}

صحیح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *سابقہ, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        صحیح r;

        أكد(a);

        STRV_FOREACH(s, b) {
                حرف *v;

                v = strjoin(strempty(سابقہ), *s, suffix);
                اگر (!v)
                        واپس -ENOMEM;

                r = strv_consume(a, v);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

صحیح strv_بانٹو_newlines_بھرا(حرف ***ret, ثابت حرف *s, نکالو_Flags flags) {
        _cleanup_strv_free_ حرف **l = تہی;
        size_t n;
        صحیح r;

        أكد(ret);
        أكد(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_بانٹو_بھرا(&l, s, NEWLINE, flags);
        اگر (r < 0)
                واپس r;

        n = strv_لمبائی(l);
        اگر (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        واپس n;
}

حرف** strv_بانٹو_newlines(ثابت حرف *s) {
        حرف **ret;

        اگر (strv_بانٹو_newlines_بھرا(&ret, s, 0) < 0)
                واپس تہی;

        واپس ret;
}

صحیح strv_بانٹو_بھرا(حرف ***t, ثابت حرف *s, ثابت حرف *separators, نکالو_Flags flags) {
        _cleanup_strv_free_ حرف **l = تہی;
        size_t n = 0;
        صحیح r;

        أكد(t);
        أكد(s);

        برائے (;;) {
                _cleanup_free_ حرف *لفظ = تہی;

                r = نکالو_پہلا_لفظ(&s, &لفظ, separators, flags);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                اگر (!GREEDY_REALLOC(l, n + 2))
                        واپس -ENOMEM;

                l[n++] = TAKE_PTR(لفظ);
                l[n] = تہی;
        }

        اگر (!l) {
                l = new0(حرف*, 1);
                اگر (!l)
                        واپس -ENOMEM;
        }

        *t = TAKE_PTR(l);

        واپس (صحیح) n;
}

حرف** strv_بانٹو(ثابت حرف *s, ثابت حرف *separators) {
        حرف **ret;

        اگر (strv_بانٹو_بھرا(&ret, s, separators, نکالو_RETAIN_ESCAPE) < 0)
                واپس تہی;

        واپس ret;
}

صحیح strv_بانٹو_and_extend_بھرا(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, نکالو_Flags flags) {
        حرف **l;
        صحیح r;

        أكد(t);
        أكد(s);

        r = strv_بانٹو_بھرا(&l, s, separators, flags);
        اگر (r < 0)
                واپس r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        اگر (r < 0)
                واپس r;

        واپس (صحیح) strv_لمبائی(*t);
}

صحیح strv_بانٹو_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates) {
        واپس strv_بانٹو_and_extend_بھرا(t, s, separators, filter_duplicates, 0);
}

صحیح strv_بانٹو_colon_جوڑے(حرف ***t, ثابت حرف *s) {
        _cleanup_strv_free_ حرف **l = تہی;
        size_t n = 0;
        صحیح r;

        أكد(t);
        أكد(s);

        برائے (;;) {
                _cleanup_free_ حرف *پہلا = تہی, *دوسرا = تہی, *tuple = تہی, *دوسرا_or_خالی_خانے = تہی;

                r = نکالو_پہلا_لفظ(&s, &tuple, تہی, نکالو_UNQUOTE|نکالو_RETAIN_ESCAPE);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                ثابت حرف *p = tuple;
                r = نکالو_many_الفاظ(&p, ":", نکالو_CUNESCAPE|نکالو_UNESCAPE_SEPARATORS,
                                       &پہلا, &دوسرا);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        جاری;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                اگر (!isempty(p))
                        واپس -EINVAL;

                دوسرا_or_خالی_خانے = strdup(strempty(دوسرا));
                اگر (!دوسرا_or_خالی_خانے)
                        واپس -ENOMEM;

                اگر (!GREEDY_REALLOC(l, n + 3))
                        واپس -ENOMEM;

                l[n++] = TAKE_PTR(پہلا);
                l[n++] = TAKE_PTR(دوسرا_or_خالی_خانے);

                l[n] = تہی;
        }

        اگر (!l) {
                l = new0(حرف*, 1);
                اگر (!l)
                        واپس -ENOMEM;
        }

        *t = TAKE_PTR(l);

        واپس (صحیح) n;
}

حرف* strv_join_بھرا(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *سابقہ) {
        حرف *r, *e;
        size_t n, k, m;

        اگر (!separator)
                separator = " ";

        k = لڑی_لمبائی(separator);
        m = strlen_ptr(سابقہ);

        n = 0;
        STRV_FOREACH(s, l) {
                اگر (s != l)
                        n += k;

                n += m + لڑی_لمبائی(*s);
        }

        r = new(حرف, n+1);
        اگر (!r)
                واپس تہی;

        e = r;
        STRV_FOREACH(s, l) {
                اگر (s != l)
                        e = stpcpy(e, separator);

                اگر (سابقہ)
                        e = stpcpy(e, سابقہ);

                e = stpcpy(e, *s);
        }

        *e = 0;

        واپس r;
}

صحیح strv_ڈالو_with_سائز(حرف ***l, size_t *n, حرف *قدر) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        أكد(l);
        اشارہ_گر_MAY_BE_NULL(n);

        اگر (!قدر)
                واپس 0;

        size_t سائز = n ? *n : SIZE_MAX;
        اگر (سائز == SIZE_MAX)
                سائز = strv_لمبائی(*l);

        /* Check for overflow */
        اگر (سائز > SIZE_MAX-2)
                واپس -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_باری_اوپر(سائز + 2), حجم(حرف*));
        اگر (!c)
                واپس -ENOMEM;

        c[سائز] = قدر;
        c[سائز+1] = تہی;

        *l = c;
        اگر (n)
                *n = سائز + 1;
        واپس 0;
}

صحیح strv_ڈالو_pair(حرف ***l, حرف *a, حرف *b) {
        حرف **c;
        size_t n;

        أكد(l);

        اگر (!a && !b)
                واپس 0;

        n = strv_لمبائی(*l);

        /* Check for overflow */
        اگر (n > SIZE_MAX-3)
                واپس -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_باری_اوپر(n + !!a + !!b + 1), حجم(حرف*));
        اگر (!c)
                واپس -ENOMEM;

        اگر (a)
                c[n++] = a;
        اگر (b)
                c[n++] = b;
        c[n] = تہی;

        *l = c;
        واپس 0;
}

صحیح strv_داخل_کرو(حرف ***l, size_t position, حرف *قدر) {
        حرف **c;
        size_t n, m;

        أكد(l);

        اگر (!قدر)
                واپس 0;

        n = strv_لمبائی(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        اگر (n > SIZE_MAX - 2)
                واپس -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_باری_اوپر(m), حجم(حرف*));
        اگر (!c)
                واپس -ENOMEM;

        اگر (n > position)
                memmove(c + position + 1, c + position, (n - position) * حجم(حرف*));

        c[position] = قدر;
        c[n + 1] = تہی;

        *l = c;
        واپس 0;
}

صحیح strv_consume_with_سائز(حرف ***l, size_t *n, حرف *قدر) {
        صحیح r;

        r = strv_ڈالو_with_سائز(l, n, قدر);
        اگر (r < 0)
                آزاد(قدر);

        واپس r;
}

صحیح strv_consume_pair(حرف ***l, حرف *a, حرف *b) {
        صحیح r;

        r = strv_ڈالو_pair(l, a, b);
        اگر (r < 0) {
                آزاد(a);
                آزاد(b);
        }

        واپس r;
}

صحیح strv_consume_prepend(حرف ***l, حرف *قدر) {
        صحیح r;

        r = strv_ڈالو_prepend(l, قدر);
        اگر (r < 0)
                آزاد(قدر);

        واپس r;
}

صحیح strv_prepend(حرف ***l, ثابت حرف *قدر) {
        حرف *v;

        اگر (!قدر)
                واپس 0;

        v = strdup(قدر);
        اگر (!v)
                واپس -ENOMEM;

        واپس strv_consume_prepend(l, v);
}

صحیح strv_extend_with_سائز(حرف ***l, size_t *n, ثابت حرف *قدر) {
        حرف *v;

        اگر (!قدر)
                واپس 0;

        v = strdup(قدر);
        اگر (!v)
                واپس -ENOMEM;

        واپس strv_consume_with_سائز(l, n, v);
}

صحیح strv_extend_many_اندرونی(حرف ***l, ثابت حرف *قدر, ...) {
        va_list ap;
        size_t n, m;
        صحیح r;

        أكد(l);

        m = n = strv_لمبائی(*l);

        r = 0;
        va_start(ap, قدر);
        برائے (ثابت حرف *s = قدر; s != اشارہ_گر_MAX; s = va_arg(ap, ثابت حرف*)) {
                اگر (!s)
                        جاری;

                اگر (m > SIZE_MAX-1) { /*حد_سے_تجاوز*/
                        r = -ENOMEM;
                        توڑو;
                }
                m++;
        }
        va_end(ap);

        اگر (r < 0)
                واپس r;
        اگر (m > SIZE_MAX-1)
                واپس -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_باری_اوپر(m+1), حجم(حرف*));
        اگر (!c)
                واپس -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, قدر);
        برائے (ثابت حرف *s = قدر; s != اشارہ_گر_MAX; s = va_arg(ap, ثابت حرف*)) {
                اگر (!s)
                        جاری;

                c[i] = strdup(s);
                اگر (!c[i]) {
                        r = -ENOMEM;
                        توڑو;
                }
                i++;
        }
        va_end(ap);

        اگر (r < 0) {
                /* rollback on error */
                برائے (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                واپس r;
        }

        c[i] = تہی;
        واپس 0;
}

حرف** strv_uniq(حرف **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        واپس l;
}

bool strv_is_uniq(حرف * ثابت *l) {
        STRV_FOREACH(i, l)
                اگر (strv_شامل_ہے(i+1, *i))
                        واپس false;

        واپس true;
}

حرف** strv_remove(حرف **l, ثابت حرف *s) {
        اگر (!l)
                واپس تہی;

        أكد(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        حرف **f, **t;
        برائے (f = t = l; *f; f++)
                اگر (streq(*f, s))
                        آزاد(*f);
                ورنہ
                        *(t++) = *f;

        *t = تہی;
        واپس l;
}

حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll) {

        اگر (strv_isempty(l))
                واپس l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        واپس l;
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) {
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                اگر (strv_شامل_ہے(b, *i))
                        واپس true;

        واپس false;
}

ساکن صحیح str_compare(حرف * ثابت *a, حرف * ثابت *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        واپس strcmp(*a, *b);
}

حرف** strv_ترتیب_دو(حرف **l) {
        typesafe_qsort(l, strv_لمبائی(l), str_compare);
        واپس l;
}

حرف** strv_ترتیب_دو_uniq_بھرا(حرف **l, صحیح (*compare)(حرف * ثابت *a, حرف * ثابت *b)) {
        أكد(compare);

        اگر (strv_isempty(l))
                واپس l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_لمبائی(l), compare);

        حرف **دم_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                اگر (compare(i, دم_2 - 1) == 0)
                        آزاد(*i);
                ورنہ
                        *(دم_2++) = *i;

        *دم_2 = تہی;
        واپس l;
}

حرف** strv_ترتیب_دو_uniq(حرف **l) {
        واپس strv_ترتیب_دو_uniq_بھرا(l, str_compare);
}

صحیح strv_compare(حرف * ثابت *a, حرف * ثابت *b) {
        صحیح r;

        اشارہ_گر_MAY_BE_NULL(a);
        اشارہ_گر_MAY_BE_NULL(b);

        اگر (strv_isempty(a)) {
                اگر (strv_isempty(b))
                        واپس 0;
                ورنہ
                        واپس -1;
        }

        اگر (strv_isempty(b))
                واپس 1;

        برائے ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                اگر (r != 0)
                        واپس r;
        }

        واپس 0;
}

bool strv_equal_ignore_ترتیب(حرف * ثابت *a, حرف * ثابت *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        اگر (a == b)
                واپس true;

        STRV_FOREACH(i, a)
                اگر (!strv_شامل_ہے(b, *i))
                        واپس false;

        STRV_FOREACH(i, b)
                اگر (!strv_شامل_ہے(a, *i))
                        واپس false;

        واپس true;
}

خالی strv_print_بھرا(حرف * ثابت *l, ثابت حرف *سابقہ) {
        STRV_FOREACH(s, l)
                لکھو("%s%s\n", strempty(سابقہ), *s);
}

صحیح strv_extendf_with_سائز(حرف ***l, size_t *n, ثابت حرف *format, ...) {
        va_list ap;
        حرف *x;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        اگر (r < 0)
                واپس -ENOMEM;

        واپس strv_consume_with_سائز(l, n, x);
}

صحیح strv_extend_joined_with_سائز_sentinel(حرف ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        حرف *x = strextendv_with_separator(/* x= */ تہی, /* separator= */ تہی, ap);
        va_end(ap);
        اگر (!x)
                واپس -ENOMEM;

        واپس strv_consume_with_سائز(l, n, x);
}

حرف* startswith_strv_اندرونی(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *ملا = (حرف*) startswith(s, *i);
                اگر (ملا)
                        واپس ملا;
        }

        واپس تہی;
}

حرف* endswith_strv_اندرونی(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *ملا = (حرف*) endswith(s, *i);
                اگر (ملا)
                        واپس ملا;
        }

        واپس تہی;
}

حرف** strv_الٹ(حرف **l) {
        size_t n;

        n = strv_لمبائی(l);
        اگر (n <= 1)
                واپس l;

        برائے (size_t i = 0; i < n / 2; i++)
                تبادلہ_TWO(l[i], l[n-1-i]);

        واپس l;
}

حرف** strv_shell_escape(حرف **l, ثابت حرف *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                حرف *v;

                v = shell_escape(*s, bad);
                اگر (!v)
                        واپس تہی;

                free_and_replace(*s, v);
        }

        واپس l;
}

bool strv_fnmatch_بھرا(
                حرف* ثابت* patterns,
                ثابت حرف *s,
                صحیح flags,
                size_t *ret_matched_pos) {

        أكد(s);

        اگر (patterns)
                برائے (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        اگر (fnmatch(patterns[i], s, flags) == 0) {
                                اگر (ret_matched_pos)
                                        *ret_matched_pos = i;
                                واپس true;
                        }

        اگر (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        واپس false;
}

حرف** strv_skip(حرف **l, size_t n) {
        جب (n > 0) {
                اگر (strv_isempty(l))
                        واپس تہی;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        اگر (strv_isempty(l))
                واپس تہی;
        واپس l;
}

صحیح strv_extend_n(حرف ***l, ثابت حرف *قدر, size_t n) {
        size_t i, k;
        حرف **nl;

        أكد(l);

        اگر (!قدر)
                واپس 0;
        اگر (n == 0)
                واپس 0;

        /* Adds the value n times to l */

        k = strv_لمبائی(*l);
        اگر (n >= SIZE_MAX - k)
                واپس -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_باری_اوپر(k + n + 1), حجم(حرف *));
        اگر (!nl)
                واپس -ENOMEM;

        *l = nl;

        برائے (i = k; i < k + n; i++) {
                nl[i] = strdup(قدر);
                اگر (!nl[i])
                        جاؤ rollback;
        }
        nl[i] = تہی;

        واپس 0;

rollback:
        برائے (size_t j = k; j < i; j++)
                آزاد(nl[j]);
        nl[k] = تہی;

        واپس -ENOMEM;
}

صحیح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs) {
        حرف *j;

        أكد(l);
        أكد(lhs);

        اگر (!rhs) /* value is optional, in which case we suppress the field */
                واپس 0;

        j = strjoin(lhs, "=", rhs);
        اگر (!j)
                واپس -ENOMEM;

        واپس strv_consume(l, j);
}

صحیح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space) {
        bool b = false;
        صحیح r;

        أكد(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        اگر (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

خالی string_strv_hashmap_remove(Hashmap *h, ثابت حرف *کلید, ثابت حرف *قدر) {
        أكد(کلید);

        اگر (قدر) {
                حرف **l = hashmap_get(h, کلید);
                اگر (!l)
                        واپس;

                strv_remove(l, قدر);
                اگر (!strv_isempty(l))
                        واپس;
        }

        غیر_مستعمل _cleanup_free_ حرف *کلید_free = تہی;
        strv_free(hashmap_remove2(h, کلید, (خالی**) &کلید_free));
}

خالی string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *کلید, ثابت حرف *قدر) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), کلید, قدر);
}

ساکن صحیح string_strv_hashmap_put_اندرونی(Hashmap *h, ثابت حرف *کلید, ثابت حرف *قدر) {
        حرف **l;
        صحیح r;

        أكد(h);
        أكد(کلید);
        أكد(قدر);

        l = hashmap_get(h, کلید);
        اگر (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                اگر (strv_شامل_ہے(l, قدر))
                        واپس 0;

                r = strv_extend(&l, قدر);
                اگر (r < 0)
                        واپس r;

                assert_se(hashmap_update(h, کلید, l) >= 0);
        } ورنہ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ حرف **l2 = تہی;
                _cleanup_free_ حرف *t = تہی;

                t = strdup(کلید);
                اگر (!t)
                        واپس -ENOMEM;

                r = strv_extend(&l2, قدر);
                اگر (r < 0)
                        واپس r;

                r = hashmap_put(h, t, l2);
                اگر (r < 0)
                        واپس r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        واپس 1;
}

صحیح string_strv_hashmap_put(Hashmap **h, ثابت حرف *کلید, ثابت حرف *قدر) {
        صحیح r;

        أكد(h);
        أكد(کلید);
        أكد(قدر);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        اگر (r < 0)
                واپس r;

        واپس string_strv_hashmap_put_اندرونی(*h, کلید, قدر);
}

صحیح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *کلید, ثابت حرف *قدر) {
        صحیح r;

        أكد(h);
        أكد(کلید);
        أكد(قدر);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        اگر (r < 0)
                واپس r;

        واپس string_strv_hashmap_put_اندرونی(PLAIN_HASHMAP(*h), کلید, قدر);
}

ساکن صحیح rebreak_flush_سطر(
                حرف ***broken,
                ثابت حرف *آغاز,
                ثابت حرف *انتہا,
                bool اندر_سابقہ,
                ثابت حرف *whitespace_begin,
                ثابت حرف *whitespace_انتہا) {

        أكد(broken);
        أكد(آغاز);
        أكد(انتہا >= آغاز);

        اگر (اندر_سابقہ) /* Never seen anything non-whitespace? Generate empty line! */
                واپس strv_extend(broken, "");

        اگر (whitespace_begin && !whitespace_انتہا) /* Ends in whitespace? Chop it off! */
                انتہا = whitespace_begin;

        _cleanup_free_ حرف *truncated = strndup(آغاز, انتہا - آغاز);
        اگر (!truncated)
                واپس -ENOMEM;

        واپس strv_consume(broken, TAKE_PTR(truncated));
}

صحیح strv_rebreak_سطریں(حرف **l, size_t width, حرف ***ret) {
        _cleanup_strv_free_ حرف **broken = تہی;
        صحیح r;

        أكد(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        اگر (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                اگر (!broken)
                        واپس -ENOMEM;

                *ret = TAKE_PTR(broken);
                واپس 0;
        }

        STRV_FOREACH(i, l) {
                ثابت حرف *آغاز = *i, *whitespace_begin = تہی, *whitespace_انتہا = تہی;
                bool اندر_سابقہ = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                برائے (ثابت حرف *p = آغاز; *p != 0; ) {
                        اگر (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_سطر(&broken, آغاز, p, اندر_سابقہ, whitespace_begin, whitespace_انتہا);
                                اگر (r < 0)
                                        واپس r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                آغاز = p;
                                اندر_سابقہ = true;
                                after_newline = true;
                                whitespace_begin = whitespace_انتہا = تہی;
                                w = 0;
                                جاری;
                        } ورنہ اگر (strchr(WHITESPACE, *p)) {
                                اگر (!اندر_سابقہ && (!whitespace_begin || whitespace_انتہا)) {
                                        whitespace_begin = p;
                                        whitespace_انتہا = تہی;
                                }
                        } ورنہ {
                                اگر (whitespace_begin && !whitespace_انتہا)
                                        whitespace_انتہا = p;

                                اندر_سابقہ = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        صحیح cw = utf8_char_console_چوڑائی(p);
                        اگر (cw < 0)
                                واپس log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        اگر (w > width && whitespace_begin && whitespace_انتہا) {
                                _cleanup_free_ حرف *truncated = تہی;

                                truncated = strndup(آغاز, whitespace_begin - آغاز);
                                اگر (!truncated)
                                        واپس -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                اگر (r < 0)
                                        واپس r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = آغاز = whitespace_انتہا;
                                after_newline = false;
                                whitespace_begin = whitespace_انتہا = تہی;
                                w = 0;
                                جاری;
                        }

                        p = utf8_اگلا_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                أكد(آغاز);
                اگر (after_newline && اندر_سابقہ)
                        جاری;

                r = rebreak_flush_سطر(&broken, آغاز, آغاز + لڑی_لمبائی(آغاز), اندر_سابقہ, whitespace_begin, whitespace_انتہا);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(broken);
        واپس 0;
}

حرف** strv_filter_سابقہ(حرف * ثابت *l, ثابت حرف *سابقہ) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        اگر (isempty(سابقہ))
                واپس strv_copy(l);

        _cleanup_strv_free_ حرف **f = تہی;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                اگر (!startswith(*i, سابقہ))
                        جاری;

                اگر (strv_extend_with_سائز(&f, &sz, *i) < 0)
                        واپس تہی;
        }

        واپس TAKE_PTR(f);
}

ثابت حرف* ثابت strv_خالی_خانے[] = { تہی };
