/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Prioq Prioq;

#ವ್ಯಾಖ್ಯಾನಿಸು PRIOQ_IDX_NULL (UINT_MAX)

Prioq* prioq_new(compare_func_t compare);
Prioq* prioq_free(Prioq *q);
DEFINE_TRIVIAL_CLEANUP_FUNC(Prioq*, prioq_free);
ಪೂರ್ಣಾಂಕ prioq_ensure_allocated(Prioq **q, compare_func_t compare_func);

ಪೂರ್ಣಾಂಕ prioq_put(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx);
ಪೂರ್ಣಾಂಕ _prioq_ensure_put(Prioq **q, compare_func_t compare_func, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx);
#ವ್ಯಾಖ್ಯಾನಿಸು prioq_ensure_put(q, compare_func, ದತ್ತಾಂಶ, idx)                    \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ (*_func_)(ಸ್ಥಿರಾಂಕ typeof((ದತ್ತಾಂಶ)[0])*, ಸ್ಥಿರಾಂಕ typeof((ದತ್ತಾಂಶ)[0])*) = compare_func; \
                _prioq_ensure_put(q, (compare_func_t) _func_, ದತ್ತಾಂಶ, idx); \
        })

ಪೂರ್ಣಾಂಕ prioq_remove(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx);
ಶೂನ್ಯ prioq_reshuffle(Prioq *q, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ *idx);

ಶೂನ್ಯ* prioq_peek_by_ಸೂಚ್ಯಂಕ(Prioq *q, ಚಿಹ್ನೆರಹಿತ idx) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *prioq_peek(Prioq *q) {
        ಹಿಂತಿರುಗಿ prioq_peek_by_ಸೂಚ್ಯಂಕ(q, 0);
}
ಶೂನ್ಯ* prioq_ತೆಗೆ(Prioq *q);

#ವ್ಯಾಖ್ಯಾನಿಸು PRIOQ_FOREACH_ಅಂಶ(q, p)                                \
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ _i = 0; (p = prioq_peek_by_ಸೂಚ್ಯಂಕ(q, _i)); _i++)

ಚಿಹ್ನೆರಹಿತ prioq_ಗಾತ್ರ(Prioq *q) ಶುದ್ಧ;
bool prioq_isempty(Prioq *q) ಶುದ್ಧ;
