/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
पूर्णाङ्क prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

पूर्णाङ्क prioq_put(Prioq *q, शून्य *data, unsigned *idx);
पूर्णाङ्क _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((data)[0])*, ध्रुव typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

पूर्णाङ्क prioq_remove(Prioq *q, शून्य *data, unsigned *idx);
शून्य prioq_reshuffle(Prioq *q, शून्य *data, unsigned *idx);

शून्य* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
स्थिर अन्तःस्थ शून्य *prioq_peek(Prioq *q) {
        प्रत्यागम prioq_peek_by_index(q, 0);
}
शून्य* prioq_निकालो(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        प्रति (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
