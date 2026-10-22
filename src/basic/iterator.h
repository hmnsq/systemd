/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Iterator {
        ଅଚଳ ଶୂନ୍ୟ *next_ଚାବି; /* expected value of that entry's key pointer */
        unsigned idx;         /* index of an entry to be iterated next */
#ଯଦି ENABLE_DEBUG_HASHMAP
        unsigned put_ଗଣନା;   /* hashmap's put_count recorded at start of iteration */
        unsigned rem_ଗଣନା;   /* hashmap's rem_count in previous iteration */
        unsigned prev_idx;    /* idx in previous iteration */
#endif
} Iterator;

#define _IDX_ITERATOR_FIRST (UINT_MAX - 1)
#define ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .next_ଚାବି = ଖାଲି })
#define ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
