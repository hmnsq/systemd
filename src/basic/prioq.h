/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
ຈຳນວນເຕັມ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

ຈຳນວນເຕັມ prioq_put(Prioq *q, void *data, unsigned *idx);
ຈຳນວນເຕັມ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, void *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                ຈຳນວນເຕັມ (*_func_)(const typeof((data)[0])*, const typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

ຈຳນວນເຕັມ prioq_remove(Prioq *q, void *data, unsigned *idx);
void prioq_reshuffle(Prioq *q, void *data, unsigned *idx);

void* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
static anta_hstha void *prioq_peek(Prioq *q) {
        ສົ່ງຄືນ prioq_peek_by_index(q, 0);
}
void* prioq_pop(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        ສຳລັບ (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
