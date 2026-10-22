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

#समावेश "alloc-util.h"
#समावेश "prioq.h"

काष्ठा prioq_item {
        व्योम *तथ्य;
        अचिन्हित *idx;
};

काष्ठा Prioq {
        compare_func_t compare_func;
        अचिन्हित n_वस्तुएँ;
        काष्ठा prioq_item *वस्तुएँ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        अगर (!q)
                वापस q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        वापस q;
}

Prioq* prioq_free(Prioq *q) {
        अगर (!q)
                वापस शून्य;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->वस्तुएँ, q->n_वस्तुएँ)
                अगर (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        मुक्त(q->वस्तुएँ);
        वापस mfree(q);
}

पूर्णांक prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        निश्चित(q);

        अगर (*q) {
                निश्चित((*q)->compare_func == compare_func);
                वापस 0;
        }

        *q = prioq_new(compare_func);
        अगर (!*q)
                वापस -ENOMEM;

        वापस 0;
}

अटल व्योम swap(Prioq *q, अचिन्हित j, अचिन्हित k) {
        निश्चित(q);
        निश्चित(j < q->n_वस्तुएँ);
        निश्चित(k < q->n_वस्तुएँ);

        निश्चित(!q->वस्तुएँ[j].idx || *(q->वस्तुएँ[j].idx) == j);
        निश्चित(!q->वस्तुएँ[k].idx || *(q->वस्तुएँ[k].idx) == k);

        SWAP_TWO(q->वस्तुएँ[j].तथ्य, q->वस्तुएँ[k].तथ्य);
        SWAP_TWO(q->वस्तुएँ[j].idx, q->वस्तुएँ[k].idx);

        अगर (q->वस्तुएँ[j].idx)
                *q->वस्तुएँ[j].idx = j;

        अगर (q->वस्तुएँ[k].idx)
                *q->वस्तुएँ[k].idx = k;
}

अटल अचिन्हित shuffle_up(Prioq *q, अचिन्हित idx) {
        निश्चित(q);
        निश्चित(idx < q->n_वस्तुएँ);

        जबतक (idx > 0) {
                अचिन्हित k;

                k = (idx-1)/2;

                अगर (q->compare_func(q->वस्तुएँ[k].तथ्य, q->वस्तुएँ[idx].तथ्य) <= 0)
                        अवरोध;

                swap(q, idx, k);
                idx = k;
        }

        वापस idx;
}

अटल अचिन्हित shuffle_down(Prioq *q, अचिन्हित idx) {
        निश्चित(q);

        क्रम (;;) {
                अचिन्हित j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                अगर (j >= q->n_वस्तुएँ)
                        अवरोध;

                अगर (q->compare_func(q->वस्तुएँ[j].तथ्य, q->वस्तुएँ[idx].तथ्य) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                अन्यथा
                        s = idx;

                अगर (k < q->n_वस्तुएँ &&
                    q->compare_func(q->वस्तुएँ[k].तथ्य, q->वस्तुएँ[s].तथ्य) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                अगर (s == idx)
                        /* No swap necessary, we're done */
                        अवरोध;

                swap(q, idx, s);
                idx = s;
        }

        वापस idx;
}

पूर्णांक prioq_put(Prioq *q, व्योम *तथ्य, अचिन्हित *idx) {
        अचिन्हित k;

        निश्चित(q);

        अगर (!GREEDY_REALLOC(q->वस्तुएँ, MAX(q->n_वस्तुएँ + 1, 16u)))
                वापस -ENOMEM;

        k = q->n_वस्तुएँ++;
        q->वस्तुएँ[k] = (काष्ठा prioq_item) {
                .तथ्य = तथ्य,
                .idx = idx,
        };

        अगर (idx)
                *idx = k;

        shuffle_up(q, k);

        वापस 0;
}

पूर्णांक _prioq_ensure_put(Prioq **q, compare_func_t compare_func, व्योम *तथ्य, अचिन्हित *idx) {
        पूर्णांक r;

        निश्चित(q);

        r = prioq_ensure_allocated(q, compare_func);
        अगर (r < 0)
                वापस r;

        वापस prioq_put(*q, तथ्य, idx);
}

अटल व्योम remove_item(Prioq *q, काष्ठा prioq_item *i) {
        काष्ठा prioq_item *l;

        निश्चित(q);
        निश्चित(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        अगर (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->वस्तुएँ + q->n_वस्तुएँ - 1;

        अगर (i == l)
                /* Last entry, let's just remove it */
                q->n_वस्तुएँ--;
        अन्यथा {
                अचिन्हित k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                निश्चित(i >= q->वस्तुएँ);
                k = i - q->वस्तुएँ;

                i->तथ्य = l->तथ्य;
                i->idx = l->idx;
                अगर (i->idx)
                        *i->idx = k;
                q->n_वस्तुएँ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

अटल काष्ठा prioq_item* find_item(Prioq *q, व्योम *तथ्य, अचिन्हित *idx) {
        काष्ठा prioq_item *i;

        निश्चित(q);

        अगर (q->n_वस्तुएँ <= 0)
                वापस शून्य;

        अगर (idx) {
                अगर (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_वस्तुएँ)
                        वापस शून्य;

                i = q->वस्तुएँ + *idx;
                अगर (i->तथ्य != तथ्य)
                        वापस शून्य;

                वापस i;
        } अन्यथा {
                क्रम (i = q->वस्तुएँ; i < q->वस्तुएँ + q->n_वस्तुएँ; i++)
                        अगर (i->तथ्य == तथ्य)
                                वापस i;
                वापस शून्य;
        }
}

पूर्णांक prioq_remove(Prioq *q, व्योम *तथ्य, अचिन्हित *idx) {
        काष्ठा prioq_item *i;

        अगर (!q)
                वापस 0;

        i = find_item(q, तथ्य, idx);
        अगर (!i)
                वापस 0;

        remove_item(q, i);
        वापस 1;
}

व्योम prioq_reshuffle(Prioq *q, व्योम *तथ्य, अचिन्हित *idx) {
        काष्ठा prioq_item *i;
        अचिन्हित k;

        निश्चित(q);

        i = find_item(q, तथ्य, idx);
        अगर (!i)
                वापस;

        निश्चित(i >= q->वस्तुएँ);
        k = i - q->वस्तुएँ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

व्योम* prioq_peek_by_सूचकांक(Prioq *q, अचिन्हित idx) {
        अगर (!q)
                वापस शून्य;

        अगर (idx >= q->n_वस्तुएँ)
                वापस शून्य;

        वापस q->वस्तुएँ[idx].तथ्य;
}

व्योम* prioq_निकालो(Prioq *q) {
        व्योम *तथ्य;

        अगर (!q)
                वापस शून्य;

        अगर (q->n_वस्तुएँ <= 0)
                वापस शून्य;

        तथ्य = q->वस्तुएँ[0].तथ्य;
        remove_item(q, q->वस्तुएँ);
        वापस तथ्य;
}

अचिन्हित prioq_आकार(Prioq *q) {

        अगर (!q)
                वापस 0;

        वापस q->n_वस्तुएँ;
}

bool prioq_isempty(Prioq *q) {

        अगर (!q)
                वापस true;

        वापस q->n_वस्तुएँ <= 0;
}
