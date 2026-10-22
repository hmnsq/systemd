/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "mempool.h"

بنية pool {
        بنية pool *بعدی_2;
        size_t n_tiles;
        size_t n_استفاده_شده;
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
        أكد(mp->at_کمترین > 0);

        إذا (mp->freelist) {
                فراغ *t;

                t = mp->freelist;
                mp->freelist = *(فراغ**) mp->freelist;
                أرجع t;
        }

        إذا (_unlikely_(!mp->اولی_pool) ||
            _unlikely_(mp->اولی_pool->n_استفاده_شده >= mp->اولی_pool->n_tiles)) {
                size_t اندازه, n;
                بنية pool *p;

                n = mp->اولی_pool ? mp->اولی_pool->n_tiles : 0;
                n = MAX(mp->at_کمترین, n * 2);
                اندازه = صفحه_ALIGN(ALIGN(حجم(بنية pool)) + n*mp->tile_اندازه);
                n = (اندازه - ALIGN(حجم(بنية pool))) / mp->tile_اندازه;

                p = حافظه_بده(اندازه);
                إذا (!p)
                        أرجع عدم;

                p->بعدی_2 = mp->اولی_pool;
                p->n_tiles = n;
                p->n_استفاده_شده = 0;

                mp->اولی_pool = p;
        }

        i = mp->اولی_pool->n_استفاده_شده++;

        أرجع (uint8_t*) pool_ptr(mp->اولی_pool) + i*mp->tile_اندازه;
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

ساكن bool pool_شامل_است(بنية mempool *mp, بنية pool *p, فراغ *ptr) {
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

ساكن bool pool_is_استفادهنشده(بنية mempool *mp, بنية pool *p) {
        أكد(mp);
        أكد(p);

        إذا (p->n_استفاده_شده == 0)
                أرجع true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        فراغ *i = mp->freelist;
        طالما (i) {
                إذا (pool_شامل_است(mp, p, i))
                        n++;

                i = *(فراغ**) i;
        }

        أكد(n <= p->n_استفاده_شده);

        أرجع n == p->n_استفاده_شده;
}

ساكن فراغ pool_unlink(بنية mempool *mp, بنية pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        إذا (p->n_استفاده_شده == 0)
                أرجع;

        فراغ **i = &mp->freelist;
        طالما (*i) {
                فراغ *d = *i;

                إذا (pool_شامل_است(mp, p, d)) {
                        *i = *(فراغ**) d;
                        m++;

                        إذا (m == p->n_استفاده_شده)
                                اكسر;
                } وإلا
                        i = (فراغ**) d;
        }
}

فراغ mempool_trim(بنية mempool *mp) {
        size_t trimmed = 0, چپ = 0;

        أكد(mp);

        بنية pool **p = &mp->اولی_pool;
        طالما (*p) {
                بنية pool *d = *p;

                إذا (pool_is_استفادهنشده(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_اندازه;
                        pool_unlink(mp, d);
                        *p = d->بعدی_2;
                        حرر(d);
                } وإلا {
                        چپ += d->n_tiles * mp->tile_اندازه;
                        p = &d->بعدی_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_بایتها(trimmed), mp, FORMAT_بایتها(چپ));
}
