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
        अचिन्हित n_वस्तुसंच;
        संरचना prioq_item *वस्तुसंच;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        जर (!q)
                परत q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        परत q;
}

Prioq* prioq_free(Prioq *q) {
        जर (!q)
                परत रिक्त;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->वस्तुसंच, q->n_वस्तुसंच)
                जर (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        मुक्त(q->वस्तुसंच);
        परत mfree(q);
}

पूर्णांक prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        प्रतिज्ञा(q);

        जर (*q) {
                प्रतिज्ञा((*q)->compare_func == compare_func);
                परत 0;
        }

        *q = prioq_new(compare_func);
        जर (!*q)
                परत -ENOMEM;

        परत 0;
}

स्थिर शून्य swap(Prioq *q, अचिन्हित j, अचिन्हित k) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(j < q->n_वस्तुसंच);
        प्रतिज्ञा(k < q->n_वस्तुसंच);

        प्रतिज्ञा(!q->वस्तुसंच[j].idx || *(q->वस्तुसंच[j].idx) == j);
        प्रतिज्ञा(!q->वस्तुसंच[k].idx || *(q->वस्तुसंच[k].idx) == k);

        SWAP_TWO(q->वस्तुसंच[j].data, q->वस्तुसंच[k].data);
        SWAP_TWO(q->वस्तुसंच[j].idx, q->वस्तुसंच[k].idx);

        जर (q->वस्तुसंच[j].idx)
                *q->वस्तुसंच[j].idx = j;

        जर (q->वस्तुसंच[k].idx)
                *q->वस्तुसंच[k].idx = k;
}

स्थिर अचिन्हित shuffle_up(Prioq *q, अचिन्हित idx) {
        प्रतिज्ञा(q);
        प्रतिज्ञा(idx < q->n_वस्तुसंच);

        पर्यंत (idx > 0) {
                अचिन्हित k;

                k = (idx-1)/2;

                जर (q->compare_func(q->वस्तुसंच[k].data, q->वस्तुसंच[idx].data) <= 0)
                        थांबा;

                swap(q, idx, k);
                idx = k;
        }

        परत idx;
}

स्थिर अचिन्हित shuffle_down(Prioq *q, अचिन्हित idx) {
        प्रतिज्ञा(q);

        साठी (;;) {
                अचिन्हित j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                जर (j >= q->n_वस्तुसंच)
                        थांबा;

                जर (q->compare_func(q->वस्तुसंच[j].data, q->वस्तुसंच[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                अन्यथा
                        s = idx;

                जर (k < q->n_वस्तुसंच &&
                    q->compare_func(q->वस्तुसंच[k].data, q->वस्तुसंच[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                जर (s == idx)
                        /* No swap necessary, we're done */
                        थांबा;

                swap(q, idx, s);
                idx = s;
        }

        परत idx;
}

पूर्णांक prioq_put(Prioq *q, शून्य *data, अचिन्हित *idx) {
        अचिन्हित k;

        प्रतिज्ञा(q);

        जर (!GREEDY_REALLOC(q->वस्तुसंच, MAX(q->n_वस्तुसंच + 1, 16u)))
                परत -ENOMEM;

        k = q->n_वस्तुसंच++;
        q->वस्तुसंच[k] = (संरचना prioq_item) {
                .data = data,
                .idx = idx,
        };

        जर (idx)
                *idx = k;

        shuffle_up(q, k);

        परत 0;
}

पूर्णांक _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *data, अचिन्हित *idx) {
        पूर्णांक r;

        प्रतिज्ञा(q);

        r = prioq_ensure_allocated(q, compare_func);
        जर (r < 0)
                परत r;

        परत prioq_put(*q, data, idx);
}

स्थिर शून्य remove_item(Prioq *q, संरचना prioq_item *i) {
        संरचना prioq_item *l;

        प्रतिज्ञा(q);
        प्रतिज्ञा(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        जर (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->वस्तुसंच + q->n_वस्तुसंच - 1;

        जर (i == l)
                /* Last entry, let's just remove it */
                q->n_वस्तुसंच--;
        अन्यथा {
                अचिन्हित k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                प्रतिज्ञा(i >= q->वस्तुसंच);
                k = i - q->वस्तुसंच;

                i->data = l->data;
                i->idx = l->idx;
                जर (i->idx)
                        *i->idx = k;
                q->n_वस्तुसंच--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

स्थिर संरचना prioq_item* find_item(Prioq *q, शून्य *data, अचिन्हित *idx) {
        संरचना prioq_item *i;

        प्रतिज्ञा(q);

        जर (q->n_वस्तुसंच <= 0)
                परत रिक्त;

        जर (idx) {
                जर (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_वस्तुसंच)
                        परत रिक्त;

                i = q->वस्तुसंच + *idx;
                जर (i->data != data)
                        परत रिक्त;

                परत i;
        } अन्यथा {
                साठी (i = q->वस्तुसंच; i < q->वस्तुसंच + q->n_वस्तुसंच; i++)
                        जर (i->data == data)
                                परत i;
                परत रिक्त;
        }
}

पूर्णांक prioq_remove(Prioq *q, शून्य *data, अचिन्हित *idx) {
        संरचना prioq_item *i;

        जर (!q)
                परत 0;

        i = find_item(q, data, idx);
        जर (!i)
                परत 0;

        remove_item(q, i);
        परत 1;
}

शून्य prioq_reshuffle(Prioq *q, शून्य *data, अचिन्हित *idx) {
        संरचना prioq_item *i;
        अचिन्हित k;

        प्रतिज्ञा(q);

        i = find_item(q, data, idx);
        जर (!i)
                परत;

        प्रतिज्ञा(i >= q->वस्तुसंच);
        k = i - q->वस्तुसंच;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

शून्य* prioq_peek_by_index(Prioq *q, अचिन्हित idx) {
        जर (!q)
                परत रिक्त;

        जर (idx >= q->n_वस्तुसंच)
                परत रिक्त;

        परत q->वस्तुसंच[idx].data;
}

शून्य* prioq_काढा(Prioq *q) {
        शून्य *data;

        जर (!q)
                परत रिक्त;

        जर (q->n_वस्तुसंच <= 0)
                परत रिक्त;

        data = q->वस्तुसंच[0].data;
        remove_item(q, q->वस्तुसंच);
        परत data;
}

अचिन्हित prioq_size(Prioq *q) {

        जर (!q)
                परत 0;

        परत q->n_वस्तुसंच;
}

bool prioq_isempty(Prioq *q) {

        जर (!q)
                परत true;

        परत q->n_वस्तुसंच <= 0;
}
