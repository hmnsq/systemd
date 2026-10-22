/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "memory-util.h"

size_t page_size(void) {
        static thread_local size_t pgsz = 0;

        ຖ້າ (_likely_(pgsz > 0))
                ສົ່ງຄືນ pgsz;

        long p = sysconf(_SC_PAGESIZE);
        assert(p > 0);

        pgsz = (size_t) p;
        ສົ່ງຄືນ pgsz;
}

void* erase_and_free(void *p) {
        size_t l;

        ຖ້າ (!p)
                ສົ່ງຄືນ NULL;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ສົ່ງຄືນ mfree(p);
}
