/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "memory-util.h"

size_t page_size(void) {
        static thread_local size_t pgsz = 0;

        ᨀᨘ (_likely_(pgsz > 0))
                ᨀᨘᨒ pgsz;

        long p = sysconf(_SC_PAGESIZE);
        assert(p > 0);

        pgsz = (size_t) p;
        ᨀᨘᨒ pgsz;
}

void* erase_and_free(void *p) {
        size_t l;

        ᨀᨘ (!p)
                ᨀᨘᨒ NULL;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ᨀᨘᨒ mfree(p);
}
