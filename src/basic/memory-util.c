/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "memory-util.h"

size_t پاڼه_اندازه(فراغ) {
        ساكن thread_local size_t pgsz = 0;

        إذا (_likely_(pgsz > 0))
                أرجع pgsz;

        اوږد p = sysconf(_SC_PAGESIZE);
        أكد(p > 0);

        pgsz = (size_t) p;
        أرجع pgsz;
}

فراغ* erase_and_free(فراغ *p) {
        size_t l;

        إذا (!p)
                أرجع عدم;

        l = MALLOC_SIZEOF_خوندي(p);
        explicit_bzero_خوندي(p, l);
        أرجع mfree(p);
}
