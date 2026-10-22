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
        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ᤀᤡᤴ q;
}

Prioq* prioq_free(Prioq *q) {
        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ᤀᤡ᤺ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ᤀᤡᤴ mfree(q);
}

puur_naa_nka prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        ᤀᤡ᤺ (*q) {
                assert((*q)->compare_func == compare_func);
                ᤀᤡᤴ 0;
        }

        *q = prioq_new(compare_func);
        ᤀᤡ᤺ (!*q)
                ᤀᤡᤴ -ENOMEM;

        ᤀᤡᤴ 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ᤀᤡ᤺ (q->items[j].idx)
                *q->items[j].idx = j;

        ᤀᤡ᤺ (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        ᤀᤠ᤺ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ᤀᤡ᤺ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ᤀᤡᤴ idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        for (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ᤀᤡ᤺ (j >= q->n_items)
                        bha_nga;

                ᤀᤡ᤺ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ᤀᤡ᤺ᤆᤡ
                        s = idx;

                ᤀᤡ᤺ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ᤀᤡ᤺ (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ᤀᤡᤴ idx;
}

puur_naa_nka prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        ᤀᤡ᤺ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ᤀᤡᤴ -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        ᤀᤡ᤺ (idx)
                *idx = k;

        shuffle_up(q, k);

        ᤀᤡᤴ 0;
}

puur_naa_nka _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        puur_naa_nka r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ᤀᤡ᤺ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ᤀᤡ᤺ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        ᤀᤡ᤺ᤆᤡ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ᤀᤡ᤺ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        ᤀᤡ᤺ (q->n_items <= 0)
                ᤀᤡᤴ NULL;

        ᤀᤡ᤺ (idx) {
                ᤀᤡ᤺ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ᤀᤡᤴ NULL;

                i = q->items + *idx;
                ᤀᤡ᤺ (i->data != data)
                        ᤀᤡᤴ NULL;

                ᤀᤡᤴ i;
        } ᤀᤡ᤺ᤆᤡ {
                for (i = q->items; i < q->items + q->n_items; i++)
                        ᤀᤡ᤺ (i->data == data)
                                ᤀᤡᤴ i;
                ᤀᤡᤴ NULL;
        }
}

puur_naa_nka prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ 0;

        i = find_item(q, data, idx);
        ᤀᤡ᤺ (!i)
                ᤀᤡᤴ 0;

        remove_item(q, i);
        ᤀᤡᤴ 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        ᤀᤡ᤺ (!i)
                ᤀᤡᤴ;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ NULL;

        ᤀᤡ᤺ (idx >= q->n_items)
                ᤀᤡᤴ NULL;

        ᤀᤡᤴ q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ NULL;

        ᤀᤡ᤺ (q->n_items <= 0)
                ᤀᤡᤴ NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ᤀᤡᤴ data;
}

unsigned prioq_size(Prioq *q) {

        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ 0;

        ᤀᤡᤴ q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ᤀᤡ᤺ (!q)
                ᤀᤡᤴ true;

        ᤀᤡᤴ q->n_items <= 0;
}
