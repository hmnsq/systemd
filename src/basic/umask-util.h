/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <sys/stat.h>

#ضمن "forward.h"

ساكن مضمن فراغ umaskp(mode_t *u) {
        umask(*u);
}

#تعریف_کرو _cleanup_umask_ _cleanup_(umaskp)

/* We make use of the fact here that the umask() syscall uses only the lower 9 bits of mode_t, although
 * mode_t has space for the file type in the bits further up. We simply OR in the file type mask S_IFMT to
 * distinguish the first and the second iteration of the WITH_UMASK() loop, so that we can run the first one,
 * and exit on the second. */

assert_cc((S_IFMT & 0777) == 0);

#تعریف_کرو WITH_UMASK(نقاب_3)                                            \
        لكل (_cleanup_umask_ mode_t _saved_umask_ = umask(نقاب_3) | S_IFMT; \
             FLAGS_سیٹ(_saved_umask_, S_IFMT);                          \
             _saved_umask_ &= 0777)

#تعریف_کرو بلاک_WITH_UMASK(نقاب_3) \
        بن_ورتیا _cleanup_umask_ mode_t _saved_umask_ = umask(نقاب_3);
