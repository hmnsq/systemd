/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

কাঠামো pool;

কাঠামো mempool {
        কাঠামো pool *first_pool;
        শূন্য *freelist;
        size_t tile_আকার;
        size_t at_least;
};

শূন্য* mempool_alloc_tile(কাঠামো mempool *mp);
শূন্য* mempool_alloc0_tile(কাঠামো mempool *mp);
শূন্য* mempool_free_tile(কাঠামো mempool *mp, শূন্য *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
স্থির কাঠামো mempool pool_name = { \
        .tile_আকার = আকার(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(শূন্য) _weak_ _pure_;

শূন্য mempool_trim(কাঠামো mempool *mp);
