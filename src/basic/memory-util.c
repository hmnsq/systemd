/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "memory-util.h"

माप_प्रकार page_आकार(व्योम) {
        अटल thread_local माप_प्रकार pgsz = 0;

        अगर (_likely_(pgsz > 0))
                वापस pgsz;

        दीर्घ p = sysconf(_SC_PAGESIZE);
        निश्चित(p > 0);

        pgsz = (माप_प्रकार) p;
        वापस pgsz;
}

व्योम* erase_and_free(व्योम *p) {
        माप_प्रकार l;

        अगर (!p)
                वापस शून्य;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        वापस mfree(p);
}
