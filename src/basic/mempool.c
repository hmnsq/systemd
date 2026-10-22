/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "mempool.h"

بنية pool {
        بنية pool *اگلا_2;
        size_t n_tiles;
        size_t n_ورتیا;
};

ساكن فراغ* pool_ptr(بنية pool *p) {
        أرجع ((uint8_t*) ASSERT_PTR(p)) + ALIGN(حجم(بنية pool));
}

فراغ* mempool_alloc_tile(بنية mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        أكد(mp);
        أكد(mp->tile_ناپ >= حجم(فراغ*));
        أكد(mp->at_گھٹ_توں_گھٹ > 0);

        إذا (mp->freelist) {
                فراغ *t;

                t = mp->freelist;
                mp->freelist = *(فراغ**) mp->freelist;
                أرجع t;
        }

        إذا (_unlikely_(!mp->پہلا_pool) ||
            _unlikely_(mp->پہلا_pool->n_ورتیا >= mp->پہلا_pool->n_tiles)) {
                size_t ناپ, n;
                بنية pool *p;

                n = mp->پہلا_pool ? mp->پہلا_pool->n_tiles : 0;
                n = MAX(mp->at_گھٹ_توں_گھٹ, n * 2);
                ناپ = ورقہ_ALIGN(ALIGN(حجم(بنية pool)) + n*mp->tile_ناپ);
                n = (ناپ - ALIGN(حجم(بنية pool))) / mp->tile_ناپ;

                p = حافظہ_دیو(ناپ);
                إذا (!p)
                        أرجع عدم;

                p->اگلا_2 = mp->پہلا_pool;
                p->n_tiles = n;
                p->n_ورتیا = 0;

                mp->پہلا_pool = p;
        }

        i = mp->پہلا_pool->n_ورتیا++;

        أرجع (uint8_t*) pool_ptr(mp->پہلا_pool) + i*mp->tile_ناپ;
}

فراغ* mempool_alloc0_tile(بنية mempool *mp) {
        فراغ *p;

        أكد(mp);

        p = mempool_alloc_tile(mp);
        إذا (p)
                memzero(p, mp->tile_ناپ);
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

ساكن bool pool_شامل_اے(بنية mempool *mp, بنية pool *p, فراغ *ptr) {
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
        إذا (off >= mp->tile_ناپ * p->n_tiles)
                أرجع false;

        أكد(off % mp->tile_ناپ == 0);
        أرجع true;
}

ساكن bool pool_is_بن_ورتیا(بنية mempool *mp, بنية pool *p) {
        أكد(mp);
        أكد(p);

        إذا (p->n_ورتیا == 0)
                أرجع true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        فراغ *i = mp->freelist;
        طالما (i) {
                إذا (pool_شامل_اے(mp, p, i))
                        n++;

                i = *(فراغ**) i;
        }

        أكد(n <= p->n_ورتیا);

        أرجع n == p->n_ورتیا;
}

ساكن فراغ pool_unlink(بنية mempool *mp, بنية pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        إذا (p->n_ورتیا == 0)
                أرجع;

        فراغ **i = &mp->freelist;
        طالما (*i) {
                فراغ *d = *i;

                إذا (pool_شامل_اے(mp, p, d)) {
                        *i = *(فراغ**) d;
                        m++;

                        إذا (m == p->n_ورتیا)
                                اكسر;
                } وإلا
                        i = (فراغ**) d;
        }
}

فراغ mempool_trim(بنية mempool *mp) {
        size_t trimmed = 0, بایاں = 0;

        أكد(mp);

        بنية pool **p = &mp->پہلا_pool;
        طالما (*p) {
                بنية pool *d = *p;

                إذا (pool_is_بن_ورتیا(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_ناپ;
                        pool_unlink(mp, d);
                        *p = d->اگلا_2;
                        حرر(d);
                } وإلا {
                        بایاں += d->n_tiles * mp->tile_ناپ;
                        p = &d->اگلا_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_بائٹاں(trimmed), mp, FORMAT_بائٹاں(بایاں));
}
