/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <threads.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "memory-util.h"

size_t page_size(শূন্য) {
        স্থির thread_local size_t pgsz = 0;

        যদি (_likely_(pgsz > 0))
                ঘূৰাই pgsz;

        long p = sysconf(_SC_PAGESIZE);
        প্রতিজ্ঞা(p > 0);

        pgsz = (size_t) p;
        ঘূৰাই pgsz;
}

শূন্য* erase_and_free(শূন্য *p) {
        size_t l;

        যদি (!p)
                ঘূৰাই রিক্ত;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ঘূৰাই mfree(p);
}
