/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "memory-util.h"

size_t page_size(ശൂന്യം) {
        സ്ഥിതം thread_local size_t pgsz = 0;

        എങ്കിൽ (_likely_(pgsz > 0))
                തിരികെ pgsz;

        long p = sysconf(_SC_PAGESIZE);
        പ്രതിജ്ഞാ(p > 0);

        pgsz = (size_t) p;
        തിരികെ pgsz;
}

ശൂന്യം* erase_and_free(ശൂന്യം *p) {
        size_t l;

        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        l = MALLOC_SIZEOF_SAFE(p);
        explicit_bzero_safe(p, l);
        തിരികെ mfree(p);
}
