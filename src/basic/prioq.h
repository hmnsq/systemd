/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنية Prioq Prioq;

#تعریف_کرو PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
صحيح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

صحيح prioq_put(Prioq *q, فراغ *انکڑے, بنانشانی *idx);
صحيح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, فراغ *انکڑے, بنانشانی *idx);
#تعریف_کرو prioq_ensure_put(q, compare_func, انکڑے, idx)                    \
        ({                                                              \
                صحيح (*_func_)(ثابت typeof((انکڑے)[0])*, ثابت typeof((انکڑے)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, انکڑے, idx); \
        })

صحيح prioq_remove(Prioq *q, فراغ *انکڑے, بنانشانی *idx);
فراغ prioq_reshuffle(Prioq *q, فراغ *انکڑے, بنانشانی *idx);

فراغ* prioq_peek_by_اشاریہ(Prioq *q, بنانشانی idx) خالص;
ساكن مضمن فراغ *prioq_peek(Prioq *q) {
        أرجع prioq_peek_by_اشاریہ(q, 0);
}
فراغ* prioq_نکالو(Prioq *q);

#تعریف_کرو PRIOQ_FOREACH_شے(q, p)                                \
        لكل (بنانشانی _i = 0; (p = prioq_peek_by_اشاریہ(q, _i)); _i++)

بنانشانی prioq_ناپ(Prioq *q) خالص;
bool prioq_isempty(Prioq *q) خالص;
