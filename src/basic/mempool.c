/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>

#include "format-util.h"
#include "log.h"
#include "memory-util.h"
#include "mempool.h"

sa_nracanaa pool {
        sa_nracanaa pool *next;
        size_t n_tiles;
        size_t n_used;
};

static void* pool_ptr(sa_nracanaa pool *p) {
        ᤀᤡᤴ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(parimaa_na(sa_nracanaa pool));
}

void* mempool_alloc_tile(sa_nracanaa mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        assert(mp);
        assert(mp->tile_size >= parimaa_na(void*));
        assert(mp->at_least > 0);

        ᤀᤡ᤺ (mp->freelist) {
                void *t;

                t = mp->freelist;
                mp->freelist = *(void**) mp->freelist;
                ᤀᤡᤴ t;
        }

        ᤀᤡ᤺ (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                sa_nracanaa pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(parimaa_na(sa_nracanaa pool)) + n*mp->tile_size);
                n = (size - ALIGN(parimaa_na(sa_nracanaa pool))) / mp->tile_size;

                p = malloc(size);
                ᤀᤡ᤺ (!p)
                        ᤀᤡᤴ NULL;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        ᤀᤡᤴ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

void* mempool_alloc0_tile(sa_nracanaa mempool *mp) {
        void *p;

        assert(mp);

        p = mempool_alloc_tile(mp);
        ᤀᤡ᤺ (p)
                memzero(p, mp->tile_size);
        ᤀᤡᤴ p;
}

void* mempool_free_tile(sa_nracanaa mempool *mp, void *p) {
        assert(mp);

        ᤀᤡ᤺ (!p)
                ᤀᤡᤴ NULL;

        *(void**) p = mp->freelist;
        mp->freelist = p;

        ᤀᤡᤴ NULL;
}

static bool pool_contains(sa_nracanaa mempool *mp, sa_nracanaa pool *p, void *ptr) {
        size_t off;
        void *a;

        assert(mp);
        assert(p);

        ᤀᤡ᤺ (!ptr)
                ᤀᤡᤴ false;

        a = pool_ptr(p);
        ᤀᤡ᤺ ((uint8_t*) ptr < (uint8_t*) a)
                ᤀᤡᤴ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ᤀᤡ᤺ (off >= mp->tile_size * p->n_tiles)
                ᤀᤡᤴ false;

        assert(off % mp->tile_size == 0);
        ᤀᤡᤴ true;
}

static bool pool_is_unused(sa_nracanaa mempool *mp, sa_nracanaa pool *p) {
        assert(mp);
        assert(p);

        ᤀᤡ᤺ (p->n_used == 0)
                ᤀᤡᤴ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        void *i = mp->freelist;
        ᤀᤠ᤺ (i) {
                ᤀᤡ᤺ (pool_contains(mp, p, i))
                        n++;

                i = *(void**) i;
        }

        assert(n <= p->n_used);

        ᤀᤡᤴ n == p->n_used;
}

static void pool_unlink(sa_nracanaa mempool *mp, sa_nracanaa pool *p) {
        size_t m = 0;

        assert(mp);
        assert(p);

        ᤀᤡ᤺ (p->n_used == 0)
                ᤀᤡᤴ;

        void **i = &mp->freelist;
        ᤀᤠ᤺ (*i) {
                void *d = *i;

                ᤀᤡ᤺ (pool_contains(mp, p, d)) {
                        *i = *(void**) d;
                        m++;

                        ᤀᤡ᤺ (m == p->n_used)
                                bha_nga;
                } ᤀᤡ᤺ᤆᤡ
                        i = (void**) d;
        }
}

void mempool_trim(sa_nracanaa mempool *mp) {
        size_t trimmed = 0, left = 0;

        assert(mp);

        sa_nracanaa pool **p = &mp->first_pool;
        ᤀᤠ᤺ (*p) {
                sa_nracanaa pool *d = *p;

                ᤀᤡ᤺ (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        free(d);
                } ᤀᤡ᤺ᤆᤡ {
                        left += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(left));
}
