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
        אם (!q)
                החזר q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        החזר q;
}

Prioq* prioq_free(Prioq *q) {
        אם (!q)
                החזר عدم;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                אם (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        حرر(q->items);
        החזר mfree(q);
}

صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        أكد(q);

        אם (*q) {
                أكد((*q)->compare_func == compare_func);
                החזר 0;
        }

        *q = prioq_new(compare_func);
        אם (!*q)
                החזר -ENOMEM;

        החזר 0;
}

ساكن فراغ swap(Prioq *q, unsigned j, unsigned k) {
        أكد(q);
        أكد(j < q->n_items);
        أكد(k < q->n_items);

        أكد(!q->items[j].idx || *(q->items[j].idx) == j);
        أكد(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        אם (q->items[j].idx)
                *q->items[j].idx = j;

        אם (q->items[k].idx)
                *q->items[k].idx = k;
}

ساكن unsigned shuffle_up(Prioq *q, unsigned idx) {
        أكد(q);
        أكد(idx < q->n_items);

        כלעוד (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                אם (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        اكسر;

                swap(q, idx, k);
                idx = k;
        }

        החזר idx;
}

ساكن unsigned shuffle_down(Prioq *q, unsigned idx) {
        أكد(q);

        עבור (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                אם (j >= q->n_items)
                        اكسر;

                אם (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                אחרת
                        s = idx;

                אם (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                אם (s == idx)
                        /* No swap necessary, we're done */
                        اكسر;

                swap(q, idx, s);
                idx = s;
        }

        החזר idx;
}

صحيح prioq_put(Prioq *q, فراغ *data, unsigned *idx) {
        unsigned k;

        أكد(q);

        אם (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                החזר -ENOMEM;

        k = q->n_items++;
        q->items[k] = (بنية prioq_item) {
                .data = data,
                .idx = idx,
        };

        אם (idx)
                *idx = k;

        shuffle_up(q, k);

        החזר 0;
}

صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *data, unsigned *idx) {
        صحيح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        אם (r < 0)
                החזר r;

        החזר prioq_put(*q, data, idx);
}

ساكن فراغ remove_item(Prioq *q, بنية prioq_item *i) {
        بنية prioq_item *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        אם (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        אם (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        אחרת {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                אם (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ساكن بنية prioq_item* find_item(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        أكد(q);

        אם (q->n_items <= 0)
                החזר عدم;

        אם (idx) {
                אם (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        החזר عدم;

                i = q->items + *idx;
                אם (i->data != data)
                        החזר عدم;

                החזר i;
        } אחרת {
                עבור (i = q->items; i < q->items + q->n_items; i++)
                        אם (i->data == data)
                                החזר i;
                החזר عدم;
        }
}

صحيح prioq_remove(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;

        אם (!q)
                החזר 0;

        i = find_item(q, data, idx);
        אם (!i)
                החזר 0;

        remove_item(q, i);
        החזר 1;
}

فراغ prioq_reshuffle(Prioq *q, فراغ *data, unsigned *idx) {
        بنية prioq_item *i;
        unsigned k;

        أكد(q);

        i = find_item(q, data, idx);
        אם (!i)
                החזר;

        أكد(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

فراغ* prioq_peek_by_index(Prioq *q, unsigned idx) {
        אם (!q)
                החזר عدم;

        אם (idx >= q->n_items)
                החזר عدم;

        החזר q->items[idx].data;
}

فراغ* prioq_pop(Prioq *q) {
        فراغ *data;

        אם (!q)
                החזר عدم;

        אם (q->n_items <= 0)
                החזר عدم;

        data = q->items[0].data;
        remove_item(q, q->items);
        החזר data;
}

unsigned prioq_size(Prioq *q) {

        אם (!q)
                החזר 0;

        החזר q->n_items;
}

bool prioq_isempty(Prioq *q) {

        אם (!q)
                החזר true;

        החזר q->n_items <= 0;
}
