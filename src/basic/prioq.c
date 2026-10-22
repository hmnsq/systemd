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

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "prioq.h"

கட்டமைப்பு prioq_item {
        வெறுமை *data;
        unsigned *idx;
};

கட்டமைப்பு Prioq {
        compare_func_t compare_func;
        unsigned n_பொருட்கள்;
        கட்டமைப்பு prioq_item *பொருட்கள்;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        எனில் (!q)
                திரும்பு q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        திரும்பு q;
}

Prioq* prioq_free(Prioq *q) {
        எனில் (!q)
                திரும்பு வெற்று;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->பொருட்கள், q->n_பொருட்கள்)
                எனில் (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        விடுவி(q->பொருட்கள்);
        திரும்பு mfree(q);
}

முழு prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        ப்ரதிஜ்ஞா(q);

        எனில் (*q) {
                ப்ரதிஜ்ஞா((*q)->compare_func == compare_func);
                திரும்பு 0;
        }

        *q = prioq_new(compare_func);
        எனில் (!*q)
                திரும்பு -ENOMEM;

        திரும்பு 0;
}

நிலையான வெறுமை swap(Prioq *q, unsigned j, unsigned k) {
        ப்ரதிஜ்ஞா(q);
        ப்ரதிஜ்ஞா(j < q->n_பொருட்கள்);
        ப்ரதிஜ்ஞா(k < q->n_பொருட்கள்);

        ப்ரதிஜ்ஞா(!q->பொருட்கள்[j].idx || *(q->பொருட்கள்[j].idx) == j);
        ப்ரதிஜ்ஞா(!q->பொருட்கள்[k].idx || *(q->பொருட்கள்[k].idx) == k);

        SWAP_TWO(q->பொருட்கள்[j].data, q->பொருட்கள்[k].data);
        SWAP_TWO(q->பொருட்கள்[j].idx, q->பொருட்கள்[k].idx);

        எனில் (q->பொருட்கள்[j].idx)
                *q->பொருட்கள்[j].idx = j;

        எனில் (q->பொருட்கள்[k].idx)
                *q->பொருட்கள்[k].idx = k;
}

நிலையான unsigned shuffle_up(Prioq *q, unsigned idx) {
        ப்ரதிஜ்ஞா(q);
        ப்ரதிஜ்ஞா(idx < q->n_பொருட்கள்);

        போது (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                எனில் (q->compare_func(q->பொருட்கள்[k].data, q->பொருட்கள்[idx].data) <= 0)
                        நிறுத்து;

                swap(q, idx, k);
                idx = k;
        }

        திரும்பு idx;
}

நிலையான unsigned shuffle_down(Prioq *q, unsigned idx) {
        ப்ரதிஜ்ஞா(q);

        க்கு (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                எனில் (j >= q->n_பொருட்கள்)
                        நிறுத்து;

                எனில் (q->compare_func(q->பொருட்கள்[j].data, q->பொருட்கள்[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                இல்லையெனில்
                        s = idx;

                எனில் (k < q->n_பொருட்கள் &&
                    q->compare_func(q->பொருட்கள்[k].data, q->பொருட்கள்[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                எனில் (s == idx)
                        /* No swap necessary, we're done */
                        நிறுத்து;

                swap(q, idx, s);
                idx = s;
        }

        திரும்பு idx;
}

முழு prioq_put(Prioq *q, வெறுமை *data, unsigned *idx) {
        unsigned k;

        ப்ரதிஜ்ஞா(q);

        எனில் (!GREEDY_REALLOC(q->பொருட்கள், MAX(q->n_பொருட்கள் + 1, 16u)))
                திரும்பு -ENOMEM;

        k = q->n_பொருட்கள்++;
        q->பொருட்கள்[k] = (கட்டமைப்பு prioq_item) {
                .data = data,
                .idx = idx,
        };

        எனில் (idx)
                *idx = k;

        shuffle_up(q, k);

        திரும்பு 0;
}

முழு _prioq_ensure_put(Prioq **q, compare_func_t compare_func, வெறுமை *data, unsigned *idx) {
        முழு r;

        ப்ரதிஜ்ஞா(q);

        r = prioq_ensure_allocated(q, compare_func);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு prioq_put(*q, data, idx);
}

நிலையான வெறுமை remove_item(Prioq *q, கட்டமைப்பு prioq_item *i) {
        கட்டமைப்பு prioq_item *l;

        ப்ரதிஜ்ஞா(q);
        ப்ரதிஜ்ஞா(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        எனில் (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->பொருட்கள் + q->n_பொருட்கள் - 1;

        எனில் (i == l)
                /* Last entry, let's just remove it */
                q->n_பொருட்கள்--;
        இல்லையெனில் {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                ப்ரதிஜ்ஞா(i >= q->பொருட்கள்);
                k = i - q->பொருட்கள்;

                i->data = l->data;
                i->idx = l->idx;
                எனில் (i->idx)
                        *i->idx = k;
                q->n_பொருட்கள்--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

நிலையான கட்டமைப்பு prioq_item* find_item(Prioq *q, வெறுமை *data, unsigned *idx) {
        கட்டமைப்பு prioq_item *i;

        ப்ரதிஜ்ஞா(q);

        எனில் (q->n_பொருட்கள் <= 0)
                திரும்பு வெற்று;

        எனில் (idx) {
                எனில் (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_பொருட்கள்)
                        திரும்பு வெற்று;

                i = q->பொருட்கள் + *idx;
                எனில் (i->data != data)
                        திரும்பு வெற்று;

                திரும்பு i;
        } இல்லையெனில் {
                க்கு (i = q->பொருட்கள்; i < q->பொருட்கள் + q->n_பொருட்கள்; i++)
                        எனில் (i->data == data)
                                திரும்பு i;
                திரும்பு வெற்று;
        }
}

முழு prioq_remove(Prioq *q, வெறுமை *data, unsigned *idx) {
        கட்டமைப்பு prioq_item *i;

        எனில் (!q)
                திரும்பு 0;

        i = find_item(q, data, idx);
        எனில் (!i)
                திரும்பு 0;

        remove_item(q, i);
        திரும்பு 1;
}

வெறுமை prioq_reshuffle(Prioq *q, வெறுமை *data, unsigned *idx) {
        கட்டமைப்பு prioq_item *i;
        unsigned k;

        ப்ரதிஜ்ஞா(q);

        i = find_item(q, data, idx);
        எனில் (!i)
                திரும்பு;

        ப்ரதிஜ்ஞா(i >= q->பொருட்கள்);
        k = i - q->பொருட்கள்;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

வெறுமை* prioq_peek_by_index(Prioq *q, unsigned idx) {
        எனில் (!q)
                திரும்பு வெற்று;

        எனில் (idx >= q->n_பொருட்கள்)
                திரும்பு வெற்று;

        திரும்பு q->பொருட்கள்[idx].data;
}

வெறுமை* prioq_எடு(Prioq *q) {
        வெறுமை *data;

        எனில் (!q)
                திரும்பு வெற்று;

        எனில் (q->n_பொருட்கள் <= 0)
                திரும்பு வெற்று;

        data = q->பொருட்கள்[0].data;
        remove_item(q, q->பொருட்கள்);
        திரும்பு data;
}

unsigned prioq_size(Prioq *q) {

        எனில் (!q)
                திரும்பு 0;

        திரும்பு q->n_பொருட்கள்;
}

bool prioq_isempty(Prioq *q) {

        எனில் (!q)
                திரும்பு true;

        திரும்பு q->n_பொருட்கள் <= 0;
}
