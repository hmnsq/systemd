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
        शून्य *दत्त;
        अचिह्नित *idx;
};

संरचना Prioq {
        compare_func_t compare_func;
        अचिह्नित n_वस्तूनि;
        संरचना prioq_item *वस्तूनि;
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
        FOREACH_ARRAY(item, q->वस्तूनि, q->n_वस्तूनि)
                यदि (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        मुक्त(q->वस्तूनि);
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

स्थिर शून्य swap(Prioq *q, अचिह्नित j, अचिह्नित k) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(j < q->n_वस्तूनि);
        प्रतिज्ञा(k < q->n_वस्तूनि);

        प्रतिज्ञा(!q->वस्तूनि[j].idx || *(q->वस्तूनि[j].idx) == j);
        प्रतिज्ञा(!q->वस्तूनि[k].idx || *(q->वस्तूनि[k].idx) == k);

        SWAP_TWO(q->वस्तूनि[j].दत्त, q->वस्तूनि[k].दत्त);
        SWAP_TWO(q->वस्तूनि[j].idx, q->वस्तूनि[k].idx);

        यदि (q->वस्तूनि[j].idx)
                *q->वस्तूनि[j].idx = j;

        यदि (q->वस्तूनि[k].idx)
                *q->वस्तूनि[k].idx = k;
}

स्थिर अचिह्नित shuffle_ऊर्ध्वम्(Prioq *q, अचिह्नित idx) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(idx < q->n_वस्तूनि);

        यावत् (idx > 0) {
                अचिह्नित k;

                k = (idx-1)/2;

                यदि (q->compare_func(q->वस्तूनि[k].दत्त, q->वस्तूनि[idx].दत्त) <= 0)
                        भङ्ग;

                swap(q, idx, k);
                idx = k;
        }

        प्रत्यागम idx;
}

स्थिर अचिह्नित shuffle_अधस्तात्(Prioq *q, अचिह्नित idx) {
        प्रतिज्ञा(q);

        प्रति (;;) {
                अचिह्नित j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                यदि (j >= q->n_वस्तूनि)
                        भङ्ग;

                यदि (q->compare_func(q->वस्तूनि[j].दत्त, q->वस्तूनि[idx].दत्त) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                अन्यथा
                        s = idx;

                यदि (k < q->n_वस्तूनि &&
                    q->compare_func(q->वस्तूनि[k].दत्त, q->वस्तूनि[s].दत्त) < 0)

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

पूर्णाङ्क prioq_put(Prioq *q, शून्य *दत्त, अचिह्नित *idx) {
        अचिह्नित k;

        प्रतिज्ञा(q);

        यदि (!GREEDY_REALLOC(q->वस्तूनि, MAX(q->n_वस्तूनि + 1, 16u)))
                प्रत्यागम -ENOMEM;

        k = q->n_वस्तूनि++;
        q->वस्तूनि[k] = (संरचना prioq_item) {
                .दत्त = दत्त,
                .idx = idx,
        };

        यदि (idx)
                *idx = k;

        shuffle_ऊर्ध्वम्(q, k);

        प्रत्यागम 0;
}

पूर्णाङ्क _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *दत्त, अचिह्नित *idx) {
        पूर्णाङ्क r;

        प्रतिज्ञा(q);

        r = prioq_ensure_allocated(q, compare_func);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम prioq_put(*q, दत्त, idx);
}

स्थिर शून्य remove_item(Prioq *q, संरचना prioq_item *i) {
        संरचना prioq_item *l;

        प्रतिज्ञा(q);
        प्रतिज्ञा(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        यदि (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->वस्तूनि + q->n_वस्तूनि - 1;

        यदि (i == l)
                /* Last entry, let's just remove it */
                q->n_वस्तूनि--;
        अन्यथा {
                अचिह्नित k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                प्रतिज्ञा(i >= q->वस्तूनि);
                k = i - q->वस्तूनि;

                i->दत्त = l->दत्त;
                i->idx = l->idx;
                यदि (i->idx)
                        *i->idx = k;
                q->n_वस्तूनि--;

                k = shuffle_अधस्तात्(q, k);
                shuffle_ऊर्ध्वम्(q, k);
        }
}

स्थिर संरचना prioq_item* find_item(Prioq *q, शून्य *दत्त, अचिह्नित *idx) {
        संरचना prioq_item *i;

        प्रतिज्ञा(q);

        यदि (q->n_वस्तूनि <= 0)
                प्रत्यागम रिक्त;

        यदि (idx) {
                यदि (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_वस्तूनि)
                        प्रत्यागम रिक्त;

                i = q->वस्तूनि + *idx;
                यदि (i->दत्त != दत्त)
                        प्रत्यागम रिक्त;

                प्रत्यागम i;
        } अन्यथा {
                प्रति (i = q->वस्तूनि; i < q->वस्तूनि + q->n_वस्तूनि; i++)
                        यदि (i->दत्त == दत्त)
                                प्रत्यागम i;
                प्रत्यागम रिक्त;
        }
}

पूर्णाङ्क prioq_remove(Prioq *q, शून्य *दत्त, अचिह्नित *idx) {
        संरचना prioq_item *i;

        यदि (!q)
                प्रत्यागम 0;

        i = find_item(q, दत्त, idx);
        यदि (!i)
                प्रत्यागम 0;

        remove_item(q, i);
        प्रत्यागम 1;
}

शून्य prioq_reshuffle(Prioq *q, शून्य *दत्त, अचिह्नित *idx) {
        संरचना prioq_item *i;
        अचिह्नित k;

        प्रतिज्ञा(q);

        i = find_item(q, दत्त, idx);
        यदि (!i)
                प्रत्यागम;

        प्रतिज्ञा(i >= q->वस्तूनि);
        k = i - q->वस्तूनि;
        k = shuffle_अधस्तात्(q, k);
        shuffle_ऊर्ध्वम्(q, k);
}

शून्य* prioq_peek_by_सूचकांक(Prioq *q, अचिह्नित idx) {
        यदि (!q)
                प्रत्यागम रिक्त;

        यदि (idx >= q->n_वस्तूनि)
                प्रत्यागम रिक्त;

        प्रत्यागम q->वस्तूनि[idx].दत्त;
}

शून्य* prioq_निष्कासय(Prioq *q) {
        शून्य *दत्त;

        यदि (!q)
                प्रत्यागम रिक्त;

        यदि (q->n_वस्तूनि <= 0)
                प्रत्यागम रिक्त;

        दत्त = q->वस्तूनि[0].दत्त;
        remove_item(q, q->वस्तूनि);
        प्रत्यागम दत्त;
}

अचिह्नित prioq_आकार(Prioq *q) {

        यदि (!q)
                प्रत्यागम 0;

        प्रत्यागम q->n_वस्तूनि;
}

bool prioq_isempty(Prioq *q) {

        यदि (!q)
                प्रत्यागम true;

        प्रत्यागम q->n_वस्तूनि <= 0;
}
