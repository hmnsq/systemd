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
        unsigned n_items;
        بنية prioq_item *items;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        ܐܢ (!q)
                ܦܢܐ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ܦܢܐ q;
}

Prioq* prioq_free(Prioq *q) {
        ܐܢ (!q)
                ܦܢܐ عدم;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ܐܢ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        حرر(q->items);
        ܦܢܐ mfree(q);
}

صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        أكد(q);

        ܐܢ (*q) {
                أكد((*q)->compare_func == compare_func);
                ܦܢܐ 0;
        }

        *q = prioq_new(compare_func);
        ܐܢ (!*q)
                ܦܢܐ -ENOMEM;

        ܦܢܐ 0;
}

ساكن فراغ swap(Prioq *q, unsigned j, unsigned k) {
        أكد(q);
        أكد(j < q->n_items);
        أكد(k < q->n_items);

        أكد(!q->items[j].idx || *(q->items[j].idx) == j);
        أكد(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ܐܢ (q->items[j].idx)
                *q->items[j].idx = j;

        ܐܢ (q->items[k].idx)
                *q->items[k].idx = k;
}

ساكن unsigned shuffle_up(Prioq *q, unsigned idx) {
        أكد(q);
        أكد(idx < q->n_items);

        ܟܡܐ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ܐܢ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        اكسر;

                swap(q, idx, k);
                idx = k;
        }

        ܦܢܐ idx;
}

ساكن unsigned shuffle_down(Prioq *q, unsigned idx) {
        أكد(q);

        لكل (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ܐܢ (j >= q->n_items)
                        اكسر;

                ܐܢ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ܐܚܪܢܐ
                        s = idx;

                ܐܢ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ܐܢ (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                swap(q, idx, s);
                idx = s;
        }

        ܦܢܐ idx;
}

صحيح prioq_put(Prioq *q, فراغ *data, unsigned *idx) {
        unsigned k;

        أكد(q);

        ܐܢ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ܦܢܐ -ENOMEM;

        k = q->n_items++;
        q->items[k] = (بنية prioq_item) {
                .data = data,
                .idx = idx,
        };

        ܐܢ (idx)
                *idx = k;

        shuffle_up(q, k);

        ܦܢܐ 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *data, unsigned *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ prioq_put(*q, data, idx);
}

ساكن فراغ remove_item(Prioq *q, بنية prioq_item *i) {
        بنية prioq_item *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ܐܢ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ܐܢ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        ܐܚܪܢܐ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ܐܢ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ساكن بنية prioq_item* find_item(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        أكد(q);

        ܐܢ (q->n_items <= 0)
                ܦܢܐ عدم;

        ܐܢ (idx) {
                ܐܢ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ܦܢܐ عدم;

                i = q->items + *idx;
                ܐܢ (i->data != data)
                        ܦܢܐ عدم;

                ܦܢܐ i;
        } ܐܚܪܢܐ {
                لكل (i = q->items; i < q->items + q->n_items; i++)
                        ܐܢ (i->data == data)
                                ܦܢܐ i;
                ܦܢܐ عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        ܐܢ (!q)
                ܦܢܐ 0;

        i = find_item(q, data, idx);
        ܐܢ (!i)
                ܦܢܐ 0;

        remove_item(q, i);
        ܦܢܐ 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;
        unsigned k;

        أكد(q);

        i = find_item(q, data, idx);
        ܐܢ (!i)
                ܦܢܐ;

        أكد(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

فراغ* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ܐܢ (!q)
                ܦܢܐ عدم;

        ܐܢ (idx >= q->n_items)
                ܦܢܐ عدم;

        ܦܢܐ q->items[idx].data;
}

فراغ* prioq_pop(Prioq *q) {
        فراغ *data;

        ܐܢ (!q)
                ܦܢܐ عدم;

        ܐܢ (q->n_items <= 0)
                ܦܢܐ عدم;

        data = q->items[0].data;
        remove_item(q, q->items);
        ܦܢܐ data;
}

unsigned prioq_size(Prioq *q) {

        ܐܢ (!q)
                ܦܢܐ 0;

        ܦܢܐ q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ܐܢ (!q)
                ܦܢܐ true;

        ܦܢܐ q->n_items <= 0;
}
