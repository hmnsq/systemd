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

#include "alloc-util.h"
#include "prioq.h"

struct prioq_item {
        void *data;
        unsigned *idx;
};

struct Prioq {
        compare_func_t compare_func;
        unsigned n_items;
        struct prioq_item *items;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        nếu (!q)
                trả_về q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        trả_về q;
}

Prioq* prioq_free(Prioq *q) {
        nếu (!q)
                trả_về NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                nếu (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        trả_về mfree(q);
}

số_nguyên prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        nếu (*q) {
                assert((*q)->compare_func == compare_func);
                trả_về 0;
        }

        *q = prioq_new(compare_func);
        nếu (!*q)
                trả_về -ENOMEM;

        trả_về 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        nếu (q->items[j].idx)
                *q->items[j].idx = j;

        nếu (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        trong_khi (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                nếu (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        break;

                swap(q, idx, k);
                idx = k;
        }

        trả_về idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        cho (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                nếu (j >= q->n_items)
                        break;

                nếu (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                không_thì
                        s = idx;

                nếu (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                nếu (s == idx)
                        /* No swap necessary, we're done */
                        break;

                swap(q, idx, s);
                idx = s;
        }

        trả_về idx;
}

số_nguyên prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        nếu (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                trả_về -ENOMEM;

        k = q->n_items++;
        q->items[k] = (struct prioq_item) {
                .data = data,
                .idx = idx,
        };

        nếu (idx)
                *idx = k;

        shuffle_up(q, k);

        trả_về 0;
}

số_nguyên _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        số_nguyên r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        nếu (r < 0)
                trả_về r;

        trả_về prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, struct prioq_item *i) {
        struct prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        nếu (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        nếu (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        không_thì {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                nếu (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static struct prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        assert(q);

        nếu (q->n_items <= 0)
                trả_về NULL;

        nếu (idx) {
                nếu (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        trả_về NULL;

                i = q->items + *idx;
                nếu (i->data != data)
                        trả_về NULL;

                trả_về i;
        } không_thì {
                cho (i = q->items; i < q->items + q->n_items; i++)
                        nếu (i->data == data)
                                trả_về i;
                trả_về NULL;
        }
}

số_nguyên prioq_remove(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        nếu (!q)
                trả_về 0;

        i = find_item(q, data, idx);
        nếu (!i)
                trả_về 0;

        remove_item(q, i);
        trả_về 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        nếu (!i)
                trả_về;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        nếu (!q)
                trả_về NULL;

        nếu (idx >= q->n_items)
                trả_về NULL;

        trả_về q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        nếu (!q)
                trả_về NULL;

        nếu (q->n_items <= 0)
                trả_về NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        trả_về data;
}

unsigned prioq_size(Prioq *q) {

        nếu (!q)
                trả_về 0;

        trả_về q->n_items;
}

bool prioq_isempty(Prioq *q) {

        nếu (!q)
                trả_về true;

        trả_về q->n_items <= 0;
}
