/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>

#شامل "format-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "mempool.h"

ساختار pool {
        ساختار pool *next;
        size_t n_tiles;
        size_t n_used;
};

ایستا تهی* pool_ptr(ساختار pool *p) {
        بازگشت ((uint8_t*) ASSERT_PTR(p)) + ALIGN(اندازه(ساختار pool));
}

تهی* mempool_alloc_tile(ساختار mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        أكد(mp);
        أكد(mp->tile_size >= اندازه(تهی*));
        أكد(mp->at_least > 0);

        اگر (mp->freelist) {
                تهی *t;

                t = mp->freelist;
                mp->freelist = *(تهی**) mp->freelist;
                بازگشت t;
        }

        اگر (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                ساختار pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(اندازه(ساختار pool)) + n*mp->tile_size);
                n = (size - ALIGN(اندازه(ساختار pool))) / mp->tile_size;

                p = malloc(size);
                اگر (!p)
                        بازگشت پوچ;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        بازگشت (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

تهی* mempool_alloc0_tile(ساختار mempool *mp) {
        تهی *p;

        أكد(mp);

        p = mempool_alloc_tile(mp);
        اگر (p)
                memzero(p, mp->tile_size);
        بازگشت p;
}

تهی* mempool_free_tile(ساختار mempool *mp, تهی *p) {
        أكد(mp);

        اگر (!p)
                بازگشت پوچ;

        *(تهی**) p = mp->freelist;
        mp->freelist = p;

        بازگشت پوچ;
}

ایستا bool pool_contains(ساختار mempool *mp, ساختار pool *p, تهی *ptr) {
        size_t off;
        تهی *a;

        أكد(mp);
        أكد(p);

        اگر (!ptr)
                بازگشت false;

        a = pool_ptr(p);
        اگر ((uint8_t*) ptr < (uint8_t*) a)
                بازگشت false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        اگر (off >= mp->tile_size * p->n_tiles)
                بازگشت false;

        أكد(off % mp->tile_size == 0);
        بازگشت true;
}

ایستا bool pool_is_unused(ساختار mempool *mp, ساختار pool *p) {
        أكد(mp);
        أكد(p);

        اگر (p->n_used == 0)
                بازگشت true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        تهی *i = mp->freelist;
        وقتی (i) {
                اگر (pool_contains(mp, p, i))
                        n++;

                i = *(تهی**) i;
        }

        أكد(n <= p->n_used);

        بازگشت n == p->n_used;
}

ایستا تهی pool_unlink(ساختار mempool *mp, ساختار pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        اگر (p->n_used == 0)
                بازگشت;

        تهی **i = &mp->freelist;
        وقتی (*i) {
                تهی *d = *i;

                اگر (pool_contains(mp, p, d)) {
                        *i = *(تهی**) d;
                        m++;

                        اگر (m == p->n_used)
                                بشکن;
                } وگرنه
                        i = (تهی**) d;
        }
}

تهی mempool_trim(ساختار mempool *mp) {
        size_t trimmed = 0, چپ = 0;

        أكد(mp);

        ساختار pool **p = &mp->first_pool;
        وقتی (*p) {
                ساختار pool *d = *p;

                اگر (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        آزاد(d);
                } وگرنه {
                        چپ += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(چپ));
}
