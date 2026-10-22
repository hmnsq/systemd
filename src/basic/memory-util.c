/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "memory-util.h"

size_t page_size(શૂન્ય) {
        સ્થિર thread_local size_t pgsz = 0;

        જો (_likely_(pgsz > 0))
                પાછા pgsz;

        long p = sysconf(_SC_PAGESIZE);
        પ્રતિજ્ઞા(p > 0);

        pgsz = (size_t) p;
        પાછા pgsz;
}

શૂન્ય* erase_and_free(શૂન્ય *p) {
        size_t l;

        જો (!p)
                પાછા ખાલી;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        પાછા mfree(p);
}
