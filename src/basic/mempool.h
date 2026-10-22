/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

கட்டமைப்பு pool;

கட்டமைப்பு mempool {
        கட்டமைப்பு pool *first_pool;
        வெறுமை *freelist;
        size_t tile_size;
        size_t at_least;
};

வெறுமை* mempool_alloc_tile(கட்டமைப்பு mempool *mp);
வெறுமை* mempool_alloc0_tile(கட்டமைப்பு mempool *mp);
வெறுமை* mempool_free_tile(கட்டமைப்பு mempool *mp, வெறுமை *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
நிலையான கட்டமைப்பு mempool pool_name = { \
        .tile_size = அளவு(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(வெறுமை) _weak_ _pure_;

வெறுமை mempool_trim(கட்டமைப்பு mempool *mp);
