/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

काष्ठा pool;

काष्ठा mempool {
        काष्ठा pool *first_pool;
        व्योम *freelist;
        माप_प्रकार tile_आकार;
        माप_प्रकार at_least;
};

व्योम* mempool_alloc_tile(काष्ठा mempool *mp);
व्योम* mempool_alloc0_tile(काष्ठा mempool *mp);
व्योम* mempool_free_tile(काष्ठा mempool *mp, व्योम *p);

#घोषणा DEFINE_MEMPOOL(pool_name, tile_type, alloc_at_least) \
अटल काष्ठा mempool pool_name = { \
        .tile_आकार = माप(tile_type), \
        .at_least = alloc_at_least, \
}

bool mempool_enabled(व्योम) _weak_ _pure_;

व्योम mempool_trim(काष्ठा mempool *mp);
