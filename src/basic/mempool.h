/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

સંરચના pool;

સંરચના mempool {
        સંરચના pool *first_pool;
        શૂન્ય *freelist;
        size_t tile_size;
        size_t at_least;
};

શૂન્ય* mempool_alloc_tile(સંરચના mempool *mp);
શૂન્ય* mempool_alloc0_tile(સંરચના mempool *mp);
શૂન્ય* mempool_free_tile(સંરચના mempool *mp, શૂન્ય *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
સ્થિર સંરચના mempool pool_name = { \
        .tile_size = કદ(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(શૂન્ય) _weak_ _pure_;

શૂન્ય mempool_trim(સંરચના mempool *mp);
