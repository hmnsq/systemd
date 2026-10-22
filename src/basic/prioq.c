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
        නම් (!q)
                ආපසු q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ආපසු q;
}

Prioq* prioq_free(Prioq *q) {
        නම් (!q)
                ආපසු NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                නම් (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ආපසු mfree(q);
}

පූර්ණ_සංඛ්‍යා prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        නම් (*q) {
                assert((*q)->compare_func == compare_func);
                ආපසු 0;
        }

        *q = prioq_new(compare_func);
        නම් (!*q)
                ආපසු -ENOMEM;

        ආපසු 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        නම් (q->items[j].idx)
                *q->items[j].idx = j;

        නම් (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        අතරතුර (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                නම් (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ආපසු idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        සඳහා (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                නම් (j >= q->n_items)
                        bha_nga;

                නම් (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                නැතහොත්
                        s = idx;

                නම් (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                නම් (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ආපසු idx;
}

පූර්ණ_සංඛ්‍යා prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        නම් (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ආපසු -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        නම් (idx)
                *idx = k;

        shuffle_up(q, k);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        පූර්ණ_සංඛ්‍යා r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        නම් (r < 0)
                ආපසු r;

        ආපසු prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        නම් (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        නම් (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        නැතහොත් {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                නම් (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        නම් (q->n_items <= 0)
                ආපසු NULL;

        නම් (idx) {
                නම් (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ආපසු NULL;

                i = q->items + *idx;
                නම් (i->data != data)
                        ආපසු NULL;

                ආපසු i;
        } නැතහොත් {
                සඳහා (i = q->items; i < q->items + q->n_items; i++)
                        නම් (i->data == data)
                                ආපසු i;
                ආපසු NULL;
        }
}

පූර්ණ_සංඛ්‍යා prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        නම් (!q)
                ආපසු 0;

        i = find_item(q, data, idx);
        නම් (!i)
                ආපසු 0;

        remove_item(q, i);
        ආපසු 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        නම් (!i)
                ආපසු;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        නම් (!q)
                ආපසු NULL;

        නම් (idx >= q->n_items)
                ආපසු NULL;

        ආපසු q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        නම් (!q)
                ආපසු NULL;

        නම් (q->n_items <= 0)
                ආපසු NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ආපසු data;
}

unsigned prioq_size(Prioq *q) {

        නම් (!q)
                ආපසු 0;

        ආපසු q->n_items;
}

bool prioq_isempty(Prioq *q) {

        නම් (!q)
                ආපසු true;

        ආපසු q->n_items <= 0;
}
