/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "mempool.h"

ഘടന pool {
        ഘടന pool *next;
        size_t n_tiles;
        size_t n_used;
};

സ്ഥിതം ശൂന്യം* pool_ptr(ഘടന pool *p) {
        തിരികെ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(വലുപ്പം(ഘടന pool));
}

ശൂന്യം* mempool_alloc_tile(ഘടന mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        പ്രതിജ്ഞാ(mp);
        പ്രതിജ്ഞാ(mp->tile_size >= വലുപ്പം(ശൂന്യം*));
        പ്രതിജ്ഞാ(mp->at_least > 0);

        എങ്കിൽ (mp->freelist) {
                ശൂന്യം *t;

                t = mp->freelist;
                mp->freelist = *(ശൂന്യം**) mp->freelist;
                തിരികെ t;
        }

        എങ്കിൽ (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                ഘടന pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(വലുപ്പം(ഘടന pool)) + n*mp->tile_size);
                n = (size - ALIGN(വലുപ്പം(ഘടന pool))) / mp->tile_size;

                p = malloc(size);
                എങ്കിൽ (!p)
                        തിരികെ ശൂന്യ;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        തിരികെ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

ശൂന്യം* mempool_alloc0_tile(ഘടന mempool *mp) {
        ശൂന്യം *p;

        പ്രതിജ്ഞാ(mp);

        p = mempool_alloc_tile(mp);
        എങ്കിൽ (p)
                memzero(p, mp->tile_size);
        തിരികെ p;
}

ശൂന്യം* mempool_free_tile(ഘടന mempool *mp, ശൂന്യം *p) {
        പ്രതിജ്ഞാ(mp);

        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        *(ശൂന്യം**) p = mp->freelist;
        mp->freelist = p;

        തിരികെ ശൂന്യ;
}

സ്ഥിതം bool pool_contains(ഘടന mempool *mp, ഘടന pool *p, ശൂന്യം *ptr) {
        size_t off;
        ശൂന്യം *a;

        പ്രതിജ്ഞാ(mp);
        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (!ptr)
                തിരികെ false;

        a = pool_ptr(p);
        എങ്കിൽ ((uint8_t*) ptr < (uint8_t*) a)
                തിരികെ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        എങ്കിൽ (off >= mp->tile_size * p->n_tiles)
                തിരികെ false;

        പ്രതിജ്ഞാ(off % mp->tile_size == 0);
        തിരികെ true;
}

സ്ഥിതം bool pool_is_unused(ഘടന mempool *mp, ഘടന pool *p) {
        പ്രതിജ്ഞാ(mp);
        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (p->n_used == 0)
                തിരികെ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        ശൂന്യം *i = mp->freelist;
        ആകുമ്പോൾ (i) {
                എങ്കിൽ (pool_contains(mp, p, i))
                        n++;

                i = *(ശൂന്യം**) i;
        }

        പ്രതിജ്ഞാ(n <= p->n_used);

        തിരികെ n == p->n_used;
}

സ്ഥിതം ശൂന്യം pool_unlink(ഘടന mempool *mp, ഘടന pool *p) {
        size_t m = 0;

        പ്രതിജ്ഞാ(mp);
        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (p->n_used == 0)
                തിരികെ;

        ശൂന്യം **i = &mp->freelist;
        ആകുമ്പോൾ (*i) {
                ശൂന്യം *d = *i;

                എങ്കിൽ (pool_contains(mp, p, d)) {
                        *i = *(ശൂന്യം**) d;
                        m++;

                        എങ്കിൽ (m == p->n_used)
                                നിർത്തുക;
                } അല്ലെങ്കിൽ
                        i = (ശൂന്യം**) d;
        }
}

ശൂന്യം mempool_trim(ഘടന mempool *mp) {
        size_t trimmed = 0, ഇടത് = 0;

        പ്രതിജ്ഞാ(mp);

        ഘടന pool **p = &mp->first_pool;
        ആകുമ്പോൾ (*p) {
                ഘടന pool *d = *p;

                എങ്കിൽ (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        സ്വതന്ത്രം(d);
                } അല്ലെങ്കിൽ {
                        ഇടത് += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(ഇടത്));
}
