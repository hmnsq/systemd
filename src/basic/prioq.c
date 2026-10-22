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

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "prioq.h"

ಸಂರಚನಾ prioq_item {
        ಶೂನ್ಯ *data;
        unsigned *idx;
};

ಸಂರಚನಾ Prioq {
        compare_func_t compare_func;
        unsigned n_ವಸ್ತುಗಳು;
        ಸಂರಚನಾ prioq_item *ವಸ್ತುಗಳು;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ಪ್ರತ್ಯಾಗಮ q;
}

Prioq* prioq_free(Prioq *q) {
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->ವಸ್ತುಗಳು, q->n_ವಸ್ತುಗಳು)
                ಯದಿ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        ಮುಕ್ತ(q->ವಸ್ತುಗಳು);
        ಪ್ರತ್ಯಾಗಮ mfree(q);
}

ಪೂರ್ಣಾಙ್ಕ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        ಪ್ರತಿಜ್ಞಾ(q);

        ಯದಿ (*q) {
                ಪ್ರತಿಜ್ಞಾ((*q)->compare_func == compare_func);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        *q = prioq_new(compare_func);
        ಯದಿ (!*q)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ swap(Prioq *q, unsigned j, unsigned k) {
        ಪ್ರತಿಜ್ಞಾ(q);
        ಪ್ರತಿಜ್ಞಾ(j < q->n_ವಸ್ತುಗಳು);
        ಪ್ರತಿಜ್ಞಾ(k < q->n_ವಸ್ತುಗಳು);

        ಪ್ರತಿಜ್ಞಾ(!q->ವಸ್ತುಗಳು[j].idx || *(q->ವಸ್ತುಗಳು[j].idx) == j);
        ಪ್ರತಿಜ್ಞಾ(!q->ವಸ್ತುಗಳು[k].idx || *(q->ವಸ್ತುಗಳು[k].idx) == k);

        SWAP_TWO(q->ವಸ್ತುಗಳು[j].data, q->ವಸ್ತುಗಳು[k].data);
        SWAP_TWO(q->ವಸ್ತುಗಳು[j].idx, q->ವಸ್ತುಗಳು[k].idx);

        ಯದಿ (q->ವಸ್ತುಗಳು[j].idx)
                *q->ವಸ್ತುಗಳು[j].idx = j;

        ಯದಿ (q->ವಸ್ತುಗಳು[k].idx)
                *q->ವಸ್ತುಗಳು[k].idx = k;
}

ಸ್ಥಿರ unsigned shuffle_up(Prioq *q, unsigned idx) {
        ಪ್ರತಿಜ್ಞಾ(q);
        ಪ್ರತಿಜ್ಞಾ(idx < q->n_ವಸ್ತುಗಳು);

        ಯಾವತ್ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                ಯದಿ (q->compare_func(q->ವಸ್ತುಗಳು[k].data, q->ವಸ್ತುಗಳು[idx].data) <= 0)
                        ಭಙ್ಗ;

                swap(q, idx, k);
                idx = k;
        }

        ಪ್ರತ್ಯಾಗಮ idx;
}

ಸ್ಥಿರ unsigned shuffle_down(Prioq *q, unsigned idx) {
        ಪ್ರತಿಜ್ಞಾ(q);

        ಪ್ರತಿ (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ಯದಿ (j >= q->n_ವಸ್ತುಗಳು)
                        ಭಙ್ಗ;

                ಯದಿ (q->compare_func(q->ವಸ್ತುಗಳು[j].data, q->ವಸ್ತುಗಳು[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ಅನ್ಯಥಾ
                        s = idx;

                ಯದಿ (k < q->n_ವಸ್ತುಗಳು &&
                    q->compare_func(q->ವಸ್ತುಗಳು[k].data, q->ವಸ್ತುಗಳು[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ಯದಿ (s == idx)
                        /* No swap necessary, we're done */
                        ಭಙ್ಗ;

                swap(q, idx, s);
                idx = s;
        }

        ಪ್ರತ್ಯಾಗಮ idx;
}

ಪೂರ್ಣಾಙ್ಕ prioq_put(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx) {
        unsigned k;

        ಪ್ರತಿಜ್ಞಾ(q);

        ಯದಿ (!GREEDY_REALLOC(q->ವಸ್ತುಗಳು, MAX(q->n_ವಸ್ತುಗಳು + 1, 16u)))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        k = q->n_ವಸ್ತುಗಳು++;
        q->ವಸ್ತುಗಳು[k] = (ಸಂರಚನಾ prioq_item) {
                .data = data,
                .idx = idx,
        };

        ಯದಿ (idx)
                *idx = k;

        shuffle_up(q, k);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ಶೂನ್ಯ *data, unsigned *idx) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(q);

        r = prioq_ensure_allocated(q, compare_func);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ prioq_put(*q, data, idx);
}

ಸ್ಥಿರ ಶೂನ್ಯ remove_item(Prioq *q, ಸಂರಚನಾ prioq_item *i) {
        ಸಂರಚನಾ prioq_item *l;

        ಪ್ರತಿಜ್ಞಾ(q);
        ಪ್ರತಿಜ್ಞಾ(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ಯದಿ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->ವಸ್ತುಗಳು + q->n_ವಸ್ತುಗಳು - 1;

        ಯದಿ (i == l)
                /* Last entry, let's just remove it */
                q->n_ವಸ್ತುಗಳು--;
        ಅನ್ಯಥಾ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                ಪ್ರತಿಜ್ಞಾ(i >= q->ವಸ್ತುಗಳು);
                k = i - q->ವಸ್ತುಗಳು;

                i->data = l->data;
                i->idx = l->idx;
                ಯದಿ (i->idx)
                        *i->idx = k;
                q->n_ವಸ್ತುಗಳು--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

ಸ್ಥಿರ ಸಂರಚನಾ prioq_item* find_item(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx) {
        ಸಂರಚನಾ prioq_item *i;

        ಪ್ರತಿಜ್ಞಾ(q);

        ಯದಿ (q->n_ವಸ್ತುಗಳು <= 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (idx) {
                ಯದಿ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_ವಸ್ತುಗಳು)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                i = q->ವಸ್ತುಗಳು + *idx;
                ಯದಿ (i->data != data)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಪ್ರತ್ಯಾಗಮ i;
        } ಅನ್ಯಥಾ {
                ಪ್ರತಿ (i = q->ವಸ್ತುಗಳು; i < q->ವಸ್ತುಗಳು + q->n_ವಸ್ತುಗಳು; i++)
                        ಯದಿ (i->data == data)
                                ಪ್ರತ್ಯಾಗಮ i;
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
}

ಪೂರ್ಣಾಙ್ಕ prioq_remove(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx) {
        ಸಂರಚನಾ prioq_item *i;

        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ 0;

        i = find_item(q, data, idx);
        ಯದಿ (!i)
                ಪ್ರತ್ಯಾಗಮ 0;

        remove_item(q, i);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ prioq_reshuffle(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx) {
        ಸಂರಚನಾ prioq_item *i;
        unsigned k;

        ಪ್ರತಿಜ್ಞಾ(q);

        i = find_item(q, data, idx);
        ಯದಿ (!i)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿಜ್ಞಾ(i >= q->ವಸ್ತುಗಳು);
        k = i - q->ವಸ್ತುಗಳು;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

ಶೂನ್ಯ* prioq_peek_by_index(Prioq *q, unsigned idx) {
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (idx >= q->n_ವಸ್ತುಗಳು)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ q->ವಸ್ತುಗಳು[idx].data;
}

ಶೂನ್ಯ* prioq_ತೆಗೆ(Prioq *q) {
        ಶೂನ್ಯ *data;

        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (q->n_ವಸ್ತುಗಳು <= 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        data = q->ವಸ್ತುಗಳು[0].data;
        remove_item(q, q->ವಸ್ತುಗಳು);
        ಪ್ರತ್ಯಾಗಮ data;
}

unsigned prioq_size(Prioq *q) {

        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ q->n_ವಸ್ತುಗಳು;
}

bool prioq_isempty(Prioq *q) {

        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ q->n_ವಸ್ತುಗಳು <= 0;
}
