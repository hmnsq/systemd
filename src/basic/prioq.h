/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന Prioq Prioq;

#define PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
പൂർണ്ണസംഖ്യ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

പൂർണ്ണസംഖ്യ prioq_put(Prioq *q, ശൂന്യം *data, unsigned *idx);
പൂർണ്ണസംഖ്യ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ശൂന്യം *data, unsigned *idx);
#define prioq_ensure_put(q, compare_func, data, idx)                    \
        ({                                                              \
                പൂർണ്ണസംഖ്യ (*_func_)(സ്ഥിരം typeof((data)[0])*, സ്ഥിരം typeof((data)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, data, idx); \
        })

പൂർണ്ണസംഖ്യ prioq_remove(Prioq *q, ശൂന്യം *data, unsigned *idx);
ശൂന്യം prioq_reshuffle(Prioq *q, ശൂന്യം *data, unsigned *idx);

ശൂന്യം* prioq_peek_by_index(Prioq *q, unsigned idx) _pure_;
സ്ഥിതം വരിയിൽ ശൂന്യം *prioq_peek(Prioq *q) {
        തിരികെ prioq_peek_by_index(q, 0);
}
ശൂന്യം* prioq_എടുക്കുക(Prioq *q);

#define PRIOQ_FOREACH_ITEM(q, p)                                \
        വേണ്ടി (unsigned _i = 0; (p = prioq_peek_by_index(q, _i)); _i++)

unsigned prioq_size(Prioq *q) _pure_;
bool prioq_isempty(Prioq *q) _pure_;
