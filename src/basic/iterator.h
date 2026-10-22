/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Iterator {
        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮುಂದಿನ_ಕೀಲಿ; /* expected value of that entry's key pointer */
        ಚಿಹ್ನೆರಹಿತ idx;         /* index of an entry to be iterated next */
#ಆದರೆ ENABLE_DEBUG_HASHMAP
        ಚಿಹ್ನೆರಹಿತ put_ಎಣಿಕೆ;   /* hashmap's put_count recorded at start of iteration */
        ಚಿಹ್ನೆರಹಿತ rem_ಎಣಿಕೆ;   /* hashmap's rem_count in previous iteration */
        ಚಿಹ್ನೆರಹಿತ prev_idx;    /* idx in previous iteration */
#ಆದರೆ_ಅಂತ್ಯ
} Iterator;

#ವ್ಯಾಖ್ಯಾನಿಸು IDX_ITERATOR_ಮೊದಲ (UINT_MAX - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು ITERATOR_ಮೊದಲ ((Iterator) { .idx = IDX_ITERATOR_ಮೊದಲ, .ಮುಂದಿನ_ಕೀಲಿ = ಖಾಲಿ })
#ವ್ಯಾಖ್ಯಾನಿಸು ITERATOR_IS_ಮೊದಲ(i) ((i).idx == IDX_ITERATOR_ಮೊದಲ)
