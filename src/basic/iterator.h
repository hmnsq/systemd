/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
تعريفنوع بنية Iterator {
        ثابت فراغ *بعدی_کلید; /* expected value of that entry's key pointer */
        بیعلامت idx;         /* index of an entry to be iterated next */
#إذا ENABLE_DEBUG_HASHMAP
        بیعلامت put_شمار;   /* hashmap's put_count recorded at start of iteration */
        بیعلامت rem_شمار;   /* hashmap's rem_count in previous iteration */
        بیعلامت prev_idx;    /* idx in previous iteration */
#پایان_اگر
} Iterator;

#تعریف_کن IDX_ITERATOR_اولی (UINT_MAX - 1)
#تعریف_کن ITERATOR_اولی ((Iterator) { .idx = IDX_ITERATOR_اولی, .بعدی_کلید = عدم })
#تعریف_کن ITERATOR_IS_اولی(i) ((i).idx == IDX_ITERATOR_اولی)
