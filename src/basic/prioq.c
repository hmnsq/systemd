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

بنية prioq_قلم {
        فراغ *داده;
        بیعلامت *idx;
};

بنية Prioq {
        compare_func_t compare_func;
        بیعلامت n_اقلام;
        بنية prioq_قلم *اقلام;
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
        FOREACH_ARRAY(قلم, q->اقلام, q->n_اقلام)
                إذا (قلم->idx)
                        *(قلم->idx) = PRIOQ_IDX_NULL;

        حرر(q->اقلام);
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

ساكن فراغ جابجا(Prioq *q, بیعلامت j, بیعلامت k) {
        أكد(q);
        أكد(j < q->n_اقلام);
        أكد(k < q->n_اقلام);

        أكد(!q->اقلام[j].idx || *(q->اقلام[j].idx) == j);
        أكد(!q->اقلام[k].idx || *(q->اقلام[k].idx) == k);

        جابجا_TWO(q->اقلام[j].داده, q->اقلام[k].داده);
        جابجا_TWO(q->اقلام[j].idx, q->اقلام[k].idx);

        إذا (q->اقلام[j].idx)
                *q->اقلام[j].idx = j;

        إذا (q->اقلام[k].idx)
                *q->اقلام[k].idx = k;
}

ساكن بیعلامت shuffle_به_بالا(Prioq *q, بیعلامت idx) {
        أكد(q);
        أكد(idx < q->n_اقلام);

        طالما (idx > 0) {
                بیعلامت k;

                k = (idx-1)/2;

                إذا (q->compare_func(q->اقلام[k].داده, q->اقلام[idx].داده) <= 0)
                        اكسر;

                جابجا(q, idx, k);
                idx = k;
        }

        أرجع idx;
}

ساكن بیعلامت shuffle_پایین(Prioq *q, بیعلامت idx) {
        أكد(q);

        لكل (;;) {
                بیعلامت j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                إذا (j >= q->n_اقلام)
                        اكسر;

                إذا (q->compare_func(q->اقلام[j].داده, q->اقلام[idx].داده) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وإلا
                        s = idx;

                إذا (k < q->n_اقلام &&
                    q->compare_func(q->اقلام[k].داده, q->اقلام[s].داده) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                إذا (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                جابجا(q, idx, s);
                idx = s;
        }

        أرجع idx;
}

صحيح prioq_put(Prioq *q, فراغ *داده, بیعلامت *idx) {
        بیعلامت k;

        أكد(q);

        إذا (!GREEDY_REALLOC(q->اقلام, MAX(q->n_اقلام + 1, 16u)))
                أرجع -ENOMEM;

        k = q->n_اقلام++;
        q->اقلام[k] = (بنية prioq_قلم) {
                .داده = داده,
                .idx = idx,
        };

        إذا (idx)
                *idx = k;

        shuffle_به_بالا(q, k);

        أرجع 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *داده, بیعلامت *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        إذا (r < 0)
                أرجع r;

        أرجع prioq_put(*q, داده, idx);
}

ساكن فراغ remove_قلم(Prioq *q, بنية prioq_قلم *i) {
        بنية prioq_قلم *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        إذا (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->اقلام + q->n_اقلام - 1;

        إذا (i == l)
                /* Last entry, let's just remove it */
                q->n_اقلام--;
        وإلا {
                بیعلامت k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->اقلام);
                k = i - q->اقلام;

                i->داده = l->داده;
                i->idx = l->idx;
                إذا (i->idx)
                        *i->idx = k;
                q->n_اقلام--;

                k = shuffle_پایین(q, k);
                shuffle_به_بالا(q, k);
        }
}

ساكن بنية prioq_قلم* پیدا_کن_قلم(Prioq *q, فراغ *داده, بیعلامت *idx) {
        بنية prioq_قلم *i;

        أكد(q);

        إذا (q->n_اقلام <= 0)
                أرجع عدم;

        إذا (idx) {
                إذا (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_اقلام)
                        أرجع عدم;

                i = q->اقلام + *idx;
                إذا (i->داده != داده)
                        أرجع عدم;

                أرجع i;
        } وإلا {
                لكل (i = q->اقلام; i < q->اقلام + q->n_اقلام; i++)
                        إذا (i->داده == داده)
                                أرجع i;
                أرجع عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *داده, بیعلامت *idx) {
        بنية prioq_قلم *i;

        إذا (!q)
                أرجع 0;

        i = پیدا_کن_قلم(q, داده, idx);
        إذا (!i)
                أرجع 0;

        remove_قلم(q, i);
        أرجع 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *داده, بیعلامت *idx) {
        بنية prioq_قلم *i;
        بیعلامت k;

        أكد(q);

        i = پیدا_کن_قلم(q, داده, idx);
        إذا (!i)
                أرجع;

        أكد(i >= q->اقلام);
        k = i - q->اقلام;
        k = shuffle_پایین(q, k);
        shuffle_به_بالا(q, k);
}

فراغ* prioq_peek_by_شاخص(Prioq *q, بیعلامت idx) {
        إذا (!q)
                أرجع عدم;

        إذا (idx >= q->n_اقلام)
                أرجع عدم;

        أرجع q->اقلام[idx].داده;
}

فراغ* prioq_برداشت(Prioq *q) {
        فراغ *داده;

        إذا (!q)
                أرجع عدم;

        إذا (q->n_اقلام <= 0)
                أرجع عدم;

        داده = q->اقلام[0].داده;
        remove_قلم(q, q->اقلام);
        أرجع داده;
}

بیعلامت prioq_اندازه(Prioq *q) {

        إذا (!q)
                أرجع 0;

        أرجع q->n_اقلام;
}

bool prioq_isempty(Prioq *q) {

        إذا (!q)
                أرجع true;

        أرجع q->n_اقلام <= 0;
}
