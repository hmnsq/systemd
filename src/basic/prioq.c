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
        unsigned n_वस्तूनि;
        संरचना prioq_item *वस्तूनि;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        सचे (!q)
                पटिनिवत्त q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        पटिनिवत्त q;
}

Prioq* prioq_free(Prioq *q) {
        सचे (!q)
                पटिनिवत्त रिक्त;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->वस्तूनि, q->n_वस्तूनि)
                सचे (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        मुक्त(q->वस्तूनि);
        पटिनिवत्त mfree(q);
}

पूर्णाङ्क prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        प्रतिज्ञा(q);

        सचे (*q) {
                प्रतिज्ञा((*q)->compare_func == compare_func);
                पटिनिवत्त 0;
        }

        *q = prioq_new(compare_func);
        सचे (!*q)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त 0;
}

स्थिर शून्य swap(Prioq *q, unsigned j, unsigned k) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(j < q->n_वस्तूनि);
        प्रतिज्ञा(k < q->n_वस्तूनि);

        प्रतिज्ञा(!q->वस्तूनि[j].idx || *(q->वस्तूनि[j].idx) == j);
        प्रतिज्ञा(!q->वस्तूनि[k].idx || *(q->वस्तूनि[k].idx) == k);

        SWAP_TWO(q->वस्तूनि[j].data, q->वस्तूनि[k].data);
        SWAP_TWO(q->वस्तूनि[j].idx, q->वस्तूनि[k].idx);

        सचे (q->वस्तूनि[j].idx)
                *q->वस्तूनि[j].idx = j;

        सचे (q->वस्तूनि[k].idx)
                *q->वस्तूनि[k].idx = k;
}

स्थिर unsigned shuffle_up(Prioq *q, unsigned idx) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(idx < q->n_वस्तूनि);

        याव (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                सचे (q->compare_func(q->वस्तूनि[k].data, q->वस्तूनि[idx].data) <= 0)
                        भङ्ग;

                swap(q, idx, k);
                idx = k;
        }

        पटिनिवत्त idx;
}

स्थिर unsigned shuffle_down(Prioq *q, unsigned idx) {
        प्रतिज्ञा(q);

        अत्थाय (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                सचे (j >= q->n_वस्तूनि)
                        भङ्ग;

                सचे (q->compare_func(q->वस्तूनि[j].data, q->वस्तूनि[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                अञ्ञथा
                        s = idx;

                सचे (k < q->n_वस्तूनि &&
                    q->compare_func(q->वस्तूनि[k].data, q->वस्तूनि[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                सचे (s == idx)
                        /* No swap necessary, we're done */
                        भङ्ग;

                swap(q, idx, s);
                idx = s;
        }

        पटिनिवत्त idx;
}

पूर्णाङ्क prioq_put(Prioq *q, शून्य *data, unsigned *idx) {
        unsigned k;

        प्रतिज्ञा(q);

        सचे (!GREEDY_REALLOC(q->वस्तूनि, MAX(q->n_वस्तूनि + 1, 16u)))
                पटिनिवत्त -ENOMEM;

        k = q->n_वस्तूनि++;
        q->वस्तूनि[k] = (संरचना prioq_item) {
                .data = data,
                .idx = idx,
        };

        सचे (idx)
                *idx = k;

        shuffle_up(q, k);

        पटिनिवत्त 0;
}

पूर्णाङ्क _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *data, unsigned *idx) {
        पूर्णाङ्क r;

        प्रतिज्ञा(q);

        r = prioq_ensure_allocated(q, compare_func);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त prioq_put(*q, data, idx);
}

स्थिर शून्य remove_item(Prioq *q, संरचना prioq_item *i) {
        संरचना prioq_item *l;

        प्रतिज्ञा(q);
        प्रतिज्ञा(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        सचे (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->वस्तूनि + q->n_वस्तूनि - 1;

        सचे (i == l)
                /* Last entry, let's just remove it */
                q->n_वस्तूनि--;
        अञ्ञथा {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                प्रतिज्ञा(i >= q->वस्तूनि);
                k = i - q->वस्तूनि;

                i->data = l->data;
                i->idx = l->idx;
                सचे (i->idx)
                        *i->idx = k;
                q->n_वस्तूनि--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

स्थिर संरचना prioq_item* find_item(Prioq *q, शून्य *data, unsigned *idx) {
        संरचना prioq_item *i;

        प्रतिज्ञा(q);

        सचे (q->n_वस्तूनि <= 0)
                पटिनिवत्त रिक्त;

        सचे (idx) {
                सचे (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_वस्तूनि)
                        पटिनिवत्त रिक्त;

                i = q->वस्तूनि + *idx;
                सचे (i->data != data)
                        पटिनिवत्त रिक्त;

                पटिनिवत्त i;
        } अञ्ञथा {
                अत्थाय (i = q->वस्तूनि; i < q->वस्तूनि + q->n_वस्तूनि; i++)
                        सचे (i->data == data)
                                पटिनिवत्त i;
                पटिनिवत्त रिक्त;
        }
}

पूर्णाङ्क prioq_remove(Prioq *q, शून्य *data, unsigned *idx) {
        संरचना prioq_item *i;

        सचे (!q)
                पटिनिवत्त 0;

        i = find_item(q, data, idx);
        सचे (!i)
                पटिनिवत्त 0;

        remove_item(q, i);
        पटिनिवत्त 1;
}

शून्य prioq_reshuffle(Prioq *q, शून्य *data, unsigned *idx) {
        संरचना prioq_item *i;
        unsigned k;

        प्रतिज्ञा(q);

        i = find_item(q, data, idx);
        सचे (!i)
                पटिनिवत्त;

        प्रतिज्ञा(i >= q->वस्तूनि);
        k = i - q->वस्तूनि;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

शून्य* prioq_peek_by_index(Prioq *q, unsigned idx) {
        सचे (!q)
                पटिनिवत्त रिक्त;

        सचे (idx >= q->n_वस्तूनि)
                पटिनिवत्त रिक्त;

        पटिनिवत्त q->वस्तूनि[idx].data;
}

शून्य* prioq_निष्कासय(Prioq *q) {
        शून्य *data;

        सचे (!q)
                पटिनिवत्त रिक्त;

        सचे (q->n_वस्तूनि <= 0)
                पटिनिवत्त रिक्त;

        data = q->वस्तूनि[0].data;
        remove_item(q, q->वस्तूनि);
        पटिनिवत्त data;
}

unsigned prioq_size(Prioq *q) {

        सचे (!q)
                पटिनिवत्त 0;

        पटिनिवत्त q->n_वस्तूनि;
}

bool prioq_isempty(Prioq *q) {

        सचे (!q)
                पटिनिवत्त true;

        पटिनिवत्त q->n_वस्तूनि <= 0;
}
