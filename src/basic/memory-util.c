/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"

size_t ಪುಟ_ಗಾತ್ರ(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local size_t pgsz = 0;

        ಆದರೆ (_likely_(pgsz > 0))
                ಹಿಂತಿರುಗಿ pgsz;

        ದೀರ್ಘ p = sysconf(_SC_PAGESIZE);
        ಪ್ರತಿಜ್ಞಾ(p > 0);

        pgsz = (size_t) p;
        ಹಿಂತಿರುಗಿ pgsz;
}

ಶೂನ್ಯ* erase_and_free(ಶೂನ್ಯ *p) {
        size_t l;

        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        l = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(p);
        explicit_bzero_ಸುರಕ್ಷಿತ(p, l);
        ಹಿಂತಿರುಗಿ mfree(p);
}
