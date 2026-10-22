/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Priority Queue
 * The prioq object implements a priority queue. That is, it orders objects by
 * their priority and allows O(1) access to the object with the highest
 * priority. Insertion and removal are Θ(log n). Optionally, the caller can
 * provide a pointer to an index which will be kept up-to-date by the prioq.
 *
 * The underlying algorithm used in this implementation is a Heap.
 */

#ضمن "alloc-util.h"
#ضمن "prioq.h"

بنية prioq_شے {
        فراغ *انکڑے;
        بنانشانی *idx;
};

بنية Prioq {
        compare_func_t compare_func;
        بنانشانی n_اشیاء;
        بنية prioq_شے *اشیاء;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        إذا (!q)
                أرجع q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        أرجع q;
}

Prioq* prioq_free(Prioq *q) {
        إذا (!q)
                أرجع عدم;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(شے, q->اشیاء, q->n_اشیاء)
                إذا (شے->idx)
                        *(شے->idx) = PRIOQ_IDX_NULL;

        حرر(q->اشیاء);
        أرجع mfree(q);
}

صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        أكد(q);

        إذا (*q) {
                أكد((*q)->compare_func == compare_func);
                أرجع 0;
        }

        *q = prioq_new(compare_func);
        إذا (!*q)
                أرجع -ENOMEM;

        أرجع 0;
}

ساكن فراغ وٹاندرا(Prioq *q, بنانشانی j, بنانشانی k) {
        أكد(q);
        أكد(j < q->n_اشیاء);
        أكد(k < q->n_اشیاء);

        أكد(!q->اشیاء[j].idx || *(q->اشیاء[j].idx) == j);
        أكد(!q->اشیاء[k].idx || *(q->اشیاء[k].idx) == k);

        وٹاندرا_TWO(q->اشیاء[j].انکڑے, q->اشیاء[k].انکڑے);
        وٹاندرا_TWO(q->اشیاء[j].idx, q->اشیاء[k].idx);

        إذا (q->اشیاء[j].idx)
                *q->اشیاء[j].idx = j;

        إذا (q->اشیاء[k].idx)
                *q->اشیاء[k].idx = k;
}

ساكن بنانشانی shuffle_اتے(Prioq *q, بنانشانی idx) {
        أكد(q);
        أكد(idx < q->n_اشیاء);

        طالما (idx > 0) {
                بنانشانی k;

                k = (idx-1)/2;

                إذا (q->compare_func(q->اشیاء[k].انکڑے, q->اشیاء[idx].انکڑے) <= 0)
                        اكسر;

                وٹاندرا(q, idx, k);
                idx = k;
        }

        أرجع idx;
}

ساكن بنانشانی shuffle_تھلے(Prioq *q, بنانشانی idx) {
        أكد(q);

        لكل (;;) {
                بنانشانی j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                إذا (j >= q->n_اشیاء)
                        اكسر;

                إذا (q->compare_func(q->اشیاء[j].انکڑے, q->اشیاء[idx].انکڑے) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وإلا
                        s = idx;

                إذا (k < q->n_اشیاء &&
                    q->compare_func(q->اشیاء[k].انکڑے, q->اشیاء[s].انکڑے) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                إذا (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                وٹاندرا(q, idx, s);
                idx = s;
        }

        أرجع idx;
}

صحيح prioq_put(Prioq *q, فراغ *انکڑے, بنانشانی *idx) {
        بنانشانی k;

        أكد(q);

        إذا (!GREEDY_REALLOC(q->اشیاء, MAX(q->n_اشیاء + 1, 16u)))
                أرجع -ENOMEM;

        k = q->n_اشیاء++;
        q->اشیاء[k] = (بنية prioq_شے) {
                .انکڑے = انکڑے,
                .idx = idx,
        };

        إذا (idx)
                *idx = k;

        shuffle_اتے(q, k);

        أرجع 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *انکڑے, بنانشانی *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        إذا (r < 0)
                أرجع r;

        أرجع prioq_put(*q, انکڑے, idx);
}

ساكن فراغ remove_شے(Prioq *q, بنية prioq_شے *i) {
        بنية prioq_شے *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        إذا (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->اشیاء + q->n_اشیاء - 1;

        إذا (i == l)
                /* Last entry, let's just remove it */
                q->n_اشیاء--;
        وإلا {
                بنانشانی k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->اشیاء);
                k = i - q->اشیاء;

                i->انکڑے = l->انکڑے;
                i->idx = l->idx;
                إذا (i->idx)
                        *i->idx = k;
                q->n_اشیاء--;

                k = shuffle_تھلے(q, k);
                shuffle_اتے(q, k);
        }
}

ساكن بنية prioq_شے* لبھو_شے(Prioq *q, فراغ *انکڑے, بنانشانی *idx) {
        بنية prioq_شے *i;

        أكد(q);

        إذا (q->n_اشیاء <= 0)
                أرجع عدم;

        إذا (idx) {
                إذا (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_اشیاء)
                        أرجع عدم;

                i = q->اشیاء + *idx;
                إذا (i->انکڑے != انکڑے)
                        أرجع عدم;

                أرجع i;
        } وإلا {
                لكل (i = q->اشیاء; i < q->اشیاء + q->n_اشیاء; i++)
                        إذا (i->انکڑے == انکڑے)
                                أرجع i;
                أرجع عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *انکڑے, بنانشانی *idx) {
        بنية prioq_شے *i;

        إذا (!q)
                أرجع 0;

        i = لبھو_شے(q, انکڑے, idx);
        إذا (!i)
                أرجع 0;

        remove_شے(q, i);
        أرجع 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *انکڑے, بنانشانی *idx) {
        بنية prioq_شے *i;
        بنانشانی k;

        أكد(q);

        i = لبھو_شے(q, انکڑے, idx);
        إذا (!i)
                أرجع;

        أكد(i >= q->اشیاء);
        k = i - q->اشیاء;
        k = shuffle_تھلے(q, k);
        shuffle_اتے(q, k);
}

فراغ* prioq_peek_by_اشاریہ(Prioq *q, بنانشانی idx) {
        إذا (!q)
                أرجع عدم;

        إذا (idx >= q->n_اشیاء)
                أرجع عدم;

        أرجع q->اشیاء[idx].انکڑے;
}

فراغ* prioq_نکالو(Prioq *q) {
        فراغ *انکڑے;

        إذا (!q)
                أرجع عدم;

        إذا (q->n_اشیاء <= 0)
                أرجع عدم;

        انکڑے = q->اشیاء[0].انکڑے;
        remove_شے(q, q->اشیاء);
        أرجع انکڑے;
}

بنانشانی prioq_ناپ(Prioq *q) {

        إذا (!q)
                أرجع 0;

        أرجع q->n_اشیاء;
}

bool prioq_isempty(Prioq *q) {

        إذا (!q)
                أرجع true;

        أرجع q->n_اشیاء <= 0;
}
