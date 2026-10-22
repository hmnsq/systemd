/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "memory-util.h"

size_t page_size(வெறுமை) {
        நிலையான thread_local size_t pgsz = 0;

        எனில் (_likely_(pgsz > 0))
                திரும்பு pgsz;

        long p = sysconf(_SC_PAGESIZE);
        ப்ரதிஜ்ஞா(p > 0);

        pgsz = (size_t) p;
        திரும்பு pgsz;
}

வெறுமை* erase_and_free(வெறுமை *p) {
        size_t l;

        எனில் (!p)
                திரும்பு வெற்று;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        திரும்பு mfree(p);
}
