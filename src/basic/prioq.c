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
        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ᮝᮜᮤ q;
}

Prioq* prioq_free(Prioq *q) {
        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ᮚᮦᮔ᮪ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ᮝᮜᮤ mfree(q);
}

puur_naa_nka prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        ᮚᮦᮔ᮪ (*q) {
                assert((*q)->compare_func == compare_func);
                ᮝᮜᮤ 0;
        }

        *q = prioq_new(compare_func);
        ᮚᮦᮔ᮪ (!*q)
                ᮝᮜᮤ -ENOMEM;

        ᮝᮜᮤ 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ᮚᮦᮔ᮪ (q->items[j].idx)
                *q->items[j].idx = j;

        ᮚᮦᮔ᮪ (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        ᮞᮨᮓᮨᮜ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ᮚᮦᮔ᮪ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ᮝᮜᮤ idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        for (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ᮚᮦᮔ᮪ (j >= q->n_items)
                        bha_nga;

                ᮚᮦᮔ᮪ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ᮜᮤᮚ
                        s = idx;

                ᮚᮦᮔ᮪ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ᮚᮦᮔ᮪ (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ᮝᮜᮤ idx;
}

puur_naa_nka prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        ᮚᮦᮔ᮪ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ᮝᮜᮤ -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        ᮚᮦᮔ᮪ (idx)
                *idx = k;

        shuffle_up(q, k);

        ᮝᮜᮤ 0;
}

puur_naa_nka _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        puur_naa_nka r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ᮚᮦᮔ᮪ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ᮚᮦᮔ᮪ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        ᮜᮤᮚ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ᮚᮦᮔ᮪ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        ᮚᮦᮔ᮪ (q->n_items <= 0)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (idx) {
                ᮚᮦᮔ᮪ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ᮝᮜᮤ NULL;

                i = q->items + *idx;
                ᮚᮦᮔ᮪ (i->data != data)
                        ᮝᮜᮤ NULL;

                ᮝᮜᮤ i;
        } ᮜᮤᮚ {
                for (i = q->items; i < q->items + q->n_items; i++)
                        ᮚᮦᮔ᮪ (i->data == data)
                                ᮝᮜᮤ i;
                ᮝᮜᮤ NULL;
        }
}

puur_naa_nka prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ 0;

        i = find_item(q, data, idx);
        ᮚᮦᮔ᮪ (!i)
                ᮝᮜᮤ 0;

        remove_item(q, i);
        ᮝᮜᮤ 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        ᮚᮦᮔ᮪ (!i)
                ᮝᮜᮤ;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (idx >= q->n_items)
                ᮝᮜᮤ NULL;

        ᮝᮜᮤ q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ NULL;

        ᮚᮦᮔ᮪ (q->n_items <= 0)
                ᮝᮜᮤ NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ᮝᮜᮤ data;
}

unsigned prioq_size(Prioq *q) {

        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ 0;

        ᮝᮜᮤ q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ᮚᮦᮔ᮪ (!q)
                ᮝᮜᮤ true;

        ᮝᮜᮤ q->n_items <= 0;
}
