/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع بنية Prioq Prioq;

#وصف_ڪريو PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

صحيح prioq_put(Prioq *q, فراغ *معلومات, بنانشان *idx);
صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *معلومات, بنانشان *idx);
#وصف_ڪريو prioq_ensure_put(q, compare_func, معلومات, idx)                    \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((معلومات)[0])*, ثابت typeof((معلومات)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, معلومات, idx); \
        })

صحيح prioq_remove(Prioq *q, فراغ *معلومات, بنانشان *idx);
فراغ prioq_reshuffle(Prioq *q, فراغ *معلومات, بنانشان *idx);

فراغ* prioq_peek_by_اشاريو(Prioq *q, بنانشان idx) خالص;
ساكن مضمن فراغ *prioq_peek(Prioq *q) {
        أرجع prioq_peek_by_اشاريو(q, 0);
}
فراغ* prioq_ڪڍو(Prioq *q);

#وصف_ڪريو PRIOQ_FOREACH_شي(q, p)                                \
        لكل (بنانشان _i = 0; (p = prioq_peek_by_اشاريو(q, _i)); _i++)

بنانشان prioq_سائيز(Prioq *q) خالص;
bool prioq_isempty(Prioq *q) خالص;
