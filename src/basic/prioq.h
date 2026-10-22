/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع ساختار Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
صحیح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

صحیح prioq_put(Prioq *q, تهی *data, unsigned *idx);
صحیح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, تهی *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                صحیح (*_func_)(ثابت typeof((data)[0])*, ثابت typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

صحیح prioq_remove(Prioq *q, تهی *data, unsigned *idx);
تهی prioq_reshuffle(Prioq *q, تهی *data, unsigned *idx);

تهی* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
ایستا درونخطی تهی *prioq_peek(Prioq *q) {
        بازگشت prioq_peek_by_index(q, 0);
}
تهی* prioq_برداشت(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        برای (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
