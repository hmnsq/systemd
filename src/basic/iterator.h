/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
प्रकार काष्ठा Iterator {
        स्थिर व्योम *अग्र_कुंजी; /* expected value of that entry's key pointer */
        अचिन्हित idx;         /* index of an entry to be iterated next */
#अगर ENABLE_DEBUG_HASHMAP
        अचिन्हित put_गिनती;   /* hashmap's put_count recorded at start of iteration */
        अचिन्हित rem_गिनती;   /* hashmap's rem_count in previous iteration */
        अचिन्हित पूर्व_idx;    /* idx in previous iteration */
#पूर्ण_अगर
} Iterator;

#घोषणा _IDX_ITERATOR_FIRST (अच_पूर्णांक_उच्च - 1)
#घोषणा ITERATOR_FIRST ((Iterator) { .idx = _IDX_ITERATOR_FIRST, .अग्र_कुंजी = शून्य })
#घोषणा ITERATOR_IS_FIRST(i) ((i).idx == _IDX_ITERATOR_FIRST)
