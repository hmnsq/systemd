/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف ساخت Prioq Prioq;

#تعریف_کرو PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
صحیح prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

صحیح prioq_put(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx);
صحیح _prioq_ensure_put(Prioq **q, compare_func_t compare_func, خالی *ڈیٹا, غیرعلامتی *idx);
#تعریف_کرو prioq_ensure_put(q, compare_func, ڈیٹا, idx)                    \
        ({                                                              \
                صحیح (*_func_)(ثابت typeof((ڈیٹا)[0])*, ثابت typeof((ڈیٹا)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, ڈیٹا, idx); \
        })

صحیح prioq_remove(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx);
خالی prioq_reshuffle(Prioq *q, خالی *ڈیٹا, غیرعلامتی *idx);

خالی* prioq_peek_by_اشاریہ(Prioq *q, غیرعلامتی idx) خالص;
ساکن اندرونی خالی *prioq_peek(Prioq *q) {
        واپس prioq_peek_by_اشاریہ(q, 0);
}
خالی* prioq_نکالو(Prioq *q);

#تعریف_کرو PRIOQ_FOREACH_شے(q, p)                                \
        برائے (غیرعلامتی _i = 0; (p = prioq_peek_by_اشاریہ(q, _i)); _i++)

غیرعلامتی prioq_سائز(Prioq *q) خالص;
bool prioq_isempty(Prioq *q) خالص;
