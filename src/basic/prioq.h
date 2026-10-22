/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை கட்டமைப்பு Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
முழு prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

முழு prioq_put(Prioq *q, வெறுமை *data, unsigned *idx);
முழு _prioq_ensure_put(Prioq **q, compare_func_t compare_func, வெறுமை *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                முழு (*_func_)(மாறிலி typeof((data)[0])*, மாறிலி typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

முழு prioq_remove(Prioq *q, வெறுமை *data, unsigned *idx);
வெறுமை prioq_reshuffle(Prioq *q, வெறுமை *data, unsigned *idx);

வெறுமை* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
நிலையான உள்வரி வெறுமை *prioq_peek(Prioq *q) {
        திரும்பு prioq_peek_by_index(q, 0);
}
வெறுமை* prioq_எடு(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        க்கு (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
