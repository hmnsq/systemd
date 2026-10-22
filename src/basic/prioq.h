/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع بنية Prioq Prioq;

#تعریف_کن PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

صحيح prioq_put(Prioq *q, فراغ *داده, بیعلامت *idx);
صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *داده, بیعلامت *idx);
#تعریف_کن prioq_ensure_put(q, compare_func, داده, idx)                    \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((داده)[0])*, ثابت typeof((داده)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, داده, idx); \
        })

صحيح prioq_remove(Prioq *q, فراغ *داده, بیعلامت *idx);
فراغ prioq_reshuffle(Prioq *q, فراغ *داده, بیعلامت *idx);

فراغ* prioq_peek_by_شاخص(Prioq *q, بیعلامت idx) خالص;
ساكن مضمن فراغ *prioq_peek(Prioq *q) {
        أرجع prioq_peek_by_شاخص(q, 0);
}
فراغ* prioq_برداشت(Prioq *q);

#تعریف_کن PRIOQ_FOREACH_قلم(q, p)                                \
        لكل (بیعلامت _i = 0; (p = prioq_peek_by_شاخص(q, _i)); _i++)

بیعلامت prioq_اندازه(Prioq *q) خالص;
bool prioq_isempty(Prioq *q) خالص;
