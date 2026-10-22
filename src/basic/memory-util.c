/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"

size_t ਪੰਨਾ_ਮਾਪ(ਖਾਲੀ) {
        ਸਥਿਰ thread_local size_t pgsz = 0;

        ਜੇ (_likely_(pgsz > 0))
                ਵਾਪਸ pgsz;

        ਲੰਮਾ p = sysconf(_SC_PAGESIZE);
        ਪ੍ਰਤਿਜ੍ਞਾ(p > 0);

        pgsz = (size_t) p;
        ਵਾਪਸ pgsz;
}

ਖਾਲੀ* erase_and_free(ਖਾਲੀ *p) {
        size_t l;

        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        l = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(p);
        explicit_bzero_ਸੁਰੱਖਿਅਤ(p, l);
        ਵਾਪਸ mfree(p);
}
