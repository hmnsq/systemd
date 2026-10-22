/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "mempool.h"

بنية pool {
        بنية pool *ايندڙ_2;
        size_t n_tiles;
        size_t n_استعمال_ٿيل;
};

ساكن فراغ* pool_ptr(بنية pool *p) {
        أرجع ((uint8_t*) ASSERT_PTR(p)) + ALIGN(حجم(بنية pool));
}

فراغ* mempool_alloc_tile(بنية mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        أكد(mp);
        أكد(mp->tile_سائيز >= حجم(فراغ*));
        أكد(mp->at_گهٽ_کان_گهٽ > 0);

        إذا (mp->freelist) {
                فراغ *t;

                t = mp->freelist;
                mp->freelist = *(فراغ**) mp->freelist;
                أرجع t;
        }

        إذا (_unlikely_(!mp->پهريون_pool) ||
            _unlikely_(mp->پهريون_pool->n_استعمال_ٿيل >= mp->پهريون_pool->n_tiles)) {
                size_t سائيز, n;
                بنية pool *p;

                n = mp->پهريون_pool ? mp->پهريون_pool->n_tiles : 0;
                n = MAX(mp->at_گهٽ_کان_گهٽ, n * 2);
                سائيز = صفحو_ALIGN(ALIGN(حجم(بنية pool)) + n*mp->tile_سائيز);
                n = (سائيز - ALIGN(حجم(بنية pool))) / mp->tile_سائيز;

                p = يادگيري_ڏيو(سائيز);
                إذا (!p)
                        أرجع عدم;

                p->ايندڙ_2 = mp->پهريون_pool;
                p->n_tiles = n;
                p->n_استعمال_ٿيل = 0;

                mp->پهريون_pool = p;
        }

        i = mp->پهريون_pool->n_استعمال_ٿيل++;

        أرجع (uint8_t*) pool_ptr(mp->پهريون_pool) + i*mp->tile_سائيز;
}

فراغ* mempool_alloc0_tile(بنية mempool *mp) {
        فراغ *p;

        أكد(mp);

        p = mempool_alloc_tile(mp);
        إذا (p)
                memzero(p, mp->tile_سائيز);
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

ساكن bool pool_شامل_آهي(بنية mempool *mp, بنية pool *p, فراغ *ptr) {
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
        إذا (off >= mp->tile_سائيز * p->n_tiles)
                أرجع false;

        أكد(off % mp->tile_سائيز == 0);
        أرجع true;
}

ساكن bool pool_is_اڻ_استعمال_ٿيل(بنية mempool *mp, بنية pool *p) {
        أكد(mp);
        أكد(p);

        إذا (p->n_استعمال_ٿيل == 0)
                أرجع true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        فراغ *i = mp->freelist;
        طالما (i) {
                إذا (pool_شامل_آهي(mp, p, i))
                        n++;

                i = *(فراغ**) i;
        }

        أكد(n <= p->n_استعمال_ٿيل);

        أرجع n == p->n_استعمال_ٿيل;
}

ساكن فراغ pool_unlink(بنية mempool *mp, بنية pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        إذا (p->n_استعمال_ٿيل == 0)
                أرجع;

        فراغ **i = &mp->freelist;
        طالما (*i) {
                فراغ *d = *i;

                إذا (pool_شامل_آهي(mp, p, d)) {
                        *i = *(فراغ**) d;
                        m++;

                        إذا (m == p->n_استعمال_ٿيل)
                                اكسر;
                } وإلا
                        i = (فراغ**) d;
        }
}

فراغ mempool_trim(بنية mempool *mp) {
        size_t trimmed = 0, کاٻو = 0;

        أكد(mp);

        بنية pool **p = &mp->پهريون_pool;
        طالما (*p) {
                بنية pool *d = *p;

                إذا (pool_is_اڻ_استعمال_ٿيل(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_سائيز;
                        pool_unlink(mp, d);
                        *p = d->ايندڙ_2;
                        حرر(d);
                } وإلا {
                        کاٻو += d->n_tiles * mp->tile_سائيز;
                        p = &d->ايندڙ_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_بائيٽون(trimmed), mp, FORMAT_بائيٽون(کاٻو));
}
