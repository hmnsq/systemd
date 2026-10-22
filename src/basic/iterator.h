/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Ideally the Iterator would be an opaque struct, but it is instantiated
 * by hashmap users, so the definition has to be here. Do not use its fields
 * directly. */
రకనిర్వచనం నిర్మాణం Iterator {
        స్థిరరాశి శూన్య *తదుపరి_కీలకం; /* expected value of that entry's key pointer */
        గుర్తులేని idx;         /* index of an entry to be iterated next */
#ఒకవేళ ENABLE_DEBUG_HASHMAP
        గుర్తులేని put_లెక్క;   /* hashmap's put_count recorded at start of iteration */
        గుర్తులేని rem_లెక్క;   /* hashmap's rem_count in previous iteration */
        గుర్తులేని prev_idx;    /* idx in previous iteration */
#అయితే_ముగింపు
} Iterator;

#నిర్వచించు IDX_ITERATOR_మొదటి (UINT_MAX - 1)
#నిర్వచించు ITERATOR_మొదటి ((Iterator) { .idx = IDX_ITERATOR_మొదటి, .తదుపరి_కీలకం = ఖాళీ })
#నిర్వచించు ITERATOR_IS_మొదటి(i) ((i).idx == IDX_ITERATOR_మొదటి)
