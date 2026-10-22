/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "mempool.h"

સંરચના pool {
        સંરચના pool *next;
        size_t n_tiles;
        size_t n_used;
};

સ્થિર શૂન્ય* pool_ptr(સંરચના pool *p) {
        પાછા ((uint8_t*) ASSERT_PTR(p)) + ALIGN(કદ(સંરચના pool));
}

શૂન્ય* mempool_alloc_tile(સંરચના mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        પ્રતિજ્ઞા(mp);
        પ્રતિજ્ઞા(mp->tile_size >= કદ(શૂન્ય*));
        પ્રતિજ્ઞા(mp->at_least > 0);

        જો (mp->freelist) {
                શૂન્ય *t;

                t = mp->freelist;
                mp->freelist = *(શૂન્ય**) mp->freelist;
                પાછા t;
        }

        જો (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                સંરચના pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(કદ(સંરચના pool)) + n*mp->tile_size);
                n = (size - ALIGN(કદ(સંરચના pool))) / mp->tile_size;

                p = malloc(size);
                જો (!p)
                        પાછા ખાલી;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        પાછા (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

શૂન્ય* mempool_alloc0_tile(સંરચના mempool *mp) {
        શૂન્ય *p;

        પ્રતિજ્ઞા(mp);

        p = mempool_alloc_tile(mp);
        જો (p)
                memzero(p, mp->tile_size);
        પાછા p;
}

શૂન્ય* mempool_free_tile(સંરચના mempool *mp, શૂન્ય *p) {
        પ્રતિજ્ઞા(mp);

        જો (!p)
                પાછા ખાલી;

        *(શૂન્ય**) p = mp->freelist;
        mp->freelist = p;

        પાછા ખાલી;
}

સ્થિર bool pool_contains(સંરચના mempool *mp, સંરચના pool *p, શૂન્ય *ptr) {
        size_t off;
        શૂન્ય *a;

        પ્રતિજ્ઞા(mp);
        પ્રતિજ્ઞા(p);

        જો (!ptr)
                પાછા false;

        a = pool_ptr(p);
        જો ((uint8_t*) ptr < (uint8_t*) a)
                પાછા false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        જો (off >= mp->tile_size * p->n_tiles)
                પાછા false;

        પ્રતિજ્ઞા(off % mp->tile_size == 0);
        પાછા true;
}

સ્થિર bool pool_is_unused(સંરચના mempool *mp, સંરચના pool *p) {
        પ્રતિજ્ઞા(mp);
        પ્રતિજ્ઞા(p);

        જો (p->n_used == 0)
                પાછા true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        શૂન્ય *i = mp->freelist;
        જ્યારે (i) {
                જો (pool_contains(mp, p, i))
                        n++;

                i = *(શૂન્ય**) i;
        }

        પ્રતિજ્ઞા(n <= p->n_used);

        પાછા n == p->n_used;
}

સ્થિર શૂન્ય pool_unlink(સંરચના mempool *mp, સંરચના pool *p) {
        size_t m = 0;

        પ્રતિજ્ઞા(mp);
        પ્રતિજ્ઞા(p);

        જો (p->n_used == 0)
                પાછા;

        શૂન્ય **i = &mp->freelist;
        જ્યારે (*i) {
                શૂન્ય *d = *i;

                જો (pool_contains(mp, p, d)) {
                        *i = *(શૂન્ય**) d;
                        m++;

                        જો (m == p->n_used)
                                તોડો;
                } નહીંતર
                        i = (શૂન્ય**) d;
        }
}

શૂન્ય mempool_trim(સંરચના mempool *mp) {
        size_t trimmed = 0, ડાબું = 0;

        પ્રતિજ્ઞા(mp);

        સંરચના pool **p = &mp->first_pool;
        જ્યારે (*p) {
                સંરચના pool *d = *p;

                જો (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        મુક્ત(d);
                } નહીંતર {
                        ડાબું += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(ડાબું));
}
