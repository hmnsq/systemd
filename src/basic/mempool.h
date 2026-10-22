/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

بنية pool;

بنية mempool {
        بنية pool *لومړی_pool;
        فراغ *freelist;
        size_t tile_اندازه;
        size_t at_لږترلږه;
};

فراغ* mempool_alloc_tile(بنية mempool *mp);
فراغ* mempool_alloc0_tile(بنية mempool *mp);
فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p);

#تعریف_کړه DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_لږترلږه) \
ساكن بنية mempool pool_name = { \
        .tile_اندازه = حجم(tile_type), \
        .at_لږترلږه = alloc_at_لږترلږه, \
}

bool mempool_enabled(فراغ) _weak_ خالص;

فراغ mempool_trim(بنية mempool *mp);
