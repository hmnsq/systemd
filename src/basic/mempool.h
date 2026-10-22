/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

నిర్మాణం pool;

నిర్మాణం mempool {
        నిర్మాణం pool *మొదటి_pool;
        శూన్య *freelist;
        size_t tile_కొలత;
        size_t at_కనిష్ఠ;
};

శూన్య* mempool_alloc_tile(నిర్మాణం mempool *mp);
శూన్య* mempool_alloc0_tile(నిర్మాణం mempool *mp);
శూన్య* mempool_free_tile(నిర్మాణం mempool *mp, శూన్య *p);

#నిర్వచించు DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_కనిష్ఠ) \
స్థిర నిర్మాణం mempool pool_name = { \
        .tile_కొలత = పరిమాణం(tile_type), \
        .at_కనిష్ఠ = alloc_at_కనిష్ఠ, \
}

bool mempool_enabled(శూన్య) _weak_ శుద్ధ;

శూన్య mempool_trim(నిర్మాణం mempool *mp);
