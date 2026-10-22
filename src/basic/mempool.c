/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "mempool.h"

ಸಂರಚನಾ pool {
        ಸಂರಚನಾ pool *next;
        size_t n_tiles;
        size_t n_used;
};

ಸ್ಥಿರ ಶೂನ್ಯ* pool_ptr(ಸಂರಚನಾ pool *p) {
        ಪ್ರತ್ಯಾಗಮ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ pool));
}

ಶೂನ್ಯ* mempool_alloc_tile(ಸಂರಚನಾ mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(mp->tile_size >= ಪರಿಮಾಣ(ಶೂನ್ಯ*));
        ಪ್ರತಿಜ್ಞಾ(mp->at_least > 0);

        ಯದಿ (mp->freelist) {
                ಶೂನ್ಯ *t;

                t = mp->freelist;
                mp->freelist = *(ಶೂನ್ಯ**) mp->freelist;
                ಪ್ರತ್ಯಾಗಮ t;
        }

        ಯದಿ (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                ಸಂರಚನಾ pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ pool)) + n*mp->tile_size);
                n = (size - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ pool))) / mp->tile_size;

                p = malloc(size);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        ಪ್ರತ್ಯಾಗಮ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

ಶೂನ್ಯ* mempool_alloc0_tile(ಸಂರಚನಾ mempool *mp) {
        ಶೂನ್ಯ *p;

        ಪ್ರತಿಜ್ಞಾ(mp);

        p = mempool_alloc_tile(mp);
        ಯದಿ (p)
                memzero(p, mp->tile_size);
        ಪ್ರತ್ಯಾಗಮ p;
}

ಶೂನ್ಯ* mempool_free_tile(ಸಂರಚನಾ mempool *mp, ಶೂನ್ಯ *p) {
        ಪ್ರತಿಜ್ಞಾ(mp);

        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *(ಶೂನ್ಯ**) p = mp->freelist;
        mp->freelist = p;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ bool pool_contains(ಸಂರಚನಾ mempool *mp, ಸಂರಚನಾ pool *p, ಶೂನ್ಯ *ptr) {
        size_t off;
        ಶೂನ್ಯ *a;

        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (!ptr)
                ಪ್ರತ್ಯಾಗಮ false;

        a = pool_ptr(p);
        ಯದಿ ((uint8_t*) ptr < (uint8_t*) a)
                ಪ್ರತ್ಯಾಗಮ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ಯದಿ (off >= mp->tile_size * p->n_tiles)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿಜ್ಞಾ(off % mp->tile_size == 0);
        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool pool_is_unused(ಸಂರಚನಾ mempool *mp, ಸಂರಚನಾ pool *p) {
        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (p->n_used == 0)
                ಪ್ರತ್ಯಾಗಮ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        ಶೂನ್ಯ *i = mp->freelist;
        ಯಾವತ್ (i) {
                ಯದಿ (pool_contains(mp, p, i))
                        n++;

                i = *(ಶೂನ್ಯ**) i;
        }

        ಪ್ರತಿಜ್ಞಾ(n <= p->n_used);

        ಪ್ರತ್ಯಾಗಮ n == p->n_used;
}

ಸ್ಥಿರ ಶೂನ್ಯ pool_unlink(ಸಂರಚನಾ mempool *mp, ಸಂರಚನಾ pool *p) {
        size_t m = 0;

        ಪ್ರತಿಜ್ಞಾ(mp);
        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (p->n_used == 0)
                ಪ್ರತ್ಯಾಗಮ;

        ಶೂನ್ಯ **i = &mp->freelist;
        ಯಾವತ್ (*i) {
                ಶೂನ್ಯ *d = *i;

                ಯದಿ (pool_contains(mp, p, d)) {
                        *i = *(ಶೂನ್ಯ**) d;
                        m++;

                        ಯದಿ (m == p->n_used)
                                ಭಙ್ಗ;
                } ಅನ್ಯಥಾ
                        i = (ಶೂನ್ಯ**) d;
        }
}

ಶೂನ್ಯ mempool_trim(ಸಂರಚನಾ mempool *mp) {
        size_t trimmed = 0, ಎಡ = 0;

        ಪ್ರತಿಜ್ಞಾ(mp);

        ಸಂರಚನಾ pool **p = &mp->first_pool;
        ಯಾವತ್ (*p) {
                ಸಂರಚನಾ pool *d = *p;

                ಯದಿ (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        ಮುಕ್ತ(d);
                } ಅನ್ಯಥಾ {
                        ಎಡ += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(ಎಡ));
}
