/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
પૂર્ણાંક prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

પૂર્ણાંક prioq_put(Prioq *q, શૂન્ય *data, unsigned *idx);
પૂર્ણાંક _prioq_ensure_put(Prioq **q, compare_func_t compare_func, શૂન્ય *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                પૂર્ણાંક (*_func_)(અચલ typeof((data)[0])*, અચલ typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

પૂર્ણાંક prioq_remove(Prioq *q, શૂન્ય *data, unsigned *idx);
શૂન્ય prioq_reshuffle(Prioq *q, શૂન્ય *data, unsigned *idx);

શૂન્ય* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
સ્થિર અંતઃરેખ શૂન્ય *prioq_peek(Prioq *q) {
        પાછા prioq_peek_by_index(q, 0);
}
શૂન્ય* prioq_કાઢો(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        માટે (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
