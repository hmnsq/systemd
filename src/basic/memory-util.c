/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "memory-util.h"

size_t page_size(शून्य) {
        स्थिर thread_local size_t pgsz = 0;

        सचे (_likely_(pgsz > 0))
                पटिनिवत्त pgsz;

        long p = sysconf(_SC_PAGESIZE);
        प्रतिज्ञा(p > 0);

        pgsz = (size_t) p;
        पटिनिवत्त pgsz;
}

शून्य* erase_and_free(शून्य *p) {
        size_t l;

        सचे (!p)
                पटिनिवत्त रिक्त;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        पटिनिवत्त mfree(p);
}
