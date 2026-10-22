/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "forward.h"

స్థిర అంతఃవరుస శూన్య umaskp(mode_t *u) {
        umask(*u);
}

#నిర్వచించు _cleanup_umask_ _cleanup_(umaskp)

/* We make use of the fact here that the umask() syscall uses only the lower 9 bits of mode_t, although
 * mode_t has space for the file type in the bits further up. We simply OR in the file type mask S_IFMT to
 * distinguish the first and the second iteration of the WITH_UMASK() loop, so that we can run the first one,
 * and exit on the second. */

assert_cc((S_IFMT & 0777) == 0);

#నిర్వచించు WITH_UMASK(ముసుగు_3)                                            \
        కొరకు (_cleanup_umask_ mode_t _saved_umask_ = umask(ముసుగు_3) | S_IFMT; \
             FLAGS_సమితి(_saved_umask_, S_IFMT);                          \
             _saved_umask_ &= 0777)

#నిర్వచించు ఖండం_WITH_UMASK(ముసుగు_3) \
        వాడని _cleanup_umask_ mode_t _saved_umask_ = umask(ముసుగు_3);
