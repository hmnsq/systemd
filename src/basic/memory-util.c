/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <threads.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "memory-util.h"

size_t page_size(تهی) {
        ایستا thread_local size_t pgsz = 0;

        اگر (_likely_(pgsz > 0))
                بازگشت pgsz;

        long p = sysconf(_SC_PAGESIZE);
        أكد(p > 0);

        pgsz = (size_t) p;
        بازگشت pgsz;
}

تهی* erase_and_free(تهی *p) {
        size_t l;

        اگر (!p)
                بازگشت پوچ;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        بازگشت mfree(p);
}
