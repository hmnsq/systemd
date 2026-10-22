/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
प्रकारनिर्देश संरचना Iterator {
        ध्रुव शून्य *next_किल्ली; /* expected value of that entry's key pointer */
        unsigned idx;         /* index of an entry to be iterated next */
#यदि ENABLE_DEBUG_HASHMAP
        unsigned put_मोजणी;   /* hashmap's put_count recorded at start of iteration */
        unsigned rem_मोजणी;   /* hashmap's rem_count in previous iteration */
        unsigned prev_idx;    /* idx in previous iteration */
#endif
} Iterator;

#define _IDX_ITERATOR_FIRST (UINT_MAX - 1)
#define ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .next_किल्ली = रिक्त })
#define ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
