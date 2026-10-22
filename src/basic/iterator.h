/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Iterator {
        ਅਚਲ ਖਾਲੀ *ਅਗਲਾ_ਕੁੰਜੀ; /* expected value of that entry's key pointer */
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;         /* index of an entry to be iterated next */
#ਜੇ ENABLE_DEBUG_HASHMAP
        ਬਿਨਾਨਿਸ਼ਾਨੀ put_ਗਿਣਤੀ;   /* hashmap's put_count recorded at start of iteration */
        ਬਿਨਾਨਿਸ਼ਾਨੀ rem_ਗਿਣਤੀ;   /* hashmap's rem_count in previous iteration */
        ਬਿਨਾਨਿਸ਼ਾਨੀ prev_idx;    /* idx in previous iteration */
#ਜੇ_ਅੰਤ
} Iterator;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_ITERATOR_ਪਹਿਲਾ (UINT_MAX - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ITERATOR_ਪਹਿਲਾ ((Iterator) { .idx = IDX_ITERATOR_ਪਹਿਲਾ, .ਅਗਲਾ_ਕੁੰਜੀ = ਨਿਰਾ })
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ITERATOR_IS_ਪਹਿਲਾ(i) ((i).idx == IDX_ITERATOR_ਪਹਿਲਾ)
