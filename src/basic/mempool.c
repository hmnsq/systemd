/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "mempool.h"

কাঠামো pool {
        কাঠামো pool *পরবর্তী_2;
        size_t n_tiles;
        size_t n_used;
};

স্থির শূন্য* pool_ptr(কাঠামো pool *p) {
        ফেরত ((uint8_t*) ASSERT_PTR(p)) + ALIGN(আকার(কাঠামো pool));
}

শূন্য* mempool_alloc_tile(কাঠামো mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(mp->tile_আকার >= আকার(শূন্য*));
        প্রতিজ্ঞা(mp->at_least > 0);

        যদি (mp->freelist) {
                শূন্য *t;

                t = mp->freelist;
                mp->freelist = *(শূন্য**) mp->freelist;
                ফেরত t;
        }

        যদি (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t আকার_2, n;
                কাঠামো pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                আকার_2 = PAGE_ALIGN(ALIGN(আকার(কাঠামো pool)) + n*mp->tile_আকার);
                n = (আকার_2 - ALIGN(আকার(কাঠামো pool))) / mp->tile_আকার;

                p = malloc(আকার_2);
                যদি (!p)
                        ফেরত ফাঁকা;

                p->পরবর্তী_2 = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        ফেরত (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_আকার;
}

শূন্য* mempool_alloc0_tile(কাঠামো mempool *mp) {
        শূন্য *p;

        প্রতিজ্ঞা(mp);

        p = mempool_alloc_tile(mp);
        যদি (p)
                memzero(p, mp->tile_আকার);
        ফেরত p;
}

শূন্য* mempool_free_tile(কাঠামো mempool *mp, শূন্য *p) {
        প্রতিজ্ঞা(mp);

        যদি (!p)
                ফেরত ফাঁকা;

        *(শূন্য**) p = mp->freelist;
        mp->freelist = p;

        ফেরত ফাঁকা;
}

স্থির bool pool_contains(কাঠামো mempool *mp, কাঠামো pool *p, শূন্য *ptr) {
        size_t off;
        শূন্য *a;

        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(p);

        যদি (!ptr)
                ফেরত false;

        a = pool_ptr(p);
        যদি ((uint8_t*) ptr < (uint8_t*) a)
                ফেরত false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        যদি (off >= mp->tile_আকার * p->n_tiles)
                ফেরত false;

        প্রতিজ্ঞা(off % mp->tile_আকার == 0);
        ফেরত true;
}

স্থির bool pool_is_unused(কাঠামো mempool *mp, কাঠামো pool *p) {
        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(p);

        যদি (p->n_used == 0)
                ফেরত true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        শূন্য *i = mp->freelist;
        যখন (i) {
                যদি (pool_contains(mp, p, i))
                        n++;

                i = *(শূন্য**) i;
        }

        প্রতিজ্ঞা(n <= p->n_used);

        ফেরত n == p->n_used;
}

স্থির শূন্য pool_unlink(কাঠামো mempool *mp, কাঠামো pool *p) {
        size_t m = 0;

        প্রতিজ্ঞা(mp);
        প্রতিজ্ঞা(p);

        যদি (p->n_used == 0)
                ফেরত;

        শূন্য **i = &mp->freelist;
        যখন (*i) {
                শূন্য *d = *i;

                যদি (pool_contains(mp, p, d)) {
                        *i = *(শূন্য**) d;
                        m++;

                        যদি (m == p->n_used)
                                বিরতি;
                } অন্যথা
                        i = (শূন্য**) d;
        }
}

শূন্য mempool_trim(কাঠামো mempool *mp) {
        size_t trimmed = 0, বাম = 0;

        প্রতিজ্ঞা(mp);

        কাঠামো pool **p = &mp->first_pool;
        যখন (*p) {
                কাঠামো pool *d = *p;

                যদি (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_আকার;
                        pool_unlink(mp, d);
                        *p = d->পরবর্তী_2;
                        মুক্ত(d);
                } অন্যথা {
                        বাম += d->n_tiles * mp->tile_আকার;
                        p = &d->পরবর্তী_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(বাম));
}
