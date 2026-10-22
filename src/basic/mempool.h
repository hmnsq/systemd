/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

ഘടന pool;

ഘടന mempool {
        ഘടന pool *first_pool;
        ശൂന്യം *freelist;
        size_t tile_size;
        size_t at_least;
};

ശൂന്യം* mempool_alloc_tile(ഘടന mempool *mp);
ശൂന്യം* mempool_alloc0_tile(ഘടന mempool *mp);
ശൂന്യം* mempool_free_tile(ഘടന mempool *mp, ശൂന്യം *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
സ്ഥിതം ഘടന mempool pool_name = { \
        .tile_size = വലുപ്പം(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(ശൂന്യം) _weak_ _pure_;

ശൂന്യം mempool_trim(ഘടന mempool *mp);
