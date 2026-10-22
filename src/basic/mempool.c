/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "mempool.h"

بنية pool {
        بنية pool *next;
        size_t n_tiles;
        size_t n_used;
};

ساكن فراغ* pool_ptr(بنية pool *p) {
        ܦܢܐ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(حجم(بنية pool));
}

فراغ* mempool_alloc_tile(بنية mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        أكد(mp);
        أكد(mp->tile_size >= حجم(فراغ*));
        أكد(mp->at_least > 0);

        ܐܢ (mp->freelist) {
                فراغ *t;

                t = mp->freelist;
                mp->freelist = *(فراغ**) mp->freelist;
                ܦܢܐ t;
        }

        ܐܢ (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                بنية pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(حجم(بنية pool)) + n*mp->tile_size);
                n = (size - ALIGN(حجم(بنية pool))) / mp->tile_size;

                p = malloc(size);
                ܐܢ (!p)
                        ܦܢܐ عدم;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        ܦܢܐ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

فراغ* mempool_alloc0_tile(بنية mempool *mp) {
        فراغ *p;

        أكد(mp);

        p = mempool_alloc_tile(mp);
        ܐܢ (p)
                memzero(p, mp->tile_size);
        ܦܢܐ p;
}

فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p) {
        أكد(mp);

        ܐܢ (!p)
                ܦܢܐ عدم;

        *(فراغ**) p = mp->freelist;
        mp->freelist = p;

        ܦܢܐ عدم;
}

ساكن bool pool_contains(بنية mempool *mp, بنية pool *p, فراغ *ptr) {
        size_t off;
        فراغ *a;

        أكد(mp);
        أكد(p);

        ܐܢ (!ptr)
                ܦܢܐ false;

        a = pool_ptr(p);
        ܐܢ ((uint8_t*) ptr < (uint8_t*) a)
                ܦܢܐ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ܐܢ (off >= mp->tile_size * p->n_tiles)
                ܦܢܐ false;

        أكد(off % mp->tile_size == 0);
        ܦܢܐ true;
}

ساكن bool pool_is_unused(بنية mempool *mp, بنية pool *p) {
        أكد(mp);
        أكد(p);

        ܐܢ (p->n_used == 0)
                ܦܢܐ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        فراغ *i = mp->freelist;
        ܟܡܐ (i) {
                ܐܢ (pool_contains(mp, p, i))
                        n++;

                i = *(فراغ**) i;
        }

        أكد(n <= p->n_used);

        ܦܢܐ n == p->n_used;
}

ساكن فراغ pool_unlink(بنية mempool *mp, بنية pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        ܐܢ (p->n_used == 0)
                ܦܢܐ;

        فراغ **i = &mp->freelist;
        ܟܡܐ (*i) {
                فراغ *d = *i;

                ܐܢ (pool_contains(mp, p, d)) {
                        *i = *(فراغ**) d;
                        m++;

                        ܐܢ (m == p->n_used)
                                اكسر;
                } ܐܚܪܢܐ
                        i = (فراغ**) d;
        }
}

فراغ mempool_trim(بنية mempool *mp) {
        size_t trimmed = 0, left = 0;

        أكد(mp);

        بنية pool **p = &mp->first_pool;
        ܟܡܐ (*p) {
                بنية pool *d = *p;

                ܐܢ (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        حرر(d);
                } ܐܚܪܢܐ {
                        left += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(left));
}
