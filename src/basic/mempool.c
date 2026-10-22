/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "mempool.h"

ರಚನೆ pool {
        ರಚನೆ pool *ಮುಂದಿನ_2;
        size_t n_tiles;
        size_t n_ಬಳಸಿದ;
};

ಸ್ಥಿರ ಶೂನ್ಯ* pool_ptr(ರಚನೆ pool *p) {
        ಹಿಂತಿರುಗಿ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(ಗಾತ್ರ(ರಚನೆ pool));
}

ಶೂನ್ಯ* mempool_alloc_tile(ರಚನೆ mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(mp->tile_ಗಾತ್ರ >= ಗಾತ್ರ(ಶೂನ್ಯ*));
        ಪ್ರತಿಜ್ಞಾ(mp->at_ಕನಿಷ್ಠ > 0);

        ಆದರೆ (mp->freelist) {
                ಶೂನ್ಯ *t;

                t = mp->freelist;
                mp->freelist = *(ಶೂನ್ಯ**) mp->freelist;
                ಹಿಂತಿರುಗಿ t;
        }

        ಆದರೆ (_unlikely_(!mp->ಮೊದಲ_pool) ||
            _unlikely_(mp->ಮೊದಲ_pool->n_ಬಳಸಿದ >= mp->ಮೊದಲ_pool->n_tiles)) {
                size_t ಗಾತ್ರ_2, n;
                ರಚನೆ pool *p;

                n = mp->ಮೊದಲ_pool ? mp->ಮೊದಲ_pool->n_tiles : 0;
                n = MAX(mp->at_ಕನಿಷ್ಠ, n * 2);
                ಗಾತ್ರ_2 = ಪುಟ_ALIGN(ALIGN(ಗಾತ್ರ(ರಚನೆ pool)) + n*mp->tile_ಗಾತ್ರ);
                n = (ಗಾತ್ರ_2 - ALIGN(ಗಾತ್ರ(ರಚನೆ pool))) / mp->tile_ಗಾತ್ರ;

                p = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                p->ಮುಂದಿನ_2 = mp->ಮೊದಲ_pool;
                p->n_tiles = n;
                p->n_ಬಳಸಿದ = 0;

                mp->ಮೊದಲ_pool = p;
        }

        i = mp->ಮೊದಲ_pool->n_ಬಳಸಿದ++;

        ಹಿಂತಿರುಗಿ (uint8_t*) pool_ptr(mp->ಮೊದಲ_pool) + i*mp->tile_ಗಾತ್ರ;
}

ಶೂನ್ಯ* mempool_alloc0_tile(ರಚನೆ mempool *mp) {
        ಶೂನ್ಯ *p;

        ಪ್ರತಿಜ್ಞಾ(mp);

        p = mempool_alloc_tile(mp);
        ಆದರೆ (p)
                memzero(p, mp->tile_ಗಾತ್ರ);
        ಹಿಂತಿರುಗಿ p;
}

ಶೂನ್ಯ* mempool_free_tile(ರಚನೆ mempool *mp, ಶೂನ್ಯ *p) {
        ಪ್ರತಿಜ್ಞಾ(mp);

        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *(ಶೂನ್ಯ**) p = mp->freelist;
        mp->freelist = p;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ bool pool_ಒಳಗೊಂಡಿದೆ(ರಚನೆ mempool *mp, ರಚನೆ pool *p, ಶೂನ್ಯ *ptr) {
        size_t off;
        ಶೂನ್ಯ *a;

        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (!ptr)
                ಹಿಂತಿರುಗಿ false;

        a = pool_ptr(p);
        ಆದರೆ ((uint8_t*) ptr < (uint8_t*) a)
                ಹಿಂತಿರುಗಿ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ಆದರೆ (off >= mp->tile_ಗಾತ್ರ * p->n_tiles)
                ಹಿಂತಿರುಗಿ false;

        ಪ್ರತಿಜ್ಞಾ(off % mp->tile_ಗಾತ್ರ == 0);
        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool pool_is_ಬಳಸದ(ರಚನೆ mempool *mp, ರಚನೆ pool *p) {
        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (p->n_ಬಳಸಿದ == 0)
                ಹಿಂತಿರುಗಿ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        ಶೂನ್ಯ *i = mp->freelist;
        ಆಗ (i) {
                ಆದರೆ (pool_ಒಳಗೊಂಡಿದೆ(mp, p, i))
                        n++;

                i = *(ಶೂನ್ಯ**) i;
        }

        ಪ್ರತಿಜ್ಞಾ(n <= p->n_ಬಳಸಿದ);

        ಹಿಂತಿರುಗಿ n == p->n_ಬಳಸಿದ;
}

ಸ್ಥಿರ ಶೂನ್ಯ pool_unlink(ರಚನೆ mempool *mp, ರಚನೆ pool *p) {
        size_t m = 0;

        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (p->n_ಬಳಸಿದ == 0)
                ಹಿಂತಿರುಗಿ;

        ಶೂನ್ಯ **i = &mp->freelist;
        ಆಗ (*i) {
                ಶೂನ್ಯ *d = *i;

                ಆದರೆ (pool_ಒಳಗೊಂಡಿದೆ(mp, p, d)) {
                        *i = *(ಶೂನ್ಯ**) d;
                        m++;

                        ಆದರೆ (m == p->n_ಬಳಸಿದ)
                                ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ
                        i = (ಶೂನ್ಯ**) d;
        }
}

ಶೂನ್ಯ mempool_trim(ರಚನೆ mempool *mp) {
        size_t trimmed = 0, ಎಡ = 0;

        ಪ್ರತಿಜ್ಞಾ(mp);

        ರಚನೆ pool **p = &mp->ಮೊದಲ_pool;
        ಆಗ (*p) {
                ರಚನೆ pool *d = *p;

                ಆದರೆ (pool_is_ಬಳಸದ(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_ಗಾತ್ರ;
                        pool_unlink(mp, d);
                        *p = d->ಮುಂದಿನ_2;
                        ಬಿಡುಗಡೆ(d);
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಎಡ += d->n_tiles * mp->tile_ಗಾತ್ರ;
                        p = &d->ಮುಂದಿನ_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_ಬೈಟುಗಳು(trimmed), mp, FORMAT_ಬೈಟುಗಳು(ಎಡ));
}
