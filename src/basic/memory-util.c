/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"

size_t page_size(ଶୂନ୍ୟ) {
        ସ୍ଥିର thread_local size_t pgsz = 0;

        ଯଦି (_likely_(pgsz > 0))
                ଫେରନ୍ତୁ pgsz;

        long p = sysconf(_SC_PAGESIZE);
        ପ୍ରତିଜ୍ଞା(p > 0);

        pgsz = (size_t) p;
        ଫେରନ୍ତୁ pgsz;
}

ଶୂନ୍ୟ* erase_and_free(ଶୂନ୍ୟ *p) {
        size_t l;

        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ଫେରନ୍ତୁ mfree(p);
}
