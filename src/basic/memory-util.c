/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"

size_t page_size(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local size_t pgsz = 0;

        ಯದಿ (_likely_(pgsz > 0))
                ಪ್ರತ್ಯಾಗಮ pgsz;

        long p = sysconf(_SC_PAGESIZE);
        ಪ್ರತಿಜ್ಞಾ(p > 0);

        pgsz = (size_t) p;
        ಪ್ರತ್ಯಾಗಮ pgsz;
}

ಶೂನ್ಯ* erase_and_free(ಶೂನ್ಯ *p) {
        size_t l;

        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        ಪ್ರತ್ಯಾಗಮ mfree(p);
}
