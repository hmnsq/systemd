/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "memory-util.h"

size_t page_size(void) {
        static thread_local size_t pgsz = 0;

        ᮚᮦᮔ᮪ (_likely_(pgsz > 0))
                ᮝᮜᮤ pgsz;

        long p = sysconf(_SC_PAGESIZE);
        assert(p > 0);

        pgsz = (size_t) p;
        ᮝᮜᮤ pgsz;
}

void* erase_and_free(void *p) {
        size_t l;

        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ᮝᮜᮤ mfree(p);
}
