/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
تعريفنوع بنية Iterator {
        ثابت فراغ *ايندڙ_ڪنجي; /* expected value of that entry's key pointer */
        بنانشان idx;         /* index of an entry to be iterated next */
#إذا ENABLE_DEBUG_HASHMAP
        بنانشان put_ڳڻپ;   /* hashmap's put_count recorded at start of iteration */
        بنانشان rem_ڳڻپ;   /* hashmap's rem_count in previous iteration */
        بنانشان prev_idx;    /* idx in previous iteration */
#جيڪڏهن_پورو
} Iterator;

#وصف_ڪريو IDX_ITERATOR_پهريون (UINT_MAX - 1)
#وصف_ڪريو ITERATOR_پهريون ((Iterator) { .idx = IDX_ITERATOR_پهريون, .ايندڙ_ڪنجي = عدم })
#وصف_ڪريو ITERATOR_IS_پهريون(i) ((i).idx == IDX_ITERATOR_پهريون)
