/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "mempool.h"

கட்டமைப்பு pool {
        கட்டமைப்பு pool *next;
        size_t n_tiles;
        size_t n_used;
};

நிலையான வெறுமை* pool_ptr(கட்டமைப்பு pool *p) {
        திரும்பு ((uint8_t*) ASSERT_PTR(p)) + ALIGN(அளவு(கட்டமைப்பு pool));
}

வெறுமை* mempool_alloc_tile(கட்டமைப்பு mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        ப்ரதிஜ்ஞா(mp);
        ப்ரதிஜ்ஞா(mp->tile_size >= அளவு(வெறுமை*));
        ப்ரதிஜ்ஞா(mp->at_least > 0);

        எனில் (mp->freelist) {
                வெறுமை *t;

                t = mp->freelist;
                mp->freelist = *(வெறுமை**) mp->freelist;
                திரும்பு t;
        }

        எனில் (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                கட்டமைப்பு pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(அளவு(கட்டமைப்பு pool)) + n*mp->tile_size);
                n = (size - ALIGN(அளவு(கட்டமைப்பு pool))) / mp->tile_size;

                p = malloc(size);
                எனில் (!p)
                        திரும்பு வெற்று;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        திரும்பு (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

வெறுமை* mempool_alloc0_tile(கட்டமைப்பு mempool *mp) {
        வெறுமை *p;

        ப்ரதிஜ்ஞா(mp);

        p = mempool_alloc_tile(mp);
        எனில் (p)
                memzero(p, mp->tile_size);
        திரும்பு p;
}

வெறுமை* mempool_free_tile(கட்டமைப்பு mempool *mp, வெறுமை *p) {
        ப்ரதிஜ்ஞா(mp);

        எனில் (!p)
                திரும்பு வெற்று;

        *(வெறுமை**) p = mp->freelist;
        mp->freelist = p;

        திரும்பு வெற்று;
}

நிலையான bool pool_contains(கட்டமைப்பு mempool *mp, கட்டமைப்பு pool *p, வெறுமை *ptr) {
        size_t off;
        வெறுமை *a;

        ப்ரதிஜ்ஞா(mp);
        ப்ரதிஜ்ஞா(p);

        எனில் (!ptr)
                திரும்பு false;

        a = pool_ptr(p);
        எனில் ((uint8_t*) ptr < (uint8_t*) a)
                திரும்பு false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        எனில் (off >= mp->tile_size * p->n_tiles)
                திரும்பு false;

        ப்ரதிஜ்ஞா(off % mp->tile_size == 0);
        திரும்பு true;
}

நிலையான bool pool_is_unused(கட்டமைப்பு mempool *mp, கட்டமைப்பு pool *p) {
        ப்ரதிஜ்ஞா(mp);
        ப்ரதிஜ்ஞா(p);

        எனில் (p->n_used == 0)
                திரும்பு true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        வெறுமை *i = mp->freelist;
        போது (i) {
                எனில் (pool_contains(mp, p, i))
                        n++;

                i = *(வெறுமை**) i;
        }

        ப்ரதிஜ்ஞா(n <= p->n_used);

        திரும்பு n == p->n_used;
}

நிலையான வெறுமை pool_unlink(கட்டமைப்பு mempool *mp, கட்டமைப்பு pool *p) {
        size_t m = 0;

        ப்ரதிஜ்ஞா(mp);
        ப்ரதிஜ்ஞா(p);

        எனில் (p->n_used == 0)
                திரும்பு;

        வெறுமை **i = &mp->freelist;
        போது (*i) {
                வெறுமை *d = *i;

                எனில் (pool_contains(mp, p, d)) {
                        *i = *(வெறுமை**) d;
                        m++;

                        எனில் (m == p->n_used)
                                நிறுத்து;
                } இல்லையெனில்
                        i = (வெறுமை**) d;
        }
}

வெறுமை mempool_trim(கட்டமைப்பு mempool *mp) {
        size_t trimmed = 0, இடது = 0;

        ப்ரதிஜ்ஞா(mp);

        கட்டமைப்பு pool **p = &mp->first_pool;
        போது (*p) {
                கட்டமைப்பு pool *d = *p;

                எனில் (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        விடுவி(d);
                } இல்லையெனில் {
                        இடது += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(இடது));
}
