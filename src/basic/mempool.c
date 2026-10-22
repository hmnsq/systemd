/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "format-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "mempool.h"

संरचना pool {
        संरचना pool *next;
        माप_प्रकार n_tiles;
        माप_प्रकार n_used;
};

स्थिर शून्य* pool_ptr(संरचना pool *p) {
        फर्काऊ ((uint8_t*) ASSERT_PTR(p)) + ALIGN(परिमाण(संरचना pool));
}

शून्य* mempool_alloc_tile(संरचना mempool *mp) {
        माप_प्रकार i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        प्रतिज्ञा(mp);
        प्रतिज्ञा(mp->tile_size >= परिमाण(शून्य*));
        प्रतिज्ञा(mp->at_least > 0);

        यदि (mp->freelist) {
                शून्य *t;

                t = mp->freelist;
                mp->freelist = *(शून्य**) mp->freelist;
                फर्काऊ t;
        }

        यदि (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                माप_प्रकार size, n;
                संरचना pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                size = PAGE_ALIGN(ALIGN(परिमाण(संरचना pool)) + n*mp->tile_size);
                n = (size - ALIGN(परिमाण(संरचना pool))) / mp->tile_size;

                p = दो_स्मृति(size);
                यदि (!p)
                        फर्काऊ रिक्त;

                p->next = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        फर्काऊ (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_size;
}

शून्य* mempool_alloc0_tile(संरचना mempool *mp) {
        शून्य *p;

        प्रतिज्ञा(mp);

        p = mempool_alloc_tile(mp);
        यदि (p)
                memzero(p, mp->tile_size);
        फर्काऊ p;
}

शून्य* mempool_free_tile(संरचना mempool *mp, शून्य *p) {
        प्रतिज्ञा(mp);

        यदि (!p)
                फर्काऊ रिक्त;

        *(शून्य**) p = mp->freelist;
        mp->freelist = p;

        फर्काऊ रिक्त;
}

स्थिर bool pool_contains(संरचना mempool *mp, संरचना pool *p, शून्य *ptr) {
        माप_प्रकार off;
        शून्य *a;

        प्रतिज्ञा(mp);
        प्रतिज्ञा(p);

        यदि (!ptr)
                फर्काऊ false;

        a = pool_ptr(p);
        यदि ((uint8_t*) ptr < (uint8_t*) a)
                फर्काऊ false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        यदि (off >= mp->tile_size * p->n_tiles)
                फर्काऊ false;

        प्रतिज्ञा(off % mp->tile_size == 0);
        फर्काऊ true;
}

स्थिर bool pool_is_unused(संरचना mempool *mp, संरचना pool *p) {
        प्रतिज्ञा(mp);
        प्रतिज्ञा(p);

        यदि (p->n_used == 0)
                फर्काऊ true;

        /* Check if all tiles in this specific pool are in the freelist. */
        माप_प्रकार n = 0;
        शून्य *i = mp->freelist;
        जबसम्म (i) {
                यदि (pool_contains(mp, p, i))
                        n++;

                i = *(शून्य**) i;
        }

        प्रतिज्ञा(n <= p->n_used);

        फर्काऊ n == p->n_used;
}

स्थिर शून्य pool_unlink(संरचना mempool *mp, संरचना pool *p) {
        माप_प्रकार m = 0;

        प्रतिज्ञा(mp);
        प्रतिज्ञा(p);

        यदि (p->n_used == 0)
                फर्काऊ;

        शून्य **i = &mp->freelist;
        जबसम्म (*i) {
                शून्य *d = *i;

                यदि (pool_contains(mp, p, d)) {
                        *i = *(शून्य**) d;
                        m++;

                        यदि (m == p->n_used)
                                रोक;
                } अन्यथा
                        i = (शून्य**) d;
        }
}

शून्य mempool_trim(संरचना mempool *mp) {
        माप_प्रकार trimmed = 0, बायाँ = 0;

        प्रतिज्ञा(mp);

        संरचना pool **p = &mp->first_pool;
        जबसम्म (*p) {
                संरचना pool *d = *p;

                यदि (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_size;
                        pool_unlink(mp, d);
                        *p = d->next;
                        मुक्त(d);
                } अन्यथा {
                        बायाँ += d->n_tiles * mp->tile_size;
                        p = &d->next;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(बायाँ));
}
