/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع بنية Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

صحيح prioq_put(Prioq *q, فراغ *data, unsigned *idx);
صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((data)[0])*, ثابت typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

صحيح prioq_remove(Prioq *q, فراغ *data, unsigned *idx);
فراغ prioq_reshuffle(Prioq *q, فراغ *data, unsigned *idx);

فراغ* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
ساكن مضمن فراغ *prioq_peek(Prioq *q) {
        החזר prioq_peek_by_index(q, 0);
}
فراغ* prioq_pop(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        עבור (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
