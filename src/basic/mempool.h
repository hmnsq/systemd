/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂରଚନା pool;

ସଂରଚନା mempool {
        ସଂରଚନା pool *first_pool;
        ଶୂନ୍ୟ *freelist;
        size_t tile_size;
        size_t at_least;
};

ଶୂନ୍ୟ* mempool_alloc_tile(ସଂରଚନା mempool *mp);
ଶୂନ୍ୟ* mempool_alloc0_tile(ସଂରଚନା mempool *mp);
ଶୂନ୍ୟ* mempool_free_tile(ସଂରଚନା mempool *mp, ଶୂନ୍ୟ *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
ସ୍ଥିର ସଂରଚନା mempool pool_name = { \
        .tile_size = ଆକାର(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(ଶୂନ୍ୟ) _weak_ _pure_;

ଶୂନ୍ୟ mempool_trim(ସଂରଚନା mempool *mp);
