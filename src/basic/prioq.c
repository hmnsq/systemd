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

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "prioq.h"

સંરચના prioq_item {
        શૂન્ય *data;
        unsigned *idx;
};

સંરચના Prioq {
        compare_func_t compare_func;
        unsigned n_વસ્તુઓ;
        સંરચના prioq_item *વસ્તુઓ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        જો (!q)
                પાછા q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        પાછા q;
}

Prioq* prioq_free(Prioq *q) {
        જો (!q)
                પાછા ખાલી;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->વસ્તુઓ, q->n_વસ્તુઓ)
                જો (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        મુક્ત(q->વસ્તુઓ);
        પાછા mfree(q);
}

પૂર્ણાંક prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        પ્રતિજ્ઞા(q);

        જો (*q) {
                પ્રતિજ્ઞા((*q)->compare_func == compare_func);
                પાછા 0;
        }

        *q = prioq_new(compare_func);
        જો (!*q)
                પાછા -ENOMEM;

        પાછા 0;
}

સ્થિર શૂન્ય swap(Prioq *q, unsigned j, unsigned k) {
        પ્રતિજ્ઞા(q);
        પ્રતિજ્ઞા(j < q->n_વસ્તુઓ);
        પ્રતિજ્ઞા(k < q->n_વસ્તુઓ);

        પ્રતિજ્ઞા(!q->વસ્તુઓ[j].idx || *(q->વસ્તુઓ[j].idx) == j);
        પ્રતિજ્ઞા(!q->વસ્તુઓ[k].idx || *(q->વસ્તુઓ[k].idx) == k);

        SWAP_TWO(q->વસ્તુઓ[j].data, q->વસ્તુઓ[k].data);
        SWAP_TWO(q->વસ્તુઓ[j].idx, q->વસ્તુઓ[k].idx);

        જો (q->વસ્તુઓ[j].idx)
                *q->વસ્તુઓ[j].idx = j;

        જો (q->વસ્તુઓ[k].idx)
                *q->વસ્તુઓ[k].idx = k;
}

સ્થિર unsigned shuffle_up(Prioq *q, unsigned idx) {
        પ્રતિજ્ઞા(q);
        પ્રતિજ્ઞા(idx < q->n_વસ્તુઓ);

        જ્યારે (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                જો (q->compare_func(q->વસ્તુઓ[k].data, q->વસ્તુઓ[idx].data) <= 0)
                        તોડો;

                swap(q, idx, k);
                idx = k;
        }

        પાછા idx;
}

સ્થિર unsigned shuffle_down(Prioq *q, unsigned idx) {
        પ્રતિજ્ઞા(q);

        માટે (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                જો (j >= q->n_વસ્તુઓ)
                        તોડો;

                જો (q->compare_func(q->વસ્તુઓ[j].data, q->વસ્તુઓ[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                નહીંતર
                        s = idx;

                જો (k < q->n_વસ્તુઓ &&
                    q->compare_func(q->વસ્તુઓ[k].data, q->વસ્તુઓ[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                જો (s == idx)
                        /* No swap necessary, we're done */
                        તોડો;

                swap(q, idx, s);
                idx = s;
        }

        પાછા idx;
}

પૂર્ણાંક prioq_put(Prioq *q, શૂન્ય *data, unsigned *idx) {
        unsigned k;

        પ્રતિજ્ઞા(q);

        જો (!GREEDY_REALLOC(q->વસ્તુઓ, MAX(q->n_વસ્તુઓ + 1, 16u)))
                પાછા -ENOMEM;

        k = q->n_વસ્તુઓ++;
        q->વસ્તુઓ[k] = (સંરચના prioq_item) {
                .data = data,
                .idx = idx,
        };

        જો (idx)
                *idx = k;

        shuffle_up(q, k);

        પાછા 0;
}

પૂર્ણાંક _prioq_ensure_put(Prioq **q, compare_func_t compare_func, શૂન્ય *data, unsigned *idx) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(q);

        r = prioq_ensure_allocated(q, compare_func);
        જો (r < 0)
                પાછા r;

        પાછા prioq_put(*q, data, idx);
}

સ્થિર શૂન્ય remove_item(Prioq *q, સંરચના prioq_item *i) {
        સંરચના prioq_item *l;

        પ્રતિજ્ઞા(q);
        પ્રતિજ્ઞા(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        જો (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->વસ્તુઓ + q->n_વસ્તુઓ - 1;

        જો (i == l)
                /* Last entry, let's just remove it */
                q->n_વસ્તુઓ--;
        નહીંતર {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                પ્રતિજ્ઞા(i >= q->વસ્તુઓ);
                k = i - q->વસ્તુઓ;

                i->data = l->data;
                i->idx = l->idx;
                જો (i->idx)
                        *i->idx = k;
                q->n_વસ્તુઓ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

સ્થિર સંરચના prioq_item* find_item(Prioq *q, શૂન્ય *data, unsigned *idx) {
        સંરચના prioq_item *i;

        પ્રતિજ્ઞા(q);

        જો (q->n_વસ્તુઓ <= 0)
                પાછા ખાલી;

        જો (idx) {
                જો (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_વસ્તુઓ)
                        પાછા ખાલી;

                i = q->વસ્તુઓ + *idx;
                જો (i->data != data)
                        પાછા ખાલી;

                પાછા i;
        } નહીંતર {
                માટે (i = q->વસ્તુઓ; i < q->વસ્તુઓ + q->n_વસ્તુઓ; i++)
                        જો (i->data == data)
                                પાછા i;
                પાછા ખાલી;
        }
}

પૂર્ણાંક prioq_remove(Prioq *q, શૂન્ય *data, unsigned *idx) {
        સંરચના prioq_item *i;

        જો (!q)
                પાછા 0;

        i = find_item(q, data, idx);
        જો (!i)
                પાછા 0;

        remove_item(q, i);
        પાછા 1;
}

શૂન્ય prioq_reshuffle(Prioq *q, શૂન્ય *data, unsigned *idx) {
        સંરચના prioq_item *i;
        unsigned k;

        પ્રતિજ્ઞા(q);

        i = find_item(q, data, idx);
        જો (!i)
                પાછા;

        પ્રતિજ્ઞા(i >= q->વસ્તુઓ);
        k = i - q->વસ્તુઓ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

શૂન્ય* prioq_peek_by_index(Prioq *q, unsigned idx) {
        જો (!q)
                પાછા ખાલી;

        જો (idx >= q->n_વસ્તુઓ)
                પાછા ખાલી;

        પાછા q->વસ્તુઓ[idx].data;
}

શૂન્ય* prioq_કાઢો(Prioq *q) {
        શૂન્ય *data;

        જો (!q)
                પાછા ખાલી;

        જો (q->n_વસ્તુઓ <= 0)
                પાછા ખાલી;

        data = q->વસ્તુઓ[0].data;
        remove_item(q, q->વસ્તુઓ);
        પાછા data;
}

unsigned prioq_size(Prioq *q) {

        જો (!q)
                પાછા 0;

        પાછા q->n_વસ્તુઓ;
}

bool prioq_isempty(Prioq *q) {

        જો (!q)
                પાછા true;

        પાછા q->n_વસ્તુઓ <= 0;
}
