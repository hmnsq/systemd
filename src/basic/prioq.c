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
        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ᬯᬮᬶ q;
}

Prioq* prioq_free(Prioq *q) {
        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ᬬᬾᬦ᭄ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ᬯᬮᬶ mfree(q);
}

puur_naa_nka prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        ᬬᬾᬦ᭄ (*q) {
                assert((*q)->compare_func == compare_func);
                ᬯᬮᬶ 0;
        }

        *q = prioq_new(compare_func);
        ᬬᬾᬦ᭄ (!*q)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ᬬᬾᬦ᭄ (q->items[j].idx)
                *q->items[j].idx = j;

        ᬬᬾᬦ᭄ (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        ᬲᭂᬤᭂᬮ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ᬬᬾᬦ᭄ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ᬯᬮᬶ idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        for (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ᬬᬾᬦ᭄ (j >= q->n_items)
                        bha_nga;

                ᬬᬾᬦ᭄ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ᬮᬶᬬ
                        s = idx;

                ᬬᬾᬦ᭄ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ᬬᬾᬦ᭄ (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ᬯᬮᬶ idx;
}

puur_naa_nka prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        ᬬᬾᬦ᭄ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ᬯᬮᬶ -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        ᬬᬾᬦ᭄ (idx)
                *idx = k;

        shuffle_up(q, k);

        ᬯᬮᬶ 0;
}

puur_naa_nka _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        puur_naa_nka r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ᬬᬾᬦ᭄ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ᬬᬾᬦ᭄ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        ᬮᬶᬬ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ᬬᬾᬦ᭄ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        ᬬᬾᬦ᭄ (q->n_items <= 0)
                ᬯᬮᬶ NULL;

        ᬬᬾᬦ᭄ (idx) {
                ᬬᬾᬦ᭄ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ᬯᬮᬶ NULL;

                i = q->items + *idx;
                ᬬᬾᬦ᭄ (i->data != data)
                        ᬯᬮᬶ NULL;

                ᬯᬮᬶ i;
        } ᬮᬶᬬ {
                for (i = q->items; i < q->items + q->n_items; i++)
                        ᬬᬾᬦ᭄ (i->data == data)
                                ᬯᬮᬶ i;
                ᬯᬮᬶ NULL;
        }
}

puur_naa_nka prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ 0;

        i = find_item(q, data, idx);
        ᬬᬾᬦ᭄ (!i)
                ᬯᬮᬶ 0;

        remove_item(q, i);
        ᬯᬮᬶ 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        ᬬᬾᬦ᭄ (!i)
                ᬯᬮᬶ;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ NULL;

        ᬬᬾᬦ᭄ (idx >= q->n_items)
                ᬯᬮᬶ NULL;

        ᬯᬮᬶ q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ NULL;

        ᬬᬾᬦ᭄ (q->n_items <= 0)
                ᬯᬮᬶ NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ᬯᬮᬶ data;
}

unsigned prioq_size(Prioq *q) {

        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ 0;

        ᬯᬮᬶ q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ᬬᬾᬦ᭄ (!q)
                ᬯᬮᬶ true;

        ᬯᬮᬶ q->n_items <= 0;
}
