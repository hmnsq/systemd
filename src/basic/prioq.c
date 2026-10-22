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

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "prioq.h"

কাঠামো prioq_item {
        শূন্য *তথ্য;
        unsigned *idx;
};

কাঠামো Prioq {
        compare_func_t compare_func;
        unsigned n_বস্তুসমূহ;
        কাঠামো prioq_item *বস্তুসমূহ;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        যদি (!q)
                ফেরত q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        ফেরত q;
}

Prioq* prioq_free(Prioq *q) {
        যদি (!q)
                ফেরত ফাঁকা;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(item, q->বস্তুসমূহ, q->n_বস্তুসমূহ)
                যদি (item->idx)
                        *(item->idx) = PRIOQ_IDX_NULL;

        মুক্ত(q->বস্তুসমূহ);
        ফেরত mfree(q);
}

পূর্ণসংখ্যা prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        প্রতিজ্ঞা(q);

        যদি (*q) {
                প্রতিজ্ঞা((*q)->compare_func == compare_func);
                ফেরত 0;
        }

        *q = prioq_new(compare_func);
        যদি (!*q)
                ফেরত -ENOMEM;

        ফেরত 0;
}

স্থির শূন্য swap(Prioq *q, unsigned j, unsigned k) {
        প্রতিজ্ঞা(q);
        প্রতিজ্ঞা(j < q->n_বস্তুসমূহ);
        প্রতিজ্ঞা(k < q->n_বস্তুসমূহ);

        প্রতিজ্ঞা(!q->বস্তুসমূহ[j].idx || *(q->বস্তুসমূহ[j].idx) == j);
        প্রতিজ্ঞা(!q->বস্তুসমূহ[k].idx || *(q->বস্তুসমূহ[k].idx) == k);

        SWAP_TWO(q->বস্তুসমূহ[j].তথ্য, q->বস্তুসমূহ[k].তথ্য);
        SWAP_TWO(q->বস্তুসমূহ[j].idx, q->বস্তুসমূহ[k].idx);

        যদি (q->বস্তুসমূহ[j].idx)
                *q->বস্তুসমূহ[j].idx = j;

        যদি (q->বস্তুসমূহ[k].idx)
                *q->বস্তুসমূহ[k].idx = k;
}

স্থির unsigned shuffle_up(Prioq *q, unsigned idx) {
        প্রতিজ্ঞা(q);
        প্রতিজ্ঞা(idx < q->n_বস্তুসমূহ);

        যখন (idx > 0) {
                unsigned k;

                k = (idx-1)/2;

                যদি (q->compare_func(q->বস্তুসমূহ[k].তথ্য, q->বস্তুসমূহ[idx].তথ্য) <= 0)
                        বিরতি;

                swap(q, idx, k);
                idx = k;
        }

        ফেরত idx;
}

স্থির unsigned shuffle_down(Prioq *q, unsigned idx) {
        প্রতিজ্ঞা(q);

        জন্য (;;) {
                unsigned j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                যদি (j >= q->n_বস্তুসমূহ)
                        বিরতি;

                যদি (q->compare_func(q->বস্তুসমূহ[j].তথ্য, q->বস্তুসমূহ[idx].তথ্য) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                অন্যথা
                        s = idx;

                যদি (k < q->n_বস্তুসমূহ &&
                    q->compare_func(q->বস্তুসমূহ[k].তথ্য, q->বস্তুসমূহ[s].তথ্য) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                যদি (s == idx)
                        /* No swap necessary, we're done */
                        বিরতি;

                swap(q, idx, s);
                idx = s;
        }

        ফেরত idx;
}

পূর্ণসংখ্যা prioq_put(Prioq *q, শূন্য *তথ্য, unsigned *idx) {
        unsigned k;

        প্রতিজ্ঞা(q);

        যদি (!GREEDY_REALLOC(q->বস্তুসমূহ, MAX(q->n_বস্তুসমূহ + 1, 16u)))
                ফেরত -ENOMEM;

        k = q->n_বস্তুসমূহ++;
        q->বস্তুসমূহ[k] = (কাঠামো prioq_item) {
                .তথ্য = তথ্য,
                .idx = idx,
        };

        যদি (idx)
                *idx = k;

        shuffle_up(q, k);

        ফেরত 0;
}

পূর্ণসংখ্যা _prioq_ensure_put(Prioq **q, compare_func_t compare_func, শূন্য *তথ্য, unsigned *idx) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(q);

        r = prioq_ensure_allocated(q, compare_func);
        যদি (r < 0)
                ফেরত r;

        ফেরত prioq_put(*q, তথ্য, idx);
}

স্থির শূন্য remove_item(Prioq *q, কাঠামো prioq_item *i) {
        কাঠামো prioq_item *l;

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

                i->তথ্য = l->তথ্য;
                i->idx = l->idx;
                যদি (i->idx)
                        *i->idx = k;
                q->n_বস্তুসমূহ--;

                k = shuffle_down(q, k);
                shuffle_up(q, k);
        }
}

স্থির কাঠামো prioq_item* find_item(Prioq *q, শূন্য *তথ্য, unsigned *idx) {
        কাঠামো prioq_item *i;

        প্রতিজ্ঞা(q);

        যদি (q->n_বস্তুসমূহ <= 0)
                ফেরত ফাঁকা;

        যদি (idx) {
                যদি (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_বস্তুসমূহ)
                        ফেরত ফাঁকা;

                i = q->বস্তুসমূহ + *idx;
                যদি (i->তথ্য != তথ্য)
                        ফেরত ফাঁকা;

                ফেরত i;
        } অন্যথা {
                জন্য (i = q->বস্তুসমূহ; i < q->বস্তুসমূহ + q->n_বস্তুসমূহ; i++)
                        যদি (i->তথ্য == তথ্য)
                                ফেরত i;
                ফেরত ফাঁকা;
        }
}

পূর্ণসংখ্যা prioq_remove(Prioq *q, শূন্য *তথ্য, unsigned *idx) {
        কাঠামো prioq_item *i;

        যদি (!q)
                ফেরত 0;

        i = find_item(q, তথ্য, idx);
        যদি (!i)
                ফেরত 0;

        remove_item(q, i);
        ফেরত 1;
}

শূন্য prioq_reshuffle(Prioq *q, শূন্য *তথ্য, unsigned *idx) {
        কাঠামো prioq_item *i;
        unsigned k;

        প্রতিজ্ঞা(q);

        i = find_item(q, তথ্য, idx);
        যদি (!i)
                ফেরত;

        প্রতিজ্ঞা(i >= q->বস্তুসমূহ);
        k = i - q->বস্তুসমূহ;
        k = shuffle_down(q, k);
        shuffle_up(q, k);
}

শূন্য* prioq_peek_by_সূচক(Prioq *q, unsigned idx) {
        যদি (!q)
                ফেরত ফাঁকা;

        যদি (idx >= q->n_বস্তুসমূহ)
                ফেরত ফাঁকা;

        ফেরত q->বস্তুসমূহ[idx].তথ্য;
}

শূন্য* prioq_বের_করো(Prioq *q) {
        শূন্য *তথ্য;

        যদি (!q)
                ফেরত ফাঁকা;

        যদি (q->n_বস্তুসমূহ <= 0)
                ফেরত ফাঁকা;

        তথ্য = q->বস্তুসমূহ[0].তথ্য;
        remove_item(q, q->বস্তুসমূহ);
        ফেরত তথ্য;
}

unsigned prioq_আকার(Prioq *q) {

        যদি (!q)
                ফেরত 0;

        ফেরত q->n_বস্তুসমূহ;
}

bool prioq_isempty(Prioq *q) {

        যদি (!q)
                ফেরত true;

        ফেরত q->n_বস্তুসমূহ <= 0;
}
