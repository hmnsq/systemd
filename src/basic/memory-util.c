/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "memory-util.h"

size_t page_size(शून्य) {
        स्थिर thread_local size_t pgsz = 0;

        यदि (_likely_(pgsz > 0))
                प्रत्यागम pgsz;

        long p = sysconf(_SC_PAGESIZE);
        प्रतिज्ञा(p > 0);

        pgsz = (size_t) p;
        प्रत्यागम pgsz;
}

शून्य* erase_and_free(शून्य *p) {
        size_t l;

        यदि (!p)
                प्रत्यागम रिक्त;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        प्रत्यागम mfree(p);
}
