/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

ساخت pool;

ساخت mempool {
        ساخت pool *پہلا_pool;
        خالی *freelist;
        size_t tile_سائز;
        size_t at_کم_سے_کم;
};

خالی* mempool_alloc_tile(ساخت mempool *mp);
خالی* mempool_alloc0_tile(ساخت mempool *mp);
خالی* mempool_free_tile(ساخت mempool *mp, خالی *p);

#تعریف_کرو DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_کم_سے_کم) \
ساکن ساخت mempool pool_name = { \
        .tile_سائز = حجم(tile_type), \
        .at_کم_سے_کم = alloc_at_کم_سے_کم, \
}

bool mempool_enabled(خالی) _weak_ خالص;

خالی mempool_trim(ساخت mempool *mp);
