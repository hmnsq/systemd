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
        unsigned n_اشیاء;
        بنية prioq_item *اشیاء;
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
        FOREACH_ARRAY(item, q->اشیاء, q->n_اشیاء)
                إذا (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

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

ساكن فراغ swap(Prioq *q, unsigned j, unsigned k) {
        أكد(q);
        أكد(j < q->n_اشیاء);
        أكد(k < q->n_اشیاء);

        أكد(!q->اشیاء[j].idx || *(q->اشیاء[j].idx) == j);
        أكد(!q->اشیاء[k].idx || *(q->اشیاء[k].idx) == k);

        SWAP_TWO(q->اشیاء[j].data, q->اشیاء[k].data);
        SWAP_TWO(q->اشیاء[j].idx, q->اشیاء[k].idx);

        إذا (q->اشیاء[j].idx)
                *q->اشیاء[j].idx = j;

        إذا (q->اشیاء[k].idx)
                *q->اشیاء[k].idx = k;
}

ساكن unsigned shuffle_up(Prioq *q, unsigned idx) {
        أكد(q);
        أكد(idx < q->n_اشیاء);

        طالما (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                إذا (q->compare_func(q->اشیاء[k].data, q->اشیاء[idx].data) <= 0)
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

                إذا (j >= q->n_اشیاء)
                        اكسر;

                إذا (q->compare_func(q->اشیاء[j].data, q->اشیاء[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وإلا
                        s = idx;

                إذا (k < q->n_اشیاء &&
                    q->compare_func(q->اشیاء[k].data, q->اشیاء[s].data) < 0)

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

        إذا (!GREEDY_REALLOC(q->اشیاء, MAX(q->n_اشیاء + 1, 16u)))
                أرجع -ENOMEM;

        k = q->n_اشیاء++;
        q->اشیاء[k] = (بنية prioq_item) {
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

        l = q->اشیاء + q->n_اشیاء - 1;

        إذا (i == l)
                /* Last entry, let's just remove it */
                q->n_اشیاء--;
        وإلا {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->اشیاء);
                k = i - q->اشیاء;

                i->data = l->data;
                i->idx = l->idx;
                إذا (i->idx)
                        *i->idx = k;
                q->n_اشیاء--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ساكن بنية prioq_item* find_item(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        أكد(q);

        إذا (q->n_اشیاء <= 0)
                أرجع عدم;

        إذا (idx) {
                إذا (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_اشیاء)
                        أرجع عدم;

                i = q->اشیاء + *idx;
                إذا (i->data != data)
                        أرجع عدم;

                أرجع i;
        } وإلا {
                لكل (i = q->اشیاء; i < q->اشیاء + q->n_اشیاء; i++)
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

        أكد(i >= q->اشیاء);
        k = i - q->اشیاء;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

فراغ* prioq_peek_by_index(Prioq *q, unsigned idx) {
        إذا (!q)
                أرجع عدم;

        إذا (idx >= q->n_اشیاء)
                أرجع عدم;

        أرجع q->اشیاء[idx].data;
}

فراغ* prioq_نکالو(Prioq *q) {
        فراغ *data;

        إذا (!q)
                أرجع عدم;

        إذا (q->n_اشیاء <= 0)
                أرجع عدم;

        data = q->اشیاء[0].data;
        remove_item(q, q->اشیاء);
        أرجع data;
}

unsigned prioq_size(Prioq *q) {

        إذا (!q)
                أرجع 0;

        أرجع q->n_اشیاء;
}

bool prioq_isempty(Prioq *q) {

        إذا (!q)
                أرجع true;

        أرجع q->n_اشیاء <= 0;
}
