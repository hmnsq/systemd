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
        အကယ်၍ (!q)
                ပြန်ပေး q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ပြန်ပေး q;
}

Prioq* prioq_free(Prioq *q) {
        အကယ်၍ (!q)
                ပြန်ပေး NULL;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->items, q->n_items)
                အကယ်၍ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        free(q->items);
        ပြန်ပေး mfree(q);
}

ကိန်းပြည့် prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        assert(q);

        အကယ်၍ (*q) {
                assert((*q)->compare_func == compare_func);
                ပြန်ပေး 0;
        }

        *q = prioq_new(compare_func);
        အကယ်၍ (!*q)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး 0;
}

static void swap(Prioq *q, unsigned j, unsigned k) {
        assert(q);
        assert(j < q->n_items);
        assert(k < q->n_items);

        assert(!q->items[j].idx || *(q->items[j].idx) == j);
        assert(!q->items[k].idx || *(q->items[k].idx) == k);

        SWAP_TWO(q->items[j].data, q->items[k].data);
        SWAP_TWO(q->items[j].idx, q->items[k].idx);

        အကယ်၍ (q->items[j].idx)
                *q->items[j].idx = j;

        အကယ်၍ (q->items[k].idx)
                *q->items[k].idx = k;
}

static unsigned shuffle_up(Prioq *q, unsigned idx) {
        assert(q);
        assert(idx < q->n_items);

        နေစဉ် (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                အကယ်၍ (q->compare_func(q->items[k].data, q->items[idx].data) <= 0)
                        bha_nga;

                swap(q, idx, k);
                idx = k;
        }

        ပြန်ပေး idx;
}

static unsigned shuffle_down(Prioq *q, unsigned idx) {
        assert(q);

        အတွက် (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                အကယ်၍ (j >= q->n_items)
                        bha_nga;

                အကယ်၍ (q->compare_func(q->items[j].data, q->items[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                မဟုတ်ပါက
                        s = idx;

                အကယ်၍ (k < q->n_items &&
                    q->compare_func(q->items[k].data, q->items[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                အကယ်၍ (s == idx)
                        /* No swap necessary, we're done */
                        bha_nga;

                swap(q, idx, s);
                idx = s;
        }

        ပြန်ပေး idx;
}

ကိန်းပြည့် prioq_put(Prioq *q, void *data, unsigned *idx) {
        unsigned k;

        assert(q);

        အကယ်၍ (!GREEDY_REALLOC(q->items, MAX(q->n_items + 1, 16u)))
                ပြန်ပေး -ENOMEM;

        k = q->n_items++;
        q->items[k] = (sa_nracanaa prioq_item) {
                .data = data,
                .idx = idx,
        };

        အကယ်၍ (idx)
                *idx = k;

        shuffle_up(q, k);

        ပြန်ပေး 0;
}

ကိန်းပြည့် _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx) {
        ကိန်းပြည့် r;

        assert(q);

        r = prioq_ensure_allocated(q, compare_func);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး prioq_put(*q, data, idx);
}

static void remove_item(Prioq *q, sa_nracanaa prioq_item *i) {
        sa_nracanaa prioq_item *l;

        assert(q);
        assert(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        အကယ်၍ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->items + q->n_items - 1;

        အကယ်၍ (i == l)
                /* Last entry, let's just remove it */
                q->n_items--;
        မဟုတ်ပါက {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                assert(i >= q->items);
                k = i - q->items;

                i->data = l->data;
                i->idx = l->idx;
                အကယ်၍ (i->idx)
                        *i->idx = k;
                q->n_items--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

static sa_nracanaa prioq_item* find_item(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        assert(q);

        အကယ်၍ (q->n_items <= 0)
                ပြန်ပေး NULL;

        အကယ်၍ (idx) {
                အကယ်၍ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_items)
                        ပြန်ပေး NULL;

                i = q->items + *idx;
                အကယ်၍ (i->data != data)
                        ပြန်ပေး NULL;

                ပြန်ပေး i;
        } မဟုတ်ပါက {
                အတွက် (i = q->items; i < q->items + q->n_items; i++)
                        အကယ်၍ (i->data == data)
                                ပြန်ပေး i;
                ပြန်ပေး NULL;
        }
}

ကိန်းပြည့် prioq_remove(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;

        အကယ်၍ (!q)
                ပြန်ပေး 0;

        i = find_item(q, data, idx);
        အကယ်၍ (!i)
                ပြန်ပေး 0;

        remove_item(q, i);
        ပြန်ပေး 1;
}

void prioq_reshuffle(Prioq *q, void *data, unsigned *idx) {
        sa_nracanaa prioq_item *i;
        unsigned k;

        assert(q);

        i = find_item(q, data, idx);
        အကယ်၍ (!i)
                ပြန်ပေး;

        assert(i >= q->items);
        k = i - q->items;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

void* prioq_peek_by_index(Prioq *q, unsigned idx) {
        အကယ်၍ (!q)
                ပြန်ပေး NULL;

        အကယ်၍ (idx >= q->n_items)
                ပြန်ပေး NULL;

        ပြန်ပေး q->items[idx].data;
}

void* prioq_pop(Prioq *q) {
        void *data;

        အကယ်၍ (!q)
                ပြန်ပေး NULL;

        အကယ်၍ (q->n_items <= 0)
                ပြန်ပေး NULL;

        data = q->items[0].data;
        remove_item(q, q->items);
        ပြန်ပေး data;
}

unsigned prioq_size(Prioq *q) {

        အကယ်၍ (!q)
                ပြန်ပေး 0;

        ပြန်ပေး q->n_items;
}

bool prioq_isempty(Prioq *q) {

        အကယ်၍ (!q)
                ပြန်ပေး true;

        ပြန်ပေး q->n_items <= 0;
}
