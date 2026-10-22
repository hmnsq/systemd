/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
प्रकारनिर्देश संरचना Iterator {
        ध्रुव शून्य *next_कुंजी; /* expected value of that entry's key pointer */
        अचिन्हित idx;         /* index of an entry to be iterated next */
#यदि ENABLE_DEBUG_HASHMAP
        अचिन्हित put_गिनती;   /* hashmap's put_count recorded at start of iteration */
        अचिन्हित rem_गिनती;   /* hashmap's rem_count in previous iteration */
        अचिन्हित prev_idx;    /* idx in previous iteration */
#endif
} Iterator;

#define _IDX_ITERATOR_FIRST (अच_पूर्णांक_उच्च - 1)
#define ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .next_कुंजी = रिक्त })
#define ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
