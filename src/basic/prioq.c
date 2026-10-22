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

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "prioq.h"

संरचना prioq_item {
        शून्य *data;
        unsigned *idx;
};

संरचना Prioq {
        compare_func_t compare_func;
        unsigned n_वस्तुएँ;
        संरचना prioq_item *वस्तुएँ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        यदि (!q)
                प्रत्यागम q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        प्रत्यागम q;
}

Prioq* prioq_free(Prioq *q) {
        यदि (!q)
                प्रत्यागम रिक्त;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->वस्तुएँ, q->n_वस्तुएँ)
                यदि (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        मुक्त(q->वस्तुएँ);
        प्रत्यागम mfree(q);
}

पूर्णाङ्क prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        प्रतिज्ञा(q);

        यदि (*q) {
                प्रतिज्ञा((*q)->compare_func == compare_func);
                प्रत्यागम 0;
        }

        *q = prioq_new(compare_func);
        यदि (!*q)
                प्रत्यागम -ENOMEM;

        प्रत्यागम 0;
}

स्थिर शून्य swap(Prioq *q, unsigned j, unsigned k) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(j < q->n_वस्तुएँ);
        प्रतिज्ञा(k < q->n_वस्तुएँ);

        प्रतिज्ञा(!q->वस्तुएँ[j].idx || *(q->वस्तुएँ[j].idx) == j);
        प्रतिज्ञा(!q->वस्तुएँ[k].idx || *(q->वस्तुएँ[k].idx) == k);

        SWAP_TWO(q->वस्तुएँ[j].data, q->वस्तुएँ[k].data);
        SWAP_TWO(q->वस्तुएँ[j].idx, q->वस्तुएँ[k].idx);

        यदि (q->वस्तुएँ[j].idx)
                *q->वस्तुएँ[j].idx = j;

        यदि (q->वस्तुएँ[k].idx)
                *q->वस्तुएँ[k].idx = k;
}

स्थिर unsigned shuffle_up(Prioq *q, unsigned idx) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(idx < q->n_वस्तुएँ);

        यावत् (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                यदि (q->compare_func(q->वस्तुएँ[k].data, q->वस्तुएँ[idx].data) <= 0)
                        भङ्ग;

                swap(q, idx, k);
                idx = k;
        }

        प्रत्यागम idx;
}

स्थिर unsigned shuffle_down(Prioq *q, unsigned idx) {
        प्रतिज्ञा(q);

        प्रति (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                यदि (j >= q->n_वस्तुएँ)
                        भङ्ग;

                यदि (q->compare_func(q->वस्तुएँ[j].data, q->वस्तुएँ[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                अन्यथा
                        s = idx;

                यदि (k < q->n_वस्तुएँ &&
                    q->compare_func(q->वस्तुएँ[k].data, q->वस्तुएँ[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                यदि (s == idx)
                        /* No swap necessary, we're done */
                        भङ्ग;

                swap(q, idx, s);
                idx = s;
        }

        प्रत्यागम idx;
}

पूर्णाङ्क prioq_put(Prioq *q, शून्य *data, unsigned *idx) {
        unsigned k;

        प्रतिज्ञा(q);

        यदि (!GREEDY_REALLOC(q->वस्तुएँ, MAX(q->n_वस्तुएँ + 1, 16u)))
                प्रत्यागम -ENOMEM;

        k = q->n_वस्तुएँ++;
        q->वस्तुएँ[k] = (संरचना prioq_item) {
                .data = data,
                .idx = idx,
        };

        यदि (idx)
                *idx = k;

        shuffle_up(q, k);

        प्रत्यागम 0;
}

पूर्णाङ्क _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *data, unsigned *idx) {
        पूर्णाङ्क r;

        प्रतिज्ञा(q);

        r = prioq_ensure_allocated(q, compare_func);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम prioq_put(*q, data, idx);
}

स्थिर शून्य remove_item(Prioq *q, संरचना prioq_item *i) {
        संरचना prioq_item *l;

        प्रतिज्ञा(q);
        प्रतिज्ञा(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        यदि (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->वस्तुएँ + q->n_वस्तुएँ - 1;

        यदि (i == l)
                /* Last entry, let's just remove it */
                q->n_वस्तुएँ--;
        अन्यथा {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                प्रतिज्ञा(i >= q->वस्तुएँ);
                k = i - q->वस्तुएँ;

                i->data = l->data;
                i->idx = l->idx;
                यदि (i->idx)
                        *i->idx = k;
                q->n_वस्तुएँ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

स्थिर संरचना prioq_item* find_item(Prioq *q, शून्य *data, unsigned *idx) {
        संरचना prioq_item *i;

        प्रतिज्ञा(q);

        यदि (q->n_वस्तुएँ <= 0)
                प्रत्यागम रिक्त;

        यदि (idx) {
                यदि (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_वस्तुएँ)
                        प्रत्यागम रिक्त;

                i = q->वस्तुएँ + *idx;
                यदि (i->data != data)
                        प्रत्यागम रिक्त;

                प्रत्यागम i;
        } अन्यथा {
                प्रति (i = q->वस्तुएँ; i < q->वस्तुएँ + q->n_वस्तुएँ; i++)
                        यदि (i->data == data)
                                प्रत्यागम i;
                प्रत्यागम रिक्त;
        }
}

पूर्णाङ्क prioq_remove(Prioq *q, शून्य *data, unsigned *idx) {
        संरचना prioq_item *i;

        यदि (!q)
                प्रत्यागम 0;

        i = find_item(q, data, idx);
        यदि (!i)
                प्रत्यागम 0;

        remove_item(q, i);
        प्रत्यागम 1;
}

शून्य prioq_reshuffle(Prioq *q, शून्य *data, unsigned *idx) {
        संरचना prioq_item *i;
        unsigned k;

        प्रतिज्ञा(q);

        i = find_item(q, data, idx);
        यदि (!i)
                प्रत्यागम;

        प्रतिज्ञा(i >= q->वस्तुएँ);
        k = i - q->वस्तुएँ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

शून्य* prioq_peek_by_index(Prioq *q, unsigned idx) {
        यदि (!q)
                प्रत्यागम रिक्त;

        यदि (idx >= q->n_वस्तुएँ)
                प्रत्यागम रिक्त;

        प्रत्यागम q->वस्तुएँ[idx].data;
}

शून्य* prioq_निकालो(Prioq *q) {
        शून्य *data;

        यदि (!q)
                प्रत्यागम रिक्त;

        यदि (q->n_वस्तुएँ <= 0)
                प्रत्यागम रिक्त;

        data = q->वस्तुएँ[0].data;
        remove_item(q, q->वस्तुएँ);
        प्रत्यागम data;
}

unsigned prioq_size(Prioq *q) {

        यदि (!q)
                प्रत्यागम 0;

        प्रत्यागम q->n_वस्तुएँ;
}

bool prioq_isempty(Prioq *q) {

        यदि (!q)
                प्रत्यागम true;

        प्रत्यागम q->n_वस्तुएँ <= 0;
}
