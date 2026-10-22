/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ರಚನೆ pool;

ರಚನೆ mempool {
        ರಚನೆ pool *ಮೊದಲ_pool;
        ಶೂನ್ಯ *freelist;
        size_t tile_ಗಾತ್ರ;
        size_t at_ಕನಿಷ್ಠ;
};

ಶೂನ್ಯ* mempool_alloc_tile(ರಚನೆ mempool *mp);
ಶೂನ್ಯ* mempool_alloc0_tile(ರಚನೆ mempool *mp);
ಶೂನ್ಯ* mempool_free_tile(ರಚನೆ mempool *mp, ಶೂನ್ಯ *p);

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_ಕನಿಷ್ಠ) \
ಸ್ಥಿರ ರಚನೆ mempool pool_name = { \
        .tile_ಗಾತ್ರ = ಗಾತ್ರ(tile_type), \
        .at_ಕನಿಷ್ಠ = alloc_at_ಕನಿಷ್ಠ, \
}

bool mempool_enabled(ಶೂನ್ಯ) _weak_ ಶುದ್ಧ;

ಶೂನ್ಯ mempool_trim(ರಚನೆ mempool *mp);
