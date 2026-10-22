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

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "prioq.h"

সংরচনা prioq_item {
        শূন্য *data;
        unsigned *idx;
};

সংরচনা Prioq {
        compare_func_t compare_func;
        unsigned n_বস্তুসমূহ;
        সংরচনা prioq_item *বস্তুসমূহ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        যদি (!q)
                ঘূৰাই q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ঘূৰাই q;
}

Prioq* prioq_free(Prioq *q) {
        যদি (!q)
                ঘূৰাই রিক্ত;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->বস্তুসমূহ, q->n_বস্তুসমূহ)
                যদি (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        মুক্ত(q->বস্তুসমূহ);
        ঘূৰাই mfree(q);
}

পূর্ণাঙ্ক prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        প্রতিজ্ঞা(q);

        যদি (*q) {
                প্রতিজ্ঞা((*q)->compare_func == compare_func);
                ঘূৰাই 0;
        }

        *q = prioq_new(compare_func);
        যদি (!*q)
                ঘূৰাই -ENOMEM;

        ঘূৰাই 0;
}

স্থির শূন্য swap(Prioq *q, unsigned j, unsigned k) {
        প্রতিজ্ঞা(q);
        প্রতিজ্ঞা(j < q->n_বস্তুসমূহ);
        প্রতিজ্ঞা(k < q->n_বস্তুসমূহ);

        প্রতিজ্ঞা(!q->বস্তুসমূহ[j].idx || *(q->বস্তুসমূহ[j].idx) == j);
        প্রতিজ্ঞা(!q->বস্তুসমূহ[k].idx || *(q->বস্তুসমূহ[k].idx) == k);

        SWAP_TWO(q->বস্তুসমূহ[j].data, q->বস্তুসমূহ[k].data);
        SWAP_TWO(q->বস্তুসমূহ[j].idx, q->বস্তুসমূহ[k].idx);

        যদি (q->বস্তুসমূহ[j].idx)
                *q->বস্তুসমূহ[j].idx = j;

        যদি (q->বস্তুসমূহ[k].idx)
                *q->বস্তুসমূহ[k].idx = k;
}

স্থির unsigned shuffle_up(Prioq *q, unsigned idx) {
        প্রতিজ্ঞা(q);
        প্রতিজ্ঞা(idx < q->n_বস্তুসমূহ);

        যেতিয়ালৈ (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                যদি (q->compare_func(q->বস্তুসমূহ[k].data, q->বস্তুসমূহ[idx].data) <= 0)
                        ভঙ্গ;

                swap(q, idx, k);
                idx = k;
        }

        ঘূৰাই idx;
}

স্থির unsigned shuffle_down(Prioq *q, unsigned idx) {
        প্রতিজ্ঞা(q);

        বাবে (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                যদি (j >= q->n_বস্তুসমূহ)
                        ভঙ্গ;

                যদি (q->compare_func(q->বস্তুসমূহ[j].data, q->বস্তুসমূহ[idx].data) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                অন্যথা
                        s = idx;

                যদি (k < q->n_বস্তুসমূহ &&
                    q->compare_func(q->বস্তুসমূহ[k].data, q->বস্তুসমূহ[s].data) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                যদি (s == idx)
                        /* No swap necessary, we're done */
                        ভঙ্গ;

                swap(q, idx, s);
                idx = s;
        }

        ঘূৰাই idx;
}

পূর্ণাঙ্ক prioq_put(Prioq *q, শূন্য *data, unsigned *idx) {
        unsigned k;

        প্রতিজ্ঞা(q);

        যদি (!GREEDY_REALLOC(q->বস্তুসমূহ, MAX(q->n_বস্তুসমূহ + 1, 16u)))
                ঘূৰাই -ENOMEM;

        k = q->n_বস্তুসমূহ++;
        q->বস্তুসমূহ[k] = (সংরচনা prioq_item) {
                .data = data,
                .idx = idx,
        };

        যদি (idx)
                *idx = k;

        shuffle_up(q, k);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক _prioq_ensure_put(Prioq **q, compare_func_t compare_func, শূন্য *data, unsigned *idx) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(q);

        r = prioq_ensure_allocated(q, compare_func);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই prioq_put(*q, data, idx);
}

স্থির শূন্য remove_item(Prioq *q, সংরচনা prioq_item *i) {
        সংরচনা prioq_item *l;

        প্রতিজ্ঞা(q);
        প্রতিজ্ঞা(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        যদি (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->বস্তুসমূহ + q->n_বস্তুসমূহ - 1;

        যদি (i == l)
                /* Last entry, let's just remove it */
                q->n_বস্তুসমূহ--;
        অন্যথা {
                unsigned k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                প্রতিজ্ঞা(i >= q->বস্তুসমূহ);
                k = i - q->বস্তুসমূহ;

                i->data = l->data;
                i->idx = l->idx;
                যদি (i->idx)
                        *i->idx = k;
                q->n_বস্তুসমূহ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

স্থির সংরচনা prioq_item* find_item(Prioq *q, শূন্য *data, unsigned *idx) {
        সংরচনা prioq_item *i;

        প্রতিজ্ঞা(q);

        যদি (q->n_বস্তুসমূহ <= 0)
                ঘূৰাই রিক্ত;

        যদি (idx) {
                যদি (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_বস্তুসমূহ)
                        ঘূৰাই রিক্ত;

                i = q->বস্তুসমূহ + *idx;
                যদি (i->data != data)
                        ঘূৰাই রিক্ত;

                ঘূৰাই i;
        } অন্যথা {
                বাবে (i = q->বস্তুসমূহ; i < q->বস্তুসমূহ + q->n_বস্তুসমূহ; i++)
                        যদি (i->data == data)
                                ঘূৰাই i;
                ঘূৰাই রিক্ত;
        }
}

পূর্ণাঙ্ক prioq_remove(Prioq *q, শূন্য *data, unsigned *idx) {
        সংরচনা prioq_item *i;

        যদি (!q)
                ঘূৰাই 0;

        i = find_item(q, data, idx);
        যদি (!i)
                ঘূৰাই 0;

        remove_item(q, i);
        ঘূৰাই 1;
}

শূন্য prioq_reshuffle(Prioq *q, শূন্য *data, unsigned *idx) {
        সংরচনা prioq_item *i;
        unsigned k;

        প্রতিজ্ঞা(q);

        i = find_item(q, data, idx);
        যদি (!i)
                ঘূৰাই;

        প্রতিজ্ঞা(i >= q->বস্তুসমূহ);
        k = i - q->বস্তুসমূহ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

শূন্য* prioq_peek_by_index(Prioq *q, unsigned idx) {
        যদি (!q)
                ঘূৰাই রিক্ত;

        যদি (idx >= q->n_বস্তুসমূহ)
                ঘূৰাই রিক্ত;

        ঘূৰাই q->বস্তুসমূহ[idx].data;
}

শূন্য* prioq_বেৰ_কৰো(Prioq *q) {
        শূন্য *data;

        যদি (!q)
                ঘূৰাই রিক্ত;

        যদি (q->n_বস্তুসমূহ <= 0)
                ঘূৰাই রিক্ত;

        data = q->বস্তুসমূহ[0].data;
        remove_item(q, q->বস্তুসমূহ);
        ঘূৰাই data;
}

unsigned prioq_size(Prioq *q) {

        যদি (!q)
                ঘূৰাই 0;

        ঘূৰাই q->n_বস্তুসমূহ;
}

bool prioq_isempty(Prioq *q) {

        যদি (!q)
                ঘূৰাই true;

        ঘূৰাই q->n_বস্তুসমূহ <= 0;
}
