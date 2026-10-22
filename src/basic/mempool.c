/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>

#شامل "format-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "mempool.h"

ساخت pool {
        ساخت pool *اگلا_2;
        size_t n_tiles;
        size_t n_مستعمل;
};

ساکن خالی* pool_ptr(ساخت pool *p) {
        واپس ((uint8_t*) ASSERT_PTR(p)) + ALIGN(حجم(ساخت pool));
}

خالی* mempool_alloc_tile(ساخت mempool *mp) {
        size_t i;

        /* When a tile is released we add it to the list and simply
         * place the next pointer at its offset 0. */

        أكد(mp);
        أكد(mp->tile_سائز >= حجم(خالی*));
        أكد(mp->at_کم_سے_کم > 0);

        اگر (mp->freelist) {
                خالی *t;

                t = mp->freelist;
                mp->freelist = *(خالی**) mp->freelist;
                واپس t;
        }

        اگر (_unlikely_(!mp->پہلا_pool) ||
            _unlikely_(mp->پہلا_pool->n_مستعمل >= mp->پہلا_pool->n_tiles)) {
                size_t سائز, n;
                ساخت pool *p;

                n = mp->پہلا_pool ? mp->پہلا_pool->n_tiles : 0;
                n = MAX(mp->at_کم_سے_کم, n * 2);
                سائز = صفحہ_ALIGN(ALIGN(حجم(ساخت pool)) + n*mp->tile_سائز);
                n = (سائز - ALIGN(حجم(ساخت pool))) / mp->tile_سائز;

                p = حافظہ_دو(سائز);
                اگر (!p)
                        واپس تہی;

                p->اگلا_2 = mp->پہلا_pool;
                p->n_tiles = n;
                p->n_مستعمل = 0;

                mp->پہلا_pool = p;
        }

        i = mp->پہلا_pool->n_مستعمل++;

        واپس (uint8_t*) pool_ptr(mp->پہلا_pool) + i*mp->tile_سائز;
}

خالی* mempool_alloc0_tile(ساخت mempool *mp) {
        خالی *p;

        أكد(mp);

        p = mempool_alloc_tile(mp);
        اگر (p)
                memzero(p, mp->tile_سائز);
        واپس p;
}

خالی* mempool_free_tile(ساخت mempool *mp, خالی *p) {
        أكد(mp);

        اگر (!p)
                واپس تہی;

        *(خالی**) p = mp->freelist;
        mp->freelist = p;

        واپس تہی;
}

ساکن bool pool_شامل_ہے(ساخت mempool *mp, ساخت pool *p, خالی *ptr) {
        size_t off;
        خالی *a;

        أكد(mp);
        أكد(p);

        اگر (!ptr)
                واپس false;

        a = pool_ptr(p);
        اگر ((uint8_t*) ptr < (uint8_t*) a)
                واپس false;

        off = (uint8_t*) ptr - (uint8_t*) a;
        اگر (off >= mp->tile_سائز * p->n_tiles)
                واپس false;

        أكد(off % mp->tile_سائز == 0);
        واپس true;
}

ساکن bool pool_is_غیر_مستعمل(ساخت mempool *mp, ساخت pool *p) {
        أكد(mp);
        أكد(p);

        اگر (p->n_مستعمل == 0)
                واپس true;

        /* Check if all tiles in this specific pool are in the freelist. */
        size_t n = 0;
        خالی *i = mp->freelist;
        جب (i) {
                اگر (pool_شامل_ہے(mp, p, i))
                        n++;

                i = *(خالی**) i;
        }

        أكد(n <= p->n_مستعمل);

        واپس n == p->n_مستعمل;
}

ساکن خالی pool_unlink(ساخت mempool *mp, ساخت pool *p) {
        size_t m = 0;

        أكد(mp);
        أكد(p);

        اگر (p->n_مستعمل == 0)
                واپس;

        خالی **i = &mp->freelist;
        جب (*i) {
                خالی *d = *i;

                اگر (pool_شامل_ہے(mp, p, d)) {
                        *i = *(خالی**) d;
                        m++;

                        اگر (m == p->n_مستعمل)
                                توڑو;
                } ورنہ
                        i = (خالی**) d;
        }
}

خالی mempool_trim(ساخت mempool *mp) {
        size_t trimmed = 0, بایاں = 0;

        أكد(mp);

        ساخت pool **p = &mp->پہلا_pool;
        جب (*p) {
                ساخت pool *d = *p;

                اگر (pool_is_غیر_مستعمل(mp, d)) {
                        trimmed += d->n_tiles * mp->tile_سائز;
                        pool_unlink(mp, d);
                        *p = d->اگلا_2;
                        آزاد(d);
                } ورنہ {
                        بایاں += d->n_tiles * mp->tile_سائز;
                        p = &d->اگلا_2;
                }
        }

        log_debug("Trimmed %s from memory pool %p. (%s left)", FORMAT_بائٹیں(trimmed), mp, FORMAT_بائٹیں(بایاں));
}
