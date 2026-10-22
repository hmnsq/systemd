/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ umaskp(mode_t *u) {
        umask(*u);
}

#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_umask_ _cleanup_(umaskp)

/* We make use of the fact here that the umask() syscall uses only the lower 9 bits of mode_t, although
 * mode_t has space for the file type in the bits further up. We simply OR in the file type mask S_IFMT to
 * distinguish the first and the second iteration of the WITH_UMASK() loop, so that we can run the first one,
 * and exit on the second. */

assert_cc((S_IFMT & 0777) == 0);

#ವ್ಯಾಖ್ಯಾನಿಸು WITH_UMASK(ಮುಖವಾಡ_3)                                            \
        ಗಾಗಿ (_cleanup_umask_ mode_t _saved_umask_ = umask(ಮುಖವಾಡ_3) | S_IFMT; \
             FLAGS_ಗಣ(_saved_umask_, S_IFMT);                          \
             _saved_umask_ &= 0777)

#ವ್ಯಾಖ್ಯಾನಿಸು ಖಂಡ_WITH_UMASK(ಮುಖವಾಡ_3) \
        ಬಳಸದ _cleanup_umask_ mode_t _saved_umask_ = umask(ಮುಖವಾಡ_3);
