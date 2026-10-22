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
        如果 (!q)
                返回 q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        返回 q;
}

Prioq* prioq_free(Prioq *q) {
        如果 (!q)
                返回 NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                如果 (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        返回 mfree(q);
}

整数 prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        如果 (*q) {
                assert((*q)->compare_func == compare_func);
                返回 0;
        }

        *q = prioq_new(compare_func);
        如果 (!*q)
                返回 -ENOMEM;

        返回 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        如果 (q->items[j].idx)
                *q->items[j].idx = j;

        如果 (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        当 (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                如果 (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        break;

                swap(q, idx, k);
                idx = k;
        }

        返回 idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        为 (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                如果 (j >= q->n_items)
                        break;

                如果 (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                否则
                        s = idx;

                如果 (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                如果 (s == idx)
                        /* No swap necessary, we're done */
                        break;

                swap(q, idx, s);
                idx = s;
        }

        返回 idx;
}

整数 prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        如果 (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                返回 -ENOMEM;

        k = q->n_items++;
        q->items[k] = (struct prioq_item) {
                .data = data,
                .idx = idx,
        };

        如果 (idx)
                *idx = k;

        shuffle_up(q, k);

        返回 0;
}

整数 _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        整数 r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        如果 (r < 0)
                返回 r;

        返回 prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, struct prioq_item *i) {
        struct prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        如果 (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        如果 (i == l)
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
                如果 (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static struct prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        assert(q);

        如果 (q->n_items <= 0)
                返回 NULL;

        如果 (idx) {
                如果 (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        返回 NULL;

                i = q->items + *idx;
                如果 (i->data != data)
                        返回 NULL;

                返回 i;
        } 否则 {
                为 (i = q->items; i < q->items + q->n_items; i++)
                        如果 (i->data == data)
                                返回 i;
                返回 NULL;
        }
}

整数 prioq_remove(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;

        如果 (!q)
                返回 0;

        i = find_item(q, data, idx);
        如果 (!i)
                返回 0;

        remove_item(q, i);
        返回 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        struct prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        如果 (!i)
                返回;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        如果 (!q)
                返回 NULL;

        如果 (idx >= q->n_items)
                返回 NULL;

        返回 q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        如果 (!q)
                返回 NULL;

        如果 (q->n_items <= 0)
                返回 NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        返回 data;
}

unsigned prioq_size(Prioq *q) {

        如果 (!q)
                返回 0;

        返回 q->n_items;
}

bool prioq_isempty(Prioq *q) {

        如果 (!q)
                返回 true;

        返回 q->n_items <= 0;
}
