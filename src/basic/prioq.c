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

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "prioq.h"

ସଂରଚନା prioq_item {
        ଶୂନ୍ୟ *data;
        unsigned *idx;
};

ସଂରଚନା Prioq {
        compare_func_t compare_func;
        unsigned n_ବସ୍ତୁଗୁଡ଼ିକ;
        ସଂରଚନା prioq_item *ବସ୍ତୁଗୁଡ଼ିକ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        ଯଦି (!q)
                ଫେରନ୍ତୁ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ଫେରନ୍ତୁ q;
}

Prioq* prioq_free(Prioq *q) {
        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->ବସ୍ତୁଗୁଡ଼ିକ, q->n_ବସ୍ତୁଗୁଡ଼ିକ)
                ଯଦି (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        ମୁକ୍ତ(q->ବସ୍ତୁଗୁଡ଼ିକ);
        ଫେରନ୍ତୁ mfree(q);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        ପ୍ରତିଜ୍ଞା(q);

        ଯଦି (*q) {
                ପ୍ରତିଜ୍ଞା((*q)->compare_func == compare_func);
                ଫେରନ୍ତୁ 0;
        }

        *q = prioq_new(compare_func);
        ଯଦି (!*q)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ swap(Prioq *q, unsigned j, unsigned k) {
        ପ୍ରତିଜ୍ଞା(q);
        ପ୍ରତିଜ୍ଞା(j < q->n_ବସ୍ତୁଗୁଡ଼ିକ);
        ପ୍ରତିଜ୍ଞା(k < q->n_ବସ୍ତୁଗୁଡ଼ିକ);

        ପ୍ରତିଜ୍ଞା(!q->ବସ୍ତୁଗୁଡ଼ିକ[j].idx || *(q->ବସ୍ତୁଗୁଡ଼ିକ[j].idx) == j);
        ପ୍ରତିଜ୍ଞା(!q->ବସ୍ତୁଗୁଡ଼ିକ[k].idx || *(q->ବସ୍ତୁଗୁଡ଼ିକ[k].idx) == k);

        SWAP_TWO(q->ବସ୍ତୁଗୁଡ଼ିକ[j].data, q->ବସ୍ତୁଗୁଡ଼ିକ[k].data);
        SWAP_TWO(q->ବସ୍ତୁଗୁଡ଼ିକ[j].idx, q->ବସ୍ତୁଗୁଡ଼ିକ[k].idx);

        ଯଦି (q->ବସ୍ତୁଗୁଡ଼ିକ[j].idx)
                *q->ବସ୍ତୁଗୁଡ଼ିକ[j].idx = j;

        ଯଦି (q->ବସ୍ତୁଗୁଡ଼ିକ[k].idx)
                *q->ବସ୍ତୁଗୁଡ଼ିକ[k].idx = k;
}

ସ୍ଥିର unsigned shuffle_up(Prioq *q, unsigned idx) {
        ପ୍ରତିଜ୍ଞା(q);
        ପ୍ରତିଜ୍ଞା(idx < q->n_ବସ୍ତୁଗୁଡ଼ିକ);

        ଯେତେବେଳେ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ଯଦି (q->compare_func(q->ବସ୍ତୁଗୁଡ଼ିକ[k].data, q->ବସ୍ତୁଗୁଡ଼ିକ[idx].data) <= 0)
                        ଭାଙ୍ଗ;

                swap(q, idx, k);
                idx = k;
        }

        ଫେରନ୍ତୁ idx;
}

ସ୍ଥିର unsigned shuffle_down(Prioq *q, unsigned idx) {
        ପ୍ରତିଜ୍ଞା(q);

        ପାଇଁ (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ଯଦି (j >= q->n_ବସ୍ତୁଗୁଡ଼ିକ)
                        ଭାଙ୍ଗ;

                ଯଦି (q->compare_func(q->ବସ୍ତୁଗୁଡ଼ିକ[j].data, q->ବସ୍ତୁଗୁଡ଼ିକ[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ଅନ୍ୟଥା
                        s = idx;

                ଯଦି (k < q->n_ବସ୍ତୁଗୁଡ଼ିକ &&
                    q->compare_func(q->ବସ୍ତୁଗୁଡ଼ିକ[k].data, q->ବସ୍ତୁଗୁଡ଼ିକ[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ଯଦି (s == idx)
                        /* No swap necessary, we're done */
                        ଭାଙ୍ଗ;

                swap(q, idx, s);
                idx = s;
        }

        ଫେରନ୍ତୁ idx;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prioq_put(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx) {
        unsigned k;

        ପ୍ରତିଜ୍ଞା(q);

        ଯଦି (!GREEDY_REALLOC(q->ବସ୍ତୁଗୁଡ଼ିକ, MAX(q->n_ବସ୍ତୁଗୁଡ଼ିକ + 1, 16u)))
                ଫେରନ୍ତୁ -ENOMEM;

        k = q->n_ବସ୍ତୁଗୁଡ଼ିକ++;
        q->ବସ୍ତୁଗୁଡ଼ିକ[k] = (ସଂରଚନା prioq_item) {
                .data = data,
                .idx = idx,
        };

        ଯଦି (idx)
                *idx = k;

        shuffle_up(q, k);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ଶୂନ୍ୟ *data, unsigned *idx) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(q);

        r = prioq_ensure_allocated(q, compare_func);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ prioq_put(*q, data, idx);
}

ସ୍ଥିର ଶୂନ୍ୟ remove_item(Prioq *q, ସଂରଚନା prioq_item *i) {
        ସଂରଚନା prioq_item *l;

        ପ୍ରତିଜ୍ଞା(q);
        ପ୍ରତିଜ୍ଞା(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ଯଦି (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->ବସ୍ତୁଗୁଡ଼ିକ + q->n_ବସ୍ତୁଗୁଡ଼ିକ - 1;

        ଯଦି (i == l)
                /* Last entry, let's just remove it */
                q->n_ବସ୍ତୁଗୁଡ଼ିକ--;
        ଅନ୍ୟଥା {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                ପ୍ରତିଜ୍ଞା(i >= q->ବସ୍ତୁଗୁଡ଼ିକ);
                k = i - q->ବସ୍ତୁଗୁଡ଼ିକ;

                i->data = l->data;
                i->idx = l->idx;
                ଯଦି (i->idx)
                        *i->idx = k;
                q->n_ବସ୍ତୁଗୁଡ଼ିକ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ସ୍ଥିର ସଂରଚନା prioq_item* find_item(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx) {
        ସଂରଚନା prioq_item *i;

        ପ୍ରତିଜ୍ଞା(q);

        ଯଦି (q->n_ବସ୍ତୁଗୁଡ଼ିକ <= 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (idx) {
                ଯଦି (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_ବସ୍ତୁଗୁଡ଼ିକ)
                        ଫେରନ୍ତୁ ଖାଲି;

                i = q->ବସ୍ତୁଗୁଡ଼ିକ + *idx;
                ଯଦି (i->data != data)
                        ଫେରନ୍ତୁ ଖାଲି;

                ଫେରନ୍ତୁ i;
        } ଅନ୍ୟଥା {
                ପାଇଁ (i = q->ବସ୍ତୁଗୁଡ଼ିକ; i < q->ବସ୍ତୁଗୁଡ଼ିକ + q->n_ବସ୍ତୁଗୁଡ଼ିକ; i++)
                        ଯଦି (i->data == data)
                                ଫେରନ୍ତୁ i;
                ଫେରନ୍ତୁ ଖାଲି;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prioq_remove(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx) {
        ସଂରଚନା prioq_item *i;

        ଯଦି (!q)
                ଫେରନ୍ତୁ 0;

        i = find_item(q, data, idx);
        ଯଦି (!i)
                ଫେରନ୍ତୁ 0;

        remove_item(q, i);
        ଫେରନ୍ତୁ 1;
}

ଶୂନ୍ୟ prioq_reshuffle(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx) {
        ସଂରଚନା prioq_item *i;
        unsigned k;

        ପ୍ରତିଜ୍ଞା(q);

        i = find_item(q, data, idx);
        ଯଦି (!i)
                ଫେରନ୍ତୁ;

        ପ୍ରତିଜ୍ଞା(i >= q->ବସ୍ତୁଗୁଡ଼ିକ);
        k = i - q->ବସ୍ତୁଗୁଡ଼ିକ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

ଶୂନ୍ୟ* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (idx >= q->n_ବସ୍ତୁଗୁଡ଼ିକ)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ q->ବସ୍ତୁଗୁଡ଼ିକ[idx].data;
}

ଶୂନ୍ୟ* prioq_କାଢ଼(Prioq *q) {
        ଶୂନ୍ୟ *data;

        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (q->n_ବସ୍ତୁଗୁଡ଼ିକ <= 0)
                ଫେରନ୍ତୁ ଖାଲି;

        data = q->ବସ୍ତୁଗୁଡ଼ିକ[0].data;
        remove_item(q, q->ବସ୍ତୁଗୁଡ଼ିକ);
        ଫେରନ୍ତୁ data;
}

unsigned prioq_size(Prioq *q) {

        ଯଦି (!q)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ q->n_ବସ୍ତୁଗୁଡ଼ିକ;
}

bool prioq_isempty(Prioq *q) {

        ଯଦି (!q)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ q->n_ବସ୍ତୁଗୁଡ଼ିକ <= 0;
}
