/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం Prioq Prioq;

#నిర్వచించు PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
పూర్ణాంకం prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

పూర్ణాంకం prioq_put(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx);
పూర్ణాంకం _prioq_ensure_put(Prioq **q, compare_func_t compare_func, శూన్య *దత్తాంశం, గుర్తులేని *idx);
#నిర్వచించు prioq_ensure_put(q, compare_func, దత్తాంశం, idx)                    \
        ({                                                              \
                పూర్ణాంకం (*_func_)(స్థిరరాశి typeof((దత్తాంశం)[0])*, స్థిరరాశి typeof((దత్తాంశం)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, దత్తాంశం, idx); \
        })

పూర్ణాంకం prioq_remove(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx);
శూన్య prioq_reshuffle(Prioq *q, శూన్య *దత్తాంశం, గుర్తులేని *idx);

శూన్య* prioq_peek_by_సూచీ(Prioq *q, గుర్తులేని idx) శుద్ధ;
స్థిర అంతఃవరుస శూన్య *prioq_peek(Prioq *q) {
        తిరిగి prioq_peek_by_సూచీ(q, 0);
}
శూన్య* prioq_తీయి(Prioq *q);

#నిర్వచించు PRIOQ_FOREACH_అంశం(q, p)                                \
        కొరకు (గుర్తులేని _i = 0; (p = prioq_peek_by_సూచీ(q, _i)); _i++)

గుర్తులేని prioq_కొలత(Prioq *q) శుద్ధ;
bool prioq_isempty(Prioq *q) శుద్ధ;
