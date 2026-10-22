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

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "prioq.h"

ഘടന prioq_item {
        ശൂന്യം *data;
        unsigned *idx;
};

ഘടന Prioq {
        compare_func_t compare_func;
        unsigned n_ഇനങ്ങൾ;
        ഘടന prioq_item *ഇനങ്ങൾ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        എങ്കിൽ (!q)
                തിരികെ q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        തിരികെ q;
}

Prioq* prioq_free(Prioq *q) {
        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->ഇനങ്ങൾ, q->n_ഇനങ്ങൾ)
                എങ്കിൽ (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        സ്വതന്ത്രം(q->ഇനങ്ങൾ);
        തിരികെ mfree(q);
}

പൂർണ്ണസംഖ്യ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        പ്രതിജ്ഞാ(q);

        എങ്കിൽ (*q) {
                പ്രതിജ്ഞാ((*q)->compare_func == compare_func);
                തിരികെ 0;
        }

        *q = prioq_new(compare_func);
        എങ്കിൽ (!*q)
                തിരികെ -ENOMEM;

        തിരികെ 0;
}

സ്ഥിതം ശൂന്യം swap(Prioq *q, unsigned j, unsigned k) {
        പ്രതിജ്ഞാ(q);
        പ്രതിജ്ഞാ(j < q->n_ഇനങ്ങൾ);
        പ്രതിജ്ഞാ(k < q->n_ഇനങ്ങൾ);

        പ്രതിജ്ഞാ(!q->ഇനങ്ങൾ[j].idx || *(q->ഇനങ്ങൾ[j].idx) == j);
        പ്രതിജ്ഞാ(!q->ഇനങ്ങൾ[k].idx || *(q->ഇനങ്ങൾ[k].idx) == k);

        SWAP_TWO(q->ഇനങ്ങൾ[j].data, q->ഇനങ്ങൾ[k].data);
        SWAP_TWO(q->ഇനങ്ങൾ[j].idx, q->ഇനങ്ങൾ[k].idx);

        എങ്കിൽ (q->ഇനങ്ങൾ[j].idx)
                *q->ഇനങ്ങൾ[j].idx = j;

        എങ്കിൽ (q->ഇനങ്ങൾ[k].idx)
                *q->ഇനങ്ങൾ[k].idx = k;
}

സ്ഥിതം unsigned shuffle_up(Prioq *q, unsigned idx) {
        പ്രതിജ്ഞാ(q);
        പ്രതിജ്ഞാ(idx < q->n_ഇനങ്ങൾ);

        ആകുമ്പോൾ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                എങ്കിൽ (q->compare_func(q->ഇനങ്ങൾ[k].data, q->ഇനങ്ങൾ[idx].data) <= 0)
                        നിർത്തുക;

                swap(q, idx, k);
                idx = k;
        }

        തിരികെ idx;
}

സ്ഥിതം unsigned shuffle_down(Prioq *q, unsigned idx) {
        പ്രതിജ്ഞാ(q);

        വേണ്ടി (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                എങ്കിൽ (j >= q->n_ഇനങ്ങൾ)
                        നിർത്തുക;

                എങ്കിൽ (q->compare_func(q->ഇനങ്ങൾ[j].data, q->ഇനങ്ങൾ[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                അല്ലെങ്കിൽ
                        s = idx;

                എങ്കിൽ (k < q->n_ഇനങ്ങൾ &&
                    q->compare_func(q->ഇനങ്ങൾ[k].data, q->ഇനങ്ങൾ[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                എങ്കിൽ (s == idx)
                        /* No swap necessary, we're done */
                        നിർത്തുക;

                swap(q, idx, s);
                idx = s;
        }

        തിരികെ idx;
}

പൂർണ്ണസംഖ്യ prioq_put(Prioq *q, ശൂന്യം *data, unsigned *idx) {
        unsigned k;

        പ്രതിജ്ഞാ(q);

        എങ്കിൽ (!GREEDY_REALLOC(q->ഇനങ്ങൾ, MAX(q->n_ഇനങ്ങൾ + 1, 16u)))
                തിരികെ -ENOMEM;

        k = q->n_ഇനങ്ങൾ++;
        q->ഇനങ്ങൾ[k] = (ഘടന prioq_item) {
                .data = data,
                .idx = idx,
        };

        എങ്കിൽ (idx)
                *idx = k;

        shuffle_up(q, k);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ശൂന്യം *data, unsigned *idx) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(q);

        r = prioq_ensure_allocated(q, compare_func);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ prioq_put(*q, data, idx);
}

സ്ഥിതം ശൂന്യം remove_item(Prioq *q, ഘടന prioq_item *i) {
        ഘടന prioq_item *l;

        പ്രതിജ്ഞാ(q);
        പ്രതിജ്ഞാ(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        എങ്കിൽ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->ഇനങ്ങൾ + q->n_ഇനങ്ങൾ - 1;

        എങ്കിൽ (i == l)
                /* Last entry, let's just remove it */
                q->n_ഇനങ്ങൾ--;
        അല്ലെങ്കിൽ {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                പ്രതിജ്ഞാ(i >= q->ഇനങ്ങൾ);
                k = i - q->ഇനങ്ങൾ;

                i->data = l->data;
                i->idx = l->idx;
                എങ്കിൽ (i->idx)
                        *i->idx = k;
                q->n_ഇനങ്ങൾ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

സ്ഥിതം ഘടന prioq_item* find_item(Prioq *q, ശൂന്യം *data, unsigned *idx) {
        ഘടന prioq_item *i;

        പ്രതിജ്ഞാ(q);

        എങ്കിൽ (q->n_ഇനങ്ങൾ <= 0)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (idx) {
                എങ്കിൽ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_ഇനങ്ങൾ)
                        തിരികെ ശൂന്യ;

                i = q->ഇനങ്ങൾ + *idx;
                എങ്കിൽ (i->data != data)
                        തിരികെ ശൂന്യ;

                തിരികെ i;
        } അല്ലെങ്കിൽ {
                വേണ്ടി (i = q->ഇനങ്ങൾ; i < q->ഇനങ്ങൾ + q->n_ഇനങ്ങൾ; i++)
                        എങ്കിൽ (i->data == data)
                                തിരികെ i;
                തിരികെ ശൂന്യ;
        }
}

പൂർണ്ണസംഖ്യ prioq_remove(Prioq *q, ശൂന്യം *data, unsigned *idx) {
        ഘടന prioq_item *i;

        എങ്കിൽ (!q)
                തിരികെ 0;

        i = find_item(q, data, idx);
        എങ്കിൽ (!i)
                തിരികെ 0;

        remove_item(q, i);
        തിരികെ 1;
}

ശൂന്യം prioq_reshuffle(Prioq *q, ശൂന്യം *data, unsigned *idx) {
        ഘടന prioq_item *i;
        unsigned k;

        പ്രതിജ്ഞാ(q);

        i = find_item(q, data, idx);
        എങ്കിൽ (!i)
                തിരികെ;

        പ്രതിജ്ഞാ(i >= q->ഇനങ്ങൾ);
        k = i - q->ഇനങ്ങൾ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

ശൂന്യം* prioq_peek_by_index(Prioq *q, unsigned idx) {
        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (idx >= q->n_ഇനങ്ങൾ)
                തിരികെ ശൂന്യ;

        തിരികെ q->ഇനങ്ങൾ[idx].data;
}

ശൂന്യം* prioq_എടുക്കുക(Prioq *q) {
        ശൂന്യം *data;

        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (q->n_ഇനങ്ങൾ <= 0)
                തിരികെ ശൂന്യ;

        data = q->ഇനങ്ങൾ[0].data;
        remove_item(q, q->ഇനങ്ങൾ);
        തിരികെ data;
}

unsigned prioq_size(Prioq *q) {

        എങ്കിൽ (!q)
                തിരികെ 0;

        തിരികെ q->n_ഇനങ്ങൾ;
}

bool prioq_isempty(Prioq *q) {

        എങ്കിൽ (!q)
                തിരികെ true;

        തിരികെ q->n_ഇനങ്ങൾ <= 0;
}
