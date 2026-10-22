/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना pool;

संरचना mempool {
        संरचना pool *first_pool;
        शून्य *freelist;
        माप_प्रकार tile_size;
        माप_प्रकार at_least;
};

शून्य* mempool_alloc_tile(संरचना mempool *mp);
शून्य* mempool_alloc0_tile(संरचना mempool *mp);
शून्य* mempool_free_tile(संरचना mempool *mp, शून्य *p);

#define DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
स्थिर संरचना mempool pool_name = { \
        .tile_size = परिमाण(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(शून्य) _weak_ _pure_;

शून्य mempool_trim(संरचना mempool *mp);
