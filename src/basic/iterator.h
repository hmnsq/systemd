/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
قسمتعریف ساخت Iterator {
        ثابت خالی *اگلا_کلید; /* expected value of that entry's key pointer */
        غیرعلامتی idx;         /* index of an entry to be iterated next */
#اگر ENABLE_DEBUG_HASHMAP
        غیرعلامتی put_گنتی;   /* hashmap's put_count recorded at start of iteration */
        غیرعلامتی rem_گنتی;   /* hashmap's rem_count in previous iteration */
        غیرعلامتی پچھلا_idx;    /* idx in previous iteration */
#اگر_ختم
} Iterator;

#تعریف_کرو IDX_ITERATOR_پہلا (UINT_MAX - 1)
#تعریف_کرو ITERATOR_پہلا ((Iterator) { .idx = IDX_ITERATOR_پہلا, .اگلا_کلید = تہی })
#تعریف_کرو ITERATOR_IS_پہلا(i) ((i).idx == IDX_ITERATOR_پہلا)
