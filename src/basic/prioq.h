/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार काष्ठा Prioq Prioq;

#घोषणा PRIOQ_IDX_NULL (अच_पूर्णांक_उच्च)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
पूर्णांक prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

पूर्णांक prioq_put(Prioq *q, व्योम *तथ्य, अचिन्हित *idx);
पूर्णांक _prioq_ensure_put(Prioq **q, compare_func_t compare_func, व्योम *तथ्य, अचिन्हित *idx);
#घोषणा prioq_ensure_put(q, compare_func, तथ्य, idx)                    \
        ({                                                              \
                पूर्णांक (*_func_)(स्थिर typeof((तथ्य)[0])*, स्थिर typeof((तथ्य)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, तथ्य, idx); \
        })

पूर्णांक prioq_remove(Prioq *q, व्योम *तथ्य, अचिन्हित *idx);
व्योम prioq_reshuffle(Prioq *q, व्योम *तथ्य, अचिन्हित *idx);

व्योम* prioq_peek_by_सूचकांक(Prioq *q, अचिन्हित idx) _pure_;
अटल अंतरभूत व्योम *prioq_peek(Prioq *q) {
        वापस prioq_peek_by_सूचकांक(q, 0);
}
व्योम* prioq_निकालो(Prioq *q);

#घोषणा PRIOQ_FOREACH_ITEM(q, p)                                \
        क्रम (अचिन्हित _i = 0; (p = prioq_peek_by_सूचकांक(q, _i)); _i++)

अचिन्हित prioq_आकार(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
