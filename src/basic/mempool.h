/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

بنية pool;

بنية mempool {
        بنية pool *اولی_pool;
        فراغ *freelist;
        size_t tile_اندازه;
        size_t at_کمترین;
};

فراغ* mempool_alloc_tile(بنية mempool *mp);
فراغ* mempool_alloc0_tile(بنية mempool *mp);
فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p);

#تعریف_کن DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_کمترین) \
ساكن بنية mempool pool_name = { \
        .tile_اندازه = حجم(tile_type), \
        .at_کمترین = alloc_at_کمترین, \
}

bool mempool_enabled(فراغ) _weak_ خالص;

فراغ mempool_trim(بنية mempool *mp);
