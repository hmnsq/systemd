/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "memory-util.h"

size_t page_size(void) {
        static thread_local size_t pgsz = 0;

        အကယ်၍ (_likely_(pgsz > 0))
                ပြန်ပေး pgsz;

        long p = sysconf(_SC_PAGESIZE);
        assert(p > 0);

        pgsz = (size_t) p;
        ပြန်ပေး pgsz;
}

void* erase_and_free(void *p) {
        size_t l;

        အကယ်၍ (!p)
                ပြန်ပေး NULL;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ပြန်ပေး mfree(p);
}
