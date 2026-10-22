/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>

#include "format-util.h"
#include "log.h"
#include "memory-util.h"
#include "mempool.h"

struct pool {
        struct pool *next;
        size_t n_tiles;
        size_t n_used;
};

static void* pool_ptr(struct pool *p) {
        trả_về ((uint8_t*) ASSERT_PTR(p)) + ALIGN(sizeof(struct pool));
}

void* mempool_alloc_tile(struct mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        assert(mp);
        assert(mp->tile_size >= sizeof(void*));
        assert(mp->at_least > 0);

        nếu (mp->freelist) {
                void *t;

                t = mp->freelist;
                mp->freelist = *(void**) mp->freelist;
                trả_về t;
        }

        nếu (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                struct pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(sizeof(struct pool)) + n*mp->tile_size);
                n = (size - ALIGN(sizeof(struct pool))) / mp->tile_size;

                p = malloc(size);
                nếu (!p)
                        trả_về NULL;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        trả_về (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

void* mempool_alloc0_tile(struct mempool *mp) {
        void *p;

        assert(mp);

        p = mempool_alloc_tile(mp);
        nếu (p)
                memzero(p, mp->tile_size);
        trả_về p;
}

void* mempool_free_tile(struct mempool *mp, void *p) {
        assert(mp);

        nếu (!p)
                trả_về NULL;

        *(void**) p = mp->freelist;
        mp->freelist = p;

        trả_về NULL;
}

static bool pool_contains(struct mempool *mp, struct pool *p, void *ptr) {
        size_t off;
        void *a;

        assert(mp);
        assert(p);

        nếu (!ptr)
                trả_về false;

        a = pool_ptr(p);
        nếu ((uint8_t*) ptr < (uint8_t*) a)
                trả_về false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        nếu (off >= mp->tile_size * p->n_tiles)
                trả_về false;

        assert(off % mp->tile_size == 0);
        trả_về true;
}

static bool pool_is_unused(struct mempool *mp, struct pool *p) {
        assert(mp);
        assert(p);

        nếu (p->n_used == 0)
                trả_về true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        void *i = mp->freelist;
        trong_khi (i) {
                nếu (pool_contains(mp, p, i))
                        n++;

                i = *(void**) i;
        }

        assert(n <= p->n_used);

        trả_về n == p->n_used;
}

static void pool_unlink(struct mempool *mp, struct pool *p) {
        size_t m = 0;

        assert(mp);
        assert(p);

        nếu (p->n_used == 0)
                trả_về;

        void **i = &mp->freelist;
        trong_khi (*i) {
                void *d = *i;

                nếu (pool_contains(mp, p, d)) {
                        *i = *(void**) d;
                        m++;

                        nếu (m == p->n_used)
                                break;
                } không_thì
                        i = (void**) d;
        }
}

void mempool_trim(struct mempool *mp) {
        size_t trimmed = 0, left = 0;

        assert(mp);

        struct pool **p = &mp->first_pool;
        trong_khi (*p) {
                struct pool *d = *p;

                nếu (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        free(d);
                } không_thì {
                        left += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(left));
}
