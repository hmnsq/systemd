/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

ساختار pool;

ساختار mempool {
        ساختار pool *first_pool;
        تهی *freelist;
        size_t tile_size;
        size_t at_least;
};

تهی* mempool_alloc_tile(ساختار mempool *mp);
تهی* mempool_alloc0_tile(ساختار mempool *mp);
تهی* mempool_free_tile(ساختار mempool *mp, تهی *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
ایستا ساختار mempool pool_name = { \
        .tile_size = اندازه(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(تهی) _weak_ _pure_;

تهی mempool_trim(ساختار mempool *mp);
