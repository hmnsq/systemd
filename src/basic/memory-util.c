/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "memory-util.h"

size_t page_size(فراغ) {
        ساكن thread_local size_t pgsz = 0;

        𐤀𐤌 (_likely_(pgsz > 0))
                أرجع pgsz;

        long p = sysconf(_SC_PAGESIZE);
        أكد(p > 0);

        pgsz = (size_t) p;
        أرجع pgsz;
}

فراغ* erase_and_free(فراغ *p) {
        size_t l;

        𐤀𐤌 (!p)
                أرجع عدم;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        أرجع mfree(p);
}
