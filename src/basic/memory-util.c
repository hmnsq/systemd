/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <threads.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "memory-util.h"

size_t صفحہ_سائز(خالی) {
        ساکن thread_local size_t pgsz = 0;

        اگر (_likely_(pgsz > 0))
                واپس pgsz;

        طویل p = sysconf(_SC_PAGESIZE);
        أكد(p > 0);

        pgsz = (size_t) p;
        واپس pgsz;
}

خالی* erase_and_free(خالی *p) {
        size_t l;

        اگر (!p)
                واپس تہی;

        l = MALLOC_SIZEOF_محفوظ(p);
        explicit_bzero_محفوظ(p, l);
        واپس mfree(p);
}
