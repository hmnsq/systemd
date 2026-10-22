/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
प्रकारनिर्देश संरचना Iterator {
        ध्रुव शून्य *अग्र_कुञ्चिका; /* expected value of that entry's key pointer */
        अचिह्नित idx;         /* index of an entry to be iterated next */
#यदि ENABLE_DEBUG_HASHMAP
        अचिह्नित put_गणनम्;   /* hashmap's put_count recorded at start of iteration */
        अचिह्नित rem_गणनम्;   /* hashmap's rem_count in previous iteration */
        अचिह्नित पूर्व_idx;    /* idx in previous iteration */
#यदि_अन्तः
} Iterator;

#परिभाषय _IDX_ITERATOR_FIRST (UINT_MAX - 1)
#परिभाषय ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .अग्र_कुञ्चिका = रिक्त })
#परिभाषय ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
