/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Prioq Prioq;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
ਪੂਰਨ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

ਪੂਰਨ prioq_put(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx);
ਪੂਰਨ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ prioq_ensure_put(q, compare_func, ਅੰਕੜੇ, idx)                    \
        ({                                                              \
                ਪੂਰਨ (*_func_)(ਅਚਲ typeof((ਅੰਕੜੇ)[0])*, ਅਚਲ typeof((ਅੰਕੜੇ)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, ਅੰਕੜੇ, idx); \
        })

ਪੂਰਨ prioq_remove(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx);
ਖਾਲੀ prioq_reshuffle(Prioq *q, ਖਾਲੀ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ *idx);

ਖਾਲੀ* prioq_peek_by_ਸੂਚਕਾਂਕ(Prioq *q, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *prioq_peek(Prioq *q) {
        ਵਾਪਸ prioq_peek_by_ਸੂਚਕਾਂਕ(q, 0);
}
ਖਾਲੀ* prioq_ਕੱਢੋ(Prioq *q);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRIOQ_FOREACH_ਵਸਤੂ(q, p)                                \
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ _i = 0; (p = prioq_peek_by_ਸੂਚਕਾਂਕ(q, _i)); _i++)

ਬਿਨਾਨਿਸ਼ਾਨੀ prioq_ਮਾਪ(Prioq *q) ਸ਼ੁੱਧ;
bool prioq_isempty(Prioq *q) ਸ਼ੁੱਧ;
