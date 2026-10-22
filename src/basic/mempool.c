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
        ត្រឡប់ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(parimaa_na(sa_nracanaa pool));
}

void* mempool_alloc_tile(sa_nracanaa mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        assert(mp);
        assert(mp->tile_size >= parimaa_na(void*));
        assert(mp->at_least > 0);

        ប្រសិនបើ (mp->freelist) {
                void *t;

                t = mp->freelist;
                mp->freelist = *(void**) mp->freelist;
                ត្រឡប់ t;
        }

        ប្រសិនបើ (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                sa_nracanaa pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(parimaa_na(sa_nracanaa pool)) + n*mp->tile_size);
                n = (size - ALIGN(parimaa_na(sa_nracanaa pool))) / mp->tile_size;

                p = malloc(size);
                ប្រសិនបើ (!p)
                        ត្រឡប់ NULL;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        ត្រឡប់ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

void* mempool_alloc0_tile(sa_nracanaa mempool *mp) {
        void *p;

        assert(mp);

        p = mempool_alloc_tile(mp);
        ប្រសិនបើ (p)
                memzero(p, mp->tile_size);
        ត្រឡប់ p;
}

void* mempool_free_tile(sa_nracanaa mempool *mp, void *p) {
        assert(mp);

        ប្រសិនបើ (!p)
                ត្រឡប់ NULL;

        *(void**) p = mp->freelist;
        mp->freelist = p;

        ត្រឡប់ NULL;
}

static bool pool_contains(sa_nracanaa mempool *mp, sa_nracanaa pool *p, void *ptr) {
        size_t off;
        void *a;

        assert(mp);
        assert(p);

        ប្រសិនបើ (!ptr)
                ត្រឡប់ false;

        a = pool_ptr(p);
        ប្រសិនបើ ((uint8_t*) ptr < (uint8_t*) a)
                ត្រឡប់ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ប្រសិនបើ (off >= mp->tile_size * p->n_tiles)
                ត្រឡប់ false;

        assert(off % mp->tile_size == 0);
        ត្រឡប់ true;
}

static bool pool_is_unused(sa_nracanaa mempool *mp, sa_nracanaa pool *p) {
        assert(mp);
        assert(p);

        ប្រសិនបើ (p->n_used == 0)
                ត្រឡប់ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        void *i = mp->freelist;
        ខណៈពេល (i) {
                ប្រសិនបើ (pool_contains(mp, p, i))
                        n++;

                i = *(void**) i;
        }

        assert(n <= p->n_used);

        ត្រឡប់ n == p->n_used;
}

static void pool_unlink(sa_nracanaa mempool *mp, sa_nracanaa pool *p) {
        size_t m = 0;

        assert(mp);
        assert(p);

        ប្រសិនបើ (p->n_used == 0)
                ត្រឡប់;

        void **i = &mp->freelist;
        ខណៈពេល (*i) {
                void *d = *i;

                ប្រសិនបើ (pool_contains(mp, p, d)) {
                        *i = *(void**) d;
                        m++;

                        ប្រសិនបើ (m == p->n_used)
                                bha_nga;
                } បើមិនដូច្នេះ
                        i = (void**) d;
        }
}

void mempool_trim(sa_nracanaa mempool *mp) {
        size_t trimmed = 0, left = 0;

        assert(mp);

        sa_nracanaa pool **p = &mp->first_pool;
        ខណៈពេល (*p) {
                sa_nracanaa pool *d = *p;

                ប្រសិនបើ (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        free(d);
                } បើមិនដូច្នេះ {
                        left += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(left));
}
