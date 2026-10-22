/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "memory-util.h"

size_t page_size(فراغ) {
        ساكن thread_local size_t pgsz = 0;

        إذا (_likely_(pgsz > 0))
                إرجاع pgsz;

        long p = sysconf(_SC_PAGESIZE);
        أكد(p > 0);

        pgsz = (size_t) p;
        إرجاع pgsz;
}

فراغ* erase_and_free(فراغ *p) {
        size_t l;

        إذا (!p)
                إرجاع عدم;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        إرجاع mfree(p);
}
