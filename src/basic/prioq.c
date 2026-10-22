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

#شامل "alloc-util.h"
#شامل "prioq.h"

ساختار prioq_item {
        تهی *data;
        unsigned *idx;
};

ساختار Prioq {
        compare_func_t compare_func;
        unsigned n_اقلام;
        ساختار prioq_item *اقلام;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        اگر (!q)
                بازگشت q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        بازگشت q;
}

Prioq* prioq_free(Prioq *q) {
        اگر (!q)
                بازگشت پوچ;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->اقلام, q->n_اقلام)
                اگر (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        آزاد(q->اقلام);
        بازگشت mfree(q);
}

صحیح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        أكد(q);

        اگر (*q) {
                أكد((*q)->compare_func == compare_func);
                بازگشت 0;
        }

        *q = prioq_new(compare_func);
        اگر (!*q)
                بازگشت -ENOMEM;

        بازگشت 0;
}

ایستا تهی swap(Prioq *q, unsigned j, unsigned k) {
        أكد(q);
        أكد(j < q->n_اقلام);
        أكد(k < q->n_اقلام);

        أكد(!q->اقلام[j].idx || *(q->اقلام[j].idx) == j);
        أكد(!q->اقلام[k].idx || *(q->اقلام[k].idx) == k);

        SWAP_TWO(q->اقلام[j].data, q->اقلام[k].data);
        SWAP_TWO(q->اقلام[j].idx, q->اقلام[k].idx);

        اگر (q->اقلام[j].idx)
                *q->اقلام[j].idx = j;

        اگر (q->اقلام[k].idx)
                *q->اقلام[k].idx = k;
}

ایستا unsigned shuffle_up(Prioq *q, unsigned idx) {
        أكد(q);
        أكد(idx < q->n_اقلام);

        وقتی (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                اگر (q->compare_func(q->اقلام[k].data, q->اقلام[idx].data) <= 0)
                        بشکن;

                swap(q, idx, k);
                idx = k;
        }

        بازگشت idx;
}

ایستا unsigned shuffle_down(Prioq *q, unsigned idx) {
        أكد(q);

        برای (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                اگر (j >= q->n_اقلام)
                        بشکن;

                اگر (q->compare_func(q->اقلام[j].data, q->اقلام[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                وگرنه
                        s = idx;

                اگر (k < q->n_اقلام &&
                    q->compare_func(q->اقلام[k].data, q->اقلام[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                اگر (s == idx)
                        /* No swap necessary, we're done */
                        بشکن;

                swap(q, idx, s);
                idx = s;
        }

        بازگشت idx;
}

صحیح prioq_put(Prioq *q, تهی *data, unsigned *idx) {
        unsigned k;

        أكد(q);

        اگر (!GREEDY_REALLOC(q->اقلام, MAX(q->n_اقلام + 1, 16u)))
                بازگشت -ENOMEM;

        k = q->n_اقلام++;
        q->اقلام[k] = (ساختار prioq_item) {
                .data = data,
                .idx = idx,
        };

        اگر (idx)
                *idx = k;

        shuffle_up(q, k);

        بازگشت 0;
}

صحیح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, تهی *data, unsigned *idx) {
        صحیح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        اگر (r < 0)
                بازگشت r;

        بازگشت prioq_put(*q, data, idx);
}

ایستا تهی remove_item(Prioq *q, ساختار prioq_item *i) {
        ساختار prioq_item *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        اگر (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->اقلام + q->n_اقلام - 1;

        اگر (i == l)
                /* Last entry, let's just remove it */
                q->n_اقلام--;
        وگرنه {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->اقلام);
                k = i - q->اقلام;

                i->data = l->data;
                i->idx = l->idx;
                اگر (i->idx)
                        *i->idx = k;
                q->n_اقلام--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ایستا ساختار prioq_item* find_item(Prioq *q, تهی *data, unsigned *idx) {
        ساختار prioq_item *i;

        أكد(q);

        اگر (q->n_اقلام <= 0)
                بازگشت پوچ;

        اگر (idx) {
                اگر (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_اقلام)
                        بازگشت پوچ;

                i = q->اقلام + *idx;
                اگر (i->data != data)
                        بازگشت پوچ;

                بازگشت i;
        } وگرنه {
                برای (i = q->اقلام; i < q->اقلام + q->n_اقلام; i++)
                        اگر (i->data == data)
                                بازگشت i;
                بازگشت پوچ;
        }
}

صحیح prioq_remove(Prioq *q, تهی *data, unsigned *idx) {
        ساختار prioq_item *i;

        اگر (!q)
                بازگشت 0;

        i = find_item(q, data, idx);
        اگر (!i)
                بازگشت 0;

        remove_item(q, i);
        بازگشت 1;
}

تهی prioq_reshuffle(Prioq *q, تهی *data, unsigned *idx) {
        ساختار prioq_item *i;
        unsigned k;

        أكد(q);

        i = find_item(q, data, idx);
        اگر (!i)
                بازگشت;

        أكد(i >= q->اقلام);
        k = i - q->اقلام;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

تهی* prioq_peek_by_index(Prioq *q, unsigned idx) {
        اگر (!q)
                بازگشت پوچ;

        اگر (idx >= q->n_اقلام)
                بازگشت پوچ;

        بازگشت q->اقلام[idx].data;
}

تهی* prioq_برداشت(Prioq *q) {
        تهی *data;

        اگر (!q)
                بازگشت پوچ;

        اگر (q->n_اقلام <= 0)
                بازگشت پوچ;

        data = q->اقلام[0].data;
        remove_item(q, q->اقلام);
        بازگشت data;
}

unsigned prioq_size(Prioq *q) {

        اگر (!q)
                بازگشت 0;

        بازگشت q->n_اقلام;
}

bool prioq_isempty(Prioq *q) {

        اگر (!q)
                بازگشت true;

        بازگشت q->n_اقلام <= 0;
}
