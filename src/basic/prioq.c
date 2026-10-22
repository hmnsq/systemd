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
        もし (!q)
                戻る q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        戻る q;
}

Prioq* prioq_free(Prioq *q) {
        もし (!q)
                戻る NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                もし (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        戻る mfree(q);
}

整数 prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        もし (*q) {
                assert((*q)->compare_func == compare_func);
                戻る 0;
        }

        *q = prioq_new(compare_func);
        もし (!*q)
                戻る -ENOMEM;

        戻る 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        もし (q->items[j].idx)
                *q->items[j].idx = j;

        もし (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        ながら (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                もし (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        break;

                swap(q, idx, k);
                idx = k;
        }

        戻る idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        ために (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                もし (j >= q->n_items)
                        break;

                もし (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                否则
                        s = idx;

                もし (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                もし (s == idx)
                        /* No swap necessary, we're done */
                        break;

                swap(q, idx, s);
                idx = s;
        }

        戻る idx;
}

整数 prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        もし (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                戻る -ENOMEM;

        k = q->n_items++;
        q->items[k] = (struct prioq_item) {
                .data = data,
                .idx = idx,
        };

        もし (idx)
                *idx = k;

        shuffle_up(q, k);

        戻る 0;
}

整数 _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        整数 r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        もし (r < 0)
                戻る r;

        戻る prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, struct prioq_item *i) {
        struct prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        もし (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        もし (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        否则 {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                もし (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static struct prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        assert(q);

        もし (q->n_items <= 0)
                戻る NULL;

        もし (idx) {
                もし (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        戻る NULL;

                i = q->items + *idx;
                もし (i->data != data)
                        戻る NULL;

                戻る i;
        } 否则 {
                ために (i = q->items; i < q->items + q->n_items; i++)
                        もし (i->data == data)
                                戻る i;
                戻る NULL;
        }
}

整数 prioq_remove(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        もし (!q)
                戻る 0;

        i = find_item(q, data, idx);
        もし (!i)
                戻る 0;

        remove_item(q, i);
        戻る 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        もし (!i)
                戻る;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        もし (!q)
                戻る NULL;

        もし (idx >= q->n_items)
                戻る NULL;

        戻る q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        もし (!q)
                戻る NULL;

        もし (q->n_items <= 0)
                戻る NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        戻る data;
}

unsigned prioq_size(Prioq *q) {

        もし (!q)
                戻る 0;

        戻る q->n_items;
}

bool prioq_isempty(Prioq *q) {

        もし (!q)
                戻る true;

        戻る q->n_items <= 0;
}
