/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mempool.h"

ਬਣਤਰ pool {
        ਬਣਤਰ pool *ਅਗਲਾ_2;
        size_t n_tiles;
        size_t n_ਵਰਤਿਆ;
};

ਸਥਿਰ ਖਾਲੀ* pool_ptr(ਬਣਤਰ pool *p) {
        ਵਾਪਸ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(ਆਕਾਰ(ਬਣਤਰ pool));
}

ਖਾਲੀ* mempool_alloc_tile(ਬਣਤਰ mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        ਪ੍ਰਤਿਜ੍ਞਾ(mp);
        ਪ੍ਰਤਿਜ੍ਞਾ(mp->tile_ਮਾਪ >= ਆਕਾਰ(ਖਾਲੀ*));
        ਪ੍ਰਤਿਜ੍ਞਾ(mp->at_ਘੱਟ_ਤੋਂ_ਘੱਟ > 0);

        ਜੇ (mp->freelist) {
                ਖਾਲੀ *t;

                t = mp->freelist;
                mp->freelist = *(ਖਾਲੀ**) mp->freelist;
                ਵਾਪਸ t;
        }

        ਜੇ (_unlikely_(!mp->ਪਹਿਲਾ_pool) ||
            _unlikely_(mp->ਪਹਿਲਾ_pool->n_ਵਰਤਿਆ >= mp->ਪਹਿਲਾ_pool->n_tiles)) {
                size_t ਮਾਪ, n;
                ਬਣਤਰ pool *p;

                n = mp->ਪਹਿਲਾ_pool ? mp->ਪਹਿਲਾ_pool->n_tiles : 0;
                n = MAX(mp->at_ਘੱਟ_ਤੋਂ_ਘੱਟ, n * 2);
                ਮਾਪ = ਪੰਨਾ_ALIGN(ALIGN(ਆਕਾਰ(ਬਣਤਰ pool)) + n*mp->tile_ਮਾਪ);
                n = (ਮਾਪ - ALIGN(ਆਕਾਰ(ਬਣਤਰ pool))) / mp->tile_ਮਾਪ;

                p = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ);
                ਜੇ (!p)
                        ਵਾਪਸ ਨਿਰਾ;

                p->ਅਗਲਾ_2 = mp->ਪਹਿਲਾ_pool;
                p->n_tiles = n;
                p->n_ਵਰਤਿਆ = 0;

                mp->ਪਹਿਲਾ_pool = p;
        }

        i = mp->ਪਹਿਲਾ_pool->n_ਵਰਤਿਆ++;

        ਵਾਪਸ (uint8_t*) pool_ptr(mp->ਪਹਿਲਾ_pool) + i*mp->tile_ਮਾਪ;
}

ਖਾਲੀ* mempool_alloc0_tile(ਬਣਤਰ mempool *mp) {
        ਖਾਲੀ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(mp);

        p = mempool_alloc_tile(mp);
        ਜੇ (p)
                memzero(p, mp->tile_ਮਾਪ);
        ਵਾਪਸ p;
}

ਖਾਲੀ* mempool_free_tile(ਬਣਤਰ mempool *mp, ਖਾਲੀ *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(mp);

        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        *(ਖਾਲੀ**) p = mp->freelist;
        mp->freelist = p;

        ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ bool pool_ਸ਼ਾਮਲ_ਹੈ(ਬਣਤਰ mempool *mp, ਬਣਤਰ pool *p, ਖਾਲੀ *ptr) {
        size_t off;
        ਖਾਲੀ *a;

        ਪ੍ਰਤਿਜ੍ਞਾ(mp);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (!ptr)
                ਵਾਪਸ false;

        a = pool_ptr(p);
        ਜੇ ((uint8_t*) ptr < (uint8_t*) a)
                ਵਾਪਸ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ਜੇ (off >= mp->tile_ਮਾਪ * p->n_tiles)
                ਵਾਪਸ false;

        ਪ੍ਰਤਿਜ੍ਞਾ(off % mp->tile_ਮਾਪ == 0);
        ਵਾਪਸ true;
}

ਸਥਿਰ bool pool_is_ਅਣਵਰਤਿਆ(ਬਣਤਰ mempool *mp, ਬਣਤਰ pool *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(mp);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (p->n_ਵਰਤਿਆ == 0)
                ਵਾਪਸ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        ਖਾਲੀ *i = mp->freelist;
        ਜਦੋਂ (i) {
                ਜੇ (pool_ਸ਼ਾਮਲ_ਹੈ(mp, p, i))
                        n++;

                i = *(ਖਾਲੀ**) i;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(n <= p->n_ਵਰਤਿਆ);

        ਵਾਪਸ n == p->n_ਵਰਤਿਆ;
}

ਸਥਿਰ ਖਾਲੀ pool_unlink(ਬਣਤਰ mempool *mp, ਬਣਤਰ pool *p) {
        size_t m = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(mp);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (p->n_ਵਰਤਿਆ == 0)
                ਵਾਪਸ;

        ਖਾਲੀ **i = &mp->freelist;
        ਜਦੋਂ (*i) {
                ਖਾਲੀ *d = *i;

                ਜੇ (pool_ਸ਼ਾਮਲ_ਹੈ(mp, p, d)) {
                        *i = *(ਖਾਲੀ**) d;
                        m++;

                        ਜੇ (m == p->n_ਵਰਤਿਆ)
                                ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ
                        i = (ਖਾਲੀ**) d;
        }
}

ਖਾਲੀ mempool_trim(ਬਣਤਰ mempool *mp) {
        size_t trimmed = 0, ਖੱਬਾ = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(mp);

        ਬਣਤਰ pool **p = &mp->ਪਹਿਲਾ_pool;
        ਜਦੋਂ (*p) {
                ਬਣਤਰ pool *d = *p;

                ਜੇ (pool_is_ਅਣਵਰਤਿਆ(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_ਮਾਪ;
                        pool_unlink(mp, d);
                        *p = d->ਅਗਲਾ_2;
                        ਮੁਕਤ(d);
                } ਨਹੀਂਤਾਂ {
                        ਖੱਬਾ += d->n_tiles * mp->tile_ਮਾਪ;
                        p = &d->ਅਗਲਾ_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_ਬਾਈਟਾਂ(trimmed), mp, FORMAT_ਬਾਈਟਾਂ(ਖੱਬਾ));
}
