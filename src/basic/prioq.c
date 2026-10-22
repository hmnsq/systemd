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
        अचिन्हित *idx;
};

संरचना Prioq {
        compare_func_t compare_func;
        अचिन्हित n_वस्तुहरू;
        संरचना prioq_item *वस्तुहरू;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        यदि (!q)
                फर्काऊ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        फर्काऊ q;
}

Prioq* prioq_free(Prioq *q) {
        यदि (!q)
                फर्काऊ रिक्त;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->वस्तुहरू, q->n_वस्तुहरू)
                यदि (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        मुक्त(q->वस्तुहरू);
        फर्काऊ mfree(q);
}

पूर्णाङ्क prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        प्रतिज्ञा(q);

        यदि (*q) {
                प्रतिज्ञा((*q)->compare_func == compare_func);
                फर्काऊ 0;
        }

        *q = prioq_new(compare_func);
        यदि (!*q)
                फर्काऊ -ENOMEM;

        फर्काऊ 0;
}

स्थिर शून्य swap(Prioq *q, अचिन्हित j, अचिन्हित k) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(j < q->n_वस्तुहरू);
        प्रतिज्ञा(k < q->n_वस्तुहरू);

        प्रतिज्ञा(!q->वस्तुहरू[j].idx || *(q->वस्तुहरू[j].idx) == j);
        प्रतिज्ञा(!q->वस्तुहरू[k].idx || *(q->वस्तुहरू[k].idx) == k);

        SWAP_TWO(q->वस्तुहरू[j].data, q->वस्तुहरू[k].data);
        SWAP_TWO(q->वस्तुहरू[j].idx, q->वस्तुहरू[k].idx);

        यदि (q->वस्तुहरू[j].idx)
                *q->वस्तुहरू[j].idx = j;

        यदि (q->वस्तुहरू[k].idx)
                *q->वस्तुहरू[k].idx = k;
}

स्थिर अचिन्हित shuffle_up(Prioq *q, अचिन्हित idx) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(idx < q->n_वस्तुहरू);

        जबसम्म (idx > 0) {
                अचिन्हित k;

                k = (idx-1)/2;

                यदि (q->compare_func(q->वस्तुहरू[k].data, q->वस्तुहरू[idx].data) <= 0)
                        रोक;

                swap(q, idx, k);
                idx = k;
        }

        फर्काऊ idx;
}

स्थिर अचिन्हित shuffle_down(Prioq *q, अचिन्हित idx) {
        प्रतिज्ञा(q);

        लागि (;;) {
                अचिन्हित j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                यदि (j >= q->n_वस्तुहरू)
                        रोक;

                यदि (q->compare_func(q->वस्तुहरू[j].data, q->वस्तुहरू[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                अन्यथा
                        s = idx;

                यदि (k < q->n_वस्तुहरू &&
                    q->compare_func(q->वस्तुहरू[k].data, q->वस्तुहरू[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                यदि (s == idx)
                        /* No swap necessary, we're done */
                        रोक;

                swap(q, idx, s);
                idx = s;
        }

        फर्काऊ idx;
}

पूर्णाङ्क prioq_put(Prioq *q, शून्य *data, अचिन्हित *idx) {
        अचिन्हित k;

        प्रतिज्ञा(q);

        यदि (!GREEDY_REALLOC(q->वस्तुहरू, MAX(q->n_वस्तुहरू + 1, 16u)))
                फर्काऊ -ENOMEM;

        k = q->n_वस्तुहरू++;
        q->वस्तुहरू[k] = (संरचना prioq_item) {
                .data = data,
                .idx = idx,
        };

        यदि (idx)
                *idx = k;

        shuffle_up(q, k);

        फर्काऊ 0;
}

पूर्णाङ्क _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *data, अचिन्हित *idx) {
        पूर्णाङ्क r;

        प्रतिज्ञा(q);

        r = prioq_ensure_allocated(q, compare_func);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ prioq_put(*q, data, idx);
}

स्थिर शून्य remove_item(Prioq *q, संरचना prioq_item *i) {
        संरचना prioq_item *l;

        प्रतिज्ञा(q);
        प्रतिज्ञा(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        यदि (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->वस्तुहरू + q->n_वस्तुहरू - 1;

        यदि (i == l)
                /* Last entry, let's just remove it */
                q->n_वस्तुहरू--;
        अन्यथा {
                अचिन्हित k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                प्रतिज्ञा(i >= q->वस्तुहरू);
                k = i - q->वस्तुहरू;

                i->data = l->data;
                i->idx = l->idx;
                यदि (i->idx)
                        *i->idx = k;
                q->n_वस्तुहरू--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

स्थिर संरचना prioq_item* find_item(Prioq *q, शून्य *data, अचिन्हित *idx) {
        संरचना prioq_item *i;

        प्रतिज्ञा(q);

        यदि (q->n_वस्तुहरू <= 0)
                फर्काऊ रिक्त;

        यदि (idx) {
                यदि (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_वस्तुहरू)
                        फर्काऊ रिक्त;

                i = q->वस्तुहरू + *idx;
                यदि (i->data != data)
                        फर्काऊ रिक्त;

                फर्काऊ i;
        } अन्यथा {
                लागि (i = q->वस्तुहरू; i < q->वस्तुहरू + q->n_वस्तुहरू; i++)
                        यदि (i->data == data)
                                फर्काऊ i;
                फर्काऊ रिक्त;
        }
}

पूर्णाङ्क prioq_remove(Prioq *q, शून्य *data, अचिन्हित *idx) {
        संरचना prioq_item *i;

        यदि (!q)
                फर्काऊ 0;

        i = find_item(q, data, idx);
        यदि (!i)
                फर्काऊ 0;

        remove_item(q, i);
        फर्काऊ 1;
}

शून्य prioq_reshuffle(Prioq *q, शून्य *data, अचिन्हित *idx) {
        संरचना prioq_item *i;
        अचिन्हित k;

        प्रतिज्ञा(q);

        i = find_item(q, data, idx);
        यदि (!i)
                फर्काऊ;

        प्रतिज्ञा(i >= q->वस्तुहरू);
        k = i - q->वस्तुहरू;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

शून्य* prioq_peek_by_index(Prioq *q, अचिन्हित idx) {
        यदि (!q)
                फर्काऊ रिक्त;

        यदि (idx >= q->n_वस्तुहरू)
                फर्काऊ रिक्त;

        फर्काऊ q->वस्तुहरू[idx].data;
}

शून्य* prioq_झिक(Prioq *q) {
        शून्य *data;

        यदि (!q)
                फर्काऊ रिक्त;

        यदि (q->n_वस्तुहरू <= 0)
                फर्काऊ रिक्त;

        data = q->वस्तुहरू[0].data;
        remove_item(q, q->वस्तुहरू);
        फर्काऊ data;
}

अचिन्हित prioq_size(Prioq *q) {

        यदि (!q)
                फर्काऊ 0;

        फर्काऊ q->n_वस्तुहरू;
}

bool prioq_isempty(Prioq *q) {

        यदि (!q)
                फर्काऊ true;

        फर्काऊ q->n_वस्तुहरू <= 0;
}
