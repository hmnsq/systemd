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

ರಚನೆ prioq_ಅಂಶ {
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;
        ಚಿಹ್ನೆರಹಿತ *idx;
};

ರಚನೆ Prioq {
        compare_func_t compare_func;
        ಚಿಹ್ನೆರಹಿತ n_ವಸ್ತುಗಳು;
        ರಚನೆ prioq_ಅಂಶ *ವಸ್ತುಗಳು;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ಹಿಂತಿರುಗಿ q;
}

Prioq* prioq_free(Prioq *q) {
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(ಅಂಶ, q->ವಸ್ತುಗಳು, q->n_ವಸ್ತುಗಳು)
                ಆದರೆ (ಅಂಶ->idx)
                        *(ಅಂಶ->idx) = PRIOQ_IDX_NULL;

        ಬಿಡುಗಡೆ(q->ವಸ್ತುಗಳು);
        ಹಿಂತಿರುಗಿ mfree(q);
}

ಪೂರ್ಣಾಂಕ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        ಪ್ರತಿಜ್ಞಾ(q);

        ಆದರೆ (*q) {
                ಪ್ರತಿಜ್ಞಾ((*q)->compare_func == compare_func);
                ಹಿಂತಿರುಗಿ 0;
        }

        *q = prioq_new(compare_func);
        ಆದರೆ (!*q)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಅದಲುಬದಲು(Prioq *q, ಚಿಹ್ನೆರಹಿತ j, ಚಿಹ್ನೆರಹಿತ k) {
        ಪ್ರತಿಜ್ಞಾ(q);
        ಪ್ರತಿಜ್ಞಾ(j < q->n_ವಸ್ತುಗಳು);
        ಪ್ರತಿಜ್ಞಾ(k < q->n_ವಸ್ತುಗಳು);

        ಪ್ರತಿಜ್ಞಾ(!q->ವಸ್ತುಗಳು[j].idx || *(q->ವಸ್ತುಗಳು[j].idx) == j);
        ಪ್ರತಿಜ್ಞಾ(!q->ವಸ್ತುಗಳು[k].idx || *(q->ವಸ್ತುಗಳು[k].idx) == k);

        ಅದಲುಬದಲು_TWO(q->ವಸ್ತುಗಳು[j].ದತ್ತಾಂಶ, q->ವಸ್ತುಗಳು[k].ದತ್ತಾಂಶ);
        ಅದಲುಬದಲು_TWO(q->ವಸ್ತುಗಳು[j].idx, q->ವಸ್ತುಗಳು[k].idx);

        ಆದರೆ (q->ವಸ್ತುಗಳು[j].idx)
                *q->ವಸ್ತುಗಳು[j].idx = j;

        ಆದರೆ (q->ವಸ್ತುಗಳು[k].idx)
                *q->ವಸ್ತುಗಳು[k].idx = k;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ shuffle_ಮೇಲೆ(Prioq *q, ಚಿಹ್ನೆರಹಿತ idx) {
        ಪ್ರತಿಜ್ಞಾ(q);
        ಪ್ರತಿಜ್ಞಾ(idx < q->n_ವಸ್ತುಗಳು);

        ಆಗ (idx > 0) {
                ಚಿಹ್ನೆರಹಿತ k;

                k = (idx-1)/2;

                ಆದರೆ (q->compare_func(q->ವಸ್ತುಗಳು[k].ದತ್ತಾಂಶ, q->ವಸ್ತುಗಳು[idx].ದತ್ತಾಂಶ) <= 0)
                        ನಿಲ್ಲಿಸು;

                ಅದಲುಬದಲು(q, idx, k);
                idx = k;
        }

        ಹಿಂತಿರುಗಿ idx;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ shuffle_ಕೆಳಕ್ಕೆ(Prioq *q, ಚಿಹ್ನೆರಹಿತ idx) {
        ಪ್ರತಿಜ್ಞಾ(q);

        ಗಾಗಿ (;;) {
                ಚಿಹ್ನೆರಹಿತ j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ಆದರೆ (j >= q->n_ವಸ್ತುಗಳು)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (q->compare_func(q->ವಸ್ತುಗಳು[j].ದತ್ತಾಂಶ, q->ವಸ್ತುಗಳು[idx].ದತ್ತಾಂಶ) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ಇಲ್ಲದಿದ್ದರೆ
                        s = idx;

                ಆದರೆ (k < q->n_ವಸ್ತುಗಳು &&
                    q->compare_func(q->ವಸ್ತುಗಳು[k].ದತ್ತಾಂಶ, q->ವಸ್ತುಗಳು[s].ದತ್ತಾಂಶ) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ಆದರೆ (s == idx)
                        /* No swap necessary, we're done */
                        ನಿಲ್ಲಿಸು;

                ಅದಲುಬದಲು(q, idx, s);
                idx = s;
        }

        ಹಿಂತಿರುಗಿ idx;
}

ಪೂರ್ಣಾಂಕ prioq_put(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx) {
        ಚಿಹ್ನೆರಹಿತ k;

        ಪ್ರತಿಜ್ಞಾ(q);

        ಆದರೆ (!GREEDY_REALLOC(q->ವಸ್ತುಗಳು, MAX(q->n_ವಸ್ತುಗಳು + 1, 16u)))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        k = q->n_ವಸ್ತುಗಳು++;
        q->ವಸ್ತುಗಳು[k] = (ರಚನೆ prioq_ಅಂಶ) {
                .ದತ್ತಾಂಶ = ದತ್ತಾಂಶ,
                .idx = idx,
        };

        ಆದರೆ (idx)
                *idx = k;

        shuffle_ಮೇಲೆ(q, k);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(q);

        r = prioq_ensure_allocated(q, compare_func);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ prioq_put(*q, ದತ್ತಾಂಶ, idx);
}

ಸ್ಥಿರ ಶೂನ್ಯ remove_ಅಂಶ(Prioq *q, ರಚನೆ prioq_ಅಂಶ *i) {
        ರಚನೆ prioq_ಅಂಶ *l;

        ಪ್ರತಿಜ್ಞಾ(q);
        ಪ್ರತಿಜ್ಞಾ(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ಆದರೆ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->ವಸ್ತುಗಳು + q->n_ವಸ್ತುಗಳು - 1;

        ಆದರೆ (i == l)
                /* Last entry, let's just remove it */
                q->n_ವಸ್ತುಗಳು--;
        ಇಲ್ಲದಿದ್ದರೆ {
                ಚಿಹ್ನೆರಹಿತ k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                ಪ್ರತಿಜ್ಞಾ(i >= q->ವಸ್ತುಗಳು);
                k = i - q->ವಸ್ತುಗಳು;

                i->ದತ್ತಾಂಶ = l->ದತ್ತಾಂಶ;
                i->idx = l->idx;
                ಆದರೆ (i->idx)
                        *i->idx = k;
                q->n_ವಸ್ತುಗಳು--;

                k = shuffle_ಕೆಳಕ್ಕೆ(q, k);
                shuffle_ಮೇಲೆ(q, k);
        }
}

ಸ್ಥಿರ ರಚನೆ prioq_ಅಂಶ* ಕಂಡುಹಿಡಿ_ಅಂಶ(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx) {
        ರಚನೆ prioq_ಅಂಶ *i;

        ಪ್ರತಿಜ್ಞಾ(q);

        ಆದರೆ (q->n_ವಸ್ತುಗಳು <= 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (idx) {
                ಆದರೆ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_ವಸ್ತುಗಳು)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                i = q->ವಸ್ತುಗಳು + *idx;
                ಆದರೆ (i->ದತ್ತಾಂಶ != ದತ್ತಾಂಶ)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಹಿಂತಿರುಗಿ i;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಾಗಿ (i = q->ವಸ್ತುಗಳು; i < q->ವಸ್ತುಗಳು + q->n_ವಸ್ತುಗಳು; i++)
                        ಆದರೆ (i->ದತ್ತಾಂಶ == ದತ್ತಾಂಶ)
                                ಹಿಂತಿರುಗಿ i;
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
}

ಪೂರ್ಣಾಂಕ prioq_remove(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx) {
        ರಚನೆ prioq_ಅಂಶ *i;

        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ 0;

        i = ಕಂಡುಹಿಡಿ_ಅಂಶ(q, ದತ್ತಾಂಶ, idx);
        ಆದರೆ (!i)
                ಹಿಂತಿರುಗಿ 0;

        remove_ಅಂಶ(q, i);
        ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ prioq_reshuffle(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx) {
        ರಚನೆ prioq_ಅಂಶ *i;
        ಚಿಹ್ನೆರಹಿತ k;

        ಪ್ರತಿಜ್ಞಾ(q);

        i = ಕಂಡುಹಿಡಿ_ಅಂಶ(q, ದತ್ತಾಂಶ, idx);
        ಆದರೆ (!i)
                ಹಿಂತಿರುಗಿ;

        ಪ್ರತಿಜ್ಞಾ(i >= q->ವಸ್ತುಗಳು);
        k = i - q->ವಸ್ತುಗಳು;
        k = shuffle_ಕೆಳಕ್ಕೆ(q, k);
        shuffle_ಮೇಲೆ(q, k);
}

ಶೂನ್ಯ* prioq_peek_by_ಸೂಚ್ಯಂಕ(Prioq *q, ಚಿಹ್ನೆರಹಿತ idx) {
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (idx >= q->n_ವಸ್ತುಗಳು)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ q->ವಸ್ತುಗಳು[idx].ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ* prioq_ತೆಗೆ(Prioq *q) {
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;

        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (q->n_ವಸ್ತುಗಳು <= 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ದತ್ತಾಂಶ = q->ವಸ್ತುಗಳು[0].ದತ್ತಾಂಶ;
        remove_ಅಂಶ(q, q->ವಸ್ತುಗಳು);
        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ಚಿಹ್ನೆರಹಿತ prioq_ಗಾತ್ರ(Prioq *q) {

        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ q->n_ವಸ್ತುಗಳು;
}

bool prioq_isempty(Prioq *q) {

        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ q->n_ವಸ್ತುಗಳು <= 0;
}
