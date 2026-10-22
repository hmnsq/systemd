/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "memory-util.h"

size_t page_size(শূন্য) {
        স্থির thread_local size_t pgsz = 0;

        যদি (_likely_(pgsz > 0))
                প্রত্যাগম pgsz;

        long p = sysconf(_SC_PAGESIZE);
        প্রতিজ্ঞা(p > 0);

        pgsz = (size_t) p;
        প্রত্যাগম pgsz;
}

শূন্য* erase_and_free(শূন্য *p) {
        size_t l;

        যদি (!p)
                প্রত্যাগম রিক্ত;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        প্রত্যাগম mfree(p);
}
