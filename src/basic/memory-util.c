/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "memory-util.h"

माप_प्रकार page_size(शून्य) {
        स्थिर thread_local माप_प्रकार pgsz = 0;

        यदि (_likely_(pgsz > 0))
                फर्काऊ pgsz;

        दीर्घ p = sysconf(_SC_PAGESIZE);
        प्रतिज्ञा(p > 0);

        pgsz = (माप_प्रकार) p;
        फर्काऊ pgsz;
}

शून्य* erase_and_free(शून्य *p) {
        माप_प्रकार l;

        यदि (!p)
                फर्काऊ रिक्त;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        फर्काऊ mfree(p);
}
