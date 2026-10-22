/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "format-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "mempool.h"

संरचना pool {
        संरचना pool *next;
        size_t n_tiles;
        size_t n_used;
};

स्थिर शून्य* pool_ptr(संरचना pool *p) {
        पटिनिवत्त ((uint8_t*) ASSERT_PTR(p)) + ALIGN(परिमाण(संरचना pool));
}

शून्य* mempool_alloc_tile(संरचना mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        प्रतिज्ञा(mp);
        प्रतिज्ञा(mp->tile_size >= परिमाण(शून्य*));
        प्रतिज्ञा(mp->at_least > 0);

        सचे (mp->freelist) {
                शून्य *t;

                t = mp->freelist;
                mp->freelist = *(शून्य**) mp->freelist;
                पटिनिवत्त t;
        }

        सचे (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                size_t size, n;
                संरचना pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(परिमाण(संरचना pool)) + n*mp->tile_size);
                n = (size - ALIGN(परिमाण(संरचना pool))) / mp->tile_size;

                p = malloc(size);
                सचे (!p)
                        पटिनिवत्त रिक्त;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        पटिनिवत्त (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

शून्य* mempool_alloc0_tile(संरचना mempool *mp) {
        शून्य *p;

        प्रतिज्ञा(mp);

        p = mempool_alloc_tile(mp);
        सचे (p)
                memzero(p, mp->tile_size);
        पटिनिवत्त p;
}

शून्य* mempool_free_tile(संरचना mempool *mp, शून्य *p) {
        प्रतिज्ञा(mp);

        सचे (!p)
                पटिनिवत्त रिक्त;

        *(शून्य**) p = mp->freelist;
        mp->freelist = p;

        पटिनिवत्त रिक्त;
}

स्थिर bool pool_contains(संरचना mempool *mp, संरचना pool *p, शून्य *ptr) {
        size_t off;
        शून्य *a;

        प्रतिज्ञा(mp);
        प्रतिज्ञा(p);

        सचे (!ptr)
                पटिनिवत्त false;

        a = pool_ptr(p);
        सचे ((uint8_t*) ptr < (uint8_t*) a)
                पटिनिवत्त false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        सचे (off >= mp->tile_size * p->n_tiles)
                पटिनिवत्त false;

        प्रतिज्ञा(off % mp->tile_size == 0);
        पटिनिवत्त true;
}

स्थिर bool pool_is_unused(संरचना mempool *mp, संरचना pool *p) {
        प्रतिज्ञा(mp);
        प्रतिज्ञा(p);

        सचे (p->n_used == 0)
                पटिनिवत्त true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        शून्य *i = mp->freelist;
        याव (i) {
                सचे (pool_contains(mp, p, i))
                        n++;

                i = *(शून्य**) i;
        }

        प्रतिज्ञा(n <= p->n_used);

        पटिनिवत्त n == p->n_used;
}

स्थिर शून्य pool_unlink(संरचना mempool *mp, संरचना pool *p) {
        size_t m = 0;

        प्रतिज्ञा(mp);
        प्रतिज्ञा(p);

        सचे (p->n_used == 0)
                पटिनिवत्त;

        शून्य **i = &mp->freelist;
        याव (*i) {
                शून्य *d = *i;

                सचे (pool_contains(mp, p, d)) {
                        *i = *(शून्य**) d;
                        m++;

                        सचे (m == p->n_used)
                                भङ्ग;
                } अञ्ञथा
                        i = (शून्य**) d;
        }
}

शून्य mempool_trim(संरचना mempool *mp) {
        size_t trimmed = 0, वामः = 0;

        प्रतिज्ञा(mp);

        संरचना pool **p = &mp->first_pool;
        याव (*p) {
                संरचना pool *d = *p;

                सचे (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        मुक्त(d);
                } अञ्ञथा {
                        वामः += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(वामः));
}
