/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "memory-util.h"

size_t పుట_కొలత(శూన్య) {
        స్థిర thread_local size_t pgsz = 0;

        ఒకవేళ (_likely_(pgsz > 0))
                తిరిగి pgsz;

        దీర్ఘ p = sysconf(_SC_PAGESIZE);
        ప్రతిజ్ఞా(p > 0);

        pgsz = (size_t) p;
        తిరిగి pgsz;
}

శూన్య* erase_and_free(శూన్య *p) {
        size_t l;

        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        l = MALLOC_SIZEOF_సురక్షిత(p);
        explicit_bzero_సురక్షిత(p, l);
        తిరిగి mfree(p);
}
