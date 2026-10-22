/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
تعريفنوع بنية Iterator {
        ثابت فراغ *اگلا_کلید; /* expected value of that entry's key pointer */
        بنانشانی idx;         /* index of an entry to be iterated next */
#إذا ENABLE_DEBUG_HASHMAP
        بنانشانی put_گنتی;   /* hashmap's put_count recorded at start of iteration */
        بنانشانی rem_گنتی;   /* hashmap's rem_count in previous iteration */
        بنانشانی prev_idx;    /* idx in previous iteration */
#جے_انت
} Iterator;

#تعریف_کرو IDX_ITERATOR_پہلا (UINT_MAX - 1)
#تعریف_کرو ITERATOR_پہلا ((Iterator) { .idx = IDX_ITERATOR_پہلا, .اگلا_کلید = عدم })
#تعریف_کرو ITERATOR_IS_پہلا(i) ((i).idx == IDX_ITERATOR_پہلا)
