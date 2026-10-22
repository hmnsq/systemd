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
        ако (!q)
                върни q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        върни q;
}

Prioq* prioq_free(Prioq *q) {
        ако (!q)
                върни NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ако (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        върни mfree(q);
}

цяло prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        ако (*q) {
                assert((*q)->compare_func == compare_func);
                върни 0;
        }

        *q = prioq_new(compare_func);
        ако (!*q)
                върни -ENOMEM;

        върни 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ако (q->items[j].idx)
                *q->items[j].idx = j;

        ако (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        докато (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ако (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        break;

                swap(q, idx, k);
                idx = k;
        }

        върни idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        за (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ако (j >= q->n_items)
                        break;

                ако (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                иначе
                        s = idx;

                ако (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ако (s == idx)
                        /* No swap necessary, we're done */
                        break;

                swap(q, idx, s);
                idx = s;
        }

        върни idx;
}

цяло prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        ако (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                върни -ENOMEM;

        k = q->n_items++;
        q->items[k] = (struct prioq_item) {
                .data = data,
                .idx = idx,
        };

        ако (idx)
                *idx = k;

        shuffle_up(q, k);

        върни 0;
}

цяло _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        цяло r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        ако (r < 0)
                върни r;

        върни prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, struct prioq_item *i) {
        struct prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ако (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ако (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        иначе {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ако (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static struct prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        assert(q);

        ако (q->n_items <= 0)
                върни NULL;

        ако (idx) {
                ако (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        върни NULL;

                i = q->items + *idx;
                ако (i->data != data)
                        върни NULL;

                върни i;
        } иначе {
                за (i = q->items; i < q->items + q->n_items; i++)
                        ако (i->data == data)
                                върни i;
                върни NULL;
        }
}

цяло prioq_remove(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        ако (!q)
                върни 0;

        i = find_item(q, data, idx);
        ако (!i)
                върни 0;

        remove_item(q, i);
        върни 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        ако (!i)
                върни;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ако (!q)
                върни NULL;

        ако (idx >= q->n_items)
                върни NULL;

        върни q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        ако (!q)
                върни NULL;

        ако (q->n_items <= 0)
                върни NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        върни data;
}

unsigned prioq_size(Prioq *q) {

        ако (!q)
                върни 0;

        върни q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ако (!q)
                върни true;

        върни q->n_items <= 0;
}
