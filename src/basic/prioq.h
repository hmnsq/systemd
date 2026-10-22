/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना Prioq Prioq;

#परिभाषय PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
पूर्णाङ्क prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

पूर्णाङ्क prioq_put(Prioq *q, शून्य *दत्त, अचिह्नित *idx);
पूर्णाङ्क _prioq_ensure_put(Prioq **q, compare_func_t compare_func, शून्य *दत्त, अचिह्नित *idx);
#परिभाषय prioq_ensure_put(q, compare_func, दत्त, idx)                    \
        ({                                                              \
                पूर्णाङ्क (*_func_)(ध्रुव typeof((दत्त)[0])*, ध्रुव typeof((दत्त)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, दत्त, idx); \
        })

पूर्णाङ्क prioq_remove(Prioq *q, शून्य *दत्त, अचिह्नित *idx);
शून्य prioq_reshuffle(Prioq *q, शून्य *दत्त, अचिह्नित *idx);

शून्य* prioq_peek_by_सूचकांक(Prioq *q, अचिह्नित idx) विशुद्धम्;
स्थिर अन्तःस्थ शून्य *prioq_peek(Prioq *q) {
        प्रत्यागम prioq_peek_by_सूचकांक(q, 0);
}
शून्य* prioq_निष्कासय(Prioq *q);

#परिभाषय PRIOQ_FOREACH_ITEM(q, p)                                \
        प्रति (अचिह्नित _i = 0; (p = prioq_peek_by_सूचकांक(q, _i)); _i++)

अचिह्नित prioq_आकार(Prioq *q) विशुद्धम्;
bool prioq_isempty(Prioq *q) विशुद्धम्;
