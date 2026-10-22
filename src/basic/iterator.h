/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
تعريفنوع بنية Iterator {
        ثابت فراغ *بل_کلی; /* expected value of that entry's key pointer */
        بینښه idx;         /* index of an entry to be iterated next */
#إذا ENABLE_DEBUG_HASHMAP
        بینښه put_شمېر;   /* hashmap's put_count recorded at start of iteration */
        بینښه rem_شمېر;   /* hashmap's rem_count in previous iteration */
        بینښه prev_idx;    /* idx in previous iteration */
#د_که_پای
} Iterator;

#تعریف_کړه IDX_ITERATOR_لومړی (UINT_MAX - 1)
#تعریف_کړه ITERATOR_لومړی ((Iterator) { .idx = IDX_ITERATOR_لومړی, .بل_کلی = عدم })
#تعریف_کړه ITERATOR_IS_لومړی(i) ((i).idx == IDX_ITERATOR_لومړی)
