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

#شامل "alloc-util.h"
#شامل "prioq.h"

ساخت prioq_شے {
        خالی *ڈیٹا;
        غیرعلامتی *idx;
};

ساخت Prioq {
        compare_func_t compare_func;
        غیرعلامتی n_اشیاء;
        ساخت prioq_شے *اشیاء;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        اگر (!q)
                واپس q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        واپس q;
}

Prioq* prioq_free(Prioq *q) {
        اگر (!q)
                واپس تہی;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(شے_2, q->اشیاء, q->n_اشیاء)
                اگر (شے_2->idx)
                        *(شے_2->idx) = PRIOQ_IDX_NULL;

        آزاد(q->اشیاء);
        واپس mfree(q);
}

صحیح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        أكد(q);

        اگر (*q) {
                أكد((*q)->compare_func == compare_func);
                واپس 0;
        }

        *q = prioq_new(compare_func);
        اگر (!*q)
                واپس -ENOMEM;

        واپس 0;
}

ساکن خالی تبادلہ(Prioq *q, غیرعلامتی j, غیرعلامتی k) {
        أكد(q);
        أكد(j < q->n_اشیاء);
        أكد(k < q->n_اشیاء);

        أكد(!q->اشیاء[j].idx || *(q->اشیاء[j].idx) == j);
        أكد(!q->اشیاء[k].idx || *(q->اشیاء[k].idx) == k);

        تبادلہ_TWO(q->اشیاء[j].ڈیٹا, q->اشیاء[k].ڈیٹا);
        تبادلہ_TWO(q->اشیاء[j].idx, q->اشیاء[k].idx);

        اگر (q->اشیاء[j].idx)
                *q->اشیاء[j].idx = j;

        اگر (q->اشیاء[k].idx)
                *q->اشیاء[k].idx = k;
}

ساکن غیرعلامتی shuffle_اوپر(Prioq *q, غیرعلامتی idx) {
        أكد(q);
        أكد(idx < q->n_اشیاء);

        جب (idx > 0) {
                غیرعلامتی k;

                k = (idx-1)/2;

                اگر (q->compare_func(q->اشیاء[k].ڈیٹا, q->اشیاء[idx].ڈیٹا) <= 0)
                        توڑو;

                تبادلہ(q, idx, k);
                idx = k;
        }

        واپس idx;
}

ساکن غیرعلامتی shuffle_نیچے_کو(Prioq *q, غیرعلامتی idx) {
        أكد(q);

        برائے (;;) {
                غیرعلامتی j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                اگر (j >= q->n_اشیاء)
                        توڑو;

                اگر (q->compare_func(q->اشیاء[j].ڈیٹا, q->اشیاء[idx].ڈیٹا) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                ورنہ
                        s = idx;

                اگر (k < q->n_اشیاء &&
                    q->compare_func(q->اشیاء[k].ڈیٹا, q->اشیاء[s].ڈیٹا) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                اگر (s == idx)
                        /* No swap necessary, we're done */
                        توڑو;

                تبادلہ(q, idx, s);
                idx = s;
        }

        واپس idx;
}

صحیح prioq_put(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx) {
        غیرعلامتی k;

        أكد(q);

        اگر (!GREEDY_REALLOC(q->اشیاء, MAX(q->n_اشیاء + 1, 16u)))
                واپس -ENOMEM;

        k = q->n_اشیاء++;
        q->اشیاء[k] = (ساخت prioq_شے) {
                .ڈیٹا = ڈیٹا,
                .idx = idx,
        };

        اگر (idx)
                *idx = k;

        shuffle_اوپر(q, k);

        واپس 0;
}

صحیح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, خالی *ڈیٹا, غیرعلامتی *idx) {
        صحیح r;

        أكد(q);

        r = prioq_ensure_allocated(q, compare_func);
        اگر (r < 0)
                واپس r;

        واپس prioq_put(*q, ڈیٹا, idx);
}

ساکن خالی remove_شے(Prioq *q, ساخت prioq_شے *i) {
        ساخت prioq_شے *l;

        أكد(q);
        أكد(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        اگر (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->اشیاء + q->n_اشیاء - 1;

        اگر (i == l)
                /* Last entry, let's just remove it */
                q->n_اشیاء--;
        ورنہ {
                غیرعلامتی k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                أكد(i >= q->اشیاء);
                k = i - q->اشیاء;

                i->ڈیٹا = l->ڈیٹا;
                i->idx = l->idx;
                اگر (i->idx)
                        *i->idx = k;
                q->n_اشیاء--;

                k = shuffle_نیچے_کو(q, k);
                shuffle_اوپر(q, k);
        }
}

ساکن ساخت prioq_شے* ڈھونڈو_شے(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx) {
        ساخت prioq_شے *i;

        أكد(q);

        اگر (q->n_اشیاء <= 0)
                واپس تہی;

        اگر (idx) {
                اگر (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_اشیاء)
                        واپس تہی;

                i = q->اشیاء + *idx;
                اگر (i->ڈیٹا != ڈیٹا)
                        واپس تہی;

                واپس i;
        } ورنہ {
                برائے (i = q->اشیاء; i < q->اشیاء + q->n_اشیاء; i++)
                        اگر (i->ڈیٹا == ڈیٹا)
                                واپس i;
                واپس تہی;
        }
}

صحیح prioq_remove(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx) {
        ساخت prioq_شے *i;

        اگر (!q)
                واپس 0;

        i = ڈھونڈو_شے(q, ڈیٹا, idx);
        اگر (!i)
                واپس 0;

        remove_شے(q, i);
        واپس 1;
}

خالی prioq_reshuffle(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx) {
        ساخت prioq_شے *i;
        غیرعلامتی k;

        أكد(q);

        i = ڈھونڈو_شے(q, ڈیٹا, idx);
        اگر (!i)
                واپس;

        أكد(i >= q->اشیاء);
        k = i - q->اشیاء;
        k = shuffle_نیچے_کو(q, k);
        shuffle_اوپر(q, k);
}

خالی* prioq_peek_by_اشاریہ(Prioq *q, غیرعلامتی idx) {
        اگر (!q)
                واپس تہی;

        اگر (idx >= q->n_اشیاء)
                واپس تہی;

        واپس q->اشیاء[idx].ڈیٹا;
}

خالی* prioq_نکالو(Prioq *q) {
        خالی *ڈیٹا;

        اگر (!q)
                واپس تہی;

        اگر (q->n_اشیاء <= 0)
                واپس تہی;

        ڈیٹا = q->اشیاء[0].ڈیٹا;
        remove_شے(q, q->اشیاء);
        واپس ڈیٹا;
}

غیرعلامتی prioq_سائز(Prioq *q) {

        اگر (!q)
                واپس 0;

        واپس q->n_اشیاء;
}

bool prioq_isempty(Prioq *q) {

        اگر (!q)
                واپس true;

        واپس q->n_اشیاء <= 0;
}
