/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <stdlib.h>

#समावेश "format-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "mempool.h"

काष्ठा pool {
        काष्ठा pool *अग्र_2;
        माप_प्रकार n_tiles;
        माप_प्रकार n_used;
};

अटल व्योम* pool_ptr(काष्ठा pool *p) {
        वापस ((uint8_t*) ASSERT_PTR(p)) + ALIGN(माप(काष्ठा pool));
}

व्योम* mempool_alloc_tile(काष्ठा mempool *mp) {
        माप_प्रकार i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        निश्चित(mp);
        निश्चित(mp->tile_आकार >= माप(व्योम*));
        निश्चित(mp->at_least > 0);

        अगर (mp->freelist) {
                व्योम *t;

                t = mp->freelist;
                mp->freelist = *(व्योम**) mp->freelist;
                वापस t;
        }

        अगर (_unlikely_(!mp->first_pool) ||
            _unlikely_(mp->first_pool->n_used >= mp->first_pool->n_tiles)) {
                माप_प्रकार आकार, n;
                काष्ठा pool *p;

                n = mp->first_pool ? mp->first_pool->n_tiles : 0;
                n = MAX(mp->at_least, n * 2);
                आकार = PAGE_ALIGN(ALIGN(माप(काष्ठा pool)) + n*mp->tile_आकार);
                n = (आकार - ALIGN(माप(काष्ठा pool))) / mp->tile_आकार;

                p = दो_स्मृति(आकार);
                अगर (!p)
                        वापस शून्य;

                p->अग्र_2 = mp->first_pool;
                p->n_tiles = n;
                p->n_used = 0;

                mp->first_pool = p;
        }

        i = mp->first_pool->n_used++;

        वापस (uint8_t*) pool_ptr(mp->first_pool) + i*mp->tile_आकार;
}

व्योम* mempool_alloc0_tile(काष्ठा mempool *mp) {
        व्योम *p;

        निश्चित(mp);

        p = mempool_alloc_tile(mp);
        अगर (p)
                memzero(p, mp->tile_आकार);
        वापस p;
}

व्योम* mempool_free_tile(काष्ठा mempool *mp, व्योम *p) {
        निश्चित(mp);

        अगर (!p)
                वापस शून्य;

        *(व्योम**) p = mp->freelist;
        mp->freelist = p;

        वापस शून्य;
}

अटल bool pool_contains(काष्ठा mempool *mp, काष्ठा pool *p, व्योम *ptr) {
        माप_प्रकार off;
        व्योम *a;

        निश्चित(mp);
        निश्चित(p);

        अगर (!ptr)
                वापस false;

        a = pool_ptr(p);
        अगर ((uint8_t*) ptr < (uint8_t*) a)
                वापस false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        अगर (off >= mp->tile_आकार * p->n_tiles)
                वापस false;

        निश्चित(off % mp->tile_आकार == 0);
        वापस true;
}

अटल bool pool_is_unused(काष्ठा mempool *mp, काष्ठा pool *p) {
        निश्चित(mp);
        निश्चित(p);

        अगर (p->n_used == 0)
                वापस true;

        /* Check if all tiles in this specific pool are in the freelist. */
        माप_प्रकार n = 0;
        व्योम *i = mp->freelist;
        जबतक (i) {
                अगर (pool_contains(mp, p, i))
                        n++;

                i = *(व्योम**) i;
        }

        निश्चित(n <= p->n_used);

        वापस n == p->n_used;
}

अटल व्योम pool_unlink(काष्ठा mempool *mp, काष्ठा pool *p) {
        माप_प्रकार m = 0;

        निश्चित(mp);
        निश्चित(p);

        अगर (p->n_used == 0)
                वापस;

        व्योम **i = &mp->freelist;
        जबतक (*i) {
                व्योम *d = *i;

                अगर (pool_contains(mp, p, d)) {
                        *i = *(व्योम**) d;
                        m++;

                        अगर (m == p->n_used)
                                अवरोध;
                } अन्यथा
                        i = (व्योम**) d;
        }
}

व्योम mempool_trim(काष्ठा mempool *mp) {
        माप_प्रकार trimmed = 0, बायाँ = 0;

        निश्चित(mp);

        काष्ठा pool **p = &mp->first_pool;
        जबतक (*p) {
                काष्ठा pool *d = *p;

                अगर (pool_is_unused(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_आकार;
                        pool_unlink(mp, d);
                        *p = d->अग्र_2;
                        मुक्त(d);
                } अन्यथा {
                        बायाँ += d->n_tiles * mp->tile_आकार;
                        p = &d->अग्र_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_BYTES(trimmed), mp, FORMAT_BYTES(बायाँ));
}
