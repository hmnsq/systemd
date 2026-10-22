/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

সংরচনা pool;

সংরচনা mempool {
        সংরচনা pool *first_pool;
        শূন্য *freelist;
        size_t tile_size;
        size_t at_least;
};

শূন্য* mempool_alloc_tile(সংরচনা mempool *mp);
শূন্য* mempool_alloc0_tile(সংরচনা mempool *mp);
শূন্য* mempool_free_tile(সংরচনা mempool *mp, শূন্য *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
স্থির সংরচনা mempool pool_name = { \
        .tile_size = পরিমাণ(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(শূন্য) _weak_ _pure_;

শূন্য mempool_trim(সংরচনা mempool *mp);
