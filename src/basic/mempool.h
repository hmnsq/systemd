/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಂರಚನಾ pool;

ಸಂರಚನಾ mempool {
        ಸಂರಚನಾ pool *first_pool;
        ಶೂನ್ಯ *freelist;
        size_t tile_size;
        size_t at_least;
};

ಶೂನ್ಯ* mempool_alloc_tile(ಸಂರಚನಾ mempool *mp);
ಶೂನ್ಯ* mempool_alloc0_tile(ಸಂರಚನಾ mempool *mp);
ಶೂನ್ಯ* mempool_free_tile(ಸಂರಚನಾ mempool *mp, ಶೂನ್ಯ *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
ಸ್ಥಿರ ಸಂರಚನಾ mempool pool_name = { \
        .tile_size = ಪರಿಮಾಣ(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(ಶೂನ್ಯ) _weak_ _pure_;

ಶೂನ್ಯ mempool_trim(ಸಂರಚನಾ mempool *mp);
