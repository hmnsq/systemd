/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या संरचना Prioq Prioq;

#define PRIOQ_IDX_NULL (अच_पूर्णांक_उच्च)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
पूर्णांक prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

पूर्णांक prioq_put(Prioq *q, शून्य *data, अचिन्हित *idx);
पूर्णांक _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *data, अचिन्हित *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                पूर्णांक (*_func_)(स्थिरांक typeof((data)[0])*, स्थिरांक typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

पूर्णांक prioq_remove(Prioq *q, शून्य *data, अचिन्हित *idx);
शून्य prioq_reshuffle(Prioq *q, शून्य *data, अचिन्हित *idx);

शून्य* prioq_peek_by_index(Prioq *q, अचिन्हित idx) _pure_;
स्थिर ओळीत शून्य *prioq_peek(Prioq *q) {
        परत prioq_peek_by_index(q, 0);
}
शून्य* prioq_काढा(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        साठी (अचिन्हित _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

अचिन्हित prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
