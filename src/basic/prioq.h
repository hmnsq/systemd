/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prioq_put(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ଶୂନ୍ୟ *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*_func_)(ଅଚଳ typeof((data)[0])*, ଅଚଳ typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prioq_remove(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx);
ଶୂନ୍ୟ prioq_reshuffle(Prioq *q, ଶୂନ୍ୟ *data, unsigned *idx);

ଶୂନ୍ୟ* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ *prioq_peek(Prioq *q) {
        ଫେରନ୍ତୁ prioq_peek_by_index(q, 0);
}
ଶୂନ୍ୟ* prioq_କାଢ଼(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        ପାଇଁ (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
