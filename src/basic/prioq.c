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

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "prioq.h"

ਬਣਤਰ prioq_ਵਸਤੂ {
        ਖਾਲੀ *ਅੰਕੜੇ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ *idx;
};

ਬਣਤਰ Prioq {
        compare_func_t compare_func;
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_ਚੀਜ਼ਾਂ;
        ਬਣਤਰ prioq_ਵਸਤੂ *ਚੀਜ਼ਾਂ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        ਜੇ (!q)
                ਵਾਪਸ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ਵਾਪਸ q;
}

Prioq* prioq_free(Prioq *q) {
        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(ਵਸਤੂ_2, q->ਚੀਜ਼ਾਂ, q->n_ਚੀਜ਼ਾਂ)
                ਜੇ (ਵਸਤੂ_2->idx)
                        *(ਵਸਤੂ_2->idx) = PRIOQ_IDX_NULL;

        ਮੁਕਤ(q->ਚੀਜ਼ਾਂ);
        ਵਾਪਸ mfree(q);
}

ਪੂਰਨ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਜੇ (*q) {
                ਪ੍ਰਤਿਜ੍ਞਾ((*q)->compare_func == compare_func);
                ਵਾਪਸ 0;
        }

        *q = prioq_new(compare_func);
        ਜੇ (!*q)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(Prioq *q, ਬਿਨਾਨਿਸ਼ਾਨੀ j, ਬਿਨਾਨਿਸ਼ਾਨੀ k) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);
        ਪ੍ਰਤਿਜ੍ਞਾ(j < q->n_ਚੀਜ਼ਾਂ);
        ਪ੍ਰਤਿਜ੍ਞਾ(k < q->n_ਚੀਜ਼ਾਂ);

        ਪ੍ਰਤਿਜ੍ਞਾ(!q->ਚੀਜ਼ਾਂ[j].idx || *(q->ਚੀਜ਼ਾਂ[j].idx) == j);
        ਪ੍ਰਤਿਜ੍ਞਾ(!q->ਚੀਜ਼ਾਂ[k].idx || *(q->ਚੀਜ਼ਾਂ[k].idx) == k);

        ਅਦਲਾ_ਬਦਲੀ_TWO(q->ਚੀਜ਼ਾਂ[j].ਅੰਕੜੇ, q->ਚੀਜ਼ਾਂ[k].ਅੰਕੜੇ);
        ਅਦਲਾ_ਬਦਲੀ_TWO(q->ਚੀਜ਼ਾਂ[j].idx, q->ਚੀਜ਼ਾਂ[k].idx);

        ਜੇ (q->ਚੀਜ਼ਾਂ[j].idx)
                *q->ਚੀਜ਼ਾਂ[j].idx = j;

        ਜੇ (q->ਚੀਜ਼ਾਂ[k].idx)
                *q->ਚੀਜ਼ਾਂ[k].idx = k;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ shuffle_ਉੱਪਰ(Prioq *q, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);
        ਪ੍ਰਤਿਜ੍ਞਾ(idx < q->n_ਚੀਜ਼ਾਂ);

        ਜਦੋਂ (idx > 0) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ k;

                k = (idx-1)/2;

                ਜੇ (q->compare_func(q->ਚੀਜ਼ਾਂ[k].ਅੰਕੜੇ, q->ਚੀਜ਼ਾਂ[idx].ਅੰਕੜੇ) <= 0)
                        ਰੋਕੋ;

                ਅਦਲਾ_ਬਦਲੀ(q, idx, k);
                idx = k;
        }

        ਵਾਪਸ idx;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ shuffle_ਥੱਲੇ(Prioq *q, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਲਈ (;;) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ਜੇ (j >= q->n_ਚੀਜ਼ਾਂ)
                        ਰੋਕੋ;

                ਜੇ (q->compare_func(q->ਚੀਜ਼ਾਂ[j].ਅੰਕੜੇ, q->ਚੀਜ਼ਾਂ[idx].ਅੰਕੜੇ) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ਨਹੀਂਤਾਂ
                        s = idx;

                ਜੇ (k < q->n_ਚੀਜ਼ਾਂ &&
                    q->compare_func(q->ਚੀਜ਼ਾਂ[k].ਅੰਕੜੇ, q->ਚੀਜ਼ਾਂ[s].ਅੰਕੜੇ) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ਜੇ (s == idx)
                        /* No swap necessary, we're done */
                        ਰੋਕੋ;

                ਅਦਲਾ_ਬਦਲੀ(q, idx, s);
                idx = s;
        }

        ਵਾਪਸ idx;
}

ਪੂਰਨ prioq_put(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ k;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਜੇ (!GREEDY_REALLOC(q->ਚੀਜ਼ਾਂ, MAX(q->n_ਚੀਜ਼ਾਂ + 1, 16u)))
                ਵਾਪਸ -ENOMEM;

        k = q->n_ਚੀਜ਼ਾਂ++;
        q->ਚੀਜ਼ਾਂ[k] = (ਬਣਤਰ prioq_ਵਸਤੂ) {
                .ਅੰਕੜੇ = ਅੰਕੜੇ,
                .idx = idx,
        };

        ਜੇ (idx)
                *idx = k;

        shuffle_ਉੱਪਰ(q, k);

        ਵਾਪਸ 0;
}

ਪੂਰਨ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        r = prioq_ensure_allocated(q, compare_func);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ prioq_put(*q, ਅੰਕੜੇ, idx);
}

ਸਥਿਰ ਖਾਲੀ remove_ਵਸਤੂ(Prioq *q, ਬਣਤਰ prioq_ਵਸਤੂ *i) {
        ਬਣਤਰ prioq_ਵਸਤੂ *l;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);
        ਪ੍ਰਤਿਜ੍ਞਾ(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ਜੇ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->ਚੀਜ਼ਾਂ + q->n_ਚੀਜ਼ਾਂ - 1;

        ਜੇ (i == l)
                /* Last entry, let's just remove it */
                q->n_ਚੀਜ਼ਾਂ--;
        ਨਹੀਂਤਾਂ {
                ਬਿਨਾਨਿਸ਼ਾਨੀ k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                ਪ੍ਰਤਿਜ੍ਞਾ(i >= q->ਚੀਜ਼ਾਂ);
                k = i - q->ਚੀਜ਼ਾਂ;

                i->ਅੰਕੜੇ = l->ਅੰਕੜੇ;
                i->idx = l->idx;
                ਜੇ (i->idx)
                        *i->idx = k;
                q->n_ਚੀਜ਼ਾਂ--;

                k = shuffle_ਥੱਲੇ(q, k);
                shuffle_ਉੱਪਰ(q, k);
        }
}

ਸਥਿਰ ਬਣਤਰ prioq_ਵਸਤੂ* ਲੱਭੋ_ਵਸਤੂ(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx) {
        ਬਣਤਰ prioq_ਵਸਤੂ *i;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        ਜੇ (q->n_ਚੀਜ਼ਾਂ <= 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (idx) {
                ਜੇ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_ਚੀਜ਼ਾਂ)
                        ਵਾਪਸ ਨਿਰਾ;

                i = q->ਚੀਜ਼ਾਂ + *idx;
                ਜੇ (i->ਅੰਕੜੇ != ਅੰਕੜੇ)
                        ਵਾਪਸ ਨਿਰਾ;

                ਵਾਪਸ i;
        } ਨਹੀਂਤਾਂ {
                ਲਈ (i = q->ਚੀਜ਼ਾਂ; i < q->ਚੀਜ਼ਾਂ + q->n_ਚੀਜ਼ਾਂ; i++)
                        ਜੇ (i->ਅੰਕੜੇ == ਅੰਕੜੇ)
                                ਵਾਪਸ i;
                ਵਾਪਸ ਨਿਰਾ;
        }
}

ਪੂਰਨ prioq_remove(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx) {
        ਬਣਤਰ prioq_ਵਸਤੂ *i;

        ਜੇ (!q)
                ਵਾਪਸ 0;

        i = ਲੱਭੋ_ਵਸਤੂ(q, ਅੰਕੜੇ, idx);
        ਜੇ (!i)
                ਵਾਪਸ 0;

        remove_ਵਸਤੂ(q, i);
        ਵਾਪਸ 1;
}

ਖਾਲੀ prioq_reshuffle(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx) {
        ਬਣਤਰ prioq_ਵਸਤੂ *i;
        ਬਿਨਾਨਿਸ਼ਾਨੀ k;

        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        i = ਲੱਭੋ_ਵਸਤੂ(q, ਅੰਕੜੇ, idx);
        ਜੇ (!i)
                ਵਾਪਸ;

        ਪ੍ਰਤਿਜ੍ਞਾ(i >= q->ਚੀਜ਼ਾਂ);
        k = i - q->ਚੀਜ਼ਾਂ;
        k = shuffle_ਥੱਲੇ(q, k);
        shuffle_ਉੱਪਰ(q, k);
}

ਖਾਲੀ* prioq_peek_by_ਸੂਚਕਾਂਕ(Prioq *q, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (idx >= q->n_ਚੀਜ਼ਾਂ)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ q->ਚੀਜ਼ਾਂ[idx].ਅੰਕੜੇ;
}

ਖਾਲੀ* prioq_ਕੱਢੋ(Prioq *q) {
        ਖਾਲੀ *ਅੰਕੜੇ;

        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (q->n_ਚੀਜ਼ਾਂ <= 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਅੰਕੜੇ = q->ਚੀਜ਼ਾਂ[0].ਅੰਕੜੇ;
        remove_ਵਸਤੂ(q, q->ਚੀਜ਼ਾਂ);
        ਵਾਪਸ ਅੰਕੜੇ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ prioq_ਮਾਪ(Prioq *q) {

        ਜੇ (!q)
                ਵਾਪਸ 0;

        ਵਾਪਸ q->n_ਚੀਜ਼ਾਂ;
}

bool prioq_isempty(Prioq *q) {

        ਜੇ (!q)
                ਵਾਪਸ true;

        ਵਾਪਸ q->n_ਚੀਜ਼ਾਂ <= 0;
}
