/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਬਣਤਰ pool;

ਬਣਤਰ mempool {
        ਬਣਤਰ pool *ਪਹਿਲਾ_pool;
        ਖਾਲੀ *freelist;
        size_t tile_ਮਾਪ;
        size_t at_ਘੱਟ_ਤੋਂ_ਘੱਟ;
};

ਖਾਲੀ* mempool_alloc_tile(ਬਣਤਰ mempool *mp);
ਖਾਲੀ* mempool_alloc0_tile(ਬਣਤਰ mempool *mp);
ਖਾਲੀ* mempool_free_tile(ਬਣਤਰ mempool *mp, ਖਾਲੀ *p);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_ਘੱਟ_ਤੋਂ_ਘੱਟ) \
ਸਥਿਰ ਬਣਤਰ mempool pool_name = { \
        .tile_ਮਾਪ = ਆਕਾਰ(tile_type), \
        .at_ਘੱਟ_ਤੋਂ_ਘੱਟ = alloc_at_ਘੱਟ_ਤੋਂ_ਘੱਟ, \
}

bool mempool_enabled(ਖਾਲੀ) _weak_ ਸ਼ੁੱਧ;

ਖਾਲੀ mempool_trim(ਬਣਤਰ mempool *mp);
