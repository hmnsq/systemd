/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

بنية pool;

بنية mempool {
        بنية pool *پهريون_pool;
        فراغ *freelist;
        size_t tile_سائيز;
        size_t at_گهٽ_کان_گهٽ;
};

فراغ* mempool_alloc_tile(بنية mempool *mp);
فراغ* mempool_alloc0_tile(بنية mempool *mp);
فراغ* mempool_free_tile(بنية mempool *mp, فراغ *p);

#وصف_ڪريو DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_گهٽ_کان_گهٽ) \
ساكن بنية mempool pool_name = { \
        .tile_سائيز = حجم(tile_type), \
        .at_گهٽ_کان_گهٽ = alloc_at_گهٽ_کان_گهٽ, \
}

bool mempool_enabled(فراغ) _weak_ خالص;

فراغ mempool_trim(بنية mempool *mp);
