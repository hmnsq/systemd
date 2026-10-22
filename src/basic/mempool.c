/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "format-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "mempool.h"

నిర్మాణం pool {
        నిర్మాణం pool *తదుపరి_2;
        size_t n_tiles;
        size_t n_వాడిన;
};

స్థిర శూన్య* pool_ptr(నిర్మాణం pool *p) {
        తిరిగి ((uint8_t*) ASSERT_PTR(p)) + ALIGN(పరిమాణం(నిర్మాణం pool));
}

శూన్య* mempool_alloc_tile(నిర్మాణం mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        ప్రతిజ్ఞా(mp);
        ప్రతిజ్ఞా(mp->tile_కొలత >= పరిమాణం(శూన్య*));
        ప్రతిజ్ఞా(mp->at_కనిష్ఠ > 0);

        ఒకవేళ (mp->freelist) {
                శూన్య *t;

                t = mp->freelist;
                mp->freelist = *(శూన్య**) mp->freelist;
                తిరిగి t;
        }

        ఒకవేళ (_unlikely_(!mp->మొదటి_pool) ||
            _unlikely_(mp->మొదటి_pool->n_వాడిన >= mp->మొదటి_pool->n_tiles)) {
                size_t కొలత, n;
                నిర్మాణం pool *p;

                n = mp->మొదటి_pool ? mp->మొదటి_pool->n_tiles : 0;
                n = MAX(mp->at_కనిష్ఠ, n * 2);
                కొలత = పుట_ALIGN(ALIGN(పరిమాణం(నిర్మాణం pool)) + n*mp->tile_కొలత);
                n = (కొలత - ALIGN(పరిమాణం(నిర్మాణం pool))) / mp->tile_కొలత;

                p = జ్ఞాపకం_ఇవ్వు(కొలత);
                ఒకవేళ (!p)
                        తిరిగి ఖాళీ;

                p->తదుపరి_2 = mp->మొదటి_pool;
                p->n_tiles = n;
                p->n_వాడిన = 0;

                mp->మొదటి_pool = p;
        }

        i = mp->మొదటి_pool->n_వాడిన++;

        తిరిగి (uint8_t*) pool_ptr(mp->మొదటి_pool) + i*mp->tile_కొలత;
}

శూన్య* mempool_alloc0_tile(నిర్మాణం mempool *mp) {
        శూన్య *p;

        ప్రతిజ్ఞా(mp);

        p = mempool_alloc_tile(mp);
        ఒకవేళ (p)
                memzero(p, mp->tile_కొలత);
        తిరిగి p;
}

శూన్య* mempool_free_tile(నిర్మాణం mempool *mp, శూన్య *p) {
        ప్రతిజ్ఞా(mp);

        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        *(శూన్య**) p = mp->freelist;
        mp->freelist = p;

        తిరిగి ఖాళీ;
}

స్థిర bool pool_కలిగియుంది(నిర్మాణం mempool *mp, నిర్మాణం pool *p, శూన్య *ptr) {
        size_t off;
        శూన్య *a;

        ప్రతిజ్ఞా(mp);
        ప్రతిజ్ఞా(p);

        ఒకవేళ (!ptr)
                తిరిగి false;

        a = pool_ptr(p);
        ఒకవేళ ((uint8_t*) ptr < (uint8_t*) a)
                తిరిగి false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ఒకవేళ (off >= mp->tile_కొలత * p->n_tiles)
                తిరిగి false;

        ప్రతిజ్ఞా(off % mp->tile_కొలత == 0);
        తిరిగి true;
}

స్థిర bool pool_is_వాడని(నిర్మాణం mempool *mp, నిర్మాణం pool *p) {
        ప్రతిజ్ఞా(mp);
        ప్రతిజ్ఞా(p);

        ఒకవేళ (p->n_వాడిన == 0)
                తిరిగి true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        శూన్య *i = mp->freelist;
        అయితే (i) {
                ఒకవేళ (pool_కలిగియుంది(mp, p, i))
                        n++;

                i = *(శూన్య**) i;
        }

        ప్రతిజ్ఞా(n <= p->n_వాడిన);

        తిరిగి n == p->n_వాడిన;
}

స్థిర శూన్య pool_unlink(నిర్మాణం mempool *mp, నిర్మాణం pool *p) {
        size_t m = 0;

        ప్రతిజ్ఞా(mp);
        ప్రతిజ్ఞా(p);

        ఒకవేళ (p->n_వాడిన == 0)
                తిరిగి;

        శూన్య **i = &mp->freelist;
        అయితే (*i) {
                శూన్య *d = *i;

                ఒకవేళ (pool_కలిగియుంది(mp, p, d)) {
                        *i = *(శూన్య**) d;
                        m++;

                        ఒకవేళ (m == p->n_వాడిన)
                                ఆపు;
                } లేకపోతే
                        i = (శూన్య**) d;
        }
}

శూన్య mempool_trim(నిర్మాణం mempool *mp) {
        size_t trimmed = 0, ఎడమ = 0;

        ప్రతిజ్ఞా(mp);

        నిర్మాణం pool **p = &mp->మొదటి_pool;
        అయితే (*p) {
                నిర్మాణం pool *d = *p;

                ఒకవేళ (pool_is_వాడని(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_కొలత;
                        pool_unlink(mp, d);
                        *p = d->తదుపరి_2;
                        విడుదల(d);
                } లేకపోతే {
                        ఎడమ += d->n_tiles * mp->tile_కొలత;
                        p = &d->తదుపరి_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_బైట్లు(trimmed), mp, FORMAT_బైట్లు(ఎడమ));
}
