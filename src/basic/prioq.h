/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ কাঠামো Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
পূর্ণসংখ্যা prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

পূর্ণসংখ্যা prioq_put(Prioq *q, শূন্য *তথ্য, unsigned *idx);
পূর্ণসংখ্যা _prioq_ensure_put(Prioq **q, compare_func_t compare_func, শূন্য *তথ্য, unsigned *idx);
#define prioq_ensure_put(q, compare_func, তথ্য, idx)                    \
        ({                                                              \
                পূর্ণসংখ্যা (*_func_)(ধ্রুবক typeof((তথ্য)[0])*, ধ্রুবক typeof((তথ্য)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, তথ্য, idx); \
        })

পূর্ণসংখ্যা prioq_remove(Prioq *q, শূন্য *তথ্য, unsigned *idx);
শূন্য prioq_reshuffle(Prioq *q, শূন্য *তথ্য, unsigned *idx);

শূন্য* prioq_peek_by_সূচক(Prioq *q, unsigned idx) _pure_;
স্থির অন্তঃরেখ শূন্য *prioq_peek(Prioq *q) {
        ফেরত prioq_peek_by_সূচক(q, 0);
}
শূন্য* prioq_বের_করো(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        জন্য (unsigned _i = 0; (p = prioq_peek_by_সূচক(q, _i)); _i++)

unsigned prioq_আকার(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
