/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ সংরচনা Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
পূর্ণাঙ্ক prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

পূর্ণাঙ্ক prioq_put(Prioq *q, শূন্য *data, unsigned *idx);
পূর্ণাঙ্ক _prioq_ensure_put(Prioq **q, compare_func_t compare_func, শূন্য *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                পূর্ণাঙ্ক (*_func_)(ধ্রুৱ typeof((data)[0])*, ধ্রুৱ typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

পূর্ণাঙ্ক prioq_remove(Prioq *q, শূন্য *data, unsigned *idx);
শূন্য prioq_reshuffle(Prioq *q, শূন্য *data, unsigned *idx);

শূন্য* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
স্থির অন্তঃস্থ শূন্য *prioq_peek(Prioq *q) {
        ঘূৰাই prioq_peek_by_index(q, 0);
}
শূন্য* prioq_বেৰ_কৰো(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        বাবে (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
