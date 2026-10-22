/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

بنية pool;

بنية mempool {
        بنية pool *پہلا_pool;
        فراغ *freelist;
        size_t tile_ناپ;
        size_t at_گھٹ_توں_گھٹ;
};

فراغ* mempool_alloc_tile(بنية mempool *mp);
فراغ* mempool_alloc0_tile(بنية mempool *mp);
فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p);

#تعریف_کرو DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_گھٹ_توں_گھٹ) \
ساكن بنية mempool pool_name = { \
        .tile_ناپ = حجم(tile_type), \
        .at_گھٹ_توں_گھٹ = alloc_at_گھٹ_توں_گھٹ, \
}

bool mempool_enabled(فراغ) _weak_ خالص;

فراغ mempool_trim(بنية mempool *mp);
