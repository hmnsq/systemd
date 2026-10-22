/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <sys/stat.h>

#ضمن "forward.h"

ساكن مضمن فراغ umaskp(mode_t *u) {
        umask(*u);
}

#تعریف_کړه _cleanup_umask_ _cleanup_(umaskp)

/* We make use of the fact here that the umask() syscall uses only the lower 9 bits of mode_t, although
 * mode_t has space for the file type in the bits further up. We simply OR in the file type mask S_IFMT to
 * distinguish the first and the second iteration of the WITH_UMASK() loop, so that we can run the first one,
 * and exit on the second. */

assert_cc((S_IFMT & 0777) == 0);

#تعریف_کړه WITH_UMASK(ماسک_3)                                            \
        لكل (_cleanup_umask_ mode_t _saved_umask_ = umask(ماسک_3) | S_IFMT; \
             FLAGS_سېټ(_saved_umask_, S_IFMT);                          \
             _saved_umask_ &= 0777)

#تعریف_کړه بلاک_WITH_UMASK(ماسک_3) \
        ناکارول_شوی _cleanup_umask_ mode_t _saved_umask_ = umask(ماسک_3);
