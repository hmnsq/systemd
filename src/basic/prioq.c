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

بنية prioq_توکی {
        فراغ *معلومات;
        بینښه *idx;
};

بنية Prioq {
        compare_func_t compare_func;
        بینښه n_توکي;
        بنية prioq_توکی *توکي;
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
        FOREACH_ARRAY(توکی, q->توکي, q->n_توکي)
                إذا (توکی->idx)
                        *(توکی->idx) = PRIOQ_IDX_NULL;

        حرر(q->توکي);
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

ساكن فراغ بدلون(Prioq *q, بینښه j, بینښه k) {
        أكد(q);
        أكد(j < q->n_توکي);
        أكد(k < q->n_توکي);

        أكد(!q->توکي[j].idx || *(q->توکي[j].idx) == j);
        أكد(!q->توکي[k].idx || *(q->توکي[k].idx) == k);

        بدلون_TWO(q->توکي[j].معلومات, q->توکي[k].معلومات);
        بدلون_TWO(q->توکي[j].idx, q->توکي[k].idx);

        إذا (q->توکي[j].idx)
                *q->توکي[j].idx = j;

        إذا (q->توکي[k].idx)
                *q->توکي[k].idx = k;
}

ساكن بینښه shuffle_پورته(Prioq *q, بینښه idx) {
        أكد(q);
        أكد(idx < q->n_توکي);

        طالما (idx > 0) {
                بینښه k;

                k = (idx-1)/2;

                إذا (q->compare_func(q->توکي[k].معلومات, q->توکي[idx].معلومات) <= 0)
                        اكسر;

                بدلون(q, idx, k);
                idx = k;
        }

        أرجع idx;
}

ساكن بینښه shuffle_ښکته(Prioq *q, بینښه idx) {
        أكد(q);

        لكل (;;) {
                بینښه j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                إذا (j >= q->n_توکي)
                        اكسر;

                إذا (q->compare_func(q->توکي[j].معلومات, q->توکي[idx].معلومات) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وإلا
                        s = idx;

                إذا (k < q->n_توکي &&
                    q->compare_func(q->توکي[k].معلومات, q->توکي[s].معلومات) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                إذا (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                بدلون(q, idx, s);
                idx = s;
        }

        أرجع idx;
}

صحيح prioq_put(Prioq *q, فراغ *معلومات, بینښه *idx) {
        بینښه k;

        أكد(q);

        إذا (!GREEDY_REALLOC(q->توکي, MAX(q->n_توکي + 1, 16u)))
                أرجع -ENOMEM;

        k = q->n_توکي++;
        q->توکي[k] = (بنية prioq_توکی) {
                .معلومات = معلومات,
                .idx = idx,
        };

        إذا (idx)
                *idx = k;

        shuffle_پورته(q, k);

        أرجع 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *معلومات, بینښه *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        إذا (r < 0)
                أرجع r;

        أرجع prioq_put(*q, معلومات, idx);
}

ساكن فراغ remove_توکی(Prioq *q, بنية prioq_توکی *i) {
        بنية prioq_توکی *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        إذا (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->توکي + q->n_توکي - 1;

        إذا (i == l)
                /* Last entry, let's just remove it */
                q->n_توکي--;
        وإلا {
                بینښه k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->توکي);
                k = i - q->توکي;

                i->معلومات = l->معلومات;
                i->idx = l->idx;
                إذا (i->idx)
                        *i->idx = k;
                q->n_توکي--;

                k = shuffle_ښکته(q, k);
                shuffle_پورته(q, k);
        }
}

ساكن بنية prioq_توکی* پیدا_کړه_توکی(Prioq *q, فراغ *معلومات, بینښه *idx) {
        بنية prioq_توکی *i;

        أكد(q);

        إذا (q->n_توکي <= 0)
                أرجع عدم;

        إذا (idx) {
                إذا (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_توکي)
                        أرجع عدم;

                i = q->توکي + *idx;
                إذا (i->معلومات != معلومات)
                        أرجع عدم;

                أرجع i;
        } وإلا {
                لكل (i = q->توکي; i < q->توکي + q->n_توکي; i++)
                        إذا (i->معلومات == معلومات)
                                أرجع i;
                أرجع عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *معلومات, بینښه *idx) {
        بنية prioq_توکی *i;

        إذا (!q)
                أرجع 0;

        i = پیدا_کړه_توکی(q, معلومات, idx);
        إذا (!i)
                أرجع 0;

        remove_توکی(q, i);
        أرجع 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *معلومات, بینښه *idx) {
        بنية prioq_توکی *i;
        بینښه k;

        أكد(q);

        i = پیدا_کړه_توکی(q, معلومات, idx);
        إذا (!i)
                أرجع;

        أكد(i >= q->توکي);
        k = i - q->توکي;
        k = shuffle_ښکته(q, k);
        shuffle_پورته(q, k);
}

فراغ* prioq_peek_by_شاخص(Prioq *q, بینښه idx) {
        إذا (!q)
                أرجع عدم;

        إذا (idx >= q->n_توکي)
                أرجع عدم;

        أرجع q->توکي[idx].معلومات;
}

فراغ* prioq_راوباسه(Prioq *q) {
        فراغ *معلومات;

        إذا (!q)
                أرجع عدم;

        إذا (q->n_توکي <= 0)
                أرجع عدم;

        معلومات = q->توکي[0].معلومات;
        remove_توکی(q, q->توکي);
        أرجع معلومات;
}

بینښه prioq_اندازه(Prioq *q) {

        إذا (!q)
                أرجع 0;

        أرجع q->n_توکي;
}

bool prioq_isempty(Prioq *q) {

        إذا (!q)
                أرجع true;

        أرجع q->n_توکي <= 0;
}
