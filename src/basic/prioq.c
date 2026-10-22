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

بنية prioq_item {
        فراغ *data;
        unsigned *idx;
};

بنية Prioq {
        compare_func_t compare_func;
        unsigned n_عناصر;
        بنية prioq_item *عناصر;
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
        FOREACH_ARRAY(item, q->عناصر, q->n_عناصر)
                إذا (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        حرر(q->عناصر);
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

ساكن فراغ swap(Prioq *q, unsigned j, unsigned k) {
        أكد(q);
        أكد(j < q->n_عناصر);
        أكد(k < q->n_عناصر);

        أكد(!q->عناصر[j].idx || *(q->عناصر[j].idx) == j);
        أكد(!q->عناصر[k].idx || *(q->عناصر[k].idx) == k);

        SWAP_TWO(q->عناصر[j].data, q->عناصر[k].data);
        SWAP_TWO(q->عناصر[j].idx, q->عناصر[k].idx);

        إذا (q->عناصر[j].idx)
                *q->عناصر[j].idx = j;

        إذا (q->عناصر[k].idx)
                *q->عناصر[k].idx = k;
}

ساكن unsigned shuffle_up(Prioq *q, unsigned idx) {
        أكد(q);
        أكد(idx < q->n_عناصر);

        طالما (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                إذا (q->compare_func(q->عناصر[k].data, q->عناصر[idx].data) <= 0)
                        اكسر;

                swap(q, idx, k);
                idx = k;
        }

        أرجع idx;
}

ساكن unsigned shuffle_down(Prioq *q, unsigned idx) {
        أكد(q);

        لكل (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                إذا (j >= q->n_عناصر)
                        اكسر;

                إذا (q->compare_func(q->عناصر[j].data, q->عناصر[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وإلا
                        s = idx;

                إذا (k < q->n_عناصر &&
                    q->compare_func(q->عناصر[k].data, q->عناصر[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                إذا (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                swap(q, idx, s);
                idx = s;
        }

        أرجع idx;
}

صحيح prioq_put(Prioq *q, فراغ *data, unsigned *idx) {
        unsigned k;

        أكد(q);

        إذا (!GREEDY_REALLOC(q->عناصر, MAX(q->n_عناصر + 1, 16u)))
                أرجع -ENOMEM;

        k = q->n_عناصر++;
        q->عناصر[k] = (بنية prioq_item) {
                .data = data,
                .idx = idx,
        };

        إذا (idx)
                *idx = k;

        shuffle_up(q, k);

        أرجع 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *data, unsigned *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        إذا (r < 0)
                أرجع r;

        أرجع prioq_put(*q, data, idx);
}

ساكن فراغ remove_item(Prioq *q, بنية prioq_item *i) {
        بنية prioq_item *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        إذا (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->عناصر + q->n_عناصر - 1;

        إذا (i == l)
                /* Last entry, let's just remove it */
                q->n_عناصر--;
        وإلا {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->عناصر);
                k = i - q->عناصر;

                i->data = l->data;
                i->idx = l->idx;
                إذا (i->idx)
                        *i->idx = k;
                q->n_عناصر--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ساكن بنية prioq_item* find_item(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        أكد(q);

        إذا (q->n_عناصر <= 0)
                أرجع عدم;

        إذا (idx) {
                إذا (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_عناصر)
                        أرجع عدم;

                i = q->عناصر + *idx;
                إذا (i->data != data)
                        أرجع عدم;

                أرجع i;
        } وإلا {
                لكل (i = q->عناصر; i < q->عناصر + q->n_عناصر; i++)
                        إذا (i->data == data)
                                أرجع i;
                أرجع عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        إذا (!q)
                أرجع 0;

        i = find_item(q, data, idx);
        إذا (!i)
                أرجع 0;

        remove_item(q, i);
        أرجع 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;
        unsigned k;

        أكد(q);

        i = find_item(q, data, idx);
        إذا (!i)
                أرجع;

        أكد(i >= q->عناصر);
        k = i - q->عناصر;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

فراغ* prioq_peek_by_index(Prioq *q, unsigned idx) {
        إذا (!q)
                أرجع عدم;

        إذا (idx >= q->n_عناصر)
                أرجع عدم;

        أرجع q->عناصر[idx].data;
}

فراغ* prioq_اسحب(Prioq *q) {
        فراغ *data;

        إذا (!q)
                أرجع عدم;

        إذا (q->n_عناصر <= 0)
                أرجع عدم;

        data = q->عناصر[0].data;
        remove_item(q, q->عناصر);
        أرجع data;
}

unsigned prioq_size(Prioq *q) {

        إذا (!q)
                أرجع 0;

        أرجع q->n_عناصر;
}

bool prioq_isempty(Prioq *q) {

        إذا (!q)
                أرجع true;

        أرجع q->n_عناصر <= 0;
}
