/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "mempool.h"

ସଂରଚନା pool {
        ସଂରଚନା pool *next;
        size_t n_tiles;
        size_t n_used;
};

ସ୍ଥିର ଶୂନ୍ୟ* pool_ptr(ସଂରଚନା pool *p) {
        ଫେରନ୍ତୁ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(ଆକାର(ସଂରଚନା pool));
}

ଶୂନ୍ୟ* mempool_alloc_tile(ସଂରଚନା mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        ପ୍ରତିଜ୍ଞା(mp);
        ପ୍ରତିଜ୍ଞା(mp->tile_size >= ଆକାର(ଶୂନ୍ୟ*));
        ପ୍ରତିଜ୍ଞା(mp->at_least > 0);

        ଯଦି (mp->freelist) {
                ଶୂନ୍ୟ *t;

                t = mp->freelist;
                mp->freelist = *(ଶୂନ୍ୟ**) mp->freelist;
                ଫେରନ୍ତୁ t;
        }

        ଯଦି (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                ସଂରଚନା pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(ଆକାର(ସଂରଚନା pool)) + n*mp->tile_size);
                n = (size - ALIGN(ଆକାର(ସଂରଚନା pool))) / mp->tile_size;

                p = malloc(size);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ ଖାଲି;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        ଫେରନ୍ତୁ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

ଶୂନ୍ୟ* mempool_alloc0_tile(ସଂରଚନା mempool *mp) {
        ଶୂନ୍ୟ *p;

        ପ୍ରତିଜ୍ଞା(mp);

        p = mempool_alloc_tile(mp);
        ଯଦି (p)
                memzero(p, mp->tile_size);
        ଫେରନ୍ତୁ p;
}

ଶୂନ୍ୟ* mempool_free_tile(ସଂରଚନା mempool *mp, ଶୂନ୍ୟ *p) {
        ପ୍ରତିଜ୍ଞା(mp);

        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        *(ଶୂନ୍ୟ**) p = mp->freelist;
        mp->freelist = p;

        ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର bool pool_contains(ସଂରଚନା mempool *mp, ସଂରଚନା pool *p, ଶୂନ୍ୟ *ptr) {
        size_t off;
        ଶୂନ୍ୟ *a;

        ପ୍ରତିଜ୍ଞା(mp);
        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (!ptr)
                ଫେରନ୍ତୁ false;

        a = pool_ptr(p);
        ଯଦି ((uint8_t*) ptr < (uint8_t*) a)
                ଫେରନ୍ତୁ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        ଯଦି (off >= mp->tile_size * p->n_tiles)
                ଫେରନ୍ତୁ false;

        ପ୍ରତିଜ୍ଞା(off % mp->tile_size == 0);
        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool pool_is_unused(ସଂରଚନା mempool *mp, ସଂରଚନା pool *p) {
        ପ୍ରତିଜ୍ଞା(mp);
        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (p->n_used == 0)
                ଫେରନ୍ତୁ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        ଶୂନ୍ୟ *i = mp->freelist;
        ଯେତେବେଳେ (i) {
                ଯଦି (pool_contains(mp, p, i))
                        n++;

                i = *(ଶୂନ୍ୟ**) i;
        }

        ପ୍ରତିଜ୍ଞା(n <= p->n_used);

        ଫେରନ୍ତୁ n == p->n_used;
}

ସ୍ଥିର ଶୂନ୍ୟ pool_unlink(ସଂରଚନା mempool *mp, ସଂରଚନା pool *p) {
        size_t m = 0;

        ପ୍ରତିଜ୍ଞା(mp);
        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (p->n_used == 0)
                ଫେରନ୍ତୁ;

        ଶୂନ୍ୟ **i = &mp->freelist;
        ଯେତେବେଳେ (*i) {
                ଶୂନ୍ୟ *d = *i;

                ଯଦି (pool_contains(mp, p, d)) {
                        *i = *(ଶୂନ୍ୟ**) d;
                        m++;

                        ଯଦି (m == p->n_used)
                                ଭାଙ୍ଗ;
                } ଅନ୍ୟଥା
                        i = (ଶୂନ୍ୟ**) d;
        }
}

ଶୂନ୍ୟ mempool_trim(ସଂରଚନା mempool *mp) {
        size_t trimmed = 0, ବାମ = 0;

        ପ୍ରତିଜ୍ଞା(mp);

        ସଂରଚନା pool **p = &mp->first_pool;
        ଯେତେବେଳେ (*p) {
                ସଂରଚନା pool *d = *p;

                ଯଦି (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        ମୁକ୍ତ(d);
                } ଅନ୍ୟଥା {
                        ବାମ += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(ବାମ));
}
