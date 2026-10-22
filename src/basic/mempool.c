/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "mempool.h"

সংরচনা pool {
        সংরচনা pool *next;
        size_t n_tiles;
        size_t n_used;
};

স্থির শূন্য* pool_ptr(সংরচনা pool *p) {
        প্রত্যাগম ((uint8_t*) ASSERT_PTR(p)) + ALIGN(পরিমাণ(সংরচনা pool));
}

শূন্য* mempool_alloc_tile(সংরচনা mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(mp->tile_size >= পরিমাণ(শূন্য*));
        প্রতিজ্ঞা(mp->at_least > 0);

        যদি (mp->freelist) {
                শূন্য *t;

                t = mp->freelist;
                mp->freelist = *(শূন্য**) mp->freelist;
                প্রত্যাগম t;
        }

        যদি (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                সংরচনা pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(পরিমাণ(সংরচনা pool)) + n*mp->tile_size);
                n = (size - ALIGN(পরিমাণ(সংরচনা pool))) / mp->tile_size;

                p = malloc(size);
                যদি (!p)
                        প্রত্যাগম রিক্ত;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        প্রত্যাগম (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

শূন্য* mempool_alloc0_tile(সংরচনা mempool *mp) {
        শূন্য *p;

        প্রতিজ্ঞা(mp);

        p = mempool_alloc_tile(mp);
        যদি (p)
                memzero(p, mp->tile_size);
        প্রত্যাগম p;
}

শূন্য* mempool_free_tile(সংরচনা mempool *mp, শূন্য *p) {
        প্রতিজ্ঞা(mp);

        যদি (!p)
                প্রত্যাগম রিক্ত;

        *(শূন্য**) p = mp->freelist;
        mp->freelist = p;

        প্রত্যাগম রিক্ত;
}

স্থির bool pool_contains(সংরচনা mempool *mp, সংরচনা pool *p, শূন্য *ptr) {
        size_t off;
        শূন্য *a;

        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(p);

        যদি (!ptr)
                প্রত্যাগম false;

        a = pool_ptr(p);
        যদি ((uint8_t*) ptr < (uint8_t*) a)
                প্রত্যাগম false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        যদি (off >= mp->tile_size * p->n_tiles)
                প্রত্যাগম false;

        প্রতিজ্ঞা(off % mp->tile_size == 0);
        প্রত্যাগম true;
}

স্থির bool pool_is_unused(সংরচনা mempool *mp, সংরচনা pool *p) {
        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(p);

        যদি (p->n_used == 0)
                প্রত্যাগম true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        শূন্য *i = mp->freelist;
        যাবত্ (i) {
                যদি (pool_contains(mp, p, i))
                        n++;

                i = *(শূন্য**) i;
        }

        প্রতিজ্ঞা(n <= p->n_used);

        প্রত্যাগম n == p->n_used;
}

স্থির শূন্য pool_unlink(সংরচনা mempool *mp, সংরচনা pool *p) {
        size_t m = 0;

        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(p);

        যদি (p->n_used == 0)
                প্রত্যাগম;

        শূন্য **i = &mp->freelist;
        যাবত্ (*i) {
                শূন্য *d = *i;

                যদি (pool_contains(mp, p, d)) {
                        *i = *(শূন্য**) d;
                        m++;

                        যদি (m == p->n_used)
                                ভঙ্গ;
                } অন্যথা
                        i = (শূন্য**) d;
        }
}

শূন্য mempool_trim(সংরচনা mempool *mp) {
        size_t trimmed = 0, বাম = 0;

        প্রতিজ্ঞা(mp);

        সংরচনা pool **p = &mp->first_pool;
        যাবত্ (*p) {
                সংরচনা pool *d = *p;

                যদি (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        মুক্ত(d);
                } অন্যথা {
                        বাম += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(বাম));
}
