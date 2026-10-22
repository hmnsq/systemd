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

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "prioq.h"

నిర్మాణం prioq_అంశం {
        శూన్య *దత్తాంశం;
        గుర్తులేని *idx;
};

నిర్మాణం Prioq {
        compare_func_t compare_func;
        గుర్తులేని n_వస్తువులు;
        నిర్మాణం prioq_అంశం *వస్తువులు;
};

Prioq* prioq_new(compare_func_t compare_func) {
        Prioq *q;

        q = new(Prioq, 1);
        ఒకవేళ (!q)
                తిరిగి q;

        *q = (Prioq) {
                .compare_func = compare_func,
        };

        తిరిగి q;
}

Prioq* prioq_free(Prioq *q) {
        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        /* Invalidate the index fields of any remaining objects */
        FOREACH_ARRAY(అంశం, q->వస్తువులు, q->n_వస్తువులు)
                ఒకవేళ (అంశం->idx)
                        *(అంశం->idx) = PRIOQ_IDX_NULL;

        విడుదల(q->వస్తువులు);
        తిరిగి mfree(q);
}

పూర్ణాంకం prioq_ensure_allocated(Prioq **q, compare_func_t compare_func) {
        ప్రతిజ్ఞా(q);

        ఒకవేళ (*q) {
                ప్రతిజ్ఞా((*q)->compare_func == compare_func);
                తిరిగి 0;
        }

        *q = prioq_new(compare_func);
        ఒకవేళ (!*q)
                తిరిగి -ENOMEM;

        తిరిగి 0;
}

స్థిర శూన్య మార్పిడి(Prioq *q, గుర్తులేని j, గుర్తులేని k) {
        ప్రతిజ్ఞా(q);
        ప్రతిజ్ఞా(j < q->n_వస్తువులు);
        ప్రతిజ్ఞా(k < q->n_వస్తువులు);

        ప్రతిజ్ఞా(!q->వస్తువులు[j].idx || *(q->వస్తువులు[j].idx) == j);
        ప్రతిజ్ఞా(!q->వస్తువులు[k].idx || *(q->వస్తువులు[k].idx) == k);

        మార్పిడి_TWO(q->వస్తువులు[j].దత్తాంశం, q->వస్తువులు[k].దత్తాంశం);
        మార్పిడి_TWO(q->వస్తువులు[j].idx, q->వస్తువులు[k].idx);

        ఒకవేళ (q->వస్తువులు[j].idx)
                *q->వస్తువులు[j].idx = j;

        ఒకవేళ (q->వస్తువులు[k].idx)
                *q->వస్తువులు[k].idx = k;
}

స్థిర గుర్తులేని shuffle_పైకి(Prioq *q, గుర్తులేని idx) {
        ప్రతిజ్ఞా(q);
        ప్రతిజ్ఞా(idx < q->n_వస్తువులు);

        అయితే (idx > 0) {
                గుర్తులేని k;

                k = (idx-1)/2;

                ఒకవేళ (q->compare_func(q->వస్తువులు[k].దత్తాంశం, q->వస్తువులు[idx].దత్తాంశం) <= 0)
                        ఆపు;

                మార్పిడి(q, idx, k);
                idx = k;
        }

        తిరిగి idx;
}

స్థిర గుర్తులేని shuffle_కిందికి(Prioq *q, గుర్తులేని idx) {
        ప్రతిజ్ఞా(q);

        కొరకు (;;) {
                గుర్తులేని j, k, s;

                k = (idx+1)*2; /* right child */
                j = k-1;       /* left child */

                ఒకవేళ (j >= q->n_వస్తువులు)
                        ఆపు;

                ఒకవేళ (q->compare_func(q->వస్తువులు[j].దత్తాంశం, q->వస్తువులు[idx].దత్తాంశం) < 0)

                        /* So our left child is smaller than we are, let's
                         * remember this fact */
                        s = j;
                లేకపోతే
                        s = idx;

                ఒకవేళ (k < q->n_వస్తువులు &&
                    q->compare_func(q->వస్తువులు[k].దత్తాంశం, q->వస్తువులు[s].దత్తాంశం) < 0)

                        /* So our right child is smaller than we are, let's
                         * remember this fact */
                        s = k;

                /* s now points to the smallest of the three items */

                ఒకవేళ (s == idx)
                        /* No swap necessary, we're done */
                        ఆపు;

                మార్పిడి(q, idx, s);
                idx = s;
        }

        తిరిగి idx;
}

పూర్ణాంకం prioq_put(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx) {
        గుర్తులేని k;

        ప్రతిజ్ఞా(q);

        ఒకవేళ (!GREEDY_REALLOC(q->వస్తువులు, MAX(q->n_వస్తువులు + 1, 16u)))
                తిరిగి -ENOMEM;

        k = q->n_వస్తువులు++;
        q->వస్తువులు[k] = (నిర్మాణం prioq_అంశం) {
                .దత్తాంశం = దత్తాంశం,
                .idx = idx,
        };

        ఒకవేళ (idx)
                *idx = k;

        shuffle_పైకి(q, k);

        తిరిగి 0;
}

పూర్ణాంకం _prioq_ensure_put(Prioq **q, compare_func_t compare_func, శూన్య *దత్తాంశం, గుర్తులేని *idx) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(q);

        r = prioq_ensure_allocated(q, compare_func);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి prioq_put(*q, దత్తాంశం, idx);
}

స్థిర శూన్య remove_అంశం(Prioq *q, నిర్మాణం prioq_అంశం *i) {
        నిర్మాణం prioq_అంశం *l;

        ప్రతిజ్ఞా(q);
        ప్రతిజ్ఞా(i);

        /* Let's invalidate the index pointer stored in the user's object to indicate the item is now removed
         * from the priority queue */
        ఒకవేళ (i->idx)
                *(i->idx) = PRIOQ_IDX_NULL;

        l = q->వస్తువులు + q->n_వస్తువులు - 1;

        ఒకవేళ (i == l)
                /* Last entry, let's just remove it */
                q->n_వస్తువులు--;
        లేకపోతే {
                గుర్తులేని k;

                /* Not last entry, let's replace the last entry with
                 * this one, and reshuffle */

                ప్రతిజ్ఞా(i >= q->వస్తువులు);
                k = i - q->వస్తువులు;

                i->దత్తాంశం = l->దత్తాంశం;
                i->idx = l->idx;
                ఒకవేళ (i->idx)
                        *i->idx = k;
                q->n_వస్తువులు--;

                k = shuffle_కిందికి(q, k);
                shuffle_పైకి(q, k);
        }
}

స్థిర నిర్మాణం prioq_అంశం* కనుగొను_అంశం(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx) {
        నిర్మాణం prioq_అంశం *i;

        ప్రతిజ్ఞా(q);

        ఒకవేళ (q->n_వస్తువులు <= 0)
                తిరిగి ఖాళీ;

        ఒకవేళ (idx) {
                ఒకవేళ (*idx == PRIOQ_IDX_NULL ||
                    *idx >= q->n_వస్తువులు)
                        తిరిగి ఖాళీ;

                i = q->వస్తువులు + *idx;
                ఒకవేళ (i->దత్తాంశం != దత్తాంశం)
                        తిరిగి ఖాళీ;

                తిరిగి i;
        } లేకపోతే {
                కొరకు (i = q->వస్తువులు; i < q->వస్తువులు + q->n_వస్తువులు; i++)
                        ఒకవేళ (i->దత్తాంశం == దత్తాంశం)
                                తిరిగి i;
                తిరిగి ఖాళీ;
        }
}

పూర్ణాంకం prioq_remove(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx) {
        నిర్మాణం prioq_అంశం *i;

        ఒకవేళ (!q)
                తిరిగి 0;

        i = కనుగొను_అంశం(q, దత్తాంశం, idx);
        ఒకవేళ (!i)
                తిరిగి 0;

        remove_అంశం(q, i);
        తిరిగి 1;
}

శూన్య prioq_reshuffle(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx) {
        నిర్మాణం prioq_అంశం *i;
        గుర్తులేని k;

        ప్రతిజ్ఞా(q);

        i = కనుగొను_అంశం(q, దత్తాంశం, idx);
        ఒకవేళ (!i)
                తిరిగి;

        ప్రతిజ్ఞా(i >= q->వస్తువులు);
        k = i - q->వస్తువులు;
        k = shuffle_కిందికి(q, k);
        shuffle_పైకి(q, k);
}

శూన్య* prioq_peek_by_సూచీ(Prioq *q, గుర్తులేని idx) {
        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        ఒకవేళ (idx >= q->n_వస్తువులు)
                తిరిగి ఖాళీ;

        తిరిగి q->వస్తువులు[idx].దత్తాంశం;
}

శూన్య* prioq_తీయి(Prioq *q) {
        శూన్య *దత్తాంశం;

        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        ఒకవేళ (q->n_వస్తువులు <= 0)
                తిరిగి ఖాళీ;

        దత్తాంశం = q->వస్తువులు[0].దత్తాంశం;
        remove_అంశం(q, q->వస్తువులు);
        తిరిగి దత్తాంశం;
}

గుర్తులేని prioq_కొలత(Prioq *q) {

        ఒకవేళ (!q)
                తిరిగి 0;

        తిరిగి q->n_వస్తువులు;
}

bool prioq_isempty(Prioq *q) {

        ఒకవేళ (!q)
                తిరిగి true;

        తిరిగి q->n_వస్తువులు <= 0;
}
