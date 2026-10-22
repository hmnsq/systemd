/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
প্রকারলেখ কাঠামো Iterator {
        ধ্রুবক শূন্য *পরবর্তী_চাবি; /* expected value of that entry's key pointer */
        unsigned idx;         /* index of an entry to be iterated next */
#যদি ENABLE_DEBUG_HASHMAP
        unsigned put_গুনতি;   /* hashmap's put_count recorded at start of iteration */
        unsigned rem_গুনতি;   /* hashmap's rem_count in previous iteration */
        unsigned পূর্ব_idx;    /* idx in previous iteration */
#endif
} Iterator;

#define _IDX_ITERATOR_FIRST (UINT_MAX - 1)
#define ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .পরবর্তী_চাবি = ফাঁকা })
#define ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
