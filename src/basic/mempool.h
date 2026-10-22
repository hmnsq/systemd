/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

بنية pool;

بنية mempool {
        بنية pool *first_pool;
        فراغ *freelist;
        size_t tile_size;
        size_t at_least;
};

فراغ* mempool_alloc_tile(بنية mempool *mp);
فراغ* mempool_alloc0_tile(بنية mempool *mp);
فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
ساكن بنية mempool pool_name = { \
        .tile_size = حجم(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(فراغ) _weak_ _pure_;

فراغ mempool_trim(بنية mempool *mp);
