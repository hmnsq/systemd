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
        ຖ້າ (!q)
                ສົ່ງຄືນ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ສົ່ງຄືນ q;
}

Prioq* prioq_free(Prioq *q) {
        ຖ້າ (!q)
                ສົ່ງຄືນ NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                ຖ້າ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ສົ່ງຄືນ mfree(q);
}

ຈຳນວນເຕັມ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        ຖ້າ (*q) {
                assert((*q)->compare_func == compare_func);
                ສົ່ງຄືນ 0;
        }

        *q = prioq_new(compare_func);
        ຖ້າ (!*q)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        ຖ້າ (q->items[j].idx)
                *q->items[j].idx = j;

        ຖ້າ (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        ໃນຂະນະທີ່ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ຖ້າ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ສົ່ງຄືນ idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        ສຳລັບ (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ຖ້າ (j >= q->n_items)
                        bha_nga;

                ຖ້າ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ບໍ່ດັ່ງນັ້ນ
                        s = idx;

                ຖ້າ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ຖ້າ (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ສົ່ງຄືນ idx;
}

ຈຳນວນເຕັມ prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        ຖ້າ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ສົ່ງຄືນ -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        ຖ້າ (idx)
                *idx = k;

        shuffle_up(q, k);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        ຈຳນວນເຕັມ r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ຖ້າ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        ຖ້າ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        ບໍ່ດັ່ງນັ້ນ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                ຖ້າ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        ຖ້າ (q->n_items <= 0)
                ສົ່ງຄືນ NULL;

        ຖ້າ (idx) {
                ຖ້າ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ສົ່ງຄືນ NULL;

                i = q->items + *idx;
                ຖ້າ (i->data != data)
                        ສົ່ງຄືນ NULL;

                ສົ່ງຄືນ i;
        } ບໍ່ດັ່ງນັ້ນ {
                ສຳລັບ (i = q->items; i < q->items + q->n_items; i++)
                        ຖ້າ (i->data == data)
                                ສົ່ງຄືນ i;
                ສົ່ງຄືນ NULL;
        }
}

ຈຳນວນເຕັມ prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        ຖ້າ (!q)
                ສົ່ງຄືນ 0;

        i = find_item(q, data, idx);
        ຖ້າ (!i)
                ສົ່ງຄືນ 0;

        remove_item(q, i);
        ສົ່ງຄືນ 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        ຖ້າ (!i)
                ສົ່ງຄືນ;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ຖ້າ (!q)
                ສົ່ງຄືນ NULL;

        ຖ້າ (idx >= q->n_items)
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        ຖ້າ (!q)
                ສົ່ງຄືນ NULL;

        ຖ້າ (q->n_items <= 0)
                ສົ່ງຄືນ NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ສົ່ງຄືນ data;
}

unsigned prioq_size(Prioq *q) {

        ຖ້າ (!q)
                ສົ່ງຄືນ 0;

        ສົ່ງຄືນ q->n_items;
}

bool prioq_isempty(Prioq *q) {

        ຖ້າ (!q)
                ສົ່ງຄືນ true;

        ສົ່ງຄືນ q->n_items <= 0;
}
