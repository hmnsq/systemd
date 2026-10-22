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
        якщо (!q)
                повернути q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        повернути q;
}

Prioq* prioq_free(Prioq *q) {
        якщо (!q)
                повернути NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                якщо (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        повернути mfree(q);
}

ціле prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        якщо (*q) {
                assert((*q)->compare_func == compare_func);
                повернути 0;
        }

        *q = prioq_new(compare_func);
        якщо (!*q)
                повернути -ENOMEM;

        повернути 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        якщо (q->items[j].idx)
                *q->items[j].idx = j;

        якщо (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        поки (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                якщо (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        break;

                swap(q, idx, k);
                idx = k;
        }

        повернути idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        для (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                якщо (j >= q->n_items)
                        break;

                якщо (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                інакше
                        s = idx;

                якщо (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                якщо (s == idx)
                        /* No swap necessary, we're done */
                        break;

                swap(q, idx, s);
                idx = s;
        }

        повернути idx;
}

ціле prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        якщо (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                повернути -ENOMEM;

        k = q->n_items++;
        q->items[k] = (struct prioq_item) {
                .data = data,
                .idx = idx,
        };

        якщо (idx)
                *idx = k;

        shuffle_up(q, k);

        повернути 0;
}

ціле _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        ціле r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        якщо (r < 0)
                повернути r;

        повернути prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, struct prioq_item *i) {
        struct prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        якщо (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        якщо (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        інакше {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                якщо (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static struct prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        assert(q);

        якщо (q->n_items <= 0)
                повернути NULL;

        якщо (idx) {
                якщо (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        повернути NULL;

                i = q->items + *idx;
                якщо (i->data != data)
                        повернути NULL;

                повернути i;
        } інакше {
                для (i = q->items; i < q->items + q->n_items; i++)
                        якщо (i->data == data)
                                повернути i;
                повернути NULL;
        }
}

ціле prioq_remove(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        якщо (!q)
                повернути 0;

        i = find_item(q, data, idx);
        якщо (!i)
                повернути 0;

        remove_item(q, i);
        повернути 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        якщо (!i)
                повернути;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        якщо (!q)
                повернути NULL;

        якщо (idx >= q->n_items)
                повернути NULL;

        повернути q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        якщо (!q)
                повернути NULL;

        якщо (q->n_items <= 0)
                повернути NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        повернути data;
}

unsigned prioq_size(Prioq *q) {

        якщо (!q)
                повернути 0;

        повернути q->n_items;
}

bool prioq_isempty(Prioq *q) {

        якщо (!q)
                повернути true;

        повернути q->n_items <= 0;
}
