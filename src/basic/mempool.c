/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "mempool.h"

بنية pool {
        بنية pool *بل_2;
        size_t n_tiles;
        size_t n_کارول_شوی;
};

ساكن فراغ* pool_ptr(بنية pool *p) {
        أرجع ((uint8_t*) ASSERT_PTR(p)) + ALIGN(حجم(بنية pool));
}

فراغ* mempool_alloc_tile(بنية mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        أكد(mp);
        أكد(mp->tile_اندازه >= حجم(فراغ*));
        أكد(mp->at_لږترلږه > 0);

        إذا (mp->freelist) {
                فراغ *t;

                t = mp->freelist;
                mp->freelist = *(فراغ**) mp->freelist;
                أرجع t;
        }

        إذا (_unlikely_(!mp->لومړی_pool) ||
            _unlikely_(mp->لومړی_pool->n_کارول_شوی >= mp->لومړی_pool->n_tiles)) {
                size_t اندازه, n;
                بنية pool *p;

                n = mp->لومړی_pool ? mp->لومړی_pool->n_tiles : 0;
                n = MAX(mp->at_لږترلږه, n * 2);
                اندازه = پاڼه_ALIGN(ALIGN(حجم(بنية pool)) + n*mp->tile_اندازه);
                n = (اندازه - ALIGN(حجم(بنية pool))) / mp->tile_اندازه;

                p = حافظه_ورکړه(اندازه);
                إذا (!p)
                        أرجع عدم;

                p->بل_2 = mp->لومړی_pool;
                p->n_tiles = n;
                p->n_کارول_شوی = 0;

                mp->لومړی_pool = p;
        }

        i = mp->لومړی_pool->n_کارول_شوی++;

        أرجع (uint8_t*) pool_ptr(mp->لومړی_pool) + i*mp->tile_اندازه;
}

فراغ* mempool_alloc0_tile(بنية mempool *mp) {
        فراغ *p;

        أكد(mp);

        p = mempool_alloc_tile(mp);
        إذا (p)
                memzero(p, mp->tile_اندازه);
        أرجع p;
}

فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p) {
        أكد(mp);

        إذا (!p)
                أرجع عدم;

        *(فراغ**) p = mp->freelist;
        mp->freelist = p;

        أرجع عدم;
}

ساكن bool pool_لري(بنية mempool *mp, بنية pool *p, فراغ *ptr) {
        size_t off;
        فراغ *a;

        أكد(mp);
        أكد(p);

        إذا (!ptr)
                أرجع false;

        a = pool_ptr(p);
        إذا ((uint8_t*) ptr < (uint8_t*) a)
                أرجع false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        إذا (off >= mp->tile_اندازه * p->n_tiles)
                أرجع false;

        أكد(off % mp->tile_اندازه == 0);
        أرجع true;
}

ساكن bool pool_is_ناکارول_شوی(بنية mempool *mp, بنية pool *p) {
        أكد(mp);
        أكد(p);

        إذا (p->n_کارول_شوی == 0)
                أرجع true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        فراغ *i = mp->freelist;
        طالما (i) {
                إذا (pool_لري(mp, p, i))
                        n++;

                i = *(فراغ**) i;
        }

        أكد(n <= p->n_کارول_شوی);

        أرجع n == p->n_کارول_شوی;
}

ساكن فراغ pool_unlink(بنية mempool *mp, بنية pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        إذا (p->n_کارول_شوی == 0)
                أرجع;

        فراغ **i = &mp->freelist;
        طالما (*i) {
                فراغ *d = *i;

                إذا (pool_لري(mp, p, d)) {
                        *i = *(فراغ**) d;
                        m++;

                        إذا (m == p->n_کارول_شوی)
                                اكسر;
                } وإلا
                        i = (فراغ**) d;
        }
}

فراغ mempool_trim(بنية mempool *mp) {
        size_t trimmed = 0, کیڼ = 0;

        أكد(mp);

        بنية pool **p = &mp->لومړی_pool;
        طالما (*p) {
                بنية pool *d = *p;

                إذا (pool_is_ناکارول_شوی(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_اندازه;
                        pool_unlink(mp, d);
                        *p = d->بل_2;
                        حرر(d);
                } وإلا {
                        کیڼ += d->n_tiles * mp->tile_اندازه;
                        p = &d->بل_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_بایټونه(trimmed), mp, FORMAT_بایټونه(کیڼ));
}
