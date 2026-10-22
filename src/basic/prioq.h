/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
ಪೂರ್ಣಾಙ್ಕ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

ಪೂರ್ಣಾಙ್ಕ prioq_put(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx);
ಪೂರ್ಣಾಙ್ಕ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ಶೂನ್ಯ *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ (*_func_)(ಧ್ರುವ typeof((data)[0])*, ಧ್ರುವ typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

ಪೂರ್ಣಾಙ್ಕ prioq_remove(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx);
ಶೂನ್ಯ prioq_reshuffle(Prioq *q, ಶೂನ್ಯ *data, unsigned *idx);

ಶೂನ್ಯ* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *prioq_peek(Prioq *q) {
        ಪ್ರತ್ಯಾಗಮ prioq_peek_by_index(q, 0);
}
ಶೂನ್ಯ* prioq_ತೆಗೆ(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        ಪ್ರತಿ (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
