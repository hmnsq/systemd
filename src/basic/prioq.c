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
        ᨀᨘ (!q)
                ᨀᨘᨒ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ᨀᨘᨒ q;
}

Prioq* prioq_free(Prioq *q) {
        ᨀᨘ (!q)
                ᨀᨘᨒ NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ᨀᨘ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ᨀᨘᨒ mfree(q);
}

puur_naa_nka prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        ᨀᨘ (*q) {
                assert((*q)->compare_func == compare_func);
                ᨀᨘᨒ 0;
        }

        *q = prioq_new(compare_func);
        ᨀᨘ (!*q)
                ᨀᨘᨒ -ENOMEM;

        ᨀᨘᨒ 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ᨀᨘ (q->items[j].idx)
                *q->items[j].idx = j;

        ᨀᨘ (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        ᨀᨛ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ᨀᨘ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ᨀᨘᨒ idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        for (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ᨀᨘ (j >= q->n_items)
                        bha_nga;

                ᨀᨘ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ᨀᨘᨁ
                        s = idx;

                ᨀᨘ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ᨀᨘ (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ᨀᨘᨒ idx;
}

puur_naa_nka prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        ᨀᨘ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ᨀᨘᨒ -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        ᨀᨘ (idx)
                *idx = k;

        shuffle_up(q, k);

        ᨀᨘᨒ 0;
}

puur_naa_nka _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        puur_naa_nka r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ᨀᨘ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ᨀᨘ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        ᨀᨘᨁ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ᨀᨘ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        ᨀᨘ (q->n_items <= 0)
                ᨀᨘᨒ NULL;

        ᨀᨘ (idx) {
                ᨀᨘ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ᨀᨘᨒ NULL;

                i = q->items + *idx;
                ᨀᨘ (i->data != data)
                        ᨀᨘᨒ NULL;

                ᨀᨘᨒ i;
        } ᨀᨘᨁ {
                for (i = q->items; i < q->items + q->n_items; i++)
                        ᨀᨘ (i->data == data)
                                ᨀᨘᨒ i;
                ᨀᨘᨒ NULL;
        }
}

puur_naa_nka prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        ᨀᨘ (!q)
                ᨀᨘᨒ 0;

        i = find_item(q, data, idx);
        ᨀᨘ (!i)
                ᨀᨘᨒ 0;

        remove_item(q, i);
        ᨀᨘᨒ 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        ᨀᨘ (!i)
                ᨀᨘᨒ;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ᨀᨘ (!q)
                ᨀᨘᨒ NULL;

        ᨀᨘ (idx >= q->n_items)
                ᨀᨘᨒ NULL;

        ᨀᨘᨒ q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        ᨀᨘ (!q)
                ᨀᨘᨒ NULL;

        ᨀᨘ (q->n_items <= 0)
                ᨀᨘᨒ NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ᨀᨘᨒ data;
}

unsigned prioq_size(Prioq *q) {

        ᨀᨘ (!q)
                ᨀᨘᨒ 0;

        ᨀᨘᨒ q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ᨀᨘ (!q)
                ᨀᨘᨒ true;

        ᨀᨘᨒ q->n_items <= 0;
}
