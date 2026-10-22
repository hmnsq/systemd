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

بنية prioq_شي {
        فراغ *معلومات;
        بنانشان *idx;
};

بنية Prioq {
        compare_func_t compare_func;
        بنانشان n_شيون;
        بنية prioq_شي *شيون;
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
        FOREACH_ARRAY(شي, q->شيون, q->n_شيون)
                إذا (شي->idx)
                        *(شي->idx) = PRIOQ_IDX_NULL;

        حرر(q->شيون);
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

ساكن فراغ مٽا_سٽا(Prioq *q, بنانشان j, بنانشان k) {
        أكد(q);
        أكد(j < q->n_شيون);
        أكد(k < q->n_شيون);

        أكد(!q->شيون[j].idx || *(q->شيون[j].idx) == j);
        أكد(!q->شيون[k].idx || *(q->شيون[k].idx) == k);

        مٽا_سٽا_TWO(q->شيون[j].معلومات, q->شيون[k].معلومات);
        مٽا_سٽا_TWO(q->شيون[j].idx, q->شيون[k].idx);

        إذا (q->شيون[j].idx)
                *q->شيون[j].idx = j;

        إذا (q->شيون[k].idx)
                *q->شيون[k].idx = k;
}

ساكن بنانشان shuffle_مٿي(Prioq *q, بنانشان idx) {
        أكد(q);
        أكد(idx < q->n_شيون);

        طالما (idx > 0) {
                بنانشان k;

                k = (idx-1)/2;

                إذا (q->compare_func(q->شيون[k].معلومات, q->شيون[idx].معلومات) <= 0)
                        اكسر;

                مٽا_سٽا(q, idx, k);
                idx = k;
        }

        أرجع idx;
}

ساكن بنانشان shuffle_هيٺ_ڏانهن(Prioq *q, بنانشان idx) {
        أكد(q);

        لكل (;;) {
                بنانشان j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                إذا (j >= q->n_شيون)
                        اكسر;

                إذا (q->compare_func(q->شيون[j].معلومات, q->شيون[idx].معلومات) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وإلا
                        s = idx;

                إذا (k < q->n_شيون &&
                    q->compare_func(q->شيون[k].معلومات, q->شيون[s].معلومات) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                إذا (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                مٽا_سٽا(q, idx, s);
                idx = s;
        }

        أرجع idx;
}

صحيح prioq_put(Prioq *q, فراغ *معلومات, بنانشان *idx) {
        بنانشان k;

        أكد(q);

        إذا (!GREEDY_REALLOC(q->شيون, MAX(q->n_شيون + 1, 16u)))
                أرجع -ENOMEM;

        k = q->n_شيون++;
        q->شيون[k] = (بنية prioq_شي) {
                .معلومات = معلومات,
                .idx = idx,
        };

        إذا (idx)
                *idx = k;

        shuffle_مٿي(q, k);

        أرجع 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *معلومات, بنانشان *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        إذا (r < 0)
                أرجع r;

        أرجع prioq_put(*q, معلومات, idx);
}

ساكن فراغ remove_شي(Prioq *q, بنية prioq_شي *i) {
        بنية prioq_شي *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        إذا (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->شيون + q->n_شيون - 1;

        إذا (i == l)
                /* Last entry, let's just remove it */
                q->n_شيون--;
        وإلا {
                بنانشان k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->شيون);
                k = i - q->شيون;

                i->معلومات = l->معلومات;
                i->idx = l->idx;
                إذا (i->idx)
                        *i->idx = k;
                q->n_شيون--;

                k = shuffle_هيٺ_ڏانهن(q, k);
                shuffle_مٿي(q, k);
        }
}

ساكن بنية prioq_شي* ڳوليو_شي(Prioq *q, فراغ *معلومات, بنانشان *idx) {
        بنية prioq_شي *i;

        أكد(q);

        إذا (q->n_شيون <= 0)
                أرجع عدم;

        إذا (idx) {
                إذا (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_شيون)
                        أرجع عدم;

                i = q->شيون + *idx;
                إذا (i->معلومات != معلومات)
                        أرجع عدم;

                أرجع i;
        } وإلا {
                لكل (i = q->شيون; i < q->شيون + q->n_شيون; i++)
                        إذا (i->معلومات == معلومات)
                                أرجع i;
                أرجع عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *معلومات, بنانشان *idx) {
        بنية prioq_شي *i;

        إذا (!q)
                أرجع 0;

        i = ڳوليو_شي(q, معلومات, idx);
        إذا (!i)
                أرجع 0;

        remove_شي(q, i);
        أرجع 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *معلومات, بنانشان *idx) {
        بنية prioq_شي *i;
        بنانشان k;

        أكد(q);

        i = ڳوليو_شي(q, معلومات, idx);
        إذا (!i)
                أرجع;

        أكد(i >= q->شيون);
        k = i - q->شيون;
        k = shuffle_هيٺ_ڏانهن(q, k);
        shuffle_مٿي(q, k);
}

فراغ* prioq_peek_by_اشاريو(Prioq *q, بنانشان idx) {
        إذا (!q)
                أرجع عدم;

        إذا (idx >= q->n_شيون)
                أرجع عدم;

        أرجع q->شيون[idx].معلومات;
}

فراغ* prioq_ڪڍو(Prioq *q) {
        فراغ *معلومات;

        إذا (!q)
                أرجع عدم;

        إذا (q->n_شيون <= 0)
                أرجع عدم;

        معلومات = q->شيون[0].معلومات;
        remove_شي(q, q->شيون);
        أرجع معلومات;
}

بنانشان prioq_سائيز(Prioq *q) {

        إذا (!q)
                أرجع 0;

        أرجع q->n_شيون;
}

bool prioq_isempty(Prioq *q) {

        إذا (!q)
                أرجع true;

        أرجع q->n_شيون <= 0;
}
