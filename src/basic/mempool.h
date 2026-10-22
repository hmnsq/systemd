/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

संरचना pool;

संरचना mempool {
        संरचना pool *first_pool;
        शून्य *freelist;
        size_t tile_आकार;
        size_t at_न्यूनतमम्;
};

शून्य* mempool_alloc_tile(संरचना mempool *mp);
शून्य* mempool_alloc0_tile(संरचना mempool *mp);
शून्य* mempool_free_tile(संरचना mempool *mp, शून्य *p);

#परिभाषय DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_न्यूनतमम्) \
स्थिर संरचना mempool pool_name = { \
        .tile_आकार = परिमाण(tile_type), \
        .at_न्यूनतमम् = alloc_at_न्यूनतमम्, \
}

bool mempool_enabled(शून्य) _weak_ विशुद्धम्;

शून्य mempool_trim(संरचना mempool *mp);
