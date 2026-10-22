/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
પ્રકારવ્યાખ્યા સંરચના Iterator {
        અચલ શૂન્ય *next_ચાવી; /* expected value of that entry's key pointer */
        unsigned idx;         /* index of an entry to be iterated next */
#જો ENABLE_DEBUG_HASHMAP
        unsigned put_ગણતરી;   /* hashmap's put_count recorded at start of iteration */
        unsigned rem_ગણતરી;   /* hashmap's rem_count in previous iteration */
        unsigned prev_idx;    /* idx in previous iteration */
#endif
} Iterator;

#define _IDX_ITERATOR_FIRST (UINT_MAX - 1)
#define ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .next_ચાવી = ખાલી })
#define ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
