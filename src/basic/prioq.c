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

sa_nracanaa prioq_item {
        void *data;
        unsigned *idx;
};

sa_nracanaa Prioq {
        compare_func_t compare_func;
        unsigned n_items;
        sa_nracanaa prioq_item *items;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        གལ་སྲིད (!q)
                ལོག q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ལོག q;
}

Prioq* prioq_free(Prioq *q) {
        གལ་སྲིད (!q)
                ལོག NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                གལ་སྲིད (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ལོག mfree(q);
}

ཧྲིལ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        གལ་སྲིད (*q) {
                assert((*q)->compare_func == compare_func);
                ལོག 0;
        }

        *q = prioq_new(compare_func);
        གལ་སྲིད (!*q)
                ལོག -ENOMEM;

        ལོག 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        གལ་སྲིད (q->items[j].idx)
                *q->items[j].idx = j;

        གལ་སྲིད (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        སྐབས (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                གལ་སྲིད (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ལོག idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        དོན་ལུ (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                གལ་སྲིད (j >= q->n_items)
                        bha_nga;

                གལ་སྲིད (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                དེ་མིན
                        s = idx;

                གལ་སྲིད (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                གལ་སྲིད (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ལོག idx;
}

ཧྲིལ prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        གལ་སྲིད (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ལོག -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        གལ་སྲིད (idx)
                *idx = k;

        shuffle_up(q, k);

        ལོག 0;
}

ཧྲིལ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        ཧྲིལ r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        གལ་སྲིད (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        གལ་སྲིད (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        དེ་མིན {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                གལ་སྲིད (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        གལ་སྲིད (q->n_items <= 0)
                ལོག NULL;

        གལ་སྲིད (idx) {
                གལ་སྲིད (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ལོག NULL;

                i = q->items + *idx;
                གལ་སྲིད (i->data != data)
                        ལོག NULL;

                ལོག i;
        } དེ་མིན {
                དོན་ལུ (i = q->items; i < q->items + q->n_items; i++)
                        གལ་སྲིད (i->data == data)
                                ལོག i;
                ལོག NULL;
        }
}

ཧྲིལ prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        གལ་སྲིད (!q)
                ལོག 0;

        i = find_item(q, data, idx);
        གལ་སྲིད (!i)
                ལོག 0;

        remove_item(q, i);
        ལོག 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        གལ་སྲིད (!i)
                ལོག;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        གལ་སྲིད (!q)
                ལོག NULL;

        གལ་སྲིད (idx >= q->n_items)
                ལོག NULL;

        ལོག q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        གལ་སྲིད (!q)
                ལོག NULL;

        གལ་སྲིད (q->n_items <= 0)
                ལོག NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ལོག data;
}

unsigned prioq_size(Prioq *q) {

        གལ་སྲིད (!q)
                ལོག 0;

        ལོག q->n_items;
}

bool prioq_isempty(Prioq *q) {

        གལ་སྲིད (!q)
                ལོག true;

        ལོག q->n_items <= 0;
}
